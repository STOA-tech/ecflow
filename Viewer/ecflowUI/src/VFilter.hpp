//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef VIEWFILTER_HPP_
#define VIEWFILTER_HPP_

#include <set>
#include <vector>

#include <QObject>
#include <QStringList>

#include "VInfo.hpp"
#include "VParam.hpp"

#include "Node.hpp"

class NodeQuery;
class NodeFilterEngine;
class ServerFilter;
class ServerHandler;
class VAttribute;
class VAttributeType;
class VInfo;
class VNode;
class VSettings;
class VTree;

#include <boost/property_tree/ptree.hpp>

class VParamSet : public QObject
{
Q_OBJECT

public:
	VParamSet();
    ~VParamSet() override = default;

    const std::vector<VParam*>& all() const {return all_;}
    const std::vector<VParam*>& current() const {return current_;}
    QStringList currentAsList() const;
    void setCurrent(const std::vector<VParam*>&,bool broadcast=true);
    void setCurrent(const std::vector<std::string>&,bool broadcast=true);
    void setCurrent(QStringList,bool broadcast=true);

    bool isEmpty() const {return empty_;}
    bool isComplete() const { return complete_;}
	bool isSet(const std::string&) const;
	bool isSet(VParam*) const;

	void writeSettings(VSettings* vs);
    void virtual readSettings(VSettings* vs);

Q_SIGNALS:
	void changed();

protected:
	void init(const std::vector<VParam*>& items);
    void addToCurrent(VParam*);

    std::vector<VParam*> all_;
    std::vector<VParam*> current_;
    std::string settingsId_;
    std::string settingsIdV0_;

private:
     void clearCurrent();

     std::vector<int> currentCache_; //we use to speed up the check in isSet()
     bool empty_{true};
     bool complete_{false};
};

class NodeStateFilter : public VParamSet
{
public:
	NodeStateFilter();
};

class AttributeFilter : public VParamSet
{
public:
	AttributeFilter();
    bool matchForceShowAttr(const VNode*,VAttributeType*) const;
    void setForceShowAttr(VAttribute* a);
    void clearForceShowAttr();
    void updateForceShowAttr();
    VAttribute* forceShowAttr() const;
    void readSettings(VSettings* vs) override;

private:
    std::vector<VParam*> defaults() const;

    VInfo_ptr forceShowAttr_;
};

class IconFilter : public VParamSet
{
public:
    IconFilter();
    void readSettings(VSettings* vs) override;
};


class TreeNodeFilter;
class TableNodeFilter;

class NodeFilterDef : public QObject
{
Q_OBJECT

friend class  TreeNodeFilter;
friend class  TableNodeFilter;

public:
	enum Scope {NodeStateScope,GeneralScope};
	NodeFilterDef(ServerFilter*,Scope);
	~NodeFilterDef() override;

	NodeStateFilter* nodeState() const {return nodeState_;}

	const std::string& exprStr() const {return exprStr_;}
	NodeQuery* query() const;
	void setQuery(NodeQuery*);
    void serverRemoved(const std::string& serverName);
    void serverRenamed(const std::string& newName, const std::string& oldName);

	void writeSettings(VSettings *vs);
	void readSettings(VSettings *vs);

Q_SIGNALS:
	void changed();

protected:
    ServerFilter *serverFilter_{nullptr};
    std::string exprStr_{"state = all"};
    NodeStateFilter *nodeState_{nullptr};
	std::string nodePath_;
	std::string nodeType_;
    NodeQuery* query_{nullptr};

	//AttributeFilter *attribute_;
	//std::string nodeType_;
	//std::string nodeName_;
};


class NodeFilter
{
    friend class NodeFilterEngine;
    friend class VTreeServer;

public:
    enum MatchMode {NoneMatch,AllMatch,VectorMatch};

    NodeFilter(NodeFilterDef* def,ServerHandler*);
	virtual ~NodeFilter();
    NodeFilter(const NodeFilter&) = delete;
    NodeFilter& operator=(const NodeFilter&) = delete;

    virtual void clear();
    virtual bool isNull()=0;
    virtual bool isComplete()=0;
    virtual int  matchCount() const = 0;
    virtual bool update()=0;

    VNode* forceShowNode() const {return forceShowNode_;}
    void setForceShowNode(VNode*);
    void clearForceShowNode();

protected:
    NodeFilterDef* def_{nullptr};
    NodeFilterEngine* queryEngine_{nullptr};
    std::set<std::string> type_;
    MatchMode matchMode_{VectorMatch};
    std::vector<VNode*> match_;
    ServerHandler * server_{nullptr};
    VNode* forceShowNode_{nullptr};
};

class TreeNodeFilter : public NodeFilter
{
public:
    explicit TreeNodeFilter(NodeFilterDef* def,ServerHandler*,VTree*);

    void clear() override;
    bool isNull() override;
    bool isComplete() override;
    int  matchCount() const override {return 0;}
    bool update() override;
    bool update(const std::vector<VNode*>& topChange,
                std::vector<VNode*>& topFilterChange);

private:
	bool filterState(VNode* node,VParamSet* stateFilter);
    bool collectTopFilterChange(VNode* n,std::vector<VNode*>& topFilterChange);

    VTree* tree_;
};

class TableNodeFilter : public NodeFilter
{
public:
    explicit TableNodeFilter(NodeFilterDef* def,ServerHandler*);

	void clear() override;
    bool isNull() override;
    bool isComplete() override;
    int  matchCount() const override {return matchCount_;}
    bool update() override;
    int indexOf(const VNode*) const;
    VNode* nodeAt(int index) const;

private:
    std::vector<int> index_;
    int matchCount_;
};

#endif
