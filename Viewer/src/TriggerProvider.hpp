//============================================================================
// Copyright 2016 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef TRIGGERPROVIDER_HPP
#define TRIGGERPROVIDER_HPP

#include <QObject>

#include "VDir.hpp"
#include "VInfo.hpp"
#include "InfoProvider.hpp"
#include "VTask.hpp"
#include "VTaskObserver.hpp"

class TriggerProvider : public QObject, public InfoProvider
{
//Q_OBJECT

public:
     explicit TriggerProvider(InfoPresenter* owner);

     void visit(VInfoNode*);
     void clear();

//private Q_SLOTS:
//	void slotOutputClientError(QString);
//    void slotOutputClientProgress(QString,int);
//	void slotOutputClientFinished();

private:
   // void fetchDir(ServerHandler*,VNode*);
    //bool fetchDirViaOutputClient(VNode *n,const std::string& fileName);
    //VDir_ptr fetchLocalDir(const std::string& path);
    //OutputDirClient* makeOutputClient(const std::string& host,const std::string& port);

    //OutputDirClient *outClient_;
};


#endif