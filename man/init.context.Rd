\name{init.context}
\alias{init.context}
\alias{init.socket}
\title{
  initailize zmq context and zmq socket
}
\description{
  initailize zmq context and zmq socket for to be used for further zmq operations.
}
\usage{
init.context()
init.socket(context, socket.type)
}

\arguments{
  \item{context}{returns a zmq context object.}
  \item{socket.type}{ The ZMQ socket type requested
    e.g. ZMQ_REQ,ZMQ_REP,ZMQ_PULL,ZMQ_PUSH, etc.}
}
\value{
  \code{init.context}{ returns a zmq context object.}
  \code{init.socket}{returns a zmq socket object.}
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
in.socket = init.socket(context,"ZMQ_PULL")
}}
\keyword{utilities}
