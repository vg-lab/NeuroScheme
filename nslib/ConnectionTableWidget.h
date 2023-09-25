/*
 * Copyright (c) 2017 VG-Lab/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __NSLIB_CONNECTION_TABLE__
#define __NSLIB_CONNECTION_TABLE__

#include <QCheckBox>
#include <shift/shift.h>
#include <QDockWidget>
#include <QGridLayout>
#include <QTableView>
#include <QItemDelegate>
#include <QLabel>
#include "EntityConnectionListWidget.h"

namespace nslib
{
  class EntityConnectionListWidget;
  class ConnectionsTableModel;

  class ConnectionTableWidget : public QTableView
  {
    Q_OBJECT

    public:
    ConnectionTableWidget( QString tableTitle_, bool entityIsOrigin_,
      bool isAggregated_, QString emptyString_,
      shift::RelationshipProperties* propConnectionType_,
      fires::PropertyCaster* entityNameCaster_, QString entityNameLabel_,
      fires::PropertySorter* entitySorter_, QGridLayout* gridLayout_,
      int gridPos_, EntityConnectionListWidget* listWidget );
    void setTableData( shift::RelationshipOneToNMapDest* connectsMap_,
      shift::Entity* entity_, bool allProperties_ = false );

    void setTableData( shift::AggregatedOneToNAggregatedDests* connectsMap_,
      shift::Entity* entity_, bool allProperties_ = false );
    void autoResize( );

    ConnectionsTableModel* conectionModel( ) const;

    private:
    ConnectionsTableModel* _model;
    QLabel* _tableTitle;
    int _gridPos;
    QGridLayout* _gridLayout;
  };

  class ConnectionsTableModel : public QAbstractTableModel
  {
    Q_OBJECT

    public:
    ConnectionsTableModel( bool entityIsOrigin_,
      bool isAggregated_, QString emptyString_,
      shift::RelationshipProperties* propConnectionType_,
      fires::PropertyCaster* entityNameCaster_,
      QString entityNameLabel_, fires::PropertySorter* entitySorter_,
      ConnectionTableWidget* tableWidget_ );

    void allProperties( bool allProperties_ );

    int rowCount( const QModelIndex& parent_ ) const override;

    int rowCount( ) const;

    int columnCount( const QModelIndex& parent_ ) const override;

    int columnCount( ) const;

    void updateColumnCount( );

    void updateRowCount( );

    bool removeRows( int row_, int count_,
      const QModelIndex& parent_ ) override;

    QVariant data( const QModelIndex& index_, int role_ ) const override;

    QVariant headerData( int section_, Qt::Orientation orientation_,
      int role_ ) const override;

    void
    sort( int column_, Qt::SortOrder order_ ) override;

    void updateData( shift::RelationshipOneToNMapDest* connectsMap_,
      shift::Entity* entity_ );

    void updateData( shift::AggregatedOneToNAggregatedDests* connectsMap_,
      shift::Entity* entity_ );

    shift::Entity* entity( ) const;

    bool isAggregated( ) const;

    bool entityIsOrigin( ) const;

    shift::Entity* connectedEntityAt( const unsigned int index_ ) const;

    private:
    ConnectionTableWidget* _tableWidget;
    int _columnCount;
    int _rowCount;
    int _numConnections;
    int _numProperties;
    shift::Entity* _entity;
    const bool _isAggregated;
    const bool _entityIsOrigin;
    bool _allProperties;
    const QString _emptyString;
    std::vector< fires::Property* > _tableData;
    std::vector< shift::Entity* > _entitiesData;
    std::vector < QString > _headerData;
    std::vector< fires::PropertyCaster* > _propertiesCaster;
    std::vector< fires::PropertySorter* > _propertiesSorter;
  };

  class ConnectionsTableDelegate : public QItemDelegate
  {
    Q_OBJECT
    public:
    ConnectionsTableDelegate( ConnectionTableWidget* tableWidget_,
      ConnectionsTableModel* model_, EntityConnectionListWidget* listWidget_,
      QWidget* parentWidget_ = nullptr );

    void paint( QPainter* painter_, const QStyleOptionViewItem& option_,
      const QModelIndex& index_ ) const override;

    bool editorEvent( QEvent* event_, QAbstractItemModel* model_,
      const QStyleOptionViewItem& option_, const QModelIndex& index_ ) override;

    private:
    ConnectionTableWidget* _tableWidget;
    ConnectionsTableModel* _model;
    EntityConnectionListWidget* _listWidget;
  };
}


#endif //NEUROSCHEME_CONNECTIONTABLE_H
