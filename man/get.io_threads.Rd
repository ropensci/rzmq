\name{get.io_threads}
\alias{get.io_threads}
\title{Get the number of io_threads used by a socket.}
\description{Get the number of io_threads used by a socket.}
\usage{
get.io_threads(context)
}
\arguments{
  \item{context}{a zmq context object.}
}

\value{An integer value representing the number of io_threads used by a socket.}

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
