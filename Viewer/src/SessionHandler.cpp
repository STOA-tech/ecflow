//============================================================================
// Copyright 2016 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include <algorithm>
#include <assert.h>
#include <fstream>

#include "SessionHandler.hpp"
#include "DirectoryHandler.hpp"
#include "Str.hpp"
#include "UserMessage.hpp"


SessionHandler* SessionHandler::instance_=0;


SessionItem::SessionItem(const std::string& name) :
  name_(name)
{
	checkDir();
}

void SessionItem::checkDir()
{
	dirPath_ = SessionHandler::sessionDirName(name_);
	DirectoryHandler::createDir(dirPath_);

    qtPath_= SessionHandler::sessionQtDirName(name_);
    DirectoryHandler::createDir(qtPath_);
}

std::string SessionItem::sessionFile() const
{
	return DirectoryHandler::concatenate(dirPath_, "session.json");
}

std::string SessionItem::windowFile() const
{
	return DirectoryHandler::concatenate(dirPath_, "window.conf");
}

std::string SessionItem::settingsFile() const
{
	return DirectoryHandler::concatenate(dirPath_, "settings.json");
}

std::string SessionItem::recentCustomCommandsFile() const
{
    return DirectoryHandler::concatenate(dirPath_, "recent_custom_commands.json");
}

std::string SessionItem::savedCustomCommandsFile() const
{
    return DirectoryHandler::concatenate(dirPath_, "saved_custom_commands.json");
}

std::string SessionItem::serverFile(const std::string& serverName) const
{
	return DirectoryHandler::concatenate(dirPath_, serverName + ".conf.json");
}

std::string SessionItem::qtDir() const
{
    return qtPath_;
}

std::string SessionItem::qtSettingsFile(const std::string name) const
{
    return DirectoryHandler::concatenate(qtPath_, name + ".conf");
}

//=================================================
//
// SessionHandler
//
//=================================================

SessionHandler::SessionHandler() :
	current_(0)
{
	//The default must always be exist!
	current_=add(defaultSessionName());
	loadedLastSessionName_ = false;

	readSessionListFromDisk();
	readLastSessionName();
}

SessionHandler* SessionHandler::instance()
{
	if(!instance_)
	{
		instance_=new SessionHandler();
	}

	return instance_;
}

std::string SessionHandler::sessionDirName(const std::string &sessionName)
{
	return DirectoryHandler::concatenate(DirectoryHandler::configDir(), sessionName + ".session");
}

std::string SessionHandler::sessionQtDirName(const std::string &sessionName)
{
	std::string basedir = sessionDirName(sessionName);
	return DirectoryHandler::concatenate(basedir, "qt");
}

SessionItem* SessionHandler::find(const std::string& name)
{
    for(std::vector<SessionItem*>::const_iterator it=sessions_.begin(); it != sessions_.end(); ++it)
    {
        if((*it)->name() == name)
            return *it;
    }
    return NULL;

}


// returns -1 if the session name is not found
int SessionHandler::indexFromName(const std::string& name)
{
    int n = 0;
    for(std::vector<SessionItem*>::const_iterator it=sessions_.begin(); it != sessions_.end(); ++it)
    {
        if((*it)->name() == name)
            return n;
        n++;
    }
    return -1;
}

void SessionHandler::readSessionListFromDisk()
{
    // get the list of existing sessions (by listing the directories)
    std::string configDir = DirectoryHandler::configDir();
    std::string filter = ".*\\.session";
    std::vector<std::string> dirs;
    DirectoryHandler::findDirs(configDir, filter, dirs);

    // add each session to our list (but remove the .session first)
    for(std::vector<std::string>::const_iterator it=dirs.begin(); it != dirs.end(); ++it)
    {
        std::string dirName       = (*it);
        std::string toRemove      = ".session";
        std::string toReplaceWith = "";
        ecf::Str::replace(dirName, toRemove, toReplaceWith);
        add(dirName);
    }
}

bool SessionHandler::loadLastSessionAtStartup()
{
    // if there was a last session file, then it means the user wanted to load at startup
    return loadedLastSessionName_;
}



SessionItem* SessionHandler::add(const std::string& name)
{
	// only add if not already there
	if (find(name) == NULL)
	{
		SessionItem *item=new SessionItem(name);
		sessions_.push_back(item);
		return item;
	}
	else
		return NULL;
}

void SessionHandler::remove(const std::string& sessionName)
{
	SessionItem *session = find(sessionName);
	assert(session);

	remove(session);
}

void SessionHandler::remove(SessionItem* session)
{
    std::vector<SessionItem*>::iterator it = std::find(sessions_.begin(), sessions_.end(), session);
    assert(it != sessions_.end());  // session was not found - should not be possible


    // remove the session's directory
    std::string errorMessage;
    bool ok = DirectoryHandler::removeDir(sessionDirName(session->name()), errorMessage);

    if (ok)
    {
        // remove it from our list
        sessions_.erase(it);
    }
    else
    {
        UserMessage::message(UserMessage::ERROR, true, errorMessage);
    }
}


void SessionHandler::rename(SessionItem* item, const std::string& newName)
{
    std::string errorMessage;
    bool ok = DirectoryHandler::renameDir(sessionDirName(item->name()), sessionDirName(newName), errorMessage);

    if (ok)
    {
        item->name(newName);
    }
    else
    {
        UserMessage::message(UserMessage::ERROR, true, errorMessage);
    }
}


void SessionHandler::current(SessionItem* item)
{
	if(std::find(sessions_.begin(),sessions_.end(),item) != sessions_.end())
	{
		current_=item;
		load();
	}
}


SessionItem* SessionHandler::current()
{
	return current_;
}

void SessionHandler::save()
{
	if(current_)
	{
		//current_->save();
	}
}

void SessionHandler::load()
{

}


bool SessionHandler::requestStartupViaSessionManager()
{
	char *sm = getenv("ECFUI_SESSION_MANAGER");
	if (sm)
		return true;
	else
		return false;
}


void SessionHandler::saveLastSessionName()
{
	std::string configDir = DirectoryHandler::configDir();
	std::string lastSessionFilename = DirectoryHandler::concatenate(configDir, "last_session.txt");

	// open the last_session.txt file and try to read it
	std::ofstream out(lastSessionFilename.c_str());

	if (out.good())
	{
		// the file is a one-line file containing the name of the current session
		std::string line = current()->name();
		out << line << std::endl;
	}
}

void SessionHandler::removeLastSessionName()
{
	std::string configDir = DirectoryHandler::configDir();
	std::string lastSessionFilename = DirectoryHandler::concatenate(configDir, "last_session.txt");

	std::string errorMessage;
	bool ok = DirectoryHandler::removeFile(lastSessionFilename, errorMessage);

	if (!ok)
	{
		UserMessage::message(UserMessage::ERROR, true, errorMessage);
	}
}


void SessionHandler::readLastSessionName()
{
	std::string configDir = DirectoryHandler::configDir();
	std::string lastSessionFilename = DirectoryHandler::concatenate(configDir, "last_session.txt");

	// open the last_session.txt file and try to read it
	std::ifstream in(lastSessionFilename.c_str());

	if (in.good())
	{
		// the file is a one-line file containing the name of the session we want
		std::string line;
		if (getline(in, line))
		{
			loadedLastSessionName_ = true;
			lastSessionName_ = line;
		}
		else
			lastSessionName_ = defaultSessionName();
	}
	else
	{
		// could not read the file, so just use the default
		lastSessionName_ = defaultSessionName();
	}


	// set this session as the current one if it exists
	SessionItem *item = find(lastSessionName_);
	if (item)
	{
		current(item);
	}
	else
	{
		lastSessionName_ = defaultSessionName();  // the last session file contained the name of a non-existant session
	}
}


SessionItem *SessionHandler::copySession(SessionItem* source, std::string &destName)
{
	// the main work is to make a copy of the source session's directory (recursively)
	std::string errorMessage;
	std::string sourceSessionDir = sessionDirName(source->name());
	std::string destSessionDir   = sessionDirName(destName);
	bool ok = DirectoryHandler::copyDir(sourceSessionDir, destSessionDir, errorMessage);
	if (ok)
	{
		// add it to our list
		SessionItem *newItem = add(destName);
		return newItem;
	}
	else
	{
		UserMessage::message(UserMessage::ERROR, true, errorMessage);
		return NULL;
	}
}

SessionItem *SessionHandler::copySession(std::string &source, std::string &destName)
{
	SessionItem *sourceSession = find(source);
	assert(sourceSession);

	copySession(sourceSession, destName);
}

