\name{get.curve.server}
\alias{get.curve.server}
\title{Test whether a socket is a Curve server.}
\description{Test whether a socket is a Curve server.}
\usage{
get.curve.server(socket)
}
\arguments{
  \item{socket}{a zmq socket object.}
}

\value{1 if the socket is a Curve server; 0 otherwise.}

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
