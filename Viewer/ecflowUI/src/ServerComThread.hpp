//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef SERVERCOMTHREAD_HPP_
#define SERVERCOMTHREAD_HPP_

#include <deque>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <QThread>

#include "AbstractObserver.hpp"
#include "Defs.hpp"
#include "VTask.hpp"

class ChangeMgrSingleton;
class ClientInvoker;
class ServerComQueue;
class ServerHandler;

// -------------------------------------------------------
// ServerComThread - a class to handler communication with
// an ecflow server.
// -------------------------------------------------------

class ServerComThread : public QThread, public AbstractObserver {
    Q_OBJECT

public:
    ServerComThread(ServerHandler* server, ClientInvoker* ci);
    ~ServerComThread() override;

    void task(VTask_ptr);
    void blockingLogout();

    // From AbstractObserver
    void update_start(const Node*, const std::vector<ecf::Aspect::Type>&) override {}
    void update_start(const Defs*, const std::vector<ecf::Aspect::Type>&) override {}
    void update(const Node*, const std::vector<ecf::Aspect::Type>&) override;
    void update(const Defs*, const std::vector<ecf::Aspect::Type>&) override;
    void update_delete(const Node*) override;
    void update_delete(const Defs*) override;

Q_SIGNALS:
    void nodeChanged(const Node*, std::vector<ecf::Aspect::Type>);
    void defsChanged(std::vector<ecf::Aspect::Type>);
    void rescanNeed();
    void failed(std::string message);
    void suiteListChanged(const std::vector<std::string>&, const std::vector<std::string>&);
    void logoutDone();

protected:
    void run() override;
    void reset();

private:
    void attach();
    void attach(defs_ptr d);
    void attach(Node* node);
    void detach();
    void detach(defs_ptr d);
    void detach(Node* node);
    void aspectToStr(std::stringstream& s, const std::vector<ecf::Aspect::Type>& t) const;

    ServerHandler* server_{nullptr};
    std::string serverName_;
    ClientInvoker* ci_{nullptr};
    VTask::Type taskType_{VTask::NoTask};
    std::vector<std::string> command_;
    std::string commandAsStr_;
    std::map<std::string, std::string> params_;
    std::vector<std::string> contents_;
    NameValueVec vars_;
    Zombie zombie_;
    std::string nodePath_;
    bool rescanNeed_{false};
    bool hasSuiteFilter_{false};
    std::vector<std::string> filteredSuites_;
    bool autoAddNewSuites_{false};
    int maxLineNum_{-1};
    bool initialResetDone_{false};
};

#endif
