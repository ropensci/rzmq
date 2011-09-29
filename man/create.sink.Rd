\name{create.sink}
\alias{create.sink}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
create.sink(address, num_items)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{address}{
%%     ~~Describe \code{address} here~~
}
  \item{num_items}{
%%     ~~Describe \code{num_items} here~~
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
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
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function (address, num_items) 
{
    .Call("createSink", address, as.integer(num_items), PACKAGE = "rzmq")
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{utilities}
