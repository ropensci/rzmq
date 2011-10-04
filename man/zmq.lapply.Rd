\name{zmq.lapply}
\alias{zmq.lapply}
\title{
  apply a function to a list.
}
\description{
  \code{zmq.lapply} returns a list of the same length as \code{X}, each element of
  which is the result of applying \code{FUN} to the corresponding element
  of \code{X}.

  The execution of the function is performed in parallel over zmq sockets.
}
\usage{
zmq.lapply(X, FUN, execution.server, sink.server)
}
\arguments{
  \item{X}{a list or vector.}
  \item{FUN}{the function to be applied to each element of \code{X}}
  \item{execution.server}{the address and port of the remote execution
    server/broker.}
  \item{sink.server}{the address and port of the remote job output
    server/broker.}
}
\details{
  Successful "parallelization" of lapply requires a broker device to run
  on the remote server to fanout the jobs to multiple machines.

  The example below assumes a remote execution server is running on one
  machine, hence the job will not run in parallel.

  See the ZMQ ventilator example for more details (http://zguide.zeromq.org/page:all#Divide-and-Conquer).
}
\value{a list of the same length as X}
\references{
  http://www.zeromq.org
  http://api.zeromq.org
  http://zguide.zeromq.org/page:all
  http://zguide.zeromq.org/page:all#Divide-and-Conquer
  https://gist.github.com/406824
  http://www.vcasmo.com/video/drewconway/8468
  http://www.r-bloggers.com/abusing-amazon%E2%80%99s-elastic-mapreduce-hadoop-service%E2%80%A6-easily-from-r
  http://code.google.com/p/segue
}
\author{
  ZMQ was written by Martin Sustrik <sustrik@250bpm.com> and Martin Lucina <mato@kotelna.sk>.
  rzmq was written by Whit Armstrong.
  segue and estimatePi were written by JD Long.
}

\seealso{
  \code{\link{connect.socket},\link{bind.socket},\link{receive.socket},\link{send.socket}}
}

\examples{\dontrun{
library(rzmq)

## run on remote machine:
#!/usr/bin/env Rscript

library(rzmq)
context = init.context()
in.socket = init.socket(context,"ZMQ_PULL")
bind.socket(in.socket,"tcp://*:5557")

out.socket = init.socket(context,"ZMQ_PUSH")
bind.socket(out.socket,"tcp://*:5558")

while(1) {
    msg = receive.socket(in.socket);
    index <- msg$index
    fun <- msg$fun
    args <- msg$args
    print(args)
    result <- do.call(fun,args)
    send.socket(out.socket,list(index=index,result=result));
}

## run locally
## estimatePi example courtesy of JD Long / Jeffrey Breen
library(rzmq)

estimatePi <- function(seed) {
    set.seed(seed)
    numDraws <- 1e5

    r <- .5
    x <- runif(numDraws, min=-r, max=r)
    y <- runif(numDraws, min=-r, max=r)
    inCircle <- ifelse( (x^2 + y^2)^.5 < r , 1, 0)
    
    sum(inCircle) / length(inCircle) * 4
}

print(system.time(ans <- zmq.lapply(as.list(1:1e2),
                                    estimatePi,
                                    execution.server="tcp://ec2-184-73-102-95.compute-1.amazonaws.com:5557",
                                    sink.server="tcp://ec2-184-73-102-95.compute-1.amazonaws.com:5558")))

print(mean(unlist(ans)))
}}
\keyword{utilities}
