///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011  Whit Armstrong                                    //
//                                                                       //
// This program is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// This program is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with this program.  If not, see <http://www.gnu.org/licenses/>. //
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <zmq.hpp>
#include "interface.h"

//enum socket_type_t {ZMQ_PAIR,ZMQ_PUB,ZMQ_SUB,ZMQ_REQ,ZMQ_REP,ZMQ_DEALER,ZMQ_ROUTER,ZMQ_PULL,ZMQ_PUSH,ZMQ_XPUB,ZMQ_XSUB,NOT_FOUND};

int string_to_socket_type(const std::string s) {
  if(s == "ZMQ_PAIR") {
    return ZMQ_PAIR;
  } else if(s == "ZMQ_PUB") {
    return ZMQ_PUB;
  } else if(s == "ZMQ_SUB") {
    return ZMQ_SUB;
  } else if(s == "ZMQ_REQ") {
    return ZMQ_REQ;
  } else if(s == "ZMQ_REP") {
    return ZMQ_REP;
  } else if(s == "ZMQ_DEALER") {
    return ZMQ_DEALER;
  } else if(s == "ZMQ_ROUTER") {
    return ZMQ_ROUTER;
  } else if(s == "ZMQ_PULL") {
    return ZMQ_PULL;
  } else if(s == "ZMQ_PUSH") {
    return ZMQ_PUSH;
  } else if(s == "ZMQ_XPUB") {
    return ZMQ_XPUB;
  } else if(s == "ZMQ_XSUB") {
    return ZMQ_XSUB;
  } else {
    return -1;
  }
}

static void contextFinalizer(SEXP context_) {
  zmq::context_t* context = reinterpret_cast<zmq::context_t*>(R_ExternalPtrAddr(context_));
  delete context;
  R_ClearExternalPtr(context_);
}

static void socketFinalizer(SEXP socket_) {
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  delete socket;
  R_ClearExternalPtr(socket_);
}

SEXP initContext() {
  SEXP context_;
  zmq::context_t* context = new zmq::context_t(1);
  PROTECT(context_ = R_MakeExternalPtr(reinterpret_cast<void*>(context),install("zmq::context_t"),R_NilValue));
  R_RegisterCFinalizerEx(context_, contextFinalizer, TRUE);
  UNPROTECT(1);
  return context_;
}

SEXP initSocket(SEXP context_, SEXP socket_type_) {
  SEXP socket_;

  if(TYPEOF(socket_type_) != STRSXP) {
    std::cerr << "socket type must be a string." << std::endl;
    return R_NilValue;
  }

  int socket_type = string_to_socket_type(CHAR(STRING_ELT(socket_type_,0)));
  if(socket_type < 0) {
    std::cerr << "socket type not found." << std::endl;
    return R_NilValue;
  }

  zmq::context_t* context = reinterpret_cast<zmq::context_t*>(R_ExternalPtrAddr(context_));
  zmq::socket_t* socket = new zmq::socket_t(*context,socket_type);
  PROTECT(socket_ = R_MakeExternalPtr(reinterpret_cast<void*>(socket),install("zmq::socket_t"),R_NilValue));
  R_RegisterCFinalizerEx(socket_, socketFinalizer, TRUE);
  UNPROTECT(1);
  return socket_;
}

SEXP bindSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));

  if(TYPEOF(address_) != STRSXP) {
    std::cerr << "address type must be a string." << std::endl;
    return R_NilValue;
  }

  try {
    socket->bind(CHAR(STRING_ELT(address_,0)));
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    LOGICAL(ans)[0] = 0;
  }

  return ans;
}

SEXP connectSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));

  if(TYPEOF(address_) != STRSXP) {
    std::cerr << "address type must be a string." << std::endl;
    return R_NilValue;
  }
  try {
    socket->connect(CHAR(STRING_ELT(address_,0)));    
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    LOGICAL(ans)[0] = 0;
  }

  return ans;
}

SEXP sendSocket(SEXP socket_, SEXP data_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  if(TYPEOF(data_) != STRSXP) {
    std::cerr << "data type must be a string." << std::endl;
    return R_NilValue;
  }

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  zmq::message_t msg (length(data_));
  memcpy(msg.data(), CHAR(STRING_ELT(data_,0)), length(data_));
  bool status = socket->send(msg);
  LOGICAL(ans)[0] = static_cast<int>(status);
  return ans;
}

SEXP receiveSocket(SEXP socket_) {
  SEXP ans;
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  bool status = socket->recv(&msg);

  if(status) {
    PROTECT(ans = allocVector(STRSXP,msg.size()));
    memcpy(const_cast<char*>(CHAR(STRING_ELT(ans,0))),msg.data(),msg.size());
    UNPROTECT(1);
    return ans;
  }

  return R_NilValue;
}
