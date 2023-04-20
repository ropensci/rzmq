\name{zmq.version}
\alias{zmq.version}
\title{
  get version of libzmq
}
\description{
  return the version string of the system zmq library
}
\usage{
zmq.version()
}

\value{
  a string of the following format: major.minor.patch
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
zmq.version()
}}
\keyword{utilities}
