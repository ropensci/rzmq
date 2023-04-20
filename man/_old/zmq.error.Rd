\name{zmq.error}
\alias{zmq.errno}
\alias{zmq.strerror}
\title{
  get libzmq error numbers and error strings
}
\description{
  return the error number or error description after a zmq call
}
\usage{
zmq.errno()
zmq.strerror()
}

\value{
  an integer for zmq.errno or a string for zmq.strerror
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
  \code{\link{connect.socket},\link{bind.socket},\link{receive.socket},\link{send.socket}}
}

\examples{\dontrun{

library(rzmq)
zmq.errno()
zmq.strerror()
}}
\keyword{utilities}
