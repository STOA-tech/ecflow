//============================================================================
// Name        : Request
// Author      : Avi
// Revision    : $Revision: #7 $ 
//
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
//============================================================================
#include <stdexcept>
#include "ClientToServerRequest.hpp"

using namespace std;

STC_Cmd_ptr ClientToServerRequest::handleRequest(AbstractServer* as) const
{
	if (cmd_.get()) {
		return cmd_->handleRequest(as);
	}

	/// means programming error somewhere
  	throw std::runtime_error("ClientToServerRequest::handleRequest: Cannot send a NULL request to the server !");
}

std::ostream& ClientToServerRequest::print( std::ostream& os ) const {
	if (cmd_.get()) {
		os << cmd_->print_short(); // avoid printing hundreds of paths in the command
		return os;
	}
	return os << "NULL request";
}

bool ClientToServerRequest::operator==(const ClientToServerRequest& rhs) const
{
	if (!cmd_.get() && !rhs.cmd_.get()) {
		return true;
	}
	if (cmd_.get() && !rhs.cmd_.get()) {
		return false;
	}
	if (!cmd_.get() && rhs.cmd_.get()) {
		return false;
	}
	return (cmd_->equals(rhs.cmd_.get()));
}

std::ostream& operator<<( std::ostream& os, const ClientToServerRequest& d ) {
	return d.print( os );
}
