//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================
#ifndef VIEWER_SRC_NODEQUERYRESULTTMP_HPP_
#define VIEWER_SRC_NODEQUERYRESULTTMP_HPP_

#include <QString>
#include <QStringList>
#include <memory>

class VNode;

struct NodeQueryResultTmp;
typedef std::shared_ptr<NodeQueryResultTmp> NodeQueryResultTmp_ptr;

struct  NodeQueryResultTmp
{
	NodeQueryResultTmp()= default;
    NodeQueryResultTmp(VNode *node) : node_(node) {}
    NodeQueryResultTmp(VNode *node,QStringList attr) : node_(node), attr_(attr) {}

	VNode* node_{nullptr};
	QStringList attr_;
};

#endif /* VIEWER_SRC_NODEQUERYRESULTTMP_HPP_ */
