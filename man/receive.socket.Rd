\name{receive.socket}
\alias{receive.socket}
\alias{receive.null.msg}
\alias{receive.string}
\alias{receive.int}
\alias{receive.double}
\title{
Receive a message from the socket referenced by the socket argument .
}
\description{
The zmq_recv() function shall receive a message from the socket
referenced by the socket argument. If there are no messages available
on the specified socket, by default the function shall block until the request
can be satisfied.
A non-blocking receive can be obtained by passing the value ZMQ_DONTWAIT
to the flags parameter. The following non-default flags are allowed:

ZMQ_DONTWAIT(integer value 1):

Specifies that the operation should be performed in
           non-blocking mode. If there are no messages available on the
           specified socket, the receive.socket() call will return -1
           immediately.

}
\usage{
receive.socket(socket, unserialize=TRUE, flags=0)
receive.null.msg(socket)
receive.string(socket)
receive.int(socket)
receive.double(socket)
}

\arguments{
\item{socket}{a zmq socket object}
\item{unserialize}{whether to call unserialize on the received data}
\item{flags}{defaults to 0, for blocking receive. Set to ZMQ_DONTWAIT for non-blocking receive.}
}
\value{
  the value sent from the remote server or NULL on failure.
  or a logical for receive.null.msg indicating whether the msg was zero
  size. If flags was ZMQ_DONTWAIT and a message was not immediately
  available for receipt, the value returned is -1.
}
\references{
http://www.zeromq.org
http://api.zeromq.org
http://zguide.zeromq.org/page:all
}
\author{
ZMQ was written by Martin Sustrik <sustrik@250bpm.com> and Martin Lucina <mato@kotelna.sk>.
rzmq was written by Whit Armstrong.
}
\seealso{
  \code{\link{connect.socket},\link{bind.socket},\link{receive.socket},\link{send.socket},\link{poll.socket}}
}
\examples{\dontrun{
library(rzmq)

remote.exec <- function(out.socket,in.socket,fun,...) {
    send.socket(out.socket,data=list(fun=fun,args=list(...)))
    receive.socket(in.socket)
}

context = init.context()
out.socket = init.socket(context,"ZMQ_PUSH")
bind.socket(out.socket,"tcp://*:5557")

in.socket = init.socket(context,"ZMQ_PULL")
bind.socket(in.socket,"tcp://*:5558")


myfun <- function(x) {
    sum(abs(x))
}

remote.exec(out.socket,in.socket,myfun,rnorm(1e3))

}}

\keyword{utilities}
