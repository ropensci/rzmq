###########################################################################
## Copyright (C) 2011  Whit Armstrong                                    ##
##                                                                       ##
## This program is free software: you can redistribute it and#or modify  ##
## it under the terms of the GNU General Public License as published by  ##
## the Free Software Foundation, either version 3 of the License, or     ##
## (at your option) any later version.                                   ##
##                                                                       ##
## This program is distributed in the hope that it will be useful,       ##
## but WITHOUT ANY WARRANTY; without even the implied warranty of        ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         ##
## GNU General Public License for more details.                          ##
##                                                                       ##
## You should have received a copy of the GNU General Public License     ##
## along with this program.  If not, see <http:##www.gnu.org#licenses#>. ##
###########################################################################

zmq.errno <- function() {
    .Call("get_zmq_errno", PACKAGE="rzmq2")
}

zmq.strerror <- function() {
    .Call("get_zmq_strerror", PACKAGE="rzmq2")
}

init.socket <- function(context, socket.type) {
    .Call("initSocket", context, socket.type, PACKAGE="rzmq2")
}

bind.socket <- function(socket, address) {
    invisible(.Call("bindSocket", socket, address, PACKAGE="rzmq2"))
}

connect.socket <- function(socket, address) {
    invisible(.Call("connectSocket", socket, address, PACKAGE="rzmq2"))
}

disconnect.socket <- function(socket, address) {
    invisible(.Call("disconnectSocket", socket, address, PACKAGE="rzmq2"))
}

send.socket <- function(socket, data, send.more=FALSE, serialize=TRUE,
                        xdr=.Platform$endian=="big") {
    if(serialize) {
        data <- serialize(data, NULL, xdr=xdr)
    }

    invisible(.Call("sendSocket", socket, data, send.more, PACKAGE="rzmq2"))
}

send.null.msg <- function(socket, send.more=FALSE) {
    .Call("sendNullMsg", socket, send.more, PACKAGE="rzmq2")
}

receive.null.msg <- function(socket) {
    .Call("receiveNullMsg", socket, PACKAGE="rzmq2")
}

receive.socket <- function(socket, unserialize=TRUE,dont.wait=FALSE) {
    ans <- .Call("receiveSocket", socket, dont.wait, PACKAGE="rzmq2")

    if(!is.null(ans) && unserialize) {
        ans <- unserialize(ans)
    }
    ans
}

receive.multipart <- function(socket) {
  parts = list(receive.socket(socket, unserialize=FALSE))
  while(get.rcvmore(socket)) {
    parts = append(parts, list(receive.socket(socket, unserialize=FALSE)))
  }
  return(parts)
}

send.multipart <- function(socket, parts) {
  for (part in parts[1:(length(parts)-1)]) {
    send.socket(socket, part, send.more=TRUE, serialize=FALSE)
  }
  send.socket(socket, parts[[length(parts)]], send.more=FALSE, serialize=FALSE)
}

send.raw.string <- function(socket,data,send.more=FALSE) {
    .Call("sendRawString", socket, data, send.more, PACKAGE="rzmq2")
}

receive.string <- function(socket) {
    .Call("receiveString", socket, PACKAGE="rzmq2")
}

receive.int <- function(socket) {
    .Call("receiveInt", socket, PACKAGE="rzmq2")
}

receive.double <- function(socket) {
    .Call("receiveDouble", socket, PACKAGE="rzmq2")
}

poll.socket <- function(sockets, events, timeout=0L) {
    if (timeout != -1L) timeout <- as.integer(timeout * 1000000)
    .Call("pollSocket", sockets, events, timeout)
}

set.hwm <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_HWM removed from libzmq3")
    } else {
        .Call("set_hwm",socket, option.value, PACKAGE="rzmq2")
    }
}

set.swap <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_SWAP removed from libzmq3")
    } else {
        .Call("set_swap",socket, option.value, PACKAGE="rzmq2")
    }
}

set.affinity <- function(socket, option.value) {
    .Call("set_affinity",socket, option.value, PACKAGE="rzmq2")
}

set.identity <- function(socket, option.value) {
    .Call("set_identity",socket, option.value, PACKAGE="rzmq2")
}

subscribe <- function(socket, option.value) {
    invisible(.Call("subscribe",socket, option.value, PACKAGE="rzmq2"))
}

unsubscribe <- function(socket, option.value) {
    .Call("unsubscribe",socket, option.value, PACKAGE="rzmq2")
}

set.rate <- function(socket, option.value) {
    .Call("set_rate",socket, option.value, PACKAGE="rzmq2")
}

set.recovery.ivl <- function(socket, option.value) {
    .Call("set_recovery_ivl",socket, option.value, PACKAGE="rzmq2")
}

set.recovery.ivl.msec <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_RECOVERY_IVL_MSEC removed from libzmq3")
    } else {
        .Call("set_recovery_ivl_msec",socket, option.value, PACKAGE="rzmq2")
    }
}

set.mcast.loop <- function(socket, option.value) {
    if(zmq.version() >= "3.0.0") {
        stop("ZMQ_MCAST_LOOP removed from libzmq3")
    } else {
        .Call("set_mcast_loop",socket, option.value, PACKAGE="rzmq2")
    }
}

set.sndbuf <- function(socket, option.value) {
    .Call("set_sndbuf",socket, option.value, PACKAGE="rzmq2")
}

set.rcvbuf <- function(socket, option.value) {
    .Call("set_rcvbuf",socket, option.value, PACKAGE="rzmq2")
}

set.linger <- function(socket, option.value) {
    .Call("set_linger",socket, option.value, PACKAGE="rzmq2")
}

set.reconnect.ivl <- function(socket, option.value) {
    .Call("set_reconnect_ivl",socket, option.value, PACKAGE="rzmq2")
}

set.zmq.backlog <- function(socket, option.value) {
    .Call("set_zmq_backlog",socket, option.value, PACKAGE="rzmq2")
}

set.reconnect.ivl.max <- function(socket, option.value) {
    .Call("set_reconnect_ivl_max",socket, option.value, PACKAGE="rzmq2")
}

get.rcvmore <- function(socket) {
    .Call("get_rcvmore",socket,PACKAGE="rzmq2")
}

set.send.timeout <- function(socket, option.value) {
    .Call("set_sndtimeo", socket, option.value, PACKAGE="rzmq2")
}

get.send.timeout <- function(socket) {
    .Call("get_sndtimeo", socket, PACKAGE="rzmq2")
}

#################################################################################
#  BDD functions
#################################################################################

zmq.version <- function( x = NULL ){

  version__ <- invisible( .Call("get_zmq_version", PACKAGE="rzmq2") )

  if( is.null( x ) ){
    return( version__ )
  }else if( toupper( x ) == 'MAJOR' ){
    return( as.integer( strsplit( split = '\\.', version__ )[[1]][1] ) )
  }else if( toupper( x ) == 'MINOR' ){
    return( as.integer( strsplit( split = '\\.', version__ )[[1]][2] ) )
  }else if( toupper( x ) == 'PATCH' ){
    return( as.integer( strsplit( split = '\\.', version__ )[[1]][3] ) )
  }else{
    stop( 'ERROR: x must be one of {NULL,"major","minor","patch"}.\n' )
  } 
}


init.context <- function( io_threads = 1 ){
  io_threads <- as.integer( io_threads )
  if( io_threads < 1 )
      stop( 'ERROR: io_threads must be at least 1.\n' )
  invisible( .Call( "initContext", io_threads, PACKAGE = "rzmq2" ) )
}

close.socket <- function( socket ){
  invisible( .Call( "closeSocket", socket, PACKAGE = "rzmq2" ) )
}

get.keypair <- function(){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.keypair.\n' )
  keypair <- invisible( .Call( "get_keypair", PACKAGE = "rzmq2" ) )
  names( keypair ) <- c( "public", "secret" )
  return( keypair )
}

set.curve.server <- function( socket ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use set.curve.server.\n' )
  invisible( .Call( "set_curve_server", socket, PACKAGE = "rzmq2" ) )
}

get.curve.server <- function( socket ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.curve.server.\n' )
  curve_server <- invisible( .Call( "get_curve_server", socket, PACKAGE = "rzmq2" ) )
  return( curve_server )
}


set.public.key <- function( socket, option.value ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use set.public.key.\n' )
  invisible( .Call( "set_key", socket, "PUBLIC", as.character( option.value ), PACKAGE = "rzmq2" ) )
}

get.public.key <- function( socket ){
   if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.public.key.\n' )
  public_key <- invisible( .Call( "get_key", socket, "PUBLIC", PACKAGE = "rzmq2" ) )
  return( public_key )
}


set.secret.key <- function( socket, option.value ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use set.secret.key.\n' )
  invisible( .Call( "set_key", socket, "SECRET", as.character( option.value ), PACKAGE = "rzmq2" ) )
}

get.secret.key <- function( socket ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.secret.key.\n' )
  secret_key <- invisible( .Call( "get_key", socket, "SECRET", PACKAGE = "rzmq2" ) )
  return( secret_key )
}


set.server.key <- function( socket, option.value ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use set.server.key.\n' )
  invisible( .Call( "set_key", socket, "SERVER", as.character( option.value ), PACKAGE = "rzmq2" ) )
}

get.server.key <- function( socket ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.server.key.\n' )
  server_key <- invisible( .Call( "get_key", socket, "SERVER", PACKAGE = "rzmq2" ) )
  return( server_key )
}

get.io_threads <- function( context ){
  if( zmq.version( 'major' ) < 4 )
      stop( 'ERROR: ZeroMQ must be version 4 or newer to use get.io_threads.\n' )
  io_threads <- invisible( .Call( "get_io_threads", context, PACKAGE = "rzmq2" ) )
  return( io_threads )
}
