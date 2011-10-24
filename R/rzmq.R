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

send.socket <- function(socket, data, send.more=FALSE) {
    .Call("sendSocket", socket, serialize(data,NULL), send.more, PACKAGE="rzmq")
}

send.null.msg <- function(socket, send.more=FALSE) {
    .Call("sendNullMsg", socket, send.more, PACKAGE="rzmq")
}

receive.null.msg <- function(socket) {
    .Call("receiveNullMsg", socket, PACKAGE="rzmq")
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

set.hwm <- function(socket, option.value) {
    .Call("set_hwm",socket, option.value, PACKAGE="rzmq")
}

set.swap <- function(socket, option.value) {
    .Call("set_swap",socket, option.value, PACKAGE="rzmq")
}

set.affinity <- function(socket, option.value) {
    .Call("set_affinity",socket, option.value, PACKAGE="rzmq")
}

set.identity <- function(socket, option.value) {
    .Call("set_identity",socket, option.value, PACKAGE="rzmq")
}

subscribe <- function(socket, option.value) {
    .Call("subscribe",socket, option.value, PACKAGE="rzmq")
}

unsubscribe <- function(socket, option.value) {
    .Call("unsubscribe",socket, option.value, PACKAGE="rzmq")
}

set.rate <- function(socket, option.value) {
    .Call("set_rate",socket, option.value, PACKAGE="rzmq")
}

set.recovery.ivl <- function(socket, option.value) {
    .Call("set_recovery_ivl",socket, option.value, PACKAGE="rzmq")
}

set.recovery.ivl.msec <- function(socket, option.value) {
    .Call("set_recovery_ivl_msec",socket, option.value, PACKAGE="rzmq")
}

set.mcast.loop <- function(socket, option.value) {
    .Call("set_mcast_loop",socket, option.value, PACKAGE="rzmq")
}

set.sndbuf <- function(socket, option.value) {
    .Call("set_sndbuf",socket, option.value, PACKAGE="rzmq")
}

set.rcvbuf <- function(socket, option.value) {
    .Call("set_rcvbuf",socket, option.value, PACKAGE="rzmq")
}

set.linger <- function(socket, option.value) {
    .Call("set_linger",socket, option.value, PACKAGE="rzmq")
}

set.reconnect.ivl <- function(socket, option.value) {
    .Call("set_reconnect_ivl",socket, option.value, PACKAGE="rzmq")
}

set.zmq.backlog <- function(socket, option.value) {
    .Call("set_zmq_backlog",socket, option.value, PACKAGE="rzmq")
}

set.reconnect.ivl.max <- function(socket, option.value) {
    .Call("set_reconnect_ivl_max",socket, option.value, PACKAGE="rzmq")
}

get.rcvmore <- function(socket) {
    .Call("get_rcvmore",socket,PACKAGE="rzmq")
}

zmq.cluster.lapply <- function(cluster,X,FUN,...,deathstar.port=6000,control.port=6001) {
    remote.exec <- function(socket,index,fun,...) {
        ## expects socket to be ZMQ_DEALER
        ## send as though this is a req message
        ## by sending null first
        send.null.msg(socket, send.more=TRUE)
        send.socket(socket,data=list(index=index,fun=fun,args=list(...)),send.more=FALSE)
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
        ## outgoing msg was in form of REQ msg
        ## but sent with DEALER socket
        ## so we have to dissect the msg envelope
        receive.null.msg(execution.socket)
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
