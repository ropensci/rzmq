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

send.socket <- function(socket, data, send.more=FALSE, serialize=TRUE) {
    if(serialize) {
        data <- serialize(data,NULL)
    }

    .Call("sendSocket", socket, data, send.more, PACKAGE="rzmq")
}

send.null.msg <- function(socket, send.more=FALSE) {
    .Call("sendNullMsg", socket, send.more, PACKAGE="rzmq")
}

receive.null.msg <- function(socket) {
    .Call("receiveNullMsg", socket, PACKAGE="rzmq")
}

receive.socket <- function(socket,unserialize=TRUE) {
    ans <- .Call("receiveSocket", socket, PACKAGE="rzmq")
    if(unserialize) {
        ans <- unserialize(ans)
    }
    ans
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

zmq.version <- function(socket) {
    .Call("zmqVersion", PACKAGE="rzmq")
}
