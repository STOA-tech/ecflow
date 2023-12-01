/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "ecflow/core/Child.hpp"

#include <cassert>

#include "ecflow/core/Str.hpp"

namespace ecf {

std::string Child::to_string(Child::ZombieType zt) {
    switch (zt) {
        case Child::USER:
            return "user";
            break;
        case Child::PATH:
            return "path";
            break;
        case Child::ECF:
            return "ecf";
            break;
        case Child::ECF_PID:
            return "ecf_pid";
            break;
        case Child::ECF_PID_PASSWD:
            return "ecf_pid_passwd";
            break;
        case Child::ECF_PASSWD:
            return "ecf_passwd";
            break;
        case Child::NOT_SET:
            return "not_set";
            break;
    }
    return std::string();
}

Child::ZombieType Child::zombie_type(const std::string& s) {
    if (s == "user")
        return Child::USER;
    if (s == "ecf")
        return Child::ECF;
    if (s == "ecf_pid")
        return Child::ECF_PID;
    if (s == "ecf_pid_passwd")
        return Child::ECF_PID_PASSWD;
    if (s == "ecf_passwd")
        return Child::ECF_PASSWD;
    if (s == "path")
        return Child::PATH;
    return Child::NOT_SET;
}

bool Child::valid_zombie_type(const std::string& s) {
    if (s == "user")
        return true;
    if (s == "ecf")
        return true;
    if (s == "ecf_pid")
        return true;
    if (s == "ecf_pid_passwd")
        return true;
    if (s == "ecf_passwd")
        return true;
    if (s == "path")
        return true;
    return false;
}

std::string Child::to_string(const std::vector<Child::CmdType>& vec) {
    std::string ret;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i == 0)
            ret += to_string(vec[i]);
        else {
            ret += ",";
            ret += to_string(vec[i]);
        }
    }
    return ret;
}

std::string Child::to_string(Child::CmdType ct) {
    switch (ct) {
        case Child::INIT:
            return "init";
            break;
        case Child::EVENT:
            return "event";
            break;
        case Child::METER:
            return "meter";
            break;
        case Child::LABEL:
            return "label";
            break;
        case Child::WAIT:
            return "wait";
            break;
        case Child::QUEUE:
            return "queue";
            break;
        case Child::ABORT:
            return "abort";
            break;
        case Child::COMPLETE:
            return "complete";
            break;
    }
    assert(false);
    return "init";
}

std::vector<Child::CmdType> Child::child_cmds(const std::string& s) {
    // expect single or , separated tokens
    std::vector<std::string> tokens;
    Str::split(s, tokens, ",");
    std::vector<Child::CmdType> ret;
    ret.reserve(tokens.size());
    for (const auto& token : tokens) {
        ret.push_back(child_cmd(token));
    }
    return ret;
}

Child::CmdType Child::child_cmd(const std::string& s) {
    if (s == "init")
        return Child::INIT;
    if (s == "event")
        return Child::EVENT;
    if (s == "meter")
        return Child::METER;
    if (s == "label")
        return Child::LABEL;
    if (s == "wait")
        return Child::WAIT;
    if (s == "queue")
        return Child::QUEUE;
    if (s == "abort")
        return Child::ABORT;
    if (s == "complete")
        return Child::COMPLETE;
    assert(false);
    return Child::INIT;
}

bool Child::valid_child_cmds(const std::string& s) {
    // empty means all children
    if (s.empty())
        return true;

    // expect single or , separated tokens
    std::vector<std::string> tokens;
    Str::split(s, tokens, ",");
    for (const auto& token : tokens) {
        if (!valid_child_cmd(token))
            return false;
    }
    return true;
}

bool Child::valid_child_cmd(const std::string& s) {
    if (s == "init")
        return true;
    if (s == "event")
        return true;
    if (s == "meter")
        return true;
    if (s == "label")
        return true;
    if (s == "wait")
        return true;
    if (s == "queue")
        return true;
    if (s == "abort")
        return true;
    if (s == "complete")
        return true;
    return false;
}

std::vector<Child::CmdType> Child::list() {
    std::vector<ecf::Child::CmdType> child_cmds;
    child_cmds.push_back(ecf::Child::INIT);
    child_cmds.push_back(ecf::Child::EVENT);
    child_cmds.push_back(ecf::Child::METER);
    child_cmds.push_back(ecf::Child::LABEL);
    child_cmds.push_back(ecf::Child::WAIT);
    child_cmds.push_back(ecf::Child::QUEUE);
    child_cmds.push_back(ecf::Child::ABORT);
    child_cmds.push_back(ecf::Child::COMPLETE);
    return child_cmds;
}

} // namespace ecf
