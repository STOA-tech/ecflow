/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "VRepeatAttr.hpp"

#include <sstream>

#include "VAttributeType.hpp"
#include "VNode.hpp"

std::string VRepeatDateAttr::subType_("date");
std::string VRepeatDateTimeAttr::subType_("datetime");
std::string VRepeatDateListAttr::subType_("datelist");
std::string VRepeatIntAttr::subType_("integer");
std::string VRepeatStringAttr::subType_("string");
std::string VRepeatEnumAttr::subType_("enumerated");
std::string VRepeatDayAttr::subType_("day");

static long ecf_repeat_date_to_julian(long ddate);
static long ecf_repeat_julian_to_date(long jdate);

long ecf_repeat_julian_to_date(long jdate) {
    long x = 0, y = 0, d = 0, m = 0, e = 0;
    long day = 0, month = 0, year = 0;

    x = 4 * jdate - 6884477;
    y = (x / 146097) * 100;
    e = x % 146097;
    d = e / 4;

    x = 4 * d + 3;
    y = (x / 1461) + y;
    e = x % 1461;
    d = e / 4 + 1;

    x = 5 * d - 3;
    m = x / 153 + 1;
    e = x % 153;
    d = e / 5 + 1;

    if (m < 11)
        month = m + 2;
    else
        month = m - 10;

    day  = d;
    year = y + m / 11;

    return year * 10000 + month * 100 + day;
}

long ecf_repeat_date_to_julian(long ddate) {
    long m1 = 0, y1 = 0, a = 0, b = 0, c = 0, d = 0, j1 = 0;

    long month = 0, day = 0, year = 0;

    year = ddate / 10000;
    ddate %= 10000;
    month = ddate / 100;
    ddate %= 100;
    day = ddate;

    if (false) {
        a  = (14 - month) / 12;
        y1 = year + 4800 - a;
        m1 = month + 12 * a - 3;
        j1 = day + (153 * m1 + 2) / 5 + 365 * y1 + y1 / 4 - y1 / 100 + y1 / 400 - 32045;
        return j1 - 0.5;
    }

    if (month > 2) {
        m1 = month - 3;
        y1 = year;
    }
    else {
        m1 = month + 9;
        y1 = year - 1;
    }
    a  = 146097 * (y1 / 100) / 4;
    d  = y1 % 100;
    b  = 1461 * d / 4;
    c  = (153 * m1 + 2) / 5 + day + 1721119;
    j1 = a + b + c;

    return j1;
}

//================================
// VRepeatAttrType
//================================

VRepeatAttrType::VRepeatAttrType() : VAttributeType("repeat") {
    dataCount_                       = 9;
    searchKeyToData_["repeat_name"]  = NameIndex;
    searchKeyToData_["repeat_value"] = ValueIndex;
    searchKeyToData_["name"]         = NameIndex;
    scanProc_                        = VRepeatAttr::scan;
}

QString VRepeatAttrType::toolTip(QStringList d) const {
    QString t = "<b>Type:</b> Repeat";
    if (d.count() == dataCount_) {
        t += " " + d[SubtypeIndex] + "<br>";

        if (d[SubtypeIndex] != "day") {
            t += "<b>Name:</b> " + d[NameIndex] + "<br>";
            t += "<b>Value:</b> " + d[ValueIndex] + "<br>";
            t += "<b>Start:</b> " + d[StartIndex] + "<br>";
            t += "<b>End:</b> " + d[EndIndex];

            if (d[SubtypeIndex] == "integer" || d[SubtypeIndex] == "date") {
                t += "<br><b>Step:</b> " + d[StepIndex];
            }
        }
        else {
            t += "<b>Step:</b> " + d[StepIndex];
        }
    }

    return t;
}

QString VRepeatAttrType::definition(QStringList d) const {
    QString t = "repeat";
    if (d.count() == dataCount_) {
        QString subType = d[SubtypeIndex];

        t += " " + subType;

        if (subType == "integer" || subType == "date" || subType == "datetime") {
            t += " " + d[NameIndex];
            t += " " + d[StartIndex];
            t += " " + d[EndIndex];
            t += " " + d[StepIndex];
        }
        else if (subType == "string" || subType == "enumerated" || subType == "datelist") {
            t += " " + d[NameIndex];
            t += " " + d[AllValuesIndex];
        }
        else {
            t += " " + d[StepIndex];
        }
    }
    return t;
}

void VRepeatAttrType::encode(const Repeat& r,
                             const VRepeatAttr* ra,
                             QStringList& data,
                             const std::string& type,
                             QString allValues) const {
    // We try to avoid creating a VRepeat object everytime we are here
    // std::string type=VRepeat::type(r);

    data << qName_ << QString::fromStdString(type) << QString::fromStdString(r.name())
         << QString::fromStdString(r.valueAsString()) << ra->startValue() << ra->endValue()
         << QString::fromStdString(ecf::Duration::format(ecf::Duration{std::chrono::seconds{r.step()}})) << allValues
         << QString::number(ra->currentPosition());
}

//=====================================================
//
// VRepeatAttr
//
//=====================================================

VRepeatAttr::VRepeatAttr(VNode* parent) : VAttribute(parent, 0) {
    // name_=e.name_or_number();
}

VAttributeType* VRepeatAttr::type() const {
    static VAttributeType* atype = VAttributeType::find("repeat");
    return atype;
}

int VRepeatAttr::step() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.step();
    }
    return 0;
}

QStringList VRepeatAttr::data(bool /*firstLine*/) const {
    static auto* atype = static_cast<VRepeatAttrType*>(type());
    QStringList s;
    if (parent_->node_) {
        const Repeat& r = parent_->node_->repeat();
        atype->encode(r, this, s, subType(), allValues());
    }
    return s;
}

std::string VRepeatAttr::strName() const {
    if (parent_->node_) {
        const Repeat& r = parent_->node_->repeat();
        if (r.empty() == false)
            return r.name();
    }
    return {};
}

void VRepeatAttr::scan(VNode* vnode, std::vector<VAttribute*>& vec) {
    if (vnode->node_) {
        const Repeat& r = vnode->node_->repeat();
        if (r.empty() == false && r.repeatBase()) {
            VRepeatAttr* a = nullptr;

            if (r.repeatBase()->isDate())
                a = new VRepeatDateAttr(vnode);
            else if (r.repeatBase()->isDateTime())
                a = new VRepeatDateTimeAttr(vnode);
            else if (r.repeatBase()->isDateList())
                a = new VRepeatDateListAttr(vnode);
            else if (r.repeatBase()->isInteger())
                a = new VRepeatIntAttr(vnode);
            else if (r.repeatBase()->isString())
                a = new VRepeatStringAttr(vnode);
            else if (r.repeatBase()->isEnumerated())
                a = new VRepeatEnumAttr(vnode);
            else if (r.repeatBase()->isDay())
                a = new VRepeatDayAttr(vnode);

            if (a)
                vec.push_back(a);
        }
    }
}

QString VRepeatAttr::allValues() const {
    return {};
}

//=====================================================
//
// VRepeatDateAttr
//
//=====================================================

int VRepeatDateAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.step() > 0) {
            long jStart = ecf_repeat_date_to_julian(r.start());
            long jEnd   = ecf_repeat_date_to_julian(r.end());

            int index = (jEnd - jStart) / r.step();
            long val  = jStart + index * r.step();
            while (val > jEnd && index >= 1) {
                index--;
                val = jStart + index * r.step();
            }
            return index;
        }
    }
    return 0;
}

int VRepeatDateAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        int cur = (ecf_repeat_date_to_julian(r.index_or_value()) - ecf_repeat_date_to_julian(r.start())) / r.step();
        return cur;
    }
    return 0;
}

QString VRepeatDateAttr::startValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return QString::number(r.start());
    }
    return {};
}

QString VRepeatDateAttr::endValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return QString::number(r.end());
    }
    return {};
}

std::string VRepeatDateAttr::value(int index) const {
    std::stringstream ss;
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        ss << (ecf_repeat_julian_to_date(ecf_repeat_date_to_julian(r.start()) + index * r.step()));
    }

    return ss.str();
}

int VRepeatDateAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.start() == r.end())
            return -1;
        else if (r.value() == r.start())
            return 0;
        else if (r.value() == r.end() ||
                 ecf_repeat_date_to_julian(r.value()) + r.step() > ecf_repeat_date_to_julian(r.end()))
            return 2;
        else
            return 1;
    }

    return -1;
}

//=====================================================
//
// VRepeatDateTimeAttr
//
//=====================================================

int VRepeatDateTimeAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.step() > 0) {
            auto jStart = ecf::coerce_to_instant(r.start());
            auto jEnd   = ecf::coerce_to_instant(r.end());

            int index   = (jEnd - jStart).as_seconds().count() / r.step();
            auto val    = jStart + ecf::Duration{std::chrono::seconds{index * r.step()}};
            while (val > jEnd && index >= 1) {
                index--;
                val = jStart + ecf::Duration{std::chrono::seconds{index * r.step()}};
            }
            return index;
        }
    }
    return 0;
}

int VRepeatDateTimeAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        auto jStart     = ecf::coerce_to_instant(r.start());
        auto jEnd       = ecf::coerce_to_instant(r.end());

        int cur         = (jEnd - jStart).as_seconds().count() / r.step();
        return cur;
    }
    return 0;
}

QString VRepeatDateTimeAttr::startValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        std::string s   = ecf::Instant::format(ecf::coerce_to_instant(r.start()));
        return QString::fromStdString(s);
    }
    return {};
}

QString VRepeatDateTimeAttr::endValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        std::string s   = ecf::Instant::format(ecf::coerce_to_instant(r.end()));
        return QString::fromStdString(s);
    }
    return {};
}

std::string VRepeatDateTimeAttr::value(int index) const {
    std::stringstream ss;
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        ss << ecf::Instant::format(ecf::coerce_to_instant(r.start() + index * r.step()));
    }
    return ss.str();
}

int VRepeatDateTimeAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.start() == r.end())
            return -1;
        else if (r.value() == r.start())
            return 0;
        else if (r.value() == r.end() || ecf::coerce_to_instant(r.value() + r.step()) > ecf::coerce_to_instant(r.end()))
            return 2;
        else
            return 1;
    }

    return -1;
}

//=====================================================
//
// VRepeatDateListAttr
//
//=====================================================

std::string VRepeatDateListAttr::value(int index) const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.value_as_string(index);
    }
    return {};
}

int VRepeatDateListAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (auto* rdl = static_cast<RepeatDateList*>(r.repeatBase())) {
            return rdl->indexNum() - 1;
        }
    }
    return 0;
}

int VRepeatDateListAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.index_or_value();
    }
    return 0;
}

QString VRepeatDateListAttr::startValue() const {
    return QString::fromStdString(value(0));
}

QString VRepeatDateListAttr::endValue() const {
    return QString::fromStdString(value(endIndex()));
}

QString VRepeatDateListAttr::allValues() const {
    QString vals;

    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();

        int start = 0;
        int end   = endIndex();

        if (end <= start) {
            return {};
        }

        for (int i = start; i <= end; i++) {
            if (!vals.isEmpty()) {
                vals += " ";
            }
            vals += QString::fromStdString(r.value_as_string(i));
        }
        return vals;
    }
    return vals;
}

int VRepeatDateListAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (auto* rdl = static_cast<RepeatDateList*>(r.repeatBase())) {
            if (rdl->indexNum() < 2)
                return -1;
            else if (rdl->index_or_value() == 0)
                return 0;
            else if (rdl->index_or_value() == rdl->indexNum() - 1)
                return 2;
            else
                return 1;
        }
    }
    return -1;
}

//=====================================================
//
// VRepeatIntAttr
//
//=====================================================

int VRepeatIntAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.step() > 0) {
            int index = (r.end() - r.start()) / r.step();
            int val   = r.start() + index * r.step();
            while (val > r.end() && index >= 1) {
                index--;
                val = r.start() + index * r.step();
            }
            return index;
        }
    }
    return 0;
}

int VRepeatIntAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.step() > 0)
            return (r.index_or_value() - r.start()) / r.step();
    }
    return 0;
}

std::string VRepeatIntAttr::value(int index) const {
    std::stringstream ss;
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        ss << r.start() + index * r.step();
    }
    return ss.str();
}

QString VRepeatIntAttr::startValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return QString::number(r.start());
    }
    return {};
}

QString VRepeatIntAttr::endValue() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return QString::number(r.end());
    }
    return {};
}

int VRepeatIntAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (r.start() == r.end())
            return -1;
        else if (r.value() == r.start())
            return 0;
        else if (r.value() == r.end() || r.value() + r.step() > r.end())
            return 2;
        else
            return 1;
    }
    return -1;
}

//=====================================================
//
// VRepeatDayAttr
//
//=====================================================

std::string VRepeatDayAttr::value(int /*index*/) const {
    std::stringstream ss;
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        ss << r.step();
    }
    return ss.str();
}

QString VRepeatDayAttr::startValue() const {
    return {};
}

QString VRepeatDayAttr::endValue() const {
    return {};
}

//=====================================================
//
// VRepeatEnumAttr
//
//=====================================================

std::string VRepeatEnumAttr::value(int index) const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.value_as_string(index);
    }
    return {};
}

int VRepeatEnumAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.end();
    }
    return 0;
}

int VRepeatEnumAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.index_or_value();
    }
    return 0;
}

QString VRepeatEnumAttr::startValue() const {
    return QString::fromStdString(value(startIndex()));
}

QString VRepeatEnumAttr::endValue() const {
    return QString::fromStdString(value(endIndex()));
}

QString VRepeatEnumAttr::allValues() const {
    QString vals;

    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        int start       = r.start();
        int end         = r.end();

        if (end <= start) {
            return {};
        }

        if (end - start > 1) {
            for (int i = start; i <= end; i++) {
                if (!vals.isEmpty())
                    vals += " ";
                vals += "\"" + QString::fromStdString(r.value_as_string(i)) + "\"";
            }
            return vals;
        }
    }
    return vals;
}

int VRepeatEnumAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (auto* rdl = static_cast<RepeatEnumerated*>(r.repeatBase())) {
            if (rdl->indexNum() < 2)
                return -1;
            else if (rdl->index_or_value() == 0)
                return 0;
            else if (rdl->index_or_value() == rdl->indexNum() - 1)
                return 2;
            else
                return 1;
        }
    }
    return -1;
}

//=====================================================
//
// VRepeatStringAttr
//
//=====================================================

std::string VRepeatStringAttr::value(int index) const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.value_as_string(index);
    }
    return {};
}

int VRepeatStringAttr::endIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.end();
    }
    return 0;
}

int VRepeatStringAttr::currentIndex() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        return r.index_or_value();
    }
    return 0;
}

QString VRepeatStringAttr::startValue() const {
    return QString::fromStdString(value(startIndex()));
}

QString VRepeatStringAttr::endValue() const {
    return QString::fromStdString(value(endIndex()));
}

QString VRepeatStringAttr::allValues() const {
    QString vals;

    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        int start       = r.start();
        int end         = r.end();

        if (end <= start) {
            return {};
        }

        if (end - start > 1) {
            for (int i = start; i <= end; i++) {
                if (!vals.isEmpty())
                    vals += " ";
                vals += "\"" + QString::fromStdString(r.value_as_string(i)) + "\"";
            }
            return vals;
        }
    }
    return vals;
}

int VRepeatStringAttr::currentPosition() const {
    if (node_ptr node = parent_->node()) {
        const Repeat& r = node->repeat();
        if (auto* rdl = static_cast<RepeatString*>(r.repeatBase())) {
            if (rdl->indexNum() < 2)
                return -1;
            else if (rdl->index_or_value() == 0)
                return 0;
            else if (rdl->index_or_value() == rdl->indexNum() - 1)
                return 2;
            else
                return 1;
        }
    }
    return -1;
}
