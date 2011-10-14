###########################################################################
## Copyright (C) 2011  Whit Armstrong                                    ##
##                                                                       ##
## This program is free software: you can redistribute it and#or modify  ##
## it under the terms of the GNU General Public License as published by  ##
## the Free Software Foundation, either version 3 of the License, or     ##
## (at your option) any later version.                                   ##
##                                                                       ##
## This program is distributed in the hope that it will be useful,       ##
## but WITHOUT ANY WARRANTY; without even the implied warranty of        ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         ##
## GNU General Public License for more details.                          ##
##                                                                       ##
## You should have received a copy of the GNU General Public License     ##
## along with this program.  If not, see <http:##www.gnu.org#licenses#>. ##
###########################################################################

init.context <- function() {
    .Call("initContext", PACKAGE="rzmq")
}

init.socket <- function(context, socket.type) {
    .Call("initSocket", context, socket.type, PACKAGE="rzmq")
}

bind.socket <- function(socket, address) {
    .Call("bindSocket", socket, address, PACKAGE="rzmq")
}

connect.socket <- function(socket, address) {
    .Call("connectSocket", socket, address, PACKAGE="rzmq")
}

send.socket <- function(socket, data) {
    .Call("sendSocket", socket, serialize(data,NULL), PACKAGE="rzmq")
}

send.null.msg <- function(socket) {
    .Call("sendNullMsg", socket, PACKAGE="rzmq")
}

receive.socket <- function(socket) {
    ans <- .Call("receiveSocket", socket, PACKAGE="rzmq")
    unserialize(ans)
}

receive.string <- function(socket) {
    .Call("receiveString", socket, PACKAGE="rzmq")
}

receive.int <- function(socket) {
    .Call("receiveInt", socket, PACKAGE="rzmq")
}

receive.double <- function(socket) {
    .Call("receiveDouble", socket, PACKAGE="rzmq")
}

create.sink <- function(address, num_items) {
    .Call("createSink", address, as.integer(num_items), PACKAGE="rzmq")
}

get.sink.results <- function(sink) {
    .Call("getSinkResults", sink, PACKAGE="rzmq")
}

zmq.cluster.lapply <- function(cluster,X,FUN,...,deathstar.port=6000,control.port=6001) {
    remote.exec <- function(socket,index,fun,...) {
        send.socket(socket,data=list(index=index,fun=fun,args=list(...)))
    }

    FUN <- match.fun(FUN)
    if (!is.vector(X) || is.object(X))
        X <- as.list(X)

    N <- length(X)
    context = init.context()
    ## using a dealer socket locally will cache the requests in local and remote buffers
    ## this could fail if your request messages are big enough to exhaust swap space
    ## on local or remote devices, however if your messaing workflow is this large, then
    ## you should either use s3 to move your data across, and use messages to index into it
    ## or write your own messaging pattern
    execution.socket = init.socket(context,"ZMQ_DEALER")

    ## connect exec socket to all remote servers
    control.endpoints <- paste("tcp://",cluster,":",control.port,sep="")
    exec.endpoints <- paste("tcp://",cluster,":",deathstar.port,sep="")

    ## ensure ndoes are available for execution
    ## to avoid msg hogging / slow joiner issues
    for(node in control.endpoints) {
        cat("checking",node,"status: ")
        control.socket = init.socket(context,"ZMQ_REQ")
        connect.socket(control.socket,node)
        send.null.msg(control.socket)
        status <- receive.string(control.socket)
        cat(status,"\n")
    }

    ## connect to remote nodes
    for(node in exec.endpoints) {
        connect.socket(execution.socket,node)
    }

    ## submit jobs
    for(i in 1:N) {
        remote.exec(socket=execution.socket,index=i,fun=FUN,X[[i]],...)
    }

    ## pick up restuls
    ans <- vector("list",N)
    for(i in 1:N) {
        ans[[i]] <- receive.socket(execution.socket)
    }

    ## reorder anser based on returned index numbers
    ans.ordered <- vector("list",N)

    ## apply names if they exist
    if(!is.null(names(X))) {
        names(ans.ordered) <- names(X)
    }

    for(i in 1:N) {
        ans.ordered[[ ans[[i]]$index ]] <- ans[[i]]$result
    }

    execution.report <- as.matrix(table(unlist(lapply(ans,"[[","node"))))
    attr(ans.ordered,"execution.report") <- execution.report
    ans.ordered
}
