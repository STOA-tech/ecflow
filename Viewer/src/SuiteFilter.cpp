//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "SuiteFilter.hpp"

#include <algorithm>

//=================================================================
//
// SuiteFilterItem
//
//=================================================================

SuiteFilterItem::SuiteFilterItem(const SuiteFilterItem& other)
{
	name_=other.name_;
	present_=other.present_;
	filtered_=other.filtered_;
}

//=================================================================
//
// SuiteFilterItem
//
//=================================================================

void SuiteFilter::current(const std::vector<std::string>& suites)
{
	current_=suites;
	adjust();
}

void SuiteFilter::adjust()
{
	items_.clear();

	//Items present in current_
	for(std::vector<std::string>::const_iterator it=current_.begin(); it != current_.end(); it++)
	{
		bool filtered=false;
		if(std::find(filter_.begin(), filter_.end(),*it) != filter_.end())
		{
			filtered=true;
		}

		items_.push_back(SuiteFilterItem(*it,true,filtered));
	}

	//Items present in filter_ only
	for(std::vector<std::string>::const_iterator it=filter_.begin(); it != filter_.end(); it++)
	{
		if(std::find(current_.begin(), current_.end(),*it) == current_.end())
		{
			items_.push_back(SuiteFilterItem(*it,false,true));
		}
	}
}

void SuiteFilter::setFiltered(int index,bool val)
{
	if(index >=0 && index < count())
	{
		items_.at(index).filtered_=val;

		const std::string& name=items_.at(index).name_;
		std::vector<std::string>::iterator it=std::find(filter_.begin(),filter_.end(),name);

		if(val == true)
		{
			if(it == filter_.end())
				filter_.push_back(name);
		}
		else
		{
			if(it != filter_.end())
				filter_.erase(it);
		}

	}
}

SuiteFilter* SuiteFilter::clone()
{
	SuiteFilter* sf=new SuiteFilter();
	sf->current_=current_;
	sf->filter_=filter_;
	sf->items_=items_;

	return sf;
}

bool SuiteFilter::update(SuiteFilter* sf)
{
	changeFlags_.clear();

	if(!sf)
		return false;

	if(sf->count() != count())
		return false;

	if(autoAddNew_ != sf->autoAddNewSuites())
	{
		autoAddNew_ = sf->autoAddNewSuites();
		changeFlags_.set(AutoAddChanged);
	}

	if(enabled_ != sf->isEnabled())
	{
		enabled_ = sf->isEnabled();
		changeFlags_.set(EnabledChanged);
	}

	if(filter_.size() != sf->filter_.size())
	{
		filter_=sf->filter_;
		changeFlags_.set(ItemChanged);
	}
	else
	{
		for(size_t i=0; i < filter_.size(); i++)
		{
			if(filter_[i] != sf->filter_.at(i))
			{
				filter_[i]=sf->filter_.at(i);
				changeFlags_.set(ItemChanged);
			}
		}
	}

	adjust();

	return (changeFlags_.isEmpty() == false);
}
