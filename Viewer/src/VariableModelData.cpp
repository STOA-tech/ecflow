//============================================================================
// Copyright 2015 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "VariableModelData.hpp"

#include "ServerHandler.hpp"
#include "VNState.hpp"

#include <QDebug>
#include <QString>

static std::string defaultStr("");

//==========================================
//
// VariableModelData
//
//==========================================

VariableModelData::VariableModelData(VInfo_ptr info) :
		info_(info)
{
	reload();
}

void VariableModelData::clear()
{
	vars_.clear();
	genVars_.clear();
}

void VariableModelData::reload()
{
	clear();

	info_->variables(vars_);
	info_->genVariables(genVars_);
}

std::string VariableModelData::fullPath()
{
	return info_->fullPath();
}

std::string VariableModelData::name()
{
	return info_->name();
}

std::string VariableModelData::type()
{
	return (info_->isNode())?"node":"server";
}

const std::string& VariableModelData::name(int index) const
{
	if(index < 0 || index >= varNum())
		return defaultStr;

	if(!isGenVar(index))
	{
		return vars_.at(index).name();
	}
	else
	{
		return genVars_.at(index-vars_.size()).name();
	}

	return defaultStr;
}

const std::string& VariableModelData::value(int index) const
{
	if(index < 0 || index >= varNum())
		return defaultStr;

	if(!isGenVar(index))
	{
		return vars_.at(index).theValue();
	}
	else
	{
		return genVars_.at(index-vars_.size()).theValue();
	}

	return defaultStr;
}

bool VariableModelData::hasName(const std::string& n) const
{
	for(std::vector<Variable>::const_iterator it=vars_.begin(); it != vars_.end(); it++)
	{
		if((*it).name() == n)
		{
			return true;
		}
	}

	for(std::vector<Variable>::const_iterator it=genVars_.begin(); it != genVars_.end(); it++)
	{
		if((*it).name() == n)
		{
			return true;
		}
	}

	return false;

}

void VariableModelData::buildAlterCommand(std::vector<std::string>& cmd,
		                            const std::string& action, const std::string& type,
		                            const std::string& name,const std::string& value)
{
	cmd.push_back("ecflow_client");
	cmd.push_back("--alter");
	cmd.push_back(action);
	cmd.push_back(type);

	if(!name.empty())
	{
		cmd.push_back(name);
		cmd.push_back(value);
	}

	cmd.push_back("<full_name>");

}

void VariableModelData::setValue(int index,const std::string& val)
{
	std::vector<std::string> cmd;
	buildAlterCommand(cmd,"change","variable",name(index),val);

	ServerHandler::command(info_,cmd,false);
}

void VariableModelData::add(const std::string& name,const std::string& val)
{
	std::vector<std::string> cmd;
	buildAlterCommand(cmd,(hasName(name))?"change":"add","variable",name,val);

	ServerHandler::command(info_,cmd,false);
}

void VariableModelData::remove(int index,const std::string& varName)
{
	if(varName == name(index))
	{
		std::vector<std::string> cmd;
		buildAlterCommand(cmd,"delete","variable",varName,"");

		ServerHandler::command(info_,cmd,false);
	}
}

bool VariableModelData::isGenVar(int index) const
{
	return (index >= vars_.size());
}

int VariableModelData::varNum() const
{
	return vars_.size() + genVars_.size();
}


//Check if the total number of variables will change. It does not update the local data!
int VariableModelData::checkUpdateDiff()
{
	std::vector<Variable> v;
	std::vector<Variable> vg;

	info_->variables(v);
	info_->genVariables(vg);

	//Return the change in the total size of variables
	return v.size()+vg.size() -(vars_.size() + genVars_.size());
}


//Check if any of the values has changed. We suppose that the number of current and new
//variables are the same but some of their names or values have been changed.
bool VariableModelData::update()
{
	std::vector<Variable> v;
	std::vector<Variable> vg;

	info_->variables(v);
	info_->genVariables(vg);

	//We must have the same numbe rof variables
	assert(v.size() == vars_.size() && vg.size() == genVars_.size());

	bool changed=false;

	for(unsigned int i=0; i < vars_.size(); i++)
	{
		if(vars_[i].name() != v[i].name() || vars_[i].theValue() != v[i].theValue())
		{
			changed=true;
			break;
		}
	}

	if(changed == false)
	{
		for(unsigned int i=0; i < genVars_.size(); i++)
		{
			if(genVars_[i].name() != vg[i].name() || genVars_[i].theValue() != vg[i].theValue())
			{
				changed=true;
				break;
			}
		}
	}

	if(changed)
	{
		vars_=v;
		genVars_=vg;
	}

	return changed;

}

//==========================================
//
// VariableModelDataHandler
//
//==========================================

VariableModelDataHandler::VariableModelDataHandler() : server_(0)
{
}

VariableModelDataHandler::~VariableModelDataHandler()
{
	clear();
}
void VariableModelDataHandler::reload(VInfo_ptr info)
{
	//Notifies the model that a change will happen
	Q_EMIT reloadBegin();

	clear();

	server_=0;
	std::vector<VNode*> nodes;

	if(info.get() != 0)
	{
		info->ancestors(&server_,nodes);

		for(std::vector<VNode*>::iterator it=nodes.begin(); it != nodes.end(); it++)
		{
			VInfo_ptr info(VInfo::make(*it));
			data_.push_back(new VariableModelData(info));
		}

		if(server_)
		{
			VInfo_ptr info(VInfo::make(server_));
			data_.push_back(new VariableModelData(info));
		}

	}

	Q_EMIT reloadEnd();

	//Reset the model (views will be n
}

void VariableModelDataHandler::reload()
{
	//Notifies the model that a change will happen
	Q_EMIT reloadBegin();

	for(std::vector<VariableModelData*>::iterator it=data_.begin(); it != data_.end(); it++)
	{
		(*it)->reload();
	}

	Q_EMIT reloadEnd();
}


void VariableModelDataHandler::clear()
{
	//Notifies the model that a change will happen
	Q_EMIT reloadBegin();

	for(std::vector<VariableModelData*>::iterator it=data_.begin(); it != data_.end(); it++)
	{
		delete *it;
	}

	data_.clear();

	Q_EMIT reloadEnd();
}

int VariableModelDataHandler::varNum(int index) const
{
	if(index >=0 && index < data_.size())
		return data_.at(index)->varNum();

	return -1;
}

VariableModelData* VariableModelDataHandler::data(int index) const
{
	if(index >=0 && index < data_.size())
		return data_.at(index);

	return 0;
}

//It is called when a node was changed.
void VariableModelDataHandler::nodeChanged(const VNode* node, const std::vector<ecf::Aspect::Type>& aspect)
{
	bool changed=false;

	//Check if some variables were added or removed.
	for(std::vector<ecf::Aspect::Type>::const_iterator it=aspect.begin(); it != aspect.end(); it++)
	{
		if(*it == ecf::Aspect::ADD_REMOVE_ATTR)
		{
			bool changed=false;
			for(unsigned int i=0; i < data_.size(); i++)
			{
				//Do not check the server
				if(data_.at(i)->type() == "server")
					continue;

				//If the number of the variables not the same we reset the whole model
				int cntDiff=data_.at(i)->checkUpdateDiff();
				if(cntDiff != 0)
				{
					//Notifies the model that rows will be added or removed for this data item
					Q_EMIT addRemoveBegin(i,cntDiff);
					data_.at(i)->reload();
					Q_EMIT addRemoveEnd(cntDiff);

					//Update the data item in the model
					Q_EMIT dataChanged(i);
				}
			}

			break;
		}
	}


	//Check if some variables' value changed
	for(std::vector<ecf::Aspect::Type>::const_iterator it=aspect.begin(); it != aspect.end(); it++)
	{
		//A variable's value changed
		if(*it == ecf::Aspect::NODE_VARIABLE)
		{
			//Do not check the server
			for(unsigned int i=0; i < data_.size(); i++)
			{
				//Do not check the server
				if(data_.at(i)->type() == "server")
					continue;

				//Update the names/values
				if(data_.at(i)->update())
				{
					//Update the data item in the model
					Q_EMIT dataChanged(i);
				}
			}

			break;
		}
	}
}

//ADD_REMOVE_ATTR?????

//It is called when the server defs was changed
void VariableModelDataHandler::defsChanged(const std::vector<ecf::Aspect::Type>& aspect)
{
	if(data_.size() == 0)
		return;

	VariableModelData* d=data_.at(data_.size()-1);
	if(d->type() != "server")
	{
		return;
	}

	int dIndex=data_.size()-1;

	for(std::vector<ecf::Aspect::Type>::const_iterator it=aspect.begin(); it != aspect.end(); it++)
	{
		if(*it == ecf::Aspect::SERVER_VARIABLE)
		{
			//Check if some variables were added or removed
			int cntDiff=d->checkUpdateDiff();
			if(cntDiff != 0)
			{
				//Notifies the model that rows will be added or removed for this data item
				Q_EMIT addRemoveBegin(dIndex,cntDiff);
				d->reload();
				Q_EMIT addRemoveEnd(cntDiff);

				//Update the data item in the model
				Q_EMIT dataChanged(dIndex);
			}
			//Otherwise Update the names/values
			else if(d->update())
			{
				//Update the data item in the model
				Q_EMIT dataChanged(dIndex);
			}

			break;
		}
	}
}
