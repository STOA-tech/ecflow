//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "InfoProvider.hpp"
#include "VNode.hpp"
#include "VReply.hpp"
#include "ServerHandler.hpp"

#include <boost/algorithm/string/predicate.hpp>

InfoProvider::InfoProvider(InfoPresenter* owner,VTask::Type taskType) :
	owner_(owner),
	taskType_(taskType)
{
	reply_=new VReply();
}

InfoProvider::~InfoProvider()
{
	delete reply_;
	if(task_)
		task_->status(VTask::CANCELLED);
}

void InfoProvider::info(VInfo_ptr info)
{
	//We keep it alive
	info_=info;

	if(task_)
	{
		task_->status(VTask::CANCELLED);
		task_.reset();
	}

	if(owner_ && info_)
		info_->accept(this);
}

//Server
void InfoProvider::visit(VInfoServer* info)
{
    reply_->reset();

    if(!info->server())
    {
        owner_->infoFailed(reply_);
    }

    //Define a task for getting the info from the server.
    task_=VTask::create(taskType_,this);

    //Run the task in the server. When it completes taskFinished() is called. The text returned
    //in the reply will be prepended to the string we generated above.
    info->server()->run(task_);
}

//Node
void InfoProvider::visit(VInfoNode* info)
{
    reply_->reset();

    if(!info->node() || !info->node()->node())
    {
        owner_->infoFailed(reply_);
    }

    //Check if we have a server
    if(!info->server())
    {
    	owner_->infoFailed(reply_);
    }

    VNode *n=info->node();

    std::string fileName;

    bool done=false;

    //We try to read the file directly from the disk
    if(info->server()->readFromDisk())
    {
    	//There is a variable defined for the filename
    	if(!fileVarName_.empty())
    	{
    		//Get the fileName
    		fileName=n->genVariable(fileVarName_);

    		if(reply_->textFromFile(fileName))
    		{
    			owner_->infoReady(reply_);
    			return;
    		}
    		else if(handleFileMissing(fileName,reply_))
    		{
    			return;
    		}
    	}
    }

    //We try to get the file contents from the server
    //(this will go through the threaded communication)

    //Define a task for getting the info from the server.
    task_=VTask::create(taskType_,n,this);

    //Run the task in the server. When it finish taskFinished() is called. The text returned
    //in the reply will be prepended to the string we generated above.
    info->server()->run(task_);

}

void InfoProvider::handleFileNotDefined(VReply *reply)
{
	reply->setInfoText(fileNotDefinedText_);
	owner_->infoReady(reply_);
}

bool InfoProvider::handleFileMissing(const std::string& fileName,VReply *reply)
{
	return false;

	//reply->setWarningText(fileMissingText_);
	//owner_->infoReady(reply_);
}

void  InfoProvider::taskChanged(VTask_ptr task)
{
    if(task_ != task)
        return;

    switch(task->status())
    {
        case VTask::FINISHED:
            //We prepend the results to the existing text
            reply_->text(task->reply()->text());
            owner_->infoReady(reply_);
            //We do not need the task anymore.
            task_.reset();
            break;
        case VTask::ABORTED:
        case VTask::REJECTED:
           reply_->setErrorText(task->reply()->errorText());
           owner_->infoFailed(reply_);
           //We do not need the task anymore.
           task_.reset();break;
        case VTask::CANCELLED:
            if(!task->reply()->errorText().empty())
        	{
            	reply_->setErrorText(task->reply()->errorText());
            	owner_->infoFailed(reply_);
        	}
            //We do not need the task anymore.
            task_.reset();break;
        default:
            break;
    }
}

JobProvider::JobProvider(InfoPresenter* owner) :
		InfoProvider(owner,VTask::JobTask)
{
	fileVarName_="ECF_JOB";

	fileNotDefinedText_="Job is <b>not</b> defined";

	fileMissingText_="Job <b>not</b> found! <br> Check <b>ECF_HOME</b> directory  \
				 for read/write access. Check for file presence and read access below. \
	             The file may have been deleted or this may be a '<i>dummy</i>' task";
}

bool JobProvider::handleFileMissing(const std::string& fileName,VReply *reply)
{
	if(fileName.find(".job0") != std::string::npos)
	{
		reply->setInfoText("No job to be expected when <b>TRYNO</b> is 0!");
		owner_->infoReady(reply_);
		return true;
	}


	/*else
	{
		reply->setWarningText(fileMissingText_);
	}*/
	return false;
}

ManualProvider::ManualProvider(InfoPresenter* owner) :
	InfoProvider(owner,VTask::ManualTask)
{
	fileVarName_="ECF_MANUAL";
	fileNotDefinedText_="Manual is <b>not</b> available";
	fileMissingText_="Manual is <b>not</b> available";
}

MessageProvider::MessageProvider(InfoPresenter* owner) :
	InfoProvider(owner,VTask::MessageTask)
{

}

ScriptProvider::ScriptProvider(InfoPresenter* owner) :
		InfoProvider(owner,VTask::ScriptTask)
{
	fileVarName_="ECF_SCRIPT";
	fileNotDefinedText_="Script is <b>not</b> defined";
	fileMissingText_="Script <b>not/b> found! <br> Check <b>ECF_FILES</b> or <b>ECF_HOME</b> directories,  \
			 for read access. Check for file presence and read access below files directory \
             or this may be a '<i>dummy</i>' task";
}

HistoryProvider::HistoryProvider(InfoPresenter* owner) :
	InfoProvider(owner,VTask::HistoryTask)
{

}

ZombieProvider::ZombieProvider(InfoPresenter* owner) :
		InfoProvider(owner,VTask::ZombieListTask)
{

}




