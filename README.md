# rzmq

> R Bindings for 'ZeroMQ'

[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](http://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/ropensci/rzmq?branch=master)](https://ci.appveyor.com/project/jeroen/rzmq)
[![Package-License](http://img.shields.io/badge/license-GPL--3-brightgreen.svg?style=flat)](http://www.gnu.org/licenses/gpl-3.0.html) [![CRAN](http://www.r-pkg.org/badges/version/rzmq)](https://cran.r-project.org/package=rzmq) [![Downloads](http://cranlogs.r-pkg.org/badges/rzmq?color=brightgreen)](http://www.r-pkg.org/pkg/rzmq)

Interface to the 'ZeroMQ' lightweight messaging kernel (see <http://www.zeromq.org/> for more information).


## Features

rzmq is a message queue for serialized R objects.
* rzmq implements most the standard socket pairs that ZMQ offers.
* ZMQ devices are not implemented yet, nor is zmq_poll.
* Look for more features shortly.

## Installation

Binary packages for __OS-X__ or __Windows__ can be installed directly from CRAN:

```r
install.packages("rzmq")
```

## Build from source

Installation from source requires [`ZeroMQ`](http://zeromq.org/area:download). On __Debian__ or __Ubuntu__ use [libzmq3-dev](https://packages.debian.org/testing/libzmq3-dev):

```
sudo apt-get install -y libzmq3-dev
```

On __Fedora__ we need [zeromq-devel](https://apps.fedoraproject.org/packages/zeromq-devel):

```
sudo yum install zeromq-devel
````

On __CentOS / RHEL__ we install [zeromq3-devel](https://apps.fedoraproject.org/packages/zeromq3-devel) via EPEL:

```
sudo yum install epel-release
sudo yum install zeromq3-devel
```

On __OS-X__ use [zeromq](https://github.com/Homebrew/homebrew-core/blob/master/Formula/zeromq.rb) from Homebrew:

```
brew install zeromq
```


## Usage

A minimal example of remote execution.

execute this R script on the remote server:
```{.r}
#!/usr/bin/env Rscript
library(rzmq)
context = init.context()
socket = init.socket(context,"ZMQ_REP")
bind.socket(socket,"tcp://*:5555")
while(1) {
    msg = receive.socket(socket);
    fun <- msg$fun
    args <- msg$args
    print(args)
    ans <- do.call(fun,args)
    send.socket(socket,ans);
}
```	

and execute this bit locally:
```{.r}
library(rzmq)

remote.exec <- function(socket,fun,...) {
    send.socket(socket,data=list(fun=fun,args=list(...)))
    receive.socket(socket)
}

substitute(expr)
context = init.context()
socket = init.socket(context,"ZMQ_REQ")
connect.socket(socket,"tcp://localhost:5555")

ans <- remote.exec(socket,sqrt,10000)
```
