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

zmq.version <- function() {
    .Call("get_zmq_version", PACKAGE="rzmq")
}

zmq.errno <- function() {
    .Call("get_zmq_errno", PACKAGE="rzmq")
}

zmq.strerror <- function() {
    .Call("get_zmq_strerror", PACKAGE="rzmq")
}

init.context <- function() {
    .Call("initContext", PACKAGE="rzmq")
}

init.socket <- function(context, socket.type) {
    .Call("initSocket", context, socket.type, PACKAGE="rzmq")
}

bind.socket <- function(socket, address) {
    invisible(.Call("bindSocket", socket, address, PACKAGE="rzmq"))
}

connect.socket <- function(socket, address) {
    invisible(.Call("connectSocket", socket, address, PACKAGE="rzmq"))
}

send.socket <- function(socket, data, send.more=FALSE, serialize=TRUE) {
    if(serialize) {
        data <- serialize(data,NULL)
    }

    invisible(.Call("sendSocket", socket, data, send.more, PACKAGE="rzmq"))
}

send.null.msg <- function(socket, send.more=FALSE) {
    .Call("sendNullMsg", socket, send.more, PACKAGE="rzmq")
}

receive.null.msg <- function(socket) {
    .Call("receiveNullMsg", socket, PACKAGE="rzmq")
}

receive.socket <- function(socket, unserialize=TRUE,dont.wait=FALSE) {
    ans <- .Call("receiveSocket", socket, dont.wait, PACKAGE="rzmq")

    if(!is.null(ans) && unserialize) {
        ans <- unserialize(ans)
    }
    ans
}

receive.multipart <- function(socket) {
  parts = list(receive.socket(socket, unserialize=FALSE))
  while(get.rcvmore(socket)) {
    parts = append(parts, list(receive.socket(socket, unserialize=FALSE)))
  }
  return(parts)
}

send.multipart <- function(socket, parts) {
  for (part in parts[1:(length(parts)-1)]) {
    send.socket(socket, part, send.more=TRUE, serialize=FALSE)
  }
  send.socket(socket, parts[[length(parts)]], send.more=FALSE, serialize=FALSE)
}

send.raw.string <- function(socket,data,send.more=FALSE) {
    .Call("sendRawString", socket, data, send.more, PACKAGE="rzmq")
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

poll.socket <- function(sockets, events, timeout=0L) {
    if (timeout != -1L) timeout <- as.integer(timeout * 1000000)
    .Call("pollSocket", sockets, events, timeout)
}

set.hwm <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_HWM removed from libzmq3")
    } else {
        .Call("set_hwm",socket, option.value, PACKAGE="rzmq")
    }
}

set.swap <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_SWAP removed from libzmq3")
    } else {
        .Call("set_swap",socket, option.value, PACKAGE="rzmq")
    }
}

set.affinity <- function(socket, option.value) {
    .Call("set_affinity",socket, option.value, PACKAGE="rzmq")
}

set.identity <- function(socket, option.value) {
    .Call("set_identity",socket, option.value, PACKAGE="rzmq")
}

subscribe <- function(socket, option.value) {
    invisible(.Call("subscribe",socket, option.value, PACKAGE="rzmq"))
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
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_RECOVERY_IVL_MSEC removed from libzmq3")
    } else {
        .Call("set_recovery_ivl_msec",socket, option.value, PACKAGE="rzmq")
    }
}

set.mcast.loop <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_MCAST_LOOP removed from libzmq3")
    } else {
        .Call("set_mcast_loop",socket, option.value, PACKAGE="rzmq")
    }
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

set.send.timeout <- function(socket, option.value) {
    .Call("set_sndtimeo", socket, option.value, PACKAGE="rzmq")
}

get.send.timeout <- function(socket) {
    .Call("get_sndtimeo", socket, PACKAGE="rzmq")
}
