// -*- mode: C++; c-indent-level: 2; c-basic-offset: 2; tab-width: 8 -*-
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

#ifndef SINK_HPP
#define SINK_HPP

#include <vector>
#include <pthread.h>
#include <zmq.hpp>
#include <Rinternals.h>

class Sink {
private:
  typedef std::vector<char*> container;
  SEXP sink_servers_;
  const size_t num_items_;
  size_t msgs_received;
  container results_;
  std::vector<size_t> msg_sizes_;
  pthread_t worker_;
public:
  Sink(SEXP sink_servers, size_t num_items):
    sink_servers_(sink_servers), num_items_(num_items), msgs_received(0), results_(num_items), msg_sizes_(num_items)
  {
    pthread_create(&worker_, NULL, &Sink::start_thread, static_cast<void*>(this));
  }

  ~Sink() {
    for (container::iterator iter = results_.begin(); iter != results_.end(); iter++) {
      delete[] *iter;
    }
  }

  static void* start_thread(void *handle) {
    //All we do here is call the do_work() function
    reinterpret_cast<Sink*>(handle)->sink_routine();
    return static_cast<void*>(NULL);
  }

  void sink_routine() {
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_PULL);
    try {
      // connect to each sink server
      for(int i = 0; i < length(sink_servers_); i++) {
        receiver.connect(CHAR(STRING_ELT(sink_servers_,i)));
      }
    } catch(std::exception& e) {
      REprintf("%s\n",e.what());
      // we don't want to execute the thread
      // if it can't connect
      return;
    }

    while(msgs_received < num_items_) {
      zmq::message_t msg;
      receiver.recv(&msg);
      msg_sizes_[msgs_received] = msg.size();
      char* dest = new char[msg.size()];
      if(dest == NULL) break;
      results_[msgs_received] = dest;
      memcpy(dest,msg.data(),msg.size());
      ++msgs_received;
    }
  }

  SEXP getResults() {
    SEXP ans, x;
    int worker_exit = pthread_join(worker_, NULL);
    if(worker_exit != 0) {
      REprintf("Sink worker had non-zero exit: %d\n",worker_exit);
      return R_NilValue;
    }
    PROTECT(ans = allocVector(VECSXP,results_.size()));
    for(size_t i = 0; i < results_.size(); i++) {
      PROTECT(x = allocVector(RAWSXP,msg_sizes_[i]));
      memcpy(RAW(x),results_[i],msg_sizes_[i]);
      SET_VECTOR_ELT(ans,i,x);
      UNPROTECT(1);
    }
    UNPROTECT(1);
    return ans;
  }
};


#endif // SINK_HPP
