//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef LOGVIEWERPROC_HPP
#define LOGVIEWERPROC_HPP

#ifdef ECFLOW_LOGVIEW

    #include <sstream>
    #include <string>
    #include <vector>

    #include <QDateTime>
    #include <QObject>

class ServerHandler;
class QLocalSocket;

class LogViewerCom : public QObject {
public:
    LogViewerCom();

    void addToApp(ServerHandler*);
    void closeApp();

protected:
    void start(QStringList);

    QString program_;
    QString logViewerId_;
};

#endif // ECFLOW_LOGVIEW

#endif // LOGVIEWERPROC_HPP
