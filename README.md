Introduction
=======

* **Date:** Sept 27, 2011
* **Authors:** Whit Armstrong
* **Contact:** armstrong.whit@gmail.com
* **Web site:** http://github.com/armstrtw/rzmq
* **License:** GPL-3
* **Status:** [![Build Status](https://travis-ci.org/snoweye/rzmq.png)](https://travis-ci.org/snoweye/rzmq)


Purpose
=======

rzmq is an R binding for ZMQ (http://www.zeromq.org/).


Features
========

rzmq is a message queue for serialized R objects.

* rzmq implements most the standard socket pairs that ZMQ offers.

* ZMQ devices are not implemented yet, nor is zmq_poll.

* Look for more features shortly.


Usage
=====

A minimal example of remote execution.

execute this R script on the remote server::
	
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
	
and execute this bit locally::

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
	
