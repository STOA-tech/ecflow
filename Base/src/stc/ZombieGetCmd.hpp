/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_base_stc_ZombieGetCmd_HPP
#define ecflow_base_stc_ZombieGetCmd_HPP

#include "ServerToClientCmd.hpp"
#include "Zombie.hpp"
class AbstractServer;

//================================================================================
// Paired with CtsCmd(GET_ZOMBIES)
// Client---CtsCmd(GET_ZOMBIES)---->Server-----(ZombieGetCmd)--->client:
//================================================================================
class ZombieGetCmd final : public ServerToClientCmd {
public:
    explicit ZombieGetCmd(AbstractServer*);
    ZombieGetCmd() : ServerToClientCmd() {}

    void init(AbstractServer*);
    bool handle_server_response(ServerReply&, Cmd_ptr cts_cmd, bool debug) const override;
    std::string print() const override;
    bool equals(ServerToClientCmd*) const override;
    void cleanup() override {
        std::vector<Zombie>().swap(zombies_);
    } /// run in the server, after command send to client

private:
    std::vector<Zombie> zombies_;

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar, std::uint32_t const version) {
        ar(cereal::base_class<ServerToClientCmd>(this), CEREAL_NVP(zombies_));
    }
};

std::ostream& operator<<(std::ostream& os, const ZombieGetCmd&);

#endif /* ecflow_base_stc_ZombieGetCmd_HPP */
