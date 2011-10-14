\name{zmq.lapply}
\alias{zmq.cluster.lapply}
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
zmq.cluster.lapply(cluster,X,FUN,...,push.port=6000,pull.port=6001,control.port=6003)

}
\arguments{
  \item{cluster}{a vector of computer names to use for the parallel lapply}
  \item{X}{a list or vector.}
  \item{FUN}{the function to be applied to each element of \code{X}}
  \item{push.port}{the port to use to send jobs.}
  \item{pull.port}{the port to use to pick up results.}
  \item{control.port}{the port to use to check whether the cluster
    memeber is ready to receive jobs.}
  \item{\dots}{additional arguments to lapply.}
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
library(getopt)

worker.id <- paste(Sys.info()["nodename"],Sys.getpid(),sep=":")

spec <- rbind(c('control','c', 1, "character"),
              c('in','i', 1, "character"),
              c('out','o', 1, "character"),
              c('log-file','l', 1, "character"))

opts <- getopt(spec)
control.endpoint <- opts[["control"]]
in.endpoint <- opts[["in"]]
out.endpoint <- opts[["out"]]
log.file <- opts[["log-file"]]

sink(log.file)
print(opts)

context = init.context()

in.socket = init.socket(context,"ZMQ_PULL")
connect.socket(in.socket,in.endpoint)

out.socket = init.socket(context,"ZMQ_PUSH")
connect.socket(out.socket,out.endpoint)

## send control message to indicate worker is up
control.socket = init.socket(context,"ZMQ_PUSH")
connect.socket(control.socket,control.endpoint)
send.null.msg(control.socket)

while(1) {
    msg = receive.socket(in.socket);
    index <- msg$index
    fun <- msg$fun
    args <- msg$args
    print(args)
    result <- try(do.call(fun,args),silent=TRUE)
    send.socket(out.socket,list(index=index,result=result,node=worker.id));
}

## run locally
## estimatePi example courtesy of JD Long / Jeffrey Breen

library(AWS.tools)
library(rzmq)

cl <- startCluster(ami="ami-4bc90522",key="kls-ec2",instance.count=5,instance.type="c1.xlarge")

estimatePi <- function(seed) {
    set.seed(seed)
    numDraws <- 1e6

    r <- .5
    x <- runif(numDraws, min=-r, max=r)
    y <- runif(numDraws, min=-r, max=r)
    inCircle <- ifelse( (x^2 + y^2)^.5 < r , 1, 0)
    
    sum(inCircle) / length(inCircle) * 4
}

run.time <- system.time(ans <- zmq.cluster.lapply(cluster=cl$instances[,"PublicDNS"],as.list(1:1000),estimatePi))

print(mean(unlist(ans)))
print(run.time)
print(attr(ans,"execution.report"))

res <- terminateCluster(cl)

pi.est <- mean(unlist(ans))
print(pi.est)

}}
\keyword{utilities}
