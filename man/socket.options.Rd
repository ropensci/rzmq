\name{socket.options}
\alias{set.hwm}
\alias{set.swap}
\alias{set.affinity}
\alias{set.identity}
\alias{subscribe}
\alias{unsubscribe}
\alias{set.rate}
\alias{set.recovery.ivl}
\alias{set.recovery.ivl.msec}
\alias{set.mcast.loop}
\alias{set.sndbuf}
\alias{set.rcvbuf}
\alias{set.linger}
\alias{set.reconnect.ivl}
\alias{set.zmq.backlog}
\alias{set.reconnect.ivl.max}
\alias{get.rcvmore}
\alias{get.send.timeout}
\alias{set.send.timeout}

\title{
  set a socket option.
}
\description{

The zmq_setsockopt() function shall set the option specified by the
option_name argument to the value pointed to by the option_value
argument for the ZMQ socket pointed to by the socket argument.
}
\usage{
set.hwm(socket, option.value)
set.swap(socket, option.value)
set.affinity(socket, option.value)
set.identity(socket, option.value)
subscribe(socket, option.value)
unsubscribe(socket, option.value)
set.rate(socket, option.value)
set.recovery.ivl(socket, option.value)
set.recovery.ivl.msec(socket, option.value)
set.mcast.loop(socket, option.value)
set.sndbuf(socket, option.value)
set.rcvbuf(socket, option.value)
set.linger(socket, option.value)
set.reconnect.ivl(socket, option.value)
set.zmq.backlog(socket, option.value)
set.reconnect.ivl.max(socket, option.value)
get.rcvmore(socket)
get.send.timeout(socket)
set.send.timeout(socket, option.value)

}

\arguments{
  \item{socket}{a zmq socket object}
  \item{option.value}{the new option value to bet set}
}
\value{
  a boolean indicating success or failure of the operation or in the
  case of getsocketoptions, the value of the requsted option.
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
context = init.context()
socket = init.socket(context,"ZMQ_REQ")

set.hwm(socket, 1L)
set.swap(socket, 100L)
set.identity(socket, "big.ass.socket")
}}
\keyword{utilities}
