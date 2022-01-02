/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #26 $ 
//
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <stdexcept>
#include "Node.hpp"
#include "Limit.hpp"
#include "Ecf.hpp"
#include "LateAttr.hpp"
#include "MiscAttrs.hpp"
#include "Expression.hpp"

using namespace ecf;
using namespace std;

void Node::deleteTime(const std::string& name )
{
   if (name.empty()) {
       times_.clear();  // delete all
       state_change_no_ = Ecf::incr_state_change_no();
 #ifdef DEBUG_STATE_CHANGE_NO
       std::cout << "Node::deleteTime\n";
 #endif
       return;
    }
    TimeAttr attr( TimeSeries::create(name) ); // can throw if parse fails
    delete_time(attr);                         // can throw if search fails
}

void Node::delete_time( const ecf::TimeAttr& attr )
{
   size_t theSize = times_.size();
   for(size_t i = 0; i < theSize; i++) {
      // Dont use '==' since that compares additional state like free_
      if (times_[i].structureEquals(attr)) {
         times_.erase( times_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();

#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::delete_time\n";
#endif
         return;
      }
   }
   throw std::runtime_error("Node::delete_time: Cannot find time attribute: ");
}

void Node::deleteToday(const std::string& name)
{
   if (name.empty()) {
      todays_.clear();
      state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Node::deleteToday\n";
#endif
      return;
   }

   TodayAttr attr( TimeSeries::create(name) ); // can throw if parse fails
   delete_today(attr);                         // can throw if search fails
}

void Node::delete_today(const ecf::TodayAttr& attr)
{
   size_t theSize = todays_.size();
   for(size_t i = 0; i < theSize; i++) {
      // Dont use '==' since that compares additional state like free_
      if (todays_[i].structureEquals(attr)) {
         todays_.erase( todays_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::delete_today\n";
#endif
         return;
      }
   }
   throw std::runtime_error("Node::delete_today: Cannot find today attribute: " + attr.toString());
}

void Node::deleteDate(const std::string& name)
{
   if (name.empty()) {
      dates_.clear();
      state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Node::deleteDate\n";
#endif
      return;
   }

   DateAttr attr( DateAttr::create(name) ); // can throw if parse fails
   delete_date(attr);                       // can throw if search fails
}

void Node::delete_date(const DateAttr& attr)
{
   for(size_t i = 0; i < dates_.size(); i++) {
      // Dont use '==' since that compares additional state like free_
      if (attr.structureEquals(dates_[i]) ) {
         dates_.erase( dates_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::delete_date\n";
#endif
         return;
      }
   }
   throw std::runtime_error("Node::delete_date: Cannot find date attribute: " + attr.toString());
}

void Node::deleteDay(const std::string& name)
{
   if (name.empty()) {
       days_.clear();
       state_change_no_ = Ecf::incr_state_change_no();
 #ifdef DEBUG_STATE_CHANGE_NO
       std::cout << "Node::deleteDay\n";
 #endif
       return;
    }

    DayAttr attr( DayAttr::create(name) ); // can throw if parse fails.
    delete_day(attr);                      // can throw if search fails
}

void Node::delete_day(const DayAttr& attr)
{
   for(size_t i = 0; i < days_.size(); i++) {
       // Dont use '==' since that compares additional state like free_
       if (attr.structureEquals(days_[i]) ) {
          days_.erase( days_.begin() + i );
          state_change_no_ = Ecf::incr_state_change_no();
 #ifdef DEBUG_STATE_CHANGE_NO
          std::cout << "Node::delete_day\n";
 #endif
          return;
       }
    }
    throw std::runtime_error("Node::delete_day: Cannot find day attribute: " + attr.toString());
 }

void Node::deleteCron(const std::string& name)
{
   if (name.empty()) {
       crons_.clear();
       state_change_no_ = Ecf::incr_state_change_no();
 #ifdef DEBUG_STATE_CHANGE_NO
       std::cout << "Node::deleteCron\n";
 #endif
       return;
    }

    CronAttr attr = CronAttr::create(name); // can throw if parse fails
    delete_cron(attr);                      // can throw if search fails
}

void Node::delete_cron(const ecf::CronAttr& attr)
{
   for(size_t i = 0; i < crons_.size(); i++) {
      // Dont use '==' since that compares additional state like free_
      if (attr.structureEquals(crons_[i]) ) {
         crons_.erase( crons_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::delete_cron\n";
#endif
         return ;
      }
   }
   throw std::runtime_error("Node::delete_cron: Cannot find cron attribute: " + attr.toString());
}


void Node::deleteVariable( const std::string& name)
{
	if (name.empty()) {
		vars_.clear(); // delete all
		state_change_no_ = Ecf::incr_state_change_no();

#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteVariable\n";
#endif
		return;
	}

	size_t theSize = vars_.size();
	for(size_t i = 0; i < theSize; i++) {
		if (vars_[i].name() == name) {
 			vars_.erase( vars_.begin() + i );
 			state_change_no_ = Ecf::incr_state_change_no();

#ifdef DEBUG_STATE_CHANGE_NO
 			std::cout << "Node::deleteVariable\n";
#endif
			return;
		}
	}
	throw std::runtime_error("Node::deleteVariable: Cannot find 'user' variable of name " + name);
}

void Node::delete_variable_no_error( const std::string& name)
{
   if (name.empty()) {
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Node::delete_variable_no_error\n";
#endif
      return;
   }

   size_t theSize = vars_.size();
   for(size_t i = 0; i < theSize; i++) {
      if (vars_[i].name() == name) {
         vars_.erase( vars_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();

#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::delete_variable_no_error\n";
#endif
         return;
      }
   }
}

void Node::deleteEvent(const std::string& name)
{
   if (name.empty()) {
      events_.clear();
      state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Node::deleteEvent\n";
#endif
      return;
   }

   size_t theSize = events_.size();
   for(size_t i = 0; i < theSize; i++) {
      if (events_[i].name_or_number() == name) {
         events_.erase( events_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::deleteEvent\n";
#endif
         return;
      }
   }
	throw std::runtime_error("Node::deleteEvent: Cannot find event: " + name);
}

void Node::deleteMeter(const std::string& name)
{
   if (name.empty()) {
      meters_.clear();
      state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Expression::clearFree()\n";
#endif
      return;
   }

   size_t theSize = meters_.size();
   for(size_t i = 0; i < theSize; i++) {
      if (meters_[i].name() == name) {
         meters_.erase( meters_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Expression::clearFree()\n";
#endif
         return;
      }
   }
	throw std::runtime_error("Node::deleteMeter: Cannot find meter: " + name);
}

void Node::deleteLabel(const std::string& name)
{
   if (name.empty()) {
      labels_.clear();
      state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
      std::cout << "Node::deleteLabel\n";
#endif
      return;
   }

   size_t theSize = labels_.size();
   for(size_t i = 0; i < theSize; i++) {
      if (labels_[i].name() == name) {
         labels_.erase( labels_.begin() + i );
         state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
         std::cout << "Node::deleteLabel\n";
#endif
         return;
      }
   }
	throw std::runtime_error("Node::deleteLabel: Cannot find label: " + name);
}

void Node::delete_queue(const std::string& name)
{
   if (misc_attrs_)  {
      misc_attrs_->delete_queue(name);
      return;
   }
   throw std::runtime_error("Node::delete_queue: Cannot find queue: " + name);
}

void Node::delete_generic(const std::string& name)
{
   if (misc_attrs_)  {
      misc_attrs_->delete_generic(name);
      return;
   }
   throw std::runtime_error("Node::delete_generic : Cannot find generic: " + name);
}

void Node::deleteTrigger()
{
	if (t_expr_)  {
	   t_expr_.reset(nullptr);
		state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteTrigger()\n";
#endif
	}
}

void Node::deleteComplete()
{
	if (c_expr_) {
		c_expr_.reset(nullptr);
		state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteComplete()\n";
#endif
	}
}

void Node::deleteRepeat()
{
	if (!repeat_.empty()) {
		repeat_.clear(); // will delete the pimple
 		state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteRepeat())\n";
#endif
	}
}

void Node::deleteLimit(const std::string& name)
{
	if (name.empty()) {
		limits_.clear();
		state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteLimit\n";
#endif
  		return;
	}

	size_t theSize = limits_.size();
	for(size_t i = 0; i < theSize; i++) {
		if (limits_[i]->name() == name) {
			limits_.erase( limits_.begin() + i );
			state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
			std::cout << "Node::deleteLimit\n";
#endif
			return;
		}
	}
	throw std::runtime_error("Node::deleteLimit: Cannot find limit: " + name);
}

void Node::delete_limit_path(const std::string& name,const std::string& path)
{
   if (name.empty()) {
      throw std::runtime_error("Node::delete_limit_path: the limit name must be provided");
   }
   if (path.empty()) {
      throw std::runtime_error("Node::delete_limit_path: the limit path must be provided");
   }

   size_t theSize = limits_.size();
   for(size_t i = 0; i < theSize; i++) {
      if (limits_[i]->name() == name) {
         limits_[i]->delete_path(path); // will update state change no
         return;
      }
   }
   throw std::runtime_error("Node::delete_limit_path: Cannot find limit: " + name);
}

void Node::deleteInlimit(const std::string& name)
{
	// if name exists but no corresponding in limit found raises an exception
	if (inLimitMgr_.deleteInlimit(name)) {
		state_change_no_ = Ecf::incr_state_change_no();
#ifdef DEBUG_STATE_CHANGE_NO
		std::cout << "Node::deleteInlimit\n";
#endif
	}
}

void Node::delete_misc_attrs_if_empty()
{
   if (misc_attrs_ && misc_attrs_->empty()) {
      misc_attrs_.reset(nullptr);
   }
}

void Node::deleteZombie(const std::string& zombie_type)
{
   if (misc_attrs_) {
      misc_attrs_->deleteZombie(zombie_type);
      delete_misc_attrs_if_empty();
   }
}

void Node::delete_zombie(Child::ZombieType zt)
{
   if (misc_attrs_) {
      misc_attrs_->delete_zombie(zt);
      delete_misc_attrs_if_empty();
   }
}

void Node::deleteLate()
{
   late_.reset(nullptr);
   state_change_no_ = Ecf::incr_state_change_no();
}


