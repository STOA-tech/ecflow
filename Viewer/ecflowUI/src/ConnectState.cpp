/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "ConnectState.hpp"

#include <map>

static std::map<ConnectState::State, std::string> descMap;

ConnectState::ConnectState() {
    init();
}

void ConnectState::init() {
    if (descMap.empty()) {
        descMap[Undef]        = "";
        descMap[Lost]         = "Connection to server lost";
        descMap[Disconnected] = "Server is disconnected";
        descMap[Normal]       = "Server is connected";
    }
}

const std::string& ConnectState::describe() const {
    static std::string empty = "";

    auto it                  = descMap.find(state_);
    if (it != descMap.end()) {
        return it->second;
    }
    return empty;
}
void ConnectState::state(State state) {
    state_ = state;

    switch (state_) {
        case Normal:
            logConnect();
            break;
        case Lost:
            logFailed();
            break;
        case Disconnected:
            logDisconnect();
            break;
        default:
            break;
    }
}

void ConnectState::logConnect() {
    lastConnect_ = time(nullptr);
}

void ConnectState::logFailed() {
    lastFailed_ = time(nullptr);
}

void ConnectState::logDisconnect() {
    lastDisconnect_ = time(nullptr);
}

void ConnectState::errorMessage(const std::string& str) {
    errMsg_         = str;

    std::size_t pos = str.find("Client environment:");
    if (pos != std::string::npos) {
        shortErrMsg_ = str.substr(0, pos);
    }
    else {
        shortErrMsg_ = str;
    }
}
