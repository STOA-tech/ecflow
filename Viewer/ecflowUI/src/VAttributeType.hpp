/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_VAttributeType_HPP
#define ecflow_viewer_VAttributeType_HPP

#include <map>
#include <string>
#include <vector>

#include "VParam.hpp"

class AttributeFilter;
class VNode;
class VAttribute;

class VAttributeType : public VParam {
public:
    ~VAttributeType() override = default;

    static std::vector<VParam*> filterItems();
    static VAttributeType* find(const std::string& name);
    static VAttributeType* find(int id);
    static const std::vector<VAttributeType*>& types() { return types_; }
    int typeId() const { return typeId_; }
    int keyToDataIndex(const std::string& key) const;
    int searchKeyToDataIndex(const std::string& key) const;
    QStringList searchKeys() const;
    virtual QString toolTip(QStringList) const { return QString(); }
    virtual QString definition(QStringList) const { return QString(); }

    static void scan(VNode* vnode, std::vector<VAttribute*>& v);
    typedef void (*ScanProc)(VNode* vnode, std::vector<VAttribute*>& vec);
    ScanProc scanProc() { return scanProc_; }

    static const std::vector<std::string>& lastNames() { return lastNames_; }
    static void saveLastNames();
    static void initLastNames();

    // Called from VConfigLoader
    static void load(VProperty*);

protected:
    explicit VAttributeType(const std::string& name);

    typedef std::vector<VAttributeType*>::const_iterator TypeIterator;
    std::map<std::string, int> keyToData_;
    std::map<std::string, int> searchKeyToData_;
    int dataCount_;
    int typeId_;
    ScanProc scanProc_;
    static std::vector<std::string> lastNames_;

private:
    static std::map<std::string, VAttributeType*> typesMap_;
    static std::vector<VAttributeType*> types_;
};

#endif /* ecflow_viewer_VAttributeType_HPP */
