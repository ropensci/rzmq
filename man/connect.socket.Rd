\name{connect.socket}
\alias{connect.socket}
\title{
Connect the socket referenced by the socket argument to the endpoint specified by the endpoint argument.
}
\description{
  The zmq_connect() function shall connect the socket referenced by the socket argument to the endpoint specified by the endpoint argument.

The endpoint argument is a string consisting of two parts as follows: transport ://address. The transport part specifies the underlying transport protocol to use. The meaning of the address part is specific to the underlying transport protocol selected.

The following transports are defined:

inproc
local in-process (inter-thread) communication transport, see zmq_inproc(7)
ipc
local inter-process communication transport, see zmq_ipc(7)
tcp
unicast transport using TCP, see zmq_tcp(7)
pgm, epgm
reliable multicast transport using PGM, see zmq_pgm(7)
With the exception of ZMQ_PAIR sockets, a single socket may be connected to multiple endpoints using zmq_connect(), while simultaneously accepting incoming connections from multiple endpoints bound to the socket using zmq_bind(). Refer to zmq_socket(3) for a description of the exact semantics involved when connecting or binding a socket to multiple endpoints.
}
\usage{
connect.socket(socket, address)
}

\arguments{
  \item{socket}{a zmq socket object.}
  \item{address}{a transport as described above.}
}
\value{TRUE if operation succeeds or FALSE if the operation fails}
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
in.socket = init.socket(context,"ZMQ_PULL")
bind.socket(in.socket,"tcp://*:5557")

out.socket = init.socket(context,"ZMQ_PUSH")
bind.socket(out.socket,"tcp://*:5558")
}}
\keyword{utilities}
