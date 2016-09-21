//============================================================================
// Copyright 2016 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "TriggerBrowser.hpp"

#include <QDebug>
#include <QPlainTextEdit>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#endif

#include "Highlighter.hpp"
#include "TriggerCollector.hpp"
#include "TriggerItemWidget.hpp"
#include "VAttribute.hpp"
#include "VItemPathParser.hpp"

TriggerBrowser::TriggerBrowser(QWidget *parent) : QWidget(parent), owner_(0)
{
    setupUi(this);

    //We use this plaintextedit to generate the syntax highligted html text for
    //the trigger expression.
    exprTe_=new QPlainTextEdit(this);
    exprTe_->hide();
    exprHighlight_=new Highlighter(exprTe_->document(),"trigger");

    triggerCollector_=new TriggerListCollector(false);

    Q_ASSERT(tab_->count() == 2);
    tab_->setCurrentIndex(tabIndexToInt(TriggerTabIndex));

    QFont f("Monospace");
    f.setStyleHint(QFont::TypeWriter);
    f.setFixedPitch(true);
    f.setPointSize(10);
    f.setStyleStrategy(QFont::PreferAntialias);
    triggerBrowser_->setFont(f);
    triggeredBrowser_->setFont(f);

    connect(triggerBrowser_,SIGNAL(anchorClicked(const QUrl&)),
            this,SLOT(anchorClicked(const QUrl&)));

    connect(triggeredBrowser_,SIGNAL(anchorClicked(const QUrl&)),
            this,SLOT(anchorClicked(const QUrl&)));
}

TriggerBrowser::~TriggerBrowser()
{
    delete triggerCollector_;
}

void TriggerBrowser::setOwner(TriggerItemWidget* owner)
{
    Q_ASSERT(!owner_);
    owner_=owner;
}

void TriggerBrowser::clear()
{
    loadedTabs_.clear();
    triggerBrowser_->clear();
    triggeredBrowser_->clear();
}

void TriggerBrowser::load()
{    
    Q_ASSERT(owner_);

    loadedTabs_.clear();

    if(tab_->currentIndex() == tabIndexToInt(TriggerTabIndex))
    {
        loadTriggerTab(true);
    }
    else if(tab_->currentIndex() == tabIndexToInt(TriggeredTabIndex))
    {
        loadTriggeredTab(true);
    }
}

void TriggerBrowser::loadTriggerTab(bool forceLoad)
{
    if(!forceLoad && isTabLoaded(TriggerTabIndex))
        return;

    VNode *n=owner_->info()->node();
    Q_ASSERT(n);

    std::string te,ce;
    n->triggerExpr(te,ce);

    QString s="<table width=\'100%\'>";

    //Trigger expression
    if(!te.empty())
    {
        //Generate syntax highlighted html text for the trigger expression
        QString tb;
        exprTe_->setPlainText(QString::fromStdString(te));
        exprHighlight_->toHtml(tb);

        //We extract the useful bit from the html text
        QRegExp rx("<!--StartFragment-->(.+)<!--EndFragment-->");
        if(rx.indexIn(tb) > -1 && rx.captureCount() == 1)
        {
            tb=rx.cap(1);
        }

        tb="<font face=\'monospace\'>" + tb + "</font>";

        s+="<tr><td colspan=\'2\' class=\'trigger_title\'>Trigger expression</td></tr><tr><td colspan=\'2\' class=\'trigger\'> <p>" +
                 tb + "</p></td></tr>";
    }

    triggerCollector_->setDependency(owner_->dependency());

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    n->triggers(triggerCollector_);
    s+=makeHtml(triggerCollector_,"Triggers directly triggering the selected node","Triggers");
    s+="</table>";
    triggerBrowser_->setHtml(s);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QGuiApplication::restoreOverrideCursor();
#endif

    loadedTabs_.insert(TriggerTabIndex);
}

void TriggerBrowser::loadTriggeredTab(bool forceLoad)
{
    if(!forceLoad && isTabLoaded(TriggeredTabIndex))
        return;

    VNode *n=owner_->info()->node();
    Q_ASSERT(n);

    TriggerListCollector c(owner_->dependency());
    n->triggered(&c,owner_->triggeredScanner());

    QString s="<table width=\'100%\'>";
    s+=makeHtml(&c,"Nodes directly triggered by the selected node","Triggered");
    s+="</table>";
    triggeredBrowser_->setHtml(s);

    loadedTabs_.insert(TriggeredTabIndex);
}

void TriggerBrowser::on_tab__currentChanged(int idx)
{
    if(owner_)
    {
        if(idx == tabIndexToInt(TriggerTabIndex))
            loadTriggerTab();
        else if(idx == tabIndexToInt(TriggeredTabIndex))
            loadTriggeredTab();
    }
}

bool TriggerBrowser::isTabLoaded(TabIndex idx) const
{
    return (loadedTabs_.find(idx) != loadedTabs_.end());
}

int TriggerBrowser::tabIndexToInt(TabIndex idx) const
{
    return static_cast<int>(idx);
}

//Updates the trigger list if the right type of change happened
void TriggerBrowser::nodeChanged(const VNode* n)
{
    if(!isTabLoaded(TriggerTabIndex))
        return;

    VNode *node=owner_->info()->node();
    Q_ASSERT(node);
    if(n == node)
    {
        loadTriggerTab(true);
        return;
    }

    const std::vector<TriggerListItem*>& items=triggerCollector_->items();
    for(unsigned int i=0; i < items.size(); i++)
    {
        if(VItem *t=items[i]->item())
        {
            if(VAttribute* a=t->isAttribute())
            {
                if(a->parent() == n)
                {
                    loadTriggerTab(true);
                    return;
                }
            }
        }
    }
}

void TriggerBrowser::anchorClicked(const QUrl& link)
{
    owner_->linkSelected(link.toString().toStdString());
    tab_->setCurrentIndex(TriggerTabIndex);
}

QString TriggerBrowser::makeHtml(TriggerListCollector *tc,QString directTitle,QString modeText) const
{
    QString s;
    bool firstDirectTrigger=true;
    const std::vector<TriggerListItem*>& items=tc->items();

    for(unsigned int i=0; i < items.size(); i++)
    {
        VItem *t=items[i]->item();
        VItem *d=items[i]->dep();
        if(!t)
            continue;

        if(!d)
        {
            if(firstDirectTrigger)
            {
                s+="<tr><td class=\'direct_title\' colspan=\'2\'>" + directTitle + "</td></tr>";
                firstDirectTrigger=false;
            }

            QString type=QString::fromStdString(t->typeName());
            QString path=QString::fromStdString(t->fullPath());
            QString anchor=QString::fromStdString(VItemPathParser::encode(t->fullPath(),t->typeName()));

            s+="<tr>";
            s+="<td width=\'100\'>" + type + "</td>";
            s+="<td><a href=\'" + anchor  + "\'>" + path +"</a>";
        }
    }

    QString prevH;

    for(unsigned int i=0; i < items.size(); i++)
    {
        VItem *t=items[i]->item();
        VItem *d=items[i]->dep();
        if(!t || !d)
            continue;

        QString h;

        if(items[i]->mode()== TriggerCollector::Parent)
           h+=modeText + " through parent";
        else
           h+=modeText + " through child";

        QString type=QString::fromStdString(d->typeName());
        QString path=QString::fromStdString(d->fullPath());
        QString anchor=QString::fromStdString(VItemPathParser::encode(d->fullPath(),d->typeName()));

        h+="  " + type;
        h+=" <a class=\'chp\' href=\'" + anchor + "\'>" + path +"</a>";

        if(h != prevH)
        {
            s+="<tr><td class=\'title\' colspan=\'2\'>" + h + "</td></tr>";
            prevH=h;
        }

        type=QString::fromStdString(t->typeName());
        path=QString::fromStdString(t->fullPath());
        anchor=QString::fromStdString(VItemPathParser::encode(t->fullPath(),t->typeName()));

        s+="<tr>";
        s+="<td width=\'100\'>" + type + "</td>";
        s+="<td><a href=\'" +  anchor + "\'>" + path +"</a>";
     }

    return s;
}