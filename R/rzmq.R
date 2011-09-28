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

init.context <- function() {
    .Call("initContext", PACKAGE="rzmq")
}

init.socket <- function(context, socket.type) {
    .Call("initSocket", context, socket.type, PACKAGE="rzmq")
}

bind.socket <- function(socket, address) {
    .Call("bindSocket", socket, address, PACKAGE="rzmq")
}

connect.socket <- function(socket, address) {
    .Call("connectSocket", socket, address, PACKAGE="rzmq")
}

send.socket <- function(socket, data) {
    .Call("sendSocket", socket, serialize(data,NULL), PACKAGE="rzmq")
}

receive.socket <- function(socket) {
    ans <- .Call("receiveSocket", socket, PACKAGE="rzmq")
    unserialize(ans)
}

create.sink <- function(address, num_items) {
    .Call("createSink", address, as.integer(num_items), PACKAGE="rzmq")
}

get.sink.results <- function(sink) {
    .Call("getSinkResults", sink, PACKAGE="rzmq")
}
