How to rebuild the win folder
=============================

This assumes that you have the latest rtools installed in `c:\rtools` and that you have rtools gcc in path!.

* download and unpack zeromq windows sources: http://download.zeromq.org/zeromq-4.0.5.zip 
* open `cmd` and change into the zeromq sources: in my case it's in `C:\rtools\zeromq-4.0.5/src`
* execute `g++ -m32 -std=c++0x -DZMQ_STATIC -DFD_SETSIZE24 -D_REENTRANT -D_THREAD_SAFE  -DZMQ_FORCE_SELECT -DZMQ_HAVE_WINDOWS -c *.cpp` and `ar r libzmq.a *.o` (`-DZMQ_STATIC -DFD_SETSIZE24` is from http://grokbase.com/t/zeromq/zeromq-dev/136sg6zk4s/build-zmq-as-static-library-under-mingw, the rest of the `-D...` from the configure script under MinGW; `-std=c++0x` comes from the default g++ invocation of the R build)
* copy the `libzmq.a` file to `win\i386`
* delete all `*.o` and `libzmq.la` in `zeromq-4.0.5/src`
* execute `g++ -m64 -std=c++0x -DZMQ_STATIC -DFD_SETSIZE24 -D_REENTRANT -D_THREAD_SAFE  -DZMQ_FORCE_SELECT -DZMQ_HAVE_WINDOWS -c *.cpp` and `ar r libzmq.a *.o`
* copy the `libzmq.a` file to `win\x86`
* copy `zmq.h` to `win`

How to build the binary packages under Windows:
===========================================

With R and rtools in path and a checkout of the rzmq as asubfolder of the current directory:
* `R CMD build rzmq` # builds the source package `rzmq_0.7.7.tar.gz `
* `R CMD check rzmq_0.7.7.tar.gz` # checks the source package
* `R CMD INSTALL --build --compile-both rzmq` -> builds the windows binary *.zip package


