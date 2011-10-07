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
static void sinkFinalizer(SEXP sink_);

extern "C" {
  SEXP initContext();
  SEXP initSocket(SEXP context_, SEXP socket_type_);
  SEXP bindSocket(SEXP socket_, SEXP address_);
  SEXP connectSocket(SEXP socket_, SEXP address_);
  SEXP sendSocket(SEXP socket_, SEXP data_);
  SEXP sendNullMsg(SEXP socket_);
  SEXP receiveSocket(SEXP socket_);
  SEXP receiveString(SEXP socket_);
  SEXP createSink(SEXP address_, SEXP num_items_);
  SEXP getSinkResults(SEXP sink_);
}

#endif // INTERFACE_HPP
