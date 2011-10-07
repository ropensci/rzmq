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

#include <stdint.h>
#include <string>
#include <zmq.hpp>
#include "interface.h"
#include "sink.h"

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

static void sinkFinalizer(SEXP sink_) {
  Sink* sink = reinterpret_cast<Sink*>(R_ExternalPtrAddr(sink_));
  delete sink;
  R_ClearExternalPtr(sink_);
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
    REprintf("socket type must be a string.\n");
    return R_NilValue;
  }

  int socket_type = string_to_socket_type(CHAR(STRING_ELT(socket_type_,0)));
  if(socket_type < 0) {
    REprintf("socket type not found.\n");
    return R_NilValue;
  }

  zmq::context_t* context = reinterpret_cast<zmq::context_t*>(R_ExternalPtrAddr(context_));
  zmq::socket_t* socket = new zmq::socket_t(*context,socket_type);

  // for debugging
  //uint64_t hwm = 1;
  //socket->setsockopt(ZMQ_HWM, &hwm, sizeof (hwm));

  PROTECT(socket_ = R_MakeExternalPtr(reinterpret_cast<void*>(socket),install("zmq::socket_t"),R_NilValue));
  R_RegisterCFinalizerEx(socket_, socketFinalizer, TRUE);
  UNPROTECT(1);
  return socket_;
}

SEXP bindSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));

  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  try {
    socket->bind(CHAR(STRING_ELT(address_,0)));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }

  UNPROTECT(1);
  return ans;
}

SEXP connectSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));

  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    UNPROTECT(1);
    return R_NilValue;
  }
  try {
    socket->connect(CHAR(STRING_ELT(address_,0)));    
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }

  UNPROTECT(1);
  return ans;
}

SEXP sendSocket(SEXP socket_, SEXP data_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status;
  if(TYPEOF(data_) != RAWSXP) {
    REprintf("data type must be raw (RAWSXP).\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  zmq::message_t msg (length(data_));
  memcpy(msg.data(), RAW(data_), length(data_));
  try {
    status = socket->send(msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  LOGICAL(ans)[0] = static_cast<int>(status);
  UNPROTECT(1);
  return ans;
}

SEXP sendNullMsg(SEXP socket_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status;

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  zmq::message_t msg(0);
  try {
    status = socket->send(msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  LOGICAL(ans)[0] = static_cast<int>(status);
  UNPROTECT(1);
  return ans;
}

SEXP receiveSocket(SEXP socket_) {
  SEXP ans;
  bool status;
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  try {
    status = socket->recv(&msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  if(status) {
    PROTECT(ans = allocVector(RAWSXP,msg.size()));
    memcpy(RAW(ans),msg.data(),msg.size());
    UNPROTECT(1);
    return ans;
  }

  return R_NilValue;
}

SEXP receiveString(SEXP socket_) {
  SEXP ans;
  bool status;
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  try {
    status = socket->recv(&msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  if(status) {
    PROTECT(ans = allocVector(STRSXP,1));
    char* string_msg = new char[msg.size() + 1];
    if(string_msg == NULL) {
      UNPROTECT(1);
      return R_NilValue;
    }
    memcpy(string_msg,msg.data(),msg.size());
    string_msg[msg.size()] = 0;
    SET_STRING_ELT(ans, 0, mkChar(string_msg));
    UNPROTECT(1);
    return ans;
  }
  return R_NilValue;
}

SEXP receiveInt(SEXP socket_) {
  SEXP ans;
  bool status;
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  try {
    status = socket->recv(&msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  if(status) {
    if(msg.size() != sizeof(int)) {
      REprintf("bad integer size on remote machine.\n");
      return R_NilValue;
    }
    PROTECT(ans = allocVector(INTSXP,1));
    memcpy(INTEGER(ans),msg.data(),msg.size());
    UNPROTECT(1);
    return ans;
  }
  return R_NilValue;
}

SEXP receiveDouble(SEXP socket_) {
  SEXP ans;
  bool status;
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  try {
    status = socket->recv(&msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  if(status) {
    if(msg.size() != sizeof(double)) {
      REprintf("bad double size on remote machine.\n");
      return R_NilValue;
    }
    PROTECT(ans = allocVector(REALSXP,1));
    memcpy(REAL(ans),msg.data(),msg.size());
    UNPROTECT(1);
    return ans;
  }
  return R_NilValue;
}

SEXP createSink(SEXP address_, SEXP num_items_) {
  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    return R_NilValue;
  }

  if(TYPEOF(num_items_) != INTSXP) {
    REprintf("num_items type must be an integer.\n");
    return R_NilValue;
  }

  SEXP sink_;
  Sink* sink = new Sink(CHAR(STRING_ELT(address_,0)),INTEGER(num_items_)[0]);
  PROTECT(sink_ = R_MakeExternalPtr(reinterpret_cast<void*>(sink),install("sink"),R_NilValue));
  R_RegisterCFinalizerEx(sink_, sinkFinalizer, TRUE);
  UNPROTECT(1);
  return sink_;
}

SEXP getSinkResults(SEXP sink_) {
  Sink* sink = reinterpret_cast<Sink*>(R_ExternalPtrAddr(sink_));
  return sink->getResults();
}
