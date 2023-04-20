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
 
#' Get the ZMQ version.
#' 
#' @return ZMQ version string.
#' @export
#' 
#' @examples
#' zmq.version()
zmq.version <- function() {
    .Call("get_zmq_version", PACKAGE="rzmq")
}

#' Returns the latest error code from ZMQ.
#' 
#' @return Error code.
#' @export
#' 
#' @examples
#' zmq.errno()
zmq.errno <- function() {
    .Call("get_zmq_errno", PACKAGE="rzmq")
}

#' Returns the latest error message from ZMQ.
#' 
#' @return Error message.
#' @export
#' 
#' @examples
#' zmq.strerror()
zmq.strerror <- function() {
    .Call("get_zmq_strerror", PACKAGE="rzmq")
}

#' Initialize a ZMQ context.
#' 
#' @param threads Number of threads for the context to use.
#' 
#' @return ZMQ context.
#' @export
#' 
#' @examples
#' init.context()
init.context <- function(threads=1L) {
    .Call("initContext", threads, PACKAGE="rzmq")
}

#' Initializes a socket of the given type.
#' 
#' @param context ZMQ context.
#' @param socket.type Type of socket to initialize.
#' 
#' @return ZMQ socket.
#' @export
#' 
#' @examples
#' context <- init.context()
#' init.socket(context, "ZMQ_REQ")
init.socket <- function(context, socket.type) {
    .Call("initSocket", context, socket.type, PACKAGE="rzmq")
}

#' Binds a socket to an address.
#' 
#' @param socket Socket.
#' @param address Transport address to bind the socket to.
#' 
#' @return `TRUE` if the socket was bound successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' bind.socket(socket, "tcp://*:8080")
#' 
bind.socket <- function(socket, address) {
    invisible(.Call("bindSocket", socket, address, PACKAGE="rzmq"))
}

#' Connects a socket to an address.
#' 
#' @param socket Socket.
#' @param address Transport address to connect the socket to.
#' 
#' @return `TRUE` if the socket connected successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
connect.socket <- function(socket, address) {
    invisible(.Call("connectSocket", socket, address, PACKAGE="rzmq"))
}

#' Disconnects a socket from an address.
#' 
#' @param socket Socket.
#' @param address Transport address to disconnect the socket from
#' 
#' @return `TRUE` if the socket connected successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' disconnect.socket(socket, "tcp://*:8080")
disconnect.socket <- function(socket, address) {
    invisible(.Call("disconnectSocket", socket, address, PACKAGE="rzmq"))
}

#' Sends a message.
#' 
#' @param socket Socket.
#' @param data Data to transmit.
#' @param send.more Whether more frames will be sent.
#' @param serialize Whether to call `serialize` before sending.
#' @param xdr Passed directly to `serialize` function.
#' 
#' @return `TRUE` if the message was sent successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' send.socket(socket, "hello")
send.socket <- function(socket, data, send.more=FALSE, serialize=TRUE,
                        xdr=.Platform$endian=="big") {
    if(serialize) {
        data <- serialize(data, NULL, xdr=xdr)
    }

    invisible(.Call("sendSocket", socket, data, send.more, PACKAGE="rzmq"))
}

#' Sends a null message.
#' 
#' @param socket Socket.
#' @param send.more Whether more frames will be sent.
#' 
#' @return `TRUE` if the message was sent successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' send.null.msg(socket)
send.null.msg <- function(socket, send.more=FALSE) {
    .Call("sendNullMsg", socket, send.more, PACKAGE="rzmq")
}

#' Creates a ZMQ message that can be sent multiple times.
#' 
#' @param data Data to transmit.
#' @param serialize Whether to call `serialize` before sending.
#' @param xdr Passed directly to `serialize` function.
#'
#' @return ZMQ message.
#' @export
#' 
#' @examples
#' init.message("hello")
init.message <- function(data, serialize=TRUE, xdr=.Platform$endian=="big") {
    if(serialize) {
        data <- serialize(data, NULL, xdr=xdr)
    }
    .Call("initMessage", data, PACKAGE="rzmq")
}

#' Send a message.
#'
#' @param socket Socket.
#' @param msg Message.
#' @param send.more Whether more frames will be sent.
#' 
#' @return `TRUE` if the message was sent successfully, `FALSE` otherwise.
#' @export
#' 
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' msg <- init.message("hello")
#' send.message.object(socket, msg)
#' send.message.object(socket, msg)
send.message.object <- function(socket, msg, send.more=FALSE) {
    .Call("sendMessageObject", socket, msg, send.more)
}

#' Receive a null message.
#'
#' @param socket Socket.
#'
#' @return Message.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' send.null.msg(socket)
#' receive.null.msg(socket)
receive.null.msg <- function(socket) {
    .Call("receiveNullMsg", socket, PACKAGE="rzmq")
}

#' Receive a message.
#'
#' @param socket Socket.
#' @param unserialize Whether to unserialize the message.
#' @param dont.wait `FALSE` to block, `TRUE` for non-blocking.
#'
#' @return Message.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' send.socket(socket, "hello")
#' msg <- receive.socket(socket)
receive.socket <- function(socket, unserialize=TRUE, dont.wait=FALSE) {
    ans <- .Call("receiveSocket", socket, dont.wait, PACKAGE="rzmq")

    if(!is.null(ans) && unserialize) {
        ans <- unserialize(ans)
    }
    ans
}

#' Receive a multipart message.
#'
#' @param socket 
#'
#' @return List of message parts. Parts are unserialized.
#' @export
#'
#' @examples
#' #' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' send.socket(socket, "hello")
#' parts <- receive.multipart(socket)
receive.multipart <- function(socket) {
  parts = list(receive.socket(socket, unserialize=FALSE))
  while(get.rcvmore(socket)) {
    parts = append(parts, list(receive.socket(socket, unserialize=FALSE)))
  }
  return(parts)
}

#' Send a multipart message.
#' Uses `send.socket` with `serialize` set to `FALSE`.
#'
#' @param socket Socket.
#' @param parts List of message parts.
#'
#' @return `TRUE` if the messages were sent successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' msgs <- c("hello", "world")
#' send.multipart(socket, msgs)
send.multipart <- function(socket, parts) {
  for (part in parts[1:(length(parts)-1)]) {
    send.socket(socket, part, send.more=TRUE, serialize=FALSE)
  }
  send.socket(socket, parts[[length(parts)]], send.more=FALSE, serialize=FALSE)
}

#' Sends a raw string.
#'
#' @param socket Socket.
#' @param data String to send.
#' @param send.more Whether more frames in the message will be sent.
#'
#' @return `TRUE` if the message was sent successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' send.raw.string(socket, "hello")
send.raw.string <- function(socket, data, send.more=FALSE) {
    .Call("sendRawString", socket, data, send.more, PACKAGE="rzmq")
}

#' Receive a message as a string.
#'
#' @param socket Socket.
#'
#' @return Recieved message as a string.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' msg <- receive.string(socket)
receive.string <- function(socket) {
    .Call("receiveString", socket, PACKAGE="rzmq")
}

#' Receive a message as an integer.
#'
#' @param socket Socket.
#'
#' @return Received message as an integer.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' msg <- receive.int(socket)
receive.int <- function(socket) {
    .Call("receiveInt", socket, PACKAGE="rzmq")
}

#' Receive a message as an double.
#'
#' @param socket Socket.
#'
#' @return Received message as an double.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' connect.socket(socket, "tcp://*:8080")
#' msg <- receive.double(socket)
receive.double <- function(socket) {
    .Call("receiveDouble", socket, PACKAGE="rzmq")
}

#' Polls a list of sockets.
#'
#' @param sockets List of sockets.
#' @param events A list of character vector corresponding to the events the
#' corresponding socket responds to. Valid events are {"read", "write", "error"}
#' @param timeout Time to wait for an event in seconds.
#' A timeout of -1L blocks until an event occurs.
#' A timeout of 0L is non-blocking.
#'
#' @return A list of pairlists corresponding to the polled sockets.
#' Each list has one of more tags from {"read", "write", "error"} with 
#' logical values indicating the results of the poll operation.
#' @export
#'
#' @examples
#' # Create a set of REP-REQ sockets that
#' # have a Send, Receive, Send, Receive, ...
#' # pattern.
#' context = init.context()
#' in.socket = init.socket(context,"ZMQ_REP")
#' bind.socket(in.socket,"tcp://*:5557")
#' out.socket = init.socket(context,"ZMQ_REQ")
#' connect.socket(out.socket,"tcp://*:5557")
#' # Poll the REP and REQ sockets for all events.
#' events <- poll.socket(list(in.socket, out.socket),
#'                       list(c("read", "write", "error"),
#'                            c("read", "write", "error")),
#'                       timeout=0L)
#' # The REQ socket is writable without blocking.
#' paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
#' paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
#' paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
#' paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)
#' # Send a message to the REP socket from the REQ socket. The
#' # REQ socket must respond before the REP socket can send
#' # another message.
#' send.socket(out.socket, "Hello World")
#' events <- poll.socket(list(in.socket, out.socket),
#'                       list(c("read", "write", "error"),
#'                            c("read", "write", "error")),
#'                       timeout=0L)
#' # The incoming message is readable on the REP socket.
#' paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
#' paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
#' paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
#' paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)
#' receive.socket(in.socket)
#' poll.socket <- function(sockets, events, timeout=0L) {
#'     if (timeout != -1L) timeout <- as.integer(timeout * 1e3)
#'     .Call("pollSocket", sockets, events, timeout)
#' }
#' events <- poll.socket(list(in.socket, out.socket),
#'                       list(c("read", "write", "error"),
#'                            c("read", "write", "error")),
#'                       timeout=0L)
#' # The REQ socket is waiting for a response from the REP socket.
#' paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
#' paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
#' paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
#' paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)
#' send.socket(in.socket, "Greetings")
#' events <- poll.socket(list(in.socket, out.socket),
#'                       list(c("read", "write", "error"),
#'                            c("read", "write", "error")),
#'                       timeout=0L)
#' # The REP response is waiting to be read on the REQ socket.
#' paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
#' paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
#' paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
#' paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)
#' # Complete the REP-REQ transaction cycle by reading
#' # the REP response.
#' receive.socket(out.socket)

#' Set the I/O thread affinity for newly created connections of the socket.
#' See ZMQ socket option `ZMQ_AFFINITY`.
#'
#' @param socket Socket.
#' @param option.value Affinity.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.affinity(socket, 1L)
set.affinity <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_AFFINITY", option.value, PACKAGE="rzmq")
}

#' Set the identity of the socket.
#' See ZMQ socket option `ZMQ_IDENTITY`.
#'
#' @param socket Socket.
#' @param option.value Identity.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.identity(socket, "my_socket")
set.identity <- function(socket, option.value) {
    .Call("setSockOptChr", socket, "ZMQ_IDENTITY", option.value, PACKAGE="rzmq")
}

#' Establish a new message filter on a `ZMQ_SUB` socket.
#' See ZMQ socket option `ZMQ_SUBSCRIBE`.
#'
#' @param socket Socket. Must be of type `ZMQ_SUB`.
#' @param option.value Filter.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_SUB")
#' subscribe(socket)
subscribe <- function(socket, option.value) {
    invisible(.Call("setSockOptChr", socket, "ZMQ_SUBSCRIBE", option.value, PACKAGE="rzmq"))
}

#' Unsubscribe from a message filter.
#' See ZMQ socket option `ZMQ_UNSUBSCRIBE`.
#'
#' @param socket  Socket. Must be of type `ZMQ_SUB`.
#' @param option.value Filter.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_SUB")
#' subscribe(socket)
#' unsubscribe(socket, "ignore")
unsubscribe <- function(socket, option.value) {
    .Call("setSockOptChr", socket, "ZMQ_UNSUBSCRIBE", option.value, PACKAGE="rzmq")
}

#' Set the socket's multicast rate.
#' See ZMQ socket option `ZMQ_RATE`.
#'
#' @param socket Socket. Must be multicast.
#' @param option.value Rate in kilobytes per second.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_PUB")
#' set.rate(socket, 100L)
set.rate <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RATE", option.value, PACKAGE="rzmq")
}

#' Set the socket's recovery interval for multicast transports.
#' See ZMQ socket option `ZMQ_RECOVERY_IVL`.
#'
#' @param socket Socket. Must be multicast.
#' @param option.value Recovery interval in milliseconds.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_SUB")
#' set.recovery.ivl(socket, 10000L)
set.recovery.ivl <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RECOVERY_IVL", option.value, PACKAGE="rzmq")
}

#' Set the kernel transmit buffer size.
#' See ZMQ socket option `ZMQ_SNDBUF`.
#'
#' @param socket Socket.
#' @param option.value Buffer size in bytes.
#' -1 specifies the OS default should be used.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.sndbuf(socket, -1L)
set.sndbuf <- function(socket, option.value) {
    .Call("setSockOptInt",socket, "ZMQ_SNDBUF", option.value, PACKAGE="rzmq")
}

#' Set the kernel receive buffer size.
#' See ZMQ socket option `ZMQ_RCVBUF`.
#'
#' @param socket Socket.
#' @param option.value Buffer size in bytes.
#' -1 specifies the OS default should be used.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' set.rcvbuf(socket, -1L)
set.rcvbuf <- function(socket, option.value) {
    .Call("setSockOptInt",socket, "ZMQ_RCVBUF", option.value, PACKAGE="rzmq")
}

#' Set the linger period.
#' See ZMQ socket option `ZMQ_LINGER`.
#'
#' @param socket Socket.
#' @param option.value Linger period in milliseconds.
#' -1 specifies in infinite period.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.linger(socket, 1L)
set.linger <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_LINGER", option.value, PACKAGE="rzmq")
}

#' Set the reconnection interval.
#' See ZMQ socket option `ZMQ_RECONNECT_IVL`.
#'
#' @param socket Sokcet.
#' @param option.value Reconnection interval in milliseconds.
#' -1 specifies no reconnection.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' set.reconnect.ivl(socket, 100L)
set.reconnect.ivl <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RECONNECT_IVL", option.value, PACKAGE="rzmq")
}

#' Set maximum length of the queue of outstanding connections
#' See ZMQ socket option `ZMQ_BACKLOG`.
#'
#' @param socket Socket. Must be connection-oriented.
#' @param option.value Number of connections.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.zmq.backlog(socket, 100L)
set.zmq.backlog <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_BACKLOG", option.value, PACKAGE="rzmq")
}

#' Set maximum reconnection interval.
#' See ZMQ socket option `ZMQ_RECONNECT_IVL_MAX`.
#'
#' @param socket Socket. Must be connection-oriented.
#' @param option.value Maximum reconnect interval in milliseconds.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.reconnect.ivl.max(socket, 1000L)
set.reconnect.ivl.max <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RECONNECT_IVL_MAX", option.value, PACKAGE="rzmq")
}

#' Gets whether the receive more flag is set on the socket.
#' See ZMQ socket option `ZMQ_RCVMORE`.
#'
#' @param socket Socket.
#'
#' @return Value of the receive more flag.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' waiting <- get.rcvmore(socket)
get.rcvmore <- function(socket) {
    .Call("get_rcvmore", socket, PACKAGE="rzmq")
}

#' Retrieve the last endpoint bound for TCP and IPC transports
#' See ZMQ socket option `ZMQ_LAST_ENDPOINT`.
#'
#' @param socket Socket.
#'
#' @return Last endpoint address.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' connect.socket(socket, "tcp://*:8080")
#' get.last.endpoint(socket)
get.last.endpoint <- function(socket) {
    .Call("get_last_endpoint", socket, PACKAGE="rzmq")
}

#' Sets the timeout for send operations.
#' See ZMQ socket option `ZMQ_SNDTIMEO`.
#'
#' @param socket Socket.
#' @param option.value Timeout in milliseconds.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.send.timeout(socket, 1000L)
set.send.timeout <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_SNDTIMEO", option.value, PACKAGE="rzmq")
}

#' Sets the timeout for receive operations.
#' See ZMQ socket option `ZMQ_RCVTIMEO`.
#'
#' @param socket Socket.
#' @param option.value Timeout in milliseconds.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' set.rcv.timeout(socket, 1000L)
set.rcv.timeout <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RCVTIMEO", option.value, PACKAGE="rzmq")
}

#' Retrieves the send timeout value.
#' See ZMQ socket option `ZMQ_SNDTIMEO`.
#'
#' @param socket Socket.
#'
#' @return Send timeout in milliseconds.
#' -1 indicates an infinite timeout.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' get.send.timeout(socket)
get.send.timeout <- function(socket) {
    .Call("get_sndtimeo", socket, PACKAGE="rzmq")
}

#' Retrieves the receive timeout value.
#' See ZMQ socket option `ZMQ_RCVTIMEO`.
#'
#' @param socket Socket.
#'
#' @return Receive timeout in milliseconds.
#' -1 indicates an infinite timeout.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' get.rcv.timeout(socket)
get.rcv.timeout <- function(socket) {
  .Call("get_rcvtimeo", socket, PACKAGE="rzmq")
}

#' Set high water mark for outbound messages.
#' See ZMQ socket option `ZMQ_SNDHWM`.
#'
#' @param socket Socket.
#' @param option.value Maximum queued messages.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REP")
#' set.send.hwm(socket, 100L)
set.send.hwm <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_SNDHWM", option.value, PACKAGE="rzmq")
}

#' Set high water mark for inbound messages.
#' See ZMQ socket option `ZMQ_RCVHWM`.
#'
#' @param socket Socket.
#' @param option.value Maximum queued messages.
#'
#' @return `TRUE` if the option was set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' set.rcv.hwm(socket, 100L)
set.rcv.hwm <- function(socket, option.value) {
    .Call("setSockOptInt", socket, "ZMQ_RCVHWM", option.value, PACKAGE="rzmq")
}

#' Sets the TCP keep alive option.
#'
#' @param socket Socket.
#' @param active Whether to activate (1L), inactivate (0L) or use OS default (-1L).
#' See ZMQ socket option `ZMQ_TCP_KEEPALIVE`.
#' @param idle Idle time until keep-alive is sent (use OS default: -1L).
#' See ZMQ socket option `ZMQ_TCP_KEEPALIVE_IDLE`.
#' @param count Number of probes missed to drop connection (use OS default: -1L).
#' See ZMQ socket option `ZMQ_TCP_KEEPALIVE_CNT`.
#' @param interval Time between keepalive probes (use OS default: -1L).
#' See ZMQ socket option `ZMQ_TCP_KEEPALIVE_INTVL`
#'
#' @return `TRUE` if the options were set successfully, `FALSE` otherwise.
#' @export
#'
#' @examples
#' context <- init.context()
#' socket <- init.socket(context, "ZMQ_REQ")
#' set.tcp.keepalive(socket)
set.tcp.keepalive <- function(socket, active=-1L, idle=-1L, count=-1L, interval=-1L) {
    .Call("setSockOptInt", socket, "ZMQ_TCP_KEEPALIVE", active, PACKAGE="rzmq")
    .Call("setSockOptInt", socket, "ZMQ_TCP_KEEPALIVE_IDLE", idle, PACKAGE="rzmq")
    .Call("setSockOptInt", socket, "ZMQ_TCP_KEEPALIVE_CNT", count, PACKAGE="rzmq")
    .Call("setSockOptInt", socket, "ZMQ_TCP_KEEPALIVE_INTVL", interval, PACKAGE="rzmq")
}
