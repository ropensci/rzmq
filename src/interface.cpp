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

#include <sstream>
#include <zmq.hpp>
#include <chrono>
#include <stdexcept>
static_assert(ZMQ_VERSION_MAJOR >= 3,"The minimum required version of libzmq is 3.0.0.");
#include "interface.h"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;

/* Check for interrupt without long jumping */
void check_interrupt_fn(void *dummy) {
    R_CheckUserInterrupt();
}

int pending_interrupt() {
    return !(R_ToplevelExec(check_interrupt_fn, NULL));
}

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

SEXP get_zmq_errno() {
  SEXP ans; PROTECT(ans = allocVector(INTSXP,1));
  INTEGER(ans)[0] = zmq_errno();
  UNPROTECT(1);
  return ans;
}

SEXP get_zmq_strerror() {
  SEXP ans; PROTECT(ans = allocVector(STRSXP,1));
  SET_STRING_ELT(ans, 0, mkChar(zmq_strerror(zmq_errno())));
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
  } else if(s == "ZMQ_XREQ") {
    return ZMQ_XREQ;
  } else if(s == "ZMQ_XREP") {
    return ZMQ_XREP;
  } else {
    return -1;
  }
}

int string_to_option_type(const std::string s) {
  if (s == "ZMQ_AFFINITY") {
    return ZMQ_AFFINITY;
  } else if (s == "ZMQ_IDENTITY") {
    return ZMQ_IDENTITY;
  } else if (s == "ZMQ_SUBSCRIBE") {
    return ZMQ_SUBSCRIBE;
  } else if (s == "ZMQ_UNSUBSCRIBE") {
    return ZMQ_UNSUBSCRIBE;
  } else if (s == "ZMQ_RATE") {
    return ZMQ_RATE;
  } else if (s == "ZMQ_RECOVERY_IVL") {
    return ZMQ_RECOVERY_IVL;
  } else if (s == "ZMQ_SNDBUF") {
    return ZMQ_SNDBUF;
  } else if (s == "ZMQ_RCVBUF") {
    return ZMQ_RCVBUF;
  } else if (s == "ZMQ_LINGER") {
    return ZMQ_LINGER;
  } else if (s == "ZMQ_RECONNECT_IVL") {
    return ZMQ_RECONNECT_IVL;
  } else if (s == "ZMQ_BACKLOG") {
    return ZMQ_BACKLOG;
  } else if (s == "ZMQ_RECONNECT_IVL_MAX") {
    return ZMQ_RECONNECT_IVL_MAX;
  } else if (s == "ZMQ_SNDTIMEO") {
    return ZMQ_SNDTIMEO;
  } else if (s == "ZMQ_SNDHWM") {
    return ZMQ_SNDHWM;
  } else if (s == "ZMQ_RCVHWM") {
    return ZMQ_RCVHWM;
  } else if (s == "ZMQ_TCP_KEEPALIVE") {
    return ZMQ_TCP_KEEPALIVE;
  } else if (s == "ZMQ_TCP_KEEPALIVE_IDLE") {
    return ZMQ_TCP_KEEPALIVE_IDLE;
  } else if (s == "ZMQ_TCP_KEEPALIVE_CNT") {
    return ZMQ_TCP_KEEPALIVE_CNT;
  } else if (s == "ZMQ_TCP_KEEPALIVE_INTVL") {
    return ZMQ_TCP_KEEPALIVE_INTVL;
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

static void messageFinalizer(SEXP msg_) {
  zmq::message_t* msg = reinterpret_cast<zmq::message_t*>(checkExternalPointer(msg_,"zmq::message_t*"));
  if(msg) {
    delete msg; // destructor will call zmq_msg_close()
    R_ClearExternalPtr(msg_);
  }
}

SEXP initContext(SEXP threads_) {
  if(TYPEOF(threads_) != INTSXP) {
    error("thread number must be an integer.");
  }

  SEXP context_;
  zmq::context_t* context;
  try {
    context = new zmq::context_t(*INTEGER(threads_));
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }

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

        int timeout = *INTEGER(timeout_);
        int rc = -1;
        auto start = Time::now();
        do {
            try {
                rc = zmq::poll(pitems, nsock, timeout);
            } catch(zmq::error_t& e) {
                if (errno != EINTR || pending_interrupt())
                    throw e;
                if (timeout != -1) {
                    ms dt = std::chrono::duration_cast<ms>(Time::now() - start);
                    timeout = timeout - dt.count();
                    if (timeout <= 0)
                        break;
                }
            }
        } while(rc < 0);

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

        // Release the result list (1), and per socket
        // events lists with associated names (2*nsock).
        UNPROTECT(1 + 2*nsock);
        return result;
    } catch(zmq::error_t& e) {
        if (errno == ETERM) {
            error("At least one of the members of the 'items' array refers to "
                  "a 'socket' whose associated 0MQ 'context' was terminated.");
        } else if (errno == EFAULT) {
            error("The provided 'items' was not valid (NULL).");
        } else if (errno == EINTR) {
            error("The operation was interrupted by delivery of a signal "
                  "before any events were available.");
        } else
            throw e;
    } catch(std::exception& e) {
        error(e.what());
    }
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

SEXP disconnectSocket(SEXP socket_, SEXP address_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = 1;

  if(TYPEOF(address_) != STRSXP) {
    REprintf("address type must be a string.\n");
    UNPROTECT(1);
    return R_NilValue;
  }
  try {
    zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
    socket->disconnect(CHAR(STRING_ELT(address_,0)));
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

  zmq::message_t msg (Rf_xlength(data_));
  memcpy(msg.data(), RAW(data_), Rf_xlength(data_));

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

SEXP initMessage(SEXP data_) {
  SEXP msg_;

  if(TYPEOF(data_) != RAWSXP) {
    REprintf("data type must be raw (RAWSXP).\n");
    UNPROTECT(1);
    return R_NilValue;
  }

  zmq::message_t* msg = new zmq::message_t(Rf_xlength(data_));
  memcpy(msg->data(), RAW(data_), Rf_xlength(data_));
// no copy below, see first that one copy works
//  zmq::message_t msg(reinterpret_cast<void*>(data_), Rf_xlength(data_), NULL);

  PROTECT(msg_ = R_MakeExternalPtr(reinterpret_cast<void*>(msg),install("zmq::message_t*"),R_NilValue));
  R_RegisterCFinalizerEx(msg_, messageFinalizer, TRUE);
  UNPROTECT(1);
  return msg_;
}

SEXP sendMessageObject(SEXP socket_, SEXP msg_, SEXP send_more_) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  bool status(false);

  if(TYPEOF(send_more_) != LGLSXP) {
    REprintf("send.more type must be logical (LGLSXP).\n");
    return R_NilValue;
  }

  zmq::message_t* msg = reinterpret_cast<zmq::message_t*>(checkExternalPointer(msg_,"zmq::message_t*"));
  if(!msg) { REprintf("bad message object.\n");return R_NilValue; }

  zmq::message_t copy;
  copy.copy(msg);

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }

  bool send_more = LOGICAL(send_more_)[0];
  try {
    if(send_more) {
      status = socket->send(copy,ZMQ_SNDMORE);
    } else {
      status = socket->send(copy);
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

SEXP receiveSocket(SEXP socket_, SEXP dont_wait_) {
  SEXP ans;
  zmq::message_t msg;

  if(TYPEOF(dont_wait_) != LGLSXP) {
    REprintf("dont_wait type must be logical (LGLSXP).\n");
    return R_NilValue;
  }
  int flags = LOGICAL(dont_wait_)[0];
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n"); return R_NilValue; }
  try {
    if(socket->recv(&msg, flags)) {
      PROTECT(ans = allocVector(RAWSXP,msg.size()));
      memcpy(RAW(ans),msg.data(),msg.size());
      UNPROTECT(1);
      return ans;
    } else {
      // socket->recv returned false, but did not throw
      // this condition implies EAGAIN
      // see here for logic: https://github.com/zeromq/cppzmq/blob/master/zmq.hpp#L449
      return R_NilValue;
    }
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
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

SEXP setSockOptInt(SEXP socket_, SEXP key_, SEXP value_) {
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if (!socket) {
    REprintf("bad socket object.\n");
    return R_NilValue;
  }
  if (TYPEOF(value_) != INTSXP) {
    REprintf("option value must be an int.\n");return R_NilValue;
  }

  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = 1;
  int key = string_to_option_type(CHAR(STRING_ELT(key_,0)));
  int value = INTEGER(value_)[0];

  try {
    socket->setsockopt(key, &value, sizeof(int));
  } catch(std::exception& e) {
    REprintf("%s\n", e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP setSockOptChr(SEXP socket_, SEXP key_, SEXP value_) {
  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if (!socket) {
    REprintf("bad socket object.\n");
    return R_NilValue;
  }
  if (TYPEOF(value_) != STRSXP) {
    REprintf("option value must be a string.\n");
    return R_NilValue;
  }

  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = 1;
  int key = string_to_option_type(CHAR(STRING_ELT(key_,0)));
  const char* value = CHAR(STRING_ELT(value_,0));

  try {
    socket->setsockopt(key, value, strlen(value));
  } catch(std::exception& e) {
    REprintf("%s\n", e.what());
    LOGICAL(ans)[0] = 0;
  }
  UNPROTECT(1);
  return ans;
}

SEXP get_sndtimeo(SEXP socket_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  int option_value;
  size_t option_value_len = sizeof(option_value);
  try {
    socket->getsockopt(ZMQ_SNDTIMEO, &option_value, &option_value_len);
  } catch(std::exception& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }
  SEXP ans; PROTECT(ans = allocVector(REALSXP,1));
  REAL(ans)[0] = static_cast<int>(option_value);
  UNPROTECT(1);
  return ans;
}


SEXP get_rcvmore(SEXP socket_) {

  zmq::socket_t* socket = reinterpret_cast<zmq::socket_t*>(checkExternalPointer(socket_,"zmq::socket_t*"));
  if(!socket) { REprintf("bad socket object.\n");return R_NilValue; }
  int option_value;
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
