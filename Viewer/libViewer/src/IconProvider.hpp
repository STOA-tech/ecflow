/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_IconProvider_HPP
#define ecflow_viewer_IconProvider_HPP

#include <map>

#include <QPixmap>

class IconItem {
public:
    explicit IconItem(QString);
    virtual ~IconItem() = default;

    QPixmap pixmap(int);
    int id() const { return id_; }
    QString path() const { return path_; }

protected:
    virtual QPixmap unknown(int);

    QString path_;
    std::map<int, QPixmap> pixmaps_;
    int id_;
};

class UnknownIconItem : public IconItem {
public:
    explicit UnknownIconItem(QString);

protected:
    QPixmap unknown(int) override;
};

class IconProvider {
public:
    IconProvider();

    static int add(QString path, QString name);

    static QString path(int id);
    static QPixmap pixmap(QString name, int size);
    static QPixmap pixmap(int id, int size);

    static QPixmap lockPixmap(int);
    static QPixmap warningPixmap(int);
    static QPixmap errorPixmap(int);
    static QPixmap infoPixmap(int);

private:
    static IconItem* icon(QString name);
    static IconItem* icon(int id);

    static std::map<QString, IconItem*> icons_;
    static std::map<int, IconItem*> iconsById_;
};

#endif /* ecflow_viewer_IconProvider_HPP */
