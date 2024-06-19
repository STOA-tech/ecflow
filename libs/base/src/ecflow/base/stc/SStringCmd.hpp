/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_base_stc_SStringCmd_HPP
#define ecflow_base_stc_SStringCmd_HPP

#include "ecflow/base/stc/ServerToClientCmd.hpp"

///================================================================================
/// Paired with CFileCmd
/// Client---(CFileCmd)---->Server-----(SStringCmd)--->client:
/// Only valid when the clients request a CFileCmd *OR* Log file
/// Other times this will be empty.
/// CFileCmd:: The file Contents(script,job,jobout,manual)
///     Can be potentially very large
/// LogCmd: The log file Can be potentially very large
///     Only really valid if the out bound request was a LogCmd(LogCmd::GET)
///================================================================================
class SStringCmd final : public ServerToClientCmd {
public:
    explicit SStringCmd(const std::string& s) : str_(s) {}
    SStringCmd() : ServerToClientCmd() {}

    void init(const std::string& s) { str_ = s; }
    std::string print() const override;
    bool equals(ServerToClientCmd*) const override;
    const std::string& get_string() const override { return str_; }
    bool handle_server_response(ServerReply& server_reply, Cmd_ptr cts_cmd, bool debug) const override;
    void cleanup() override { std::string().swap(str_); } /// run in the server, after command send to client

private:
    std::string str_;

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar, std::uint32_t const version) {
        ar(cereal::base_class<ServerToClientCmd>(this), CEREAL_NVP(str_));
    }
};

std::ostream& operator<<(std::ostream& os, const SStringCmd&);

#endif /* ecflow_base_stc_SStringCmd_HPP */
