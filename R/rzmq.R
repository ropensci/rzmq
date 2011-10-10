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

zmq.cluster.lapply <- function(cluster,X,FUN,...,push.port=6000,pull.port=6001,control.port=6003) {
    remote.exec <- function(socket,index,fun,...) {
        send.socket(socket,data=list(index=index,fun=fun,args=list(...)))
    }

    FUN <- match.fun(FUN)
    if (!is.vector(X) || is.object(X))
        X <- as.list(X)

    context = init.context()
    execution.socket = init.socket(context,"ZMQ_PUSH")

    ## connect push socket to all remote servers
    control.points <- paste("tcp://",cluster,":",control.port,sep="")
    push.points <- paste("tcp://",cluster,":",push.port,sep="")
    pull.points <- paste("tcp://",cluster,":",pull.port,sep="")

    ## ensure ndoes are available for execution
    ## to avoid msg hogging / slow joiner issues
    for(node in control.points) {
        cat("checking",node,"status: ")
        control.socket = init.socket(context,"ZMQ_REQ")
        connect.socket(control.socket,node)
        send.null.msg(control.socket)
        status <- receive.string(control.socket)
        cat(status,"\n")
    }

    for(node in push.points) {
        connect.socket(execution.socket,node)
    }

    ## listen for results on the sink server
    N <- length(X)
    sink <- create.sink(pull.points,N)

    ## submit jobs
    for(i in 1:N) {
        remote.exec(socket=execution.socket,index=i,fun=FUN,X[[i]],...)
    }

    ## pick up restuls
    ans.raw <- get.sink.results(sink)
    ans <- lapply(ans.raw,unserialize)

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
