\name{zmq.version}
\alias{zmq.version}
\title{
  get version of libzmq
}
\description{
  return the version string of the system zmq library
}
\usage{
zmq.version(x=NULL)
}

\arguments{
  \item{x}{The desired level in the version hierarchy.}
}
  
\value{
  If x == 'major' the return value is an integer representing the ZeroMQ
  major version.
  If x == 'minor' the return value is an integer representing the ZeroMQ
  minor version.
  If x == 'patch' the return value is an integer representing the ZeroMQ
  patch.
  If x is NULL the return value is a string of the following format:
  major.minor.patch. This is the default.
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
