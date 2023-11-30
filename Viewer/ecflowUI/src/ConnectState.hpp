/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_ConnectState_HPP
#define ecflow_viewer_ConnectState_HPP

#include <ctime>
#include <string>

class ConnectState {
public:
    ConnectState();

    enum State {
        Undef,
        Normal,
        Disconnected,
        Lost,
        VersionIncompatible,
        SslIncompatible,
        SslCertificateError,
        FailedClient
    };

    void state(State state);
    State state() const { return state_; }
    const std::string& describe() const;
    void errorMessage(const std::string&);
    std::time_t lastConnectTime() const { return lastConnect_; }
    std::time_t lastLostTime() const { return lastFailed_; }
    std::time_t lastDisconnectTime() const { return lastDisconnect_; }
    const std::string& errorMessage() const { return errMsg_; }
    const std::string& shortErrorMessage() const { return shortErrMsg_; }

protected:
    static void init();
    void logConnect();
    void logFailed();
    void logDisconnect();

    State state_{Undef};
    std::time_t lastConnect_{0};
    std::time_t lastFailed_{0};
    std::time_t lastDisconnect_{0};
    std::string errMsg_;
    std::string shortErrMsg_;
};

#endif /* ecflow_viewer_ConnectState_HPP */
