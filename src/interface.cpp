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
#include <sstream>
#include <stdexcept>
#include "cppzmq.h"
#include "interface.h"

SEXP get_zmq_version() {
  SEXP ans;
  int major, minor, patch;
  std::stringstream out;
  zmq::version(&major, &minor, &patch);
  out << major << "." << minor << "." << patch;
  PROTECT(ans = allocVector(STRSXP,1));
  SET_STRING_ELT(ans, 0, mkChar(out.str().c_str()));
  UNPROTECT(1);
  return ans;
}

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

void* checkExternalPointer(SEXP xp_, const char* valid_tag) {
  if(xp_ == R_NilValue) {
    throw std::logic_error("External pointer is NULL.");
  }
  if(TYPEOF(xp_) != EXTPTRSXP) {
    throw std::logic_error("Not an external pointer.");
  }

  if(R_ExternalPtrTag(xp_)==R_NilValue) {
    throw std::logic_error("External pointer tag is NULL.");
  }
  const char* xp_tag = CHAR(PRINTNAME(R_ExternalPtrTag(xp_)));
  if(!xp_tag) {
    throw std::logic_error("External pointer tag is blank.");
  }
  if(strcmp(xp_tag,valid_tag) != 0) {
    throw std::logic_error("External pointer tag does not match.");
  }
  if(R_ExternalPtrAddr(xp_)==NULL) {
    throw std::logic_error("External pointer address is null.");
  }
  return R_ExternalPtrAddr(xp_);
}

static void contextFinalizer(SEXP context_) {
  zmq::context_t* context = reinterpret_cast<zmq::context_t*>(R_ExternalPtrAddr(context_));
  if(context) {
    delete context;
    R_ClearExternalPtr(context_);
  }
}

static void socketFinalizer(SEXP socket_) {
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(socket) {
    delete socket;
    R_ClearExternalPtr(socket_);
  }
}

SEXP initContext() {
  SEXP context_;
  zmq::context_t* context = new zmq::context_t(1);
  if(context) {
    PROTECT(context_ = R_MakeExternalPtr(reinterpret_cast<void*>(context),install("zmq::context_t*"),R_NilValue));
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

  zmq::context_t* context(NULL);
  try {
    context = reinterpret_cast<zmq::context_t*>(checkExternalPointer(context_,"zmq::context_t*"));
  } catch(std::logic_error &e) {
      REprintf("%s\n",e.what());
      return R_NilValue;
  }

  zmq::socket_t* socket = new zmq::socket_t(*context,socket_type);
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  // for debugging
  //uint64_t hwm = 1;
  //socket->setsockopt(ZMQ_HWM, &hwm, sizeof (hwm));

  PROTECT(socket_ = R_MakeExternalPtr(reinterpret_cast<void*>(socket),install("zmq::socket_t*"),R_NilValue));
  R_RegisterCFinalizerEx(socket_, socketFinalizer, TRUE);
  UNPROTECT(1);
  return socket_;
}

SEXP bindSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  try {
    zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
    socket->bind(CHAR(STRING_ELT(address_,0)));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }

  UNPROTECT(1);
  return ans;
}

static short rzmq_build_event_bitmask(SEXP askevents) {
    short bitmask = 0;
    if(TYPEOF(askevents) == STRSXP) {
        for (int i = 0; i < LENGTH(askevents); i++) {
            const char *ask = translateChar(STRING_ELT(askevents, i));
            if (strcmp(ask, "read") == 0) {
                bitmask |= ZMQ_POLLIN;
            } else if (strcmp(ask, "write") == 0) {
                bitmask |= ZMQ_POLLOUT;
            } else if (strcmp(ask, "error") == 0) {
                bitmask |= ZMQ_POLLERR;
            } else {
                error("unrecognized requests poll event %s.", ask);
            }
        }
    } else {
        error("event list passed to poll must be a string or vector of strings");
    }
    return bitmask;
}

SEXP pollSocket(SEXP sockets_, SEXP events_, SEXP timeout_) {
    SEXP result;
    
    if(TYPEOF(timeout_) != INTSXP) {
        error("poll timeout must be an integer.");
    }

    if(TYPEOF(sockets_) != VECSXP || LENGTH(sockets_) == 0) {
        error("A non-empy list of sockets is required as first argument.");
    }

    int nsock = LENGTH(sockets_);
    PROTECT(result = allocVector(VECSXP, nsock));

    if (TYPEOF(events_) != VECSXP) {
        error("event list must be a list of strings or a list of vectors of strings.");
    }
    if(LENGTH(events_) != nsock) {
        error("event list must be the same length as socket list.");
    }

    zmq_pollitem_t *pitems = (zmq_pollitem_t*)R_alloc(nsock, sizeof(zmq_pollitem_t));
    if (pitems == NULL) {
        error("failed to allocate memory for zmq_pollitem_t array.");
    }

    try {
        for (int i = 0; i < nsock; i++) {
            zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(VECTOR_ELT(sockets_, i), "zmq::socket_t*"));
            pitems[i].socket = (void*)*socket;
            pitems[i].events = rzmq_build_event_bitmask(VECTOR_ELT(events_, i));
        }

        int rc = zmq::poll(pitems, nsock, *INTEGER(timeout_));

        if(rc >= 0) {
            for (int i = 0; i < nsock; i++) {
                SEXP events, names;

                // Pre count number of polled events so we can
                // allocate appropriately sized lists.
                short eventcount = 0;
                if (pitems[i].events & ZMQ_POLLIN) eventcount++;
                if (pitems[i].events & ZMQ_POLLOUT) eventcount++;
                if (pitems[i].events & ZMQ_POLLERR) eventcount++;

                PROTECT(events = allocVector(VECSXP, eventcount));
                PROTECT(names = allocVector(VECSXP, eventcount));

                eventcount = 0;
                if (pitems[i].events & ZMQ_POLLIN) {
                    SET_VECTOR_ELT(events, eventcount, ScalarLogical(pitems[i].revents & ZMQ_POLLIN));
                    SET_VECTOR_ELT(names, eventcount, mkChar("read"));
                    eventcount++;
                }

                if (pitems[i].events & ZMQ_POLLOUT) {
                    SET_VECTOR_ELT(names, eventcount, mkChar("write"));

                    SET_VECTOR_ELT(events, eventcount, ScalarLogical(pitems[i].revents & ZMQ_POLLOUT));
                    eventcount++;
                }

                if (pitems[i].events & ZMQ_POLLERR) {
                    SET_VECTOR_ELT(names, eventcount, mkChar("error"));
                    SET_VECTOR_ELT(events, eventcount, ScalarLogical(pitems[i].revents & ZMQ_POLLERR));
                }
                setAttrib(events, R_NamesSymbol, names);
                SET_VECTOR_ELT(result, i, events);
            }
        } else {
            error("polling zmq sockets failed.");
        }
    } catch(std::exception& e) {
        error(e.what());
    }
    // Release the result list (1), and per socket
    // events lists with associated names (2*nsock).
    UNPROTECT(1 + 2*nsock);
    return result;
}

SEXP connectSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    UNPROTECT(1);
    return R_NilValue;
  }
  try {
    zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

SEXP receiveSocket(SEXP socket_, SEXP flags_) {
  SEXP ans, sflags;
  bool status(false);
  zmq::message_t msg;
  int flags;
  int nflag = length(flags_);
  if (nflag < 1) {
    flags = 0;
  } else {
    flags = INTEGER(flags_)[0];
  }
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  try {
    status = socket->recv(&msg, flags);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
  }

  if (0 == status && errno == EAGAIN) {
    PROTECT(ans = allocVector(INTSXP,1));
    INTEGER(ans)[0] = -1;
    UNPROTECT(1);
    return ans;
  }

  if(status) {
    PROTECT(ans = allocVector(RAWSXP,msg.size()));
    memcpy(RAW(ans),msg.data(),msg.size());
    UNPROTECT(1);
    return ans;
  }
  return R_NilValue;
}


SEXP sendRawString(SEXP socket_, SEXP data_, SEXP send_more_) {
  SEXP ans;
  bool status(false);
  if(TYPEOF(data_) != STRSXP) {
    REprintf("data type must be raw (STRSXP).\n");
    return R_NilValue;
  }

  if(TYPEOF(send_more_) != LGLSXP) {
    REprintf("send.more type must be logical (LGLSXP).\n");
    return R_NilValue;
  }

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) {
    REprintf("bad socket object.\n");
    return R_NilValue;
  }

  const char* data = CHAR(STRING_ELT(data_,0));
  zmq::message_t msg (strlen(data));
  memcpy(msg.data(), data, strlen(data));

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
  PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = static_cast<int>(status);
  UNPROTECT(1);
  return ans;
}


SEXP receiveString(SEXP socket_) {
  SEXP ans;
  bool status(false);
  zmq::message_t msg;
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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
  try {
    zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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
  try {
    zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

#if ZMQ_VERSION_MAJOR < 3
// removed from libzmq3
SEXP set_hwm(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

#endif

#if ZMQ_VERSION_MAJOR < 3
// removed from libzmq3
SEXP set_swap(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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
#endif

SEXP set_affinity(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
#if ZMQ_VERSION_MAJOR > 2
  int option_value;
#else
  int64_t option_value;
#endif

  option_value = INTEGER(option_value_)[0];
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;
#if ZMQ_VERSION_MAJOR > 2
  int option_value;
#else
  int64_t option_value;
#endif
  option_value = INTEGER(option_value_)[0];
  try {
    socket->setsockopt(ZMQ_RECOVERY_IVL, &option_value, sizeof(int64_t));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}


#if ZMQ_VERSION_MAJOR < 3
// removed from libzmq3
SEXP set_recovery_ivl_msec(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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
#endif

#if ZMQ_VERSION_MAJOR < 3
// removed from libzmq3
SEXP set_mcast_loop(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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
#endif

SEXP set_sndbuf(SEXP socket_, SEXP option_value_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

#if ZMQ_VERSION_MAJOR > 2
  int option_value;
#else
  int64_t option_value;
#endif

  option_value = INTEGER(option_value_)[0];
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  if(TYPEOF(option_value_)!=INTSXP) { REprintf("option value must be an int.\n");return R_NilValue; }
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

#if ZMQ_VERSION_MAJOR > 2
  int option_value;
#else
  int64_t option_value;
#endif

  option_value = INTEGER(option_value_)[0];
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
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

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
#if ZMQ_VERSION_MAJOR > 2
  int option_value;
#else
  int64_t option_value;
#endif
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

SEXP rzmq_serialize(SEXP data, SEXP rho) {
  static SEXP R_serialize_fun  = findVar(install("serialize"), R_GlobalEnv);
  SEXP R_fcall, ans;

  if(!isEnvironment(rho)) error("'rho' should be an environment");
  PROTECT(R_fcall = lang3(R_serialize_fun, data, R_NilValue));
  PROTECT(ans = eval(R_fcall, rho));
  UNPROTECT(2);
  return ans;
}

SEXP rzmq_unserialize(SEXP data, SEXP rho) {
  static SEXP R_unserialize_fun  = findVar(install("unserialize"), R_GlobalEnv);
  SEXP R_fcall, ans;

  if(!isEnvironment(rho)) error("'rho' should be an environment");
  PROTECT(R_fcall = lang2(R_unserialize_fun, data));
  PROTECT(ans = eval(R_fcall, rho));
  UNPROTECT(2);
  return ans;
}
