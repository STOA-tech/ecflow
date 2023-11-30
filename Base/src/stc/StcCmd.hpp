/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_base_stc_StcCmd_HPP
#define ecflow_base_stc_StcCmd_HPP

#include "ServerToClientCmd.hpp"

// Command that are simple replies to the client.
// Originally we had separate commands. However this lead
// TOC overflow on the AIX. Hence in order to minimise global
// symbols due to use of boost serialisation, will use a single command
class StcCmd final : public ServerToClientCmd {
public:
    enum Api {
        OK,
        BLOCK_CLIENT_SERVER_HALTED,
        BLOCK_CLIENT_ON_HOME_SERVER,
        DELETE_ALL,
        INVALID_ARGUMENT, // Created on the Client side
        END_OF_FILE
    };
    explicit StcCmd(Api a) : api_(a) {}
    StcCmd() = default;

    void init(Api a) { api_ = a; }
    Api api() const { return api_; }

    std::string print() const override;
    bool equals(ServerToClientCmd*) const override;
    bool handle_server_response(ServerReply& server_reply, Cmd_ptr cts_cmd, bool debug) const override;

    /// Other legitimate ServerToClientCmd commands also return ok() as true
    bool ok() const override { return api_ == OK; } // used by group command

    // avoid adding OK to group command. Since groupCmd will return true from GroupSTCCmd::ok() *IF* there are no errors
    bool is_returnable_in_group_cmd() const override { return api_ == DELETE_ALL; }

private:
    Api api_{OK};

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar, std::uint32_t const version) {
        ar(cereal::base_class<ServerToClientCmd>(this), CEREAL_NVP(api_));
    }
};

std::ostream& operator<<(std::ostream& os, const StcCmd&);

#endif /* ecflow_base_stc_StcCmd_HPP */
