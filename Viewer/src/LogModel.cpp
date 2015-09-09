//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "LogModel.hpp"

#include <QDebug>
#include <QStringList>

#include "IconProvider.hpp"

LogModelLine::LogModelLine(QString s) : type_(NoType)
{
	QString s1=s.section("]",0,0);
	date_=s1.section("[",1);
	entry_=s.section("]",1);

	QString t=s1.section("[",0);

	if(t.contains("MSG:"))
	{
		type_=MessageType;
	}
	else if(t.contains("LOG:"))
	{
		type_=LogType;
	}
	else if(t.contains("ERR:"))
	{
		type_=ErrorType;
	}
	else if(t.contains("WAR:"))
	{
		type_=WarningType;
	}
	else if(t.contains("DBG:"))
	{
		type_=DebugType;
	}
}


LogModel::LogModel(QObject *parent) :
          QAbstractItemModel(parent)
{
	IconProvider::add(":/viewer/log_error.svg","log_error");
	IconProvider::add(":/viewer/log_info.svg","log_info");
	IconProvider::add(":/viewer/log_warning.svg","log_warning");
}

LogModel::~LogModel()
{
}

void LogModel::setData(const std::string& data)
{
	beginResetModel();

	data_.clear();

	QString in=QString::fromStdString(data);
	Q_FOREACH(QString s,in.split("\n"))
	{
		if(!s.simplified().isEmpty())
			data_ << LogModelLine(s);
	}

	endResetModel();
}

void LogModel::setData(const std::vector<std::string>& data)
{
	beginResetModel();

	data_.clear();

	for(std::vector<std::string>::const_iterator it=data.begin(); it != data.end(); it++)
	{
		QString s=QString::fromStdString(*it);
		if(!s.simplified().isEmpty())
		{
			data_ << LogModelLine(s);
		}
	}
	endResetModel();
}

void LogModel::appendData(const std::vector<std::string>& data)
{
	int num=0;

	for(std::vector<std::string>::const_iterator it=data.begin(); it != data.end(); it++)
	{
		QString s=QString::fromStdString(*it);
		if(!s.simplified().isEmpty())
		{
			num++;
		}
	}

	if(num >0)
	{
		beginInsertRows(QModelIndex(),rowCount(),rowCount()+num-1);

		for(std::vector<std::string>::const_iterator it=data.begin(); it != data.end(); it++)
		{
			QString s=QString::fromStdString(*it);
			if(!s.simplified().isEmpty())
			{
				data_ << LogModelLine(s);
			}
		}

		endInsertRows();
	}
}



void LogModel::clearData()
{
	beginResetModel();
	data_.clear();
	endResetModel();
}

bool LogModel::hasData() const
{
	return !data_.empty();
}

int LogModel::columnCount( const QModelIndex& /*parent */ ) const
{
   	 return 3;
}

int LogModel::rowCount( const QModelIndex& parent) const
{
	if(!hasData())
		return 0;

	//Parent is the root:
	if(!parent.isValid())
	{
		return data_.size();
	}

	return 0;
}

Qt::ItemFlags LogModel::flags ( const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant LogModel::data( const QModelIndex& index, int role ) const
{
	if(!index.isValid() || !hasData())
    {
		return QVariant();
	}
	int row=index.row();
	if(row < 0 || row >= data_.size())
		return QVariant();


	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
		case 0:
			{
				switch(data_.at(row).type_)
				{
				case LogModelLine::MessageType:
					return "MSG ";
				case LogModelLine::LogType:
					return "LOG ";
				case LogModelLine::ErrorType:
					return "ERR ";
				case LogModelLine::WarningType:
					return "WAR ";
				case LogModelLine::DebugType:
					return "DBG ";
				default:
					return QVariant();
				}
			}
			break;
		case 1:
			return data_.at(row).date_;
			break;
		case 2:
			return data_.at(row).entry_;
			break;
		default:
			break;
		}
	}

	else if(role == Qt::DecorationRole)
	{
		if(index.column() ==0)
		{
			switch(data_.at(row).type_)
			{
				case LogModelLine::MessageType:
					return IconProvider::pixmap("log_info",12);
				case LogModelLine::LogType:
					return IconProvider::pixmap("log_info",12);
				case LogModelLine::ErrorType:
					return IconProvider::pixmap("log_error",12);
				case LogModelLine::WarningType:
					return IconProvider::pixmap("log_warning",12);
				default:
					return QVariant();
			}
		}
	}

	else if(role == Qt::BackgroundRole)
	{
		/*if(data_.at(row).type_ == LogModelLine::ErrorType)
		{
		 return QColor(223,152,152);
		}*/
	}


	else if(role == Qt::FontRole)
	{
		QFont f;

		/*if(data_.at(row).type_ == LogModelLine::ErrorType)
		{
			f.setBold(true);
		}*/

		return f;
	}
	/*else if(role == Qt::SizeHintRole)
	{
		QFont f;
		f.setBold(true);
		QFontMetrics fm(f);
		return fm.height()+10;
	}*/


	return QVariant();
}

QVariant LogModel::headerData( const int section, const Qt::Orientation orient , const int role ) const
{
	if ( orient != Qt::Horizontal || (role != Qt::DisplayRole &&  role != Qt::ToolTipRole))
      		  return QAbstractItemModel::headerData( section, orient, role );

   	if(role == Qt::DisplayRole)
   	{
   		switch ( section )
   		{
   		case 0: return tr("Type");
   		case 1: return tr("Date");
   		case 2: return tr("Entry");
   		default: return QVariant();
   		}
   	}
   	else if(role== Qt::ToolTipRole)
   	{
   		switch ( section )
   		{
   		case 0: return tr("Type");
   		case 1: return tr("Date");
   		case 2: return tr("Entry");
   		default: return QVariant();
   		}
   	}
    return QVariant();
}

QModelIndex LogModel::index( int row, int column, const QModelIndex & parent ) const
{
	if(!hasData() || row < 0 || column < 0)
	{
		return QModelIndex();
	}

	//When parent is the root this index refers to a node or server
	if(!parent.isValid())
	{
		return createIndex(row,column);
	}

	return QModelIndex();

}

QModelIndex LogModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}

QModelIndex LogModel::lastIndex() const
{
	return index(rowCount()-1,0);
}

//========================================================
//
// LogDelegate
//
//========================================================

LogDelegate::LogDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

void LogDelegate::paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter,option,index);

	/*if(index.column()==11)
	{
		QStyleOptionViewItemV4 vopt(option);
		initStyleOption(&vopt, index);

		const QStyle *style = vopt.widget ? vopt.widget->style() : QApplication::style();
		const QWidget* widget = vopt.widget;

		QString text=index.data(Qt::DisplayRole).toString();
		QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &vopt, widget);
		if(text == "ERR")
		{
			QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &vopt, widget);
		}

		painter->fillRect(textRect,Qt::red);
		painter->drawText(textRect,Qt::AlignLeft | Qt::AlignVCenter,text);
	}
	else
	{
		QStyledItemDelegate::paint(painter,option,index);
	}*/
}


QSize LogDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QSize size=QStyledItemDelegate::sizeHint(option,index);

	size+=QSize(0,2);

    return size;
}