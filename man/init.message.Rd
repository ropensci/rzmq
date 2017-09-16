\name{init.message}
\alias{init.message}
\title{
  create a message object.
}
\description{
  Create a ZeroMQ message object that can be sent multiple times
}
\usage{
init.message(data, serialize=TRUE, xdr=.Platform$endian=="big")
}

\arguments{
  \item{data}{the R object to be sent}
  \item{serialize}{whether to call serialize before sending the data}
  \item{xdr}{passed directly to serialize command if serialize is requested}
}
\value{
  a ZeroMQ message object as external pointer
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
  \code{\link{send.message.object}}
}
\examples{\dontrun{

## remote execution server in rzmq
library(rzmq)
data = list(x=5)
msg = init.message(data)
}}
\keyword{utilities}
