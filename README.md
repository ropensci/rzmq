## rzmq 
[![Build Status](https://travis-ci.org/armstrtw/rzmq.svg)](https://travis-ci.org/armstrtw/rzmq)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/armstrtw/rzmq?branch=master&svg=true)](https://ci.appveyor.com/project/armstrtw/rzmq)
[![Package-License](http://img.shields.io/badge/license-GPL--3-brightgreen.svg?style=flat)](http://www.gnu.org/licenses/gpl-3.0.html) [![CRAN](http://www.r-pkg.org/badges/version/rzmq)](https://cran.r-project.org/package=rzmq) [![Downloads](http://cranlogs.r-pkg.org/badges/rzmq?color=brightgreen)](http://www.r-pkg.org/pkg/rzmq)

### Purpose

rzmq is an R binding for [ZMQ](http://www.zeromq.org/).

### Features
rzmq is a message queue for serialized R objects.
* rzmq implements most the standard socket pairs that ZMQ offers.
* ZMQ devices are not implemented yet, nor is zmq_poll.
* Look for more features shortly.

### Usage
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
