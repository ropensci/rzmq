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


#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <Rinternals.h>

static void contextFinalizer(SEXP context_);
static void socketFinalizer(SEXP socket_);
static void messageFinalizer(SEXP msg_);
SEXP rzmq_serialize(SEXP data, SEXP rho);
SEXP rzmq_unserialize(SEXP data, SEXP rho);

extern "C" {
  SEXP get_zmq_version();
  SEXP get_zmq_errno();
  SEXP get_zmq_strerror();
  SEXP initContext();
  SEXP initSocket(SEXP context_, SEXP socket_type_);
  SEXP bindSocket(SEXP socket_, SEXP address_);
  SEXP connectSocket(SEXP socket_, SEXP address_);
  SEXP disconnectSocket(SEXP socket_, SEXP address_);
  SEXP sendSocket(SEXP socket_, SEXP data_, SEXP send_more_);
  SEXP sendNullMsg(SEXP socket_, SEXP send_more_);
  SEXP receiveNullMsg(SEXP socket_);
  SEXP sendRawString(SEXP socket_, SEXP data_, SEXP send_more_);
  SEXP initMessage(SEXP data_);
  SEXP sendMessageObject(SEXP socket_, SEXP data_, SEXP send_more_);
  SEXP receiveSocket(SEXP socket_, SEXP flags_);
  SEXP receiveString(SEXP socket_);
  SEXP receiveInt(SEXP socket_);
  SEXP receiveDouble(SEXP socket_);
  SEXP set_hwm(SEXP socket_, SEXP option_value_);
  SEXP set_swap(SEXP socket_, SEXP option_value_);
  SEXP set_affinity(SEXP socket_, SEXP option_value_);
  SEXP set_identity(SEXP socket_, SEXP option_value_);
  SEXP subscribe(SEXP socket_, SEXP option_value_);
  SEXP unsubscribe(SEXP socket_, SEXP option_value_);
  SEXP set_rate(SEXP socket_, SEXP option_value_);
  SEXP set_recovery_ivl(SEXP socket_, SEXP option_value_);
  SEXP set_recovery_ivl_msec(SEXP socket_, SEXP option_value_);
  SEXP set_mcast_loop(SEXP socket_, SEXP option_value_);
  SEXP set_sndbuf(SEXP socket_, SEXP option_value_);
  SEXP set_rcvbuf(SEXP socket_, SEXP option_value_);
  SEXP set_linger(SEXP socket_, SEXP option_value_);
  SEXP set_reconnect_ivl(SEXP socket_, SEXP option_value_);
  SEXP set_zmq_backlog(SEXP socket_, SEXP option_value_);
  SEXP set_reconnect_ivl_max(SEXP socket_, SEXP option_value_);
  SEXP get_rcvmore(SEXP socket_);
  SEXP pollSocket(SEXP socket_, SEXP events_, SEXP timeout_);
  SEXP get_sndtimeo(SEXP socket_);
  SEXP set_sndtimeo(SEXP socket_, SEXP option_value_);
}

#endif // INTERFACE_HPP
