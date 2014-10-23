\name{send.multipart}
\alias{send.multipart}
\title{Send multipart ZMQ message.}
\usage{
send.multipart(socket, parts)
}
\arguments{
  \item{socket}{The ZMQ socket on which to send data}

  \item{parts}{A list of raw vectors; each component will be sent
  as one part of the message, in the order of the list}
}
\description{
  Queue a list of raw vectors to be sent as a series of ZMQ message parts. Each
  part before the last will be sent with the SNDMORE flag.
}
