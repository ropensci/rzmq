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
  if(context) {
    delete context;
    R_ClearExternalPtr(context_);
  }
}

static void socketFinalizer(SEXP socket_) {
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(socket) {
    delete socket;
    R_ClearExternalPtr(socket_);
  }
}

SEXP initContext() {
  SEXP context_;
  zmq::context_t* context = new zmq::context_t(1);
  if(context) {
    PROTECT(context_ = R_MakeExternalPtr(reinterpret_cast<void*>(context),install("zmq::context_t"),R_NilValue));
    R_RegisterCFinalizerEx(context_, contextFinalizer, TRUE);
    UNPROTECT(1);
    return context_;
  } else {
    return R_NilValue;
  }
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
  if(!context) { REprintf("bad context object.\n");return R_NilValue; }
  zmq::socket_t* socket = new zmq::socket_t(*context,socket_type);
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
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
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }

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
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }

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

SEXP sendSocket(SEXP socket_, SEXP data_, SEXP send_more_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status(false);
  if(TYPEOF(data_) != RAWSXP) {
    REprintf("data type must be raw (RAWSXP).\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  if(TYPEOF(send_more_) != LGLSXP) {
    REprintf("send.more type must be logical (LGLSXP).\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }

  zmq::message_t msg (length(data_));
  memcpy(msg.data(), RAW(data_), length(data_));

  bool send_more = LOGICAL(send_more_)[0];
  try {
    if(send_more) {
      status = socket->send(msg,ZMQ_SNDMORE);
    } else {
      status = socket->send(msg);
    }
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  LOGICAL(ans)[0] = static_cast<int>(status);
  UNPROTECT(1);
  return ans;
}

SEXP sendNullMsg(SEXP socket_, SEXP send_more_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status(false);

  if(TYPEOF(send_more_) != LGLSXP) {
    REprintf("send.more type must be logical (LGLSXP).\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  zmq::message_t msg(0);

  bool send_more = LOGICAL(send_more_)[0];
  try {
    if(send_more) {
      status = socket->send(msg,ZMQ_SNDMORE);
    } else {
      status = socket->send(msg);
    }
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  LOGICAL(ans)[0] = static_cast<int>(status);
  UNPROTECT(1);
  return ans;
}

SEXP receiveNullMsg(SEXP socket_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status(false);

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  zmq::message_t msg;
  try {
    status = socket->recv(&msg);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }
  LOGICAL(ans)[0] = static_cast<int>(status) && (msg.size() == 0);
  UNPROTECT(1);
  return ans;
}

SEXP receiveSocket(SEXP socket_) {
  SEXP ans;
  bool status(false);
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
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
  bool status(false);
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
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
  bool status(false);
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
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
  bool status(false);
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
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

SEXP set_hwm(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  uint64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_HWM, &option_value, sizeof(uint64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_swap(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_SWAP, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}


SEXP set_affinity(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  uint64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_AFFINITY, &option_value, sizeof(uint64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_identity(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=STRSXP) { REprintf("option value must be a string.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  const char* option_value = CHAR(STRING_ELT(option_value_,0));
  try {
    socket->setsockopt(ZMQ_IDENTITY, option_value,strlen(option_value));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP subscribe(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=STRSXP) { REprintf("option value must be a string.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  const char* option_value = CHAR(STRING_ELT(option_value_,0));
  try {
    socket->setsockopt(ZMQ_SUBSCRIBE, option_value,strlen(option_value));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP unsubscribe(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=STRSXP) { REprintf("option value must be a string.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
  const char* option_value = CHAR(STRING_ELT(option_value_,0));
  try {
    socket->setsockopt(ZMQ_UNSUBSCRIBE, option_value,strlen(option_value));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_rate(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RATE, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_recovery_ivl(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RECOVERY_IVL, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_recovery_ivl_msec(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RECOVERY_IVL_MSEC, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_mcast_loop(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=LGLSXP) { REprintf("option value must be a logical.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int64_t option_value(LOGICAL(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_MCAST_LOOP, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_sndbuf(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  uint64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_SNDBUF, &option_value, sizeof(uint64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_rcvbuf(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  uint64_t option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RCVBUF, &option_value, sizeof(uint64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_linger(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_LINGER, &option_value, sizeof(int));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_reconnect_ivl(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RECONNECT_IVL, &option_value, sizeof(int));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_zmq_backlog(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_BACKLOG, &option_value, sizeof(int));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP set_reconnect_ivl_max(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  int option_value(INTEGER(option_value_)[0]);
  try {
    socket->setsockopt(ZMQ_RECONNECT_IVL_MAX, &option_value, sizeof(int));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}


SEXP get_rcvmore(SEXP socket_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(R_ExternalPtrAddr(socket_));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }

  int64_t option_value;
  size_t option_value_len = sizeof(option_value);
  try {
    socket->getsockopt(ZMQ_RCVMORE, &option_value, &option_value_len);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = static_cast<int>(option_value);
  UNPROTECT(1);
  return ans;
}

// #define ZMQ_RCVMORE 13
// #define ZMQ_FD 14
// #define ZMQ_EVENTS 15
// #define ZMQ_TYPE 16
