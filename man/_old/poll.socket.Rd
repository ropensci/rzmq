\name{poll.socket}
\alias{poll.socket}
\title{Polls a list of sockets, waiting for the presence of a nonblocking read, write, or error event.}
\description{The zmq_poll() function shall poll a list of a sockets for either read, write, or error conditions subject to a millisecond resolution timeout.}
\usage{
    poll.socket(sockets, events, timeout=0L)
}

\arguments{
    \item{sockets}{a list of zmq socket objects.}
    \item{events}{a list of character vectors containing one or more events in \{read, write, error\}. The first element in the list corresponds to the first zmq socket, and so on...}
    \item{timeout}{the numbers of seconds to wait for events. Fractional seconds are supported. ZeroMQ guarantees at most millisecond resolution. A timeout of -1L blocks until an event occurs; a timeout of 0L is non-blocking.}
}
\value{A list of pairlists corresponding to the polled zmq sockets. Each list has one of more tags from \{read, write, error\} with logical values indicating the results of the poll operation.}

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

# Create a set of REP-REQ sockets that
# have a Send, Receive, Send, Receive, ...
# pattern.
context = init.context()
in.socket = init.socket(context,"ZMQ_REP")
bind.socket(in.socket,"tcp://*:5557")

out.socket = init.socket(context,"ZMQ_REQ")
connect.socket(out.socket,"tcp://*:5557")

# Poll the REP and REQ sockets for all events.
events <- poll.socket(list(in.socket, out.socket),
                      list(c("read", "write", "error"),
                           c("read", "write", "error")),
                      timeout=0L)

# The REQ socket is writable without blocking.
paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)

# Send a message to the REP socket from the REQ socket. The
# REQ socket must respond before the REP socket can send
# another message.
send.socket(out.socket, "Hello World")

events <- poll.socket(list(in.socket, out.socket),
                      list(c("read", "write", "error"),
                           c("read", "write", "error")),
                      timeout=0L)

# The incoming message is readable on the REP socket.
paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)

receive.socket(in.socket)

events <- poll.socket(list(in.socket, out.socket),
                      list(c("read", "write", "error"),
                           c("read", "write", "error")),
                      timeout=0L)

# The REQ socket is waiting for a response from the REP socket. 
paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)

send.socket(in.socket, "Greetings")

events <- poll.socket(list(in.socket, out.socket),
                      list(c("read", "write", "error"),
                           c("read", "write", "error")),
                      timeout=0L)

# The REP response is waiting to be read on the REQ socket.
paste("Is upstream REP socket readable without blocking?", events[[1]]$read)
paste("Is upstream REP socket writable without blocking?", events[[1]]$write)
paste("Is downstream REQ socket readable without blocking?", events[[2]]$read)
paste("Is downstream REQ socket writable without blocking?", events[[2]]$write)

# Complete the REP-REQ transaction cycle by reading
# the REP response.
receive.socket(out.socket)
}}
\keyword{utilities}
