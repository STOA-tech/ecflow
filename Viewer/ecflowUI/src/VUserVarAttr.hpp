/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_VUserVarAttr_HPP
#define ecflow_viewer_VUserVarAttr_HPP

#include <string>
#include <vector>

#include <QStringList>

#include "VAttribute.hpp"
#include "VAttributeType.hpp"

class AttributeFilter;
class VAttributeType;
class VNode;
class Variable;

class VUserVarAttrType : public VAttributeType {
public:
    explicit VUserVarAttrType();
    QString toolTip(QStringList d) const override;
    void encode(const Variable&, QStringList&) const;

private:
    enum DataIndex { TypeIndex = 0, NameIndex = 1, ValueIndex = 2 };
};

class VUserVarAttr : public VAttribute {
public:
    VUserVarAttr(VNode* parent, const Variable&, int index);

    VAttributeType* type() const override;
    QStringList data(bool firstLine) const override;
    std::string strName() const override;

    static void scan(VNode* vnode, std::vector<VAttribute*>& vec);
};

#endif /* ecflow_viewer_VUserVarAttr_HPP */
