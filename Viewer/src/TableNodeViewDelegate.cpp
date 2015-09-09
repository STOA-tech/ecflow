//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "TableNodeViewDelegate.hpp"

#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <QPainter>

#include <QStyleOptionViewItem>

#include "AbstractNodeModel.hpp"
#include "Animation.hpp"
#include "ModelColumn.hpp"
#include "PropertyMapper.hpp"

static std::vector<std::string> propVec;

TableNodeViewDelegate::TableNodeViewDelegate(QWidget *parent)
{
	borderPen_=QPen(QColor(230,230,230));

	columns_=ModelColumn::def("table_columns");

    //Property
    if(propVec.empty())
    {
        propVec.push_back("view.tree.font");
        propVec.push_back("view.tree.displayChildCount");
    }

    prop_=new PropertyMapper(propVec,this);

    updateSettings();
}

TableNodeViewDelegate::~TableNodeViewDelegate()
{
}

void TableNodeViewDelegate::updateSettings()
{

}


void TableNodeViewDelegate::paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const
{
    //Background
    QStyleOptionViewItemV4 vopt(option);
    initStyleOption(&vopt, index);

    const QStyle *style = vopt.widget ? vopt.widget->style() : QApplication::style();
    const QWidget* widget = vopt.widget;

    //Save painter state
    painter->save();

    //Selection - we only do it once
    /*if(index.column() == 0)
    {
        QRect fullRect=QRect(0,option.rect.y(),painter->device()->width(),option.rect.height());

        if(option.state & QStyle::State_Selected)
        {
            //QRect fillRect=option.rect.adjusted(0,1,-1,-textRect.height()-1);
            painter->fillRect(fullRect,selectBrush_);
            painter->setPen(selectPen_);
            painter->drawLine(fullRect.topLeft(),fullRect.topRight());
            painter->drawLine(fullRect.bottomLeft(),fullRect.bottomRight());
        }
        else if(option.state & QStyle::State_MouseOver)
        {
            //QRect fillRect=option.rect.adjusted(0,1,-1,-1);
            painter->fillRect(fullRect,hoverBrush_);
            painter->setPen(hoverPen_);
            painter->drawLine(fullRect.topLeft(),fullRect.topRight());
            painter->drawLine(fullRect.bottomLeft(),fullRect.bottomRight());
        }
    }*/

    //Different background for lost connection?
   /* if(index.data(AbstractNodeModel::ConnectionRole).toInt() == 0)
    {
        QRect fullRect=QRect(0,option.rect.y(),painter->device()->width(),option.rect.height());
        painter->fillRect(fullRect,lostConnectBgBrush_);
        QRect bandRect=QRect(0,option.rect.y(),5,option.rect.height());
        painter->fillRect(bandRect,lostConnectBandBrush_);

    }*/

    QString id=columns_->id(index.column());


    if(id == "path")
    {
    	QString text=index.data(Qt::DisplayRole).toString();
    	renderNode(painter,index,vopt,text);
    }
    else if(id == "status")
    {
    	 renderStatus(painter,index,vopt);
    }

    //Render attributes
    else if(id == "event" || id == "label" || id == "meter" || id == "trigger")
    {
    	QVariant va=index.data(Qt::DisplayRole);
    	if(va.type() == QVariant::StringList)
    	{
    		QStringList lst=va.toStringList();
    		if(lst.count() > 0)
    		{
    			QMap<QString,AttributeRendererProc>::const_iterator it=attrRenderers_.find(lst.at(0));
    			if(it != attrRenderers_.end())
    			{
    				AttributeRendererProc a=it.value();
    				(this->*a)(painter,lst,vopt);
    			}
    		}
    	}
	}

    //rest of the columns
    else
    {
    	QString text=index.data(Qt::DisplayRole).toString();
    	QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &vopt, widget);
    	painter->setPen(Qt::black);
    	painter->drawText(textRect,Qt::AlignLeft | Qt::AlignVCenter,text);
    }

    //Render the horizontal border for rows. We only render the top border line.
    //With this technique we miss the bottom border line of the last row!!!
    //QRect fullRect=QRect(0,option.rect.y(),painter->device()->width(),option.rect.height());
    QRect bgRect=option.rect;
    painter->setPen(borderPen_);
    painter->drawLine(bgRect.topLeft(),bgRect.topRight());


    painter->restore();
}



void TableNodeViewDelegate::renderNode(QPainter *painter,const QModelIndex& index,
        							const QStyleOptionViewItemV4& option,QString text) const
{
	int offset=4;

	QFontMetrics fm(font_);
	int deltaH=(option.rect.height()-(fm.height()+4))/2;

	//The initial filled rect (we will adjust its  width)
	QRect fillRect=option.rect.adjusted(offset,deltaH,0,-deltaH-1);
	if(option.state & QStyle::State_Selected)
		fillRect.adjust(0,0,0,-0);

	//The text rectangle
	QRect textRect = fillRect.adjusted(offset,0,0,0);

	int textWidth=fm.width(text);
	textRect.setWidth(textWidth);

	//Adjust the filled rect width
	fillRect.setRight(textRect.right()+offset);

	int currentRight=fillRect.right();

	//Icons area
	QList<QPixmap> pixLst;
	QList<QRect> pixRectLst;
	QVariant va=index.data(AbstractNodeModel::IconRole);
	if(va.type() == QVariant::List)
	{
		QVariantList lst=va.toList();
		int xp=currentRight+5;
		int yp=fillRect.top();
		for(int i=0; i < lst.count(); i++)
		{
			pixLst << lst[i].value<QPixmap>();
			pixRectLst << QRect(xp,yp,pixLst.back().width(),pixLst.back().height());
			xp+=pixLst.back().width();
		}

		if(!pixRectLst.isEmpty())
			currentRight=pixRectLst.back().right();
	}

	//Define clipping
	int rightPos=currentRight+1;
	const bool setClipRect = rightPos > option.rect.right();
	if(setClipRect)
	{
		painter->save();
		painter->setClipRect(option.rect);
	}

	//Draw text
	QColor fg=index.data(Qt::ForegroundRole).value<QColor>();
	painter->setPen(fg);
	painter->drawText(textRect,Qt::AlignLeft | Qt::AlignVCenter,text);


	//Draw icons
	for(int i=0; i < pixLst.count(); i++)
	{
		painter->drawPixmap(pixRectLst[i],pixLst[i]);
	}

	if(setClipRect)
	{
		painter->restore();
	}
}

void TableNodeViewDelegate::renderStatus(QPainter *painter,const QModelIndex& index,
                                    const QStyleOptionViewItemV4& option) const
{
    int offset=4;

    QFontMetrics fm(font_);
    int deltaH=(option.rect.height()-(fm.height()+4))/2;

    //The initial filled rect (we will adjust its  width)
    QRect fillRect=option.rect.adjusted(offset,deltaH,-offset,-deltaH-1);
    if(option.state & QStyle::State_Selected)
        fillRect.adjust(0,0,0,-0);

    int currentRight=fillRect.right();

    //The text rectangle
    QString text=index.data(Qt::DisplayRole).toString();
    int textWidth=fm.width(text);
    QRect textRect = fillRect.adjusted(offset,0,0,0);
    textRect.setWidth(textWidth);

    //Define clipping
    int rightPos=currentRight+1;
    const bool setClipRect = false; //rightPos > option.rect.right();
    if(setClipRect)
    {
        painter->save();
        painter->setClipRect(option.rect);
    }

    //Fill rect
    QColor bgCol=index.data(Qt::BackgroundRole).value<QColor>();
    painter->fillRect(fillRect,bgCol);

    //Draw text
    painter->setPen(Qt::black);
    painter->drawText(textRect,Qt::AlignLeft | Qt::AlignVCenter,text);


    if(setClipRect)
    {
        painter->restore();
    }
}
