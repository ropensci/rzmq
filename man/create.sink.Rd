\name{create.sink}
\alias{create.sink}
\alias{get.sink.results}
\title{
  create a sink object.
}
\description{
  Create a sink object in a separate thread which acts as an async
  reciever for remotely executed jobs.

  The job results can be later retreived via get.sink.results.
}
\usage{
create.sink(address, num_items)
get.sink.results(sink)
}
\arguments{
  \item{address}{the transport endpoint for the remote sink server.}
  \item{num_items}{the number of items to indicate the remote job has
    completed.}
  \item{sink}{the sink object.}
}
\value{
  create.sink returns a sink object
  get.sink.results returns a list of the remotely executed jobs
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
## an example in the spirit of lapply
x <- as.list(1:100)
context = init.context()
execution.socket = init.socket(context,"ZMQ_PUSH")
connect.socket(execution.socket,"tcp://my.remote.server:5557")

## listen for results
N <- length(x)
sink <- create.sink("tcp://my.results.server:5558",N)
for(i in 1:N) {
remote.exec(execution.socket,FUN,x[[i]])
}
ans <- get.sink.results(sink)
lapply(ans,unserialize)
}}

\keyword{utilities}
