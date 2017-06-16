\name{set.server.key}
\alias{set.server.key}
\title{Get the server key associated with a client socket.}
\description{Get the server key associated with a client socket.}
\usage{
set.server.key(socket,option.value)
}
\arguments{
  \item{socket}{a zmq socket object.}
  \item{option.value}{A string representing the server key to associate witht
    the socket.}
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
