\name{tcp.keepalive}
\alias{set.tcp.keepalive}

\title{
  set TCP keepalive socket options.
}
\description{

The zmq_setsockopt() function shall set the option specified by the
option_name argument to the value pointed to by the option_value
argument for the ZMQ socket pointed to by the socket argument.
}
\usage{
set.tcp.keepalive(socket, active=-1L, idle=-1L, count=-1L, interval=-1L)

}

\arguments{
  \item{socket}{a zmq socket object}
  \item{active}{whether to activate (1L), inactivate (0L) or use OS default (-1L)}
  \item{idle}{idle time until keep-alive is sent (use OS default: -1L)}
  \item{count}{number of probes missed to drop connection (use OS default: -1L)}
  \item{interval}{time between keepalive probes (use OS default: -1L)}
}
\value{
  a boolean indicating success or failure of the operation or in the
  case of getsocketoptions, the value of the requsted option.
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

\keyword{utilities}
