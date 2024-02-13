/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_LogProvider_HPP
#define ecflow_viewer_LogProvider_HPP

#include <QObject>
#include <QStringList>

#include "InfoProvider.hpp"
#include "VDir.hpp"
#include "VInfo.hpp"
#include "VTask.hpp"
#include "VTaskObserver.hpp"

class FileWatcher;

class LogProvider : public QObject, public InfoProvider {
    Q_OBJECT

public:
    explicit LogProvider(InfoPresenter* owner, QObject* parent = nullptr);

    void visit(VInfoServer*) override;
    void clear() override;
    void setAutoUpdate(bool) override;

public Q_SLOTS:
    void slotLinesAppend(QStringList);

private:
    void fetchFile();
    void fetchFile(ServerHandler* server, const std::string& fileName);
    void watchFile(const std::string&, size_t);
    void stopWatchFile();

    FileWatcher* fileWatcher_;
};

#endif /* ecflow_viewer_LogProvider_HPP */
