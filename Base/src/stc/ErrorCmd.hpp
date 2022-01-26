#ifndef ERROR_CMD_HPP_
#define ERROR_CMD_HPP_
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
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
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include "ServerToClientCmd.hpp"

class ErrorCmd final : public ServerToClientCmd {
public:
   explicit ErrorCmd(const std::string& errorMsg);
	ErrorCmd() : ServerToClientCmd() {}

	void init( const std::string& errorMsg);
	std::string print() const override;
	bool equals(ServerToClientCmd*) const override;
  	bool handle_server_response( ServerReply&, Cmd_ptr cts_cmd, bool debug ) const override;

 	std::string error() const override { return error_msg_;}   /// Used by test
	bool ok() const override { return false; }                 /// Used by group command
   void cleanup() override { std::string().swap(error_msg_);} /// run in the server, after command send to client

private:
 	std::string error_msg_;

   friend class cereal::access;
   template<class Archive>
   void serialize(Archive & ar, std::uint32_t const version )
   {
 		ar(cereal::base_class< ServerToClientCmd >( this ),
 		   CEREAL_NVP(error_msg_));
  	}
};

std::ostream& operator<<(std::ostream& os, const ErrorCmd&);

#endif
