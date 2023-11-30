/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_base_stc_ServerToClientCmd_HPP
#define ecflow_base_stc_ServerToClientCmd_HPP

#include "Cmd.hpp"
#include "NodeFwd.hpp"
#include "Serialization.hpp"
#include "ServerReply.hpp"

//================================================================================
// Start of Server->client
//================================================================================
class ServerToClientCmd {
public:
    virtual ~ServerToClientCmd();

    virtual void cleanup() {} /// After the command has run this function can be used to reclaim memory

    virtual std::string print() const = 0;
    virtual bool equals(ServerToClientCmd*) const { return true; }

    virtual const std::string&
    get_string() const;                      /// Used by group command, can return any string, including file contents
    virtual bool ok() const { return true; } /// Used by group command
    virtual bool is_returnable_in_group_cmd() const { return true; } /// used by group command

    virtual std::string error() const { return std::string(); } /// Used by test

    // Called in client, if any data to be returned , the set on class ServerReply
    // Cmd_ptr cts_cmd  can used for additional context.
    // return true, if client should exit, returns false, if further work required, ie blocking, errors, etc
    virtual bool handle_server_response(ServerReply&, Cmd_ptr cts_cmd, bool debug) const = 0;

protected:
    ServerToClientCmd() = default;

private:
    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar, std::uint32_t const version) {}
};

#endif /* ecflow_base_stc_ServerToClientCmd_HPP */
