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

#include "ConnectionTableWidget.h"
#include "EntityConnectionListWidget.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "InteractionManager.h"
#include "Loggers.h"

#include <QApplication>
#include <QHeaderView>

namespace nslib
{
  ConnectionsTableModel::ConnectionsTableModel( bool entityIsOrigin_,
    bool isAggregated_, QString emptyString_,
    shift::RelationshipProperties* propConnectionType_,
    fires::PropertyCaster* entityNameCaster_,
    QString entityNameLabel_, fires::PropertySorter* entitySorter_,
    ConnectionTableWidget* tableWidget_ )
    : QAbstractTableModel( tableWidget_ )
    , _tableWidget(tableWidget_ )
    , _columnCount( 0 )
    , _rowCount( 0 )
    , _numConnections( 0 )
    , _entity( nullptr )
    , _isAggregated( isAggregated_ )
    , _entityIsOrigin( entityIsOrigin_ )
    , _allProperties( false )
    , _emptyString( emptyString_ )
    , _tableData( std::vector< fires::Property * >( ))
    , _entitiesData( std::vector<shift::Entity*>( ))
    , _headerData( std::vector<QString>( ))
    , _propertiesCaster( std::vector< fires::PropertyCaster* >( ))
    , _propertiesSorter( std::vector< fires::PropertySorter* >( ))
  {
    _headerData.push_back( entityNameLabel_ );
    _propertiesCaster.push_back( entityNameCaster_ );
    _propertiesSorter.push_back( entitySorter_ );
    for( const auto& propPair: propConnectionType_->properties( ))
    {
      const auto prop = propPair.first;
      _propertiesCaster.push_back(
        fires::PropertyManager::getPropertyCaster( prop ));
      std::string propertyLabel =
        fires::PropertyGIDsManager::getPropertyLabel( prop );
      _headerData.push_back( QString::fromStdString( propertyLabel ));
      _propertiesSorter.push_back( fires::PropertyManager::getSorter( prop ));
    }
    _numProperties = static_cast< int >( _headerData.size( ));
  }

  void ConnectionsTableModel::updateData(
    shift::RelationshipOneToNMapDest* connectsMap_, shift::Entity* entity_ )
  {
    _entity = entity_;
    _entitiesData.clear( );
    _tableData.clear( );
    if( connectsMap_ )
    {
      auto iterator = connectsMap_->begin( );
      while( iterator != connectsMap_->end( ))
      {
        auto connectedEntity = DataManager::entities( ).at( iterator->first );
        fires::Property& entityName =
          connectedEntity->getProperty( "Entity name" );
        auto connectionProperties = iterator++->second;
        _tableData.push_back( &entityName );
        _entitiesData.push_back( connectedEntity );
        for( auto& propPair : connectionProperties->properties( ))
        {
          fires::Property& property = propPair.second;
          _tableData.push_back( &property );
        }
      }
    }
    _numConnections = static_cast< int >( _entitiesData.size( ));
    updateColumnCount( );
    updateRowCount( );
  }

  void ConnectionsTableModel::updateData(
    shift::AggregatedOneToNAggregatedDests* connectsMap_,
    shift::Entity* entity_ )
  {
    _entity = entity_;
    _entitiesData.clear( );
    _tableData.clear( );
    if( connectsMap_ )
    {
      auto iterator = connectsMap_->begin( );
      while( iterator != connectsMap_->end( ))
      {
        auto connectedEntity = DataManager::entities( ).at( iterator->first );
        fires::Property& entityName = connectedEntity->getProperty(
          "Entity name" );
        auto connectionProperties = iterator++
          ->second.relationshipAggregatedProperties;
        _tableData.push_back( &entityName );
        _entitiesData.push_back( connectedEntity );
        for( auto& propPair : connectionProperties->properties( ))
        {
          fires::Property& property = propPair.second;
          _tableData.push_back( &property );
        }
      }
    }
    _numConnections = static_cast<int>( _entitiesData.size( ));
    updateColumnCount( );
    updateRowCount( );
  }

  int ConnectionsTableModel::rowCount( const QModelIndex& /*parent_*/ ) const
  {
    return _rowCount;
  }

  int ConnectionsTableModel::rowCount( ) const
  {
    return _rowCount;
  }

  int ConnectionsTableModel::columnCount( const QModelIndex& /*parent_*/ ) const
  {
    return _columnCount;
  }

  int ConnectionsTableModel::columnCount( ) const
  {
    return _columnCount;
  }

  void ConnectionsTableModel::updateColumnCount( )
  {
    if( _numConnections == 0 )
    {
      if( _columnCount != 1 )
      {
        beginRemoveColumns( QModelIndex( ), 1, _columnCount - 1 );
        _columnCount = 1;
        endRemoveColumns( );
      }
    }
    else
    {
      int columnCount = _allProperties ? _numProperties + 1 : 3;
      if( columnCount > _columnCount )
      {
        if( _columnCount == 1 )
        {
          beginInsertColumns( QModelIndex( ), 1, columnCount - 1 );
        }
        else
        {
          beginInsertColumns( QModelIndex( ), 2, columnCount - 2 );
        }
        _columnCount = columnCount;
        endInsertColumns( );
      }
      else if( columnCount != _columnCount )
      {
        beginRemoveColumns( QModelIndex( ), 2, _columnCount - 2 );
        _columnCount = columnCount;
        endRemoveColumns( );
      }
    }
  }

  void ConnectionsTableModel::updateRowCount( )
  {
    int rowCount = _numConnections == 0 ? 1 : _numConnections;
    if( rowCount > _rowCount )
    {
      beginInsertRows( QModelIndex( ), _rowCount, rowCount - 1 );
      _rowCount = rowCount;
      endInsertRows( );
    }
    else if( rowCount != _rowCount )
    {
      beginRemoveRows( QModelIndex( ), rowCount, _rowCount - 1 );
      _rowCount = rowCount;
      endRemoveRows( );
    }
    if( rowCount == 1 || _rowCount == 1 )
    {
      updateColumnCount( );
    }
  }

  QVariant
  ConnectionsTableModel::data( const QModelIndex& index_, int role_ ) const
  {
    if( index_.isValid( ) && role_ == Qt::DisplayRole )
    {
      const auto indexRow = index_.row( );
      if( _numConnections == 0 )
      {
        return _emptyString;
      }
      else if( indexRow < _rowCount )
      {
        auto indexColumn = index_.column( );
        if( indexColumn < _columnCount - 1 )
        {
          fires::Property p = *( _tableData.at( static_cast< unsigned long >(
            indexRow * _numProperties + indexColumn )));
          return QString::fromStdString( _propertiesCaster.at(
            static_cast< unsigned long >(indexColumn) )->toString( p ));
        }
      }
    }

    return QVariant( );
  }

  QVariant
  ConnectionsTableModel::headerData( int section_, Qt::Orientation orientation_,
    int role_ ) const
  {
    if( role_ == Qt::DisplayRole && orientation_ == Qt::Horizontal )
    {
      if( _numConnections != 0 )
      {
        if( section_ < _columnCount )
        {
          if( section_ == ( _columnCount - 1 ))
          {
            return QString( " " );
          }
          else
          {
            return _headerData.at( static_cast< unsigned long >( section_ ) );
          }
        }
      }
    }
    return QVariant( );
  }

  void ConnectionsTableModel::allProperties( bool allProperties_ )
  {
    if( _allProperties != allProperties_ )
    {
      _allProperties = allProperties_;
      updateColumnCount( );
      _tableWidget->show( );
    }
  }

  void ConnectionsTableModel::sort( int column_, Qt::SortOrder order_ )
  {
    //Cannot use fires::sort(objects) since it would copy de properties
    if( _numConnections < 2 )
      return;
    const bool descending = order_ == Qt::SortOrder::DescendingOrder;

    fires::PropertySorter* propertySorter = _propertiesSorter.at( column_ );

    auto numConnection = static_cast< unsigned long >( _numConnections );
    auto entitiesIt1 = _tableData.begin( ) + column_;
    for( unsigned long index1 = 0; index1 < numConnection; ++index1 )
    {
      fires::Property* sortProperty = *entitiesIt1;
      unsigned long sortIndex = index1;
      auto entitiesIt2 = entitiesIt1 = entitiesIt1 + _numProperties;

      for( unsigned long  index2 = index1 + 1; index2 < numConnection;
        ++index2 )
      {
        fires::Property* evalProperty = *entitiesIt2;
        if( descending ? propertySorter->isLowerThan(
          *evalProperty, *sortProperty )
          : !( propertySorter->isLowerThan( *evalProperty, *sortProperty ) ||
          propertySorter->isEqual( *evalProperty, *sortProperty )))
        {
          sortIndex = index2;
          sortProperty = evalProperty;
        }
        entitiesIt2 += _numProperties;
      }

      if( index1 != sortIndex )
      {
        auto entitiesIt3 = _tableData.begin( )+ ( index1 *_numProperties );
        entitiesIt2 = _tableData.begin( )+ ( sortIndex *_numProperties );
        for( int i = 0; i < _numProperties; ++i )
        {
          sortProperty = *entitiesIt3;
          *entitiesIt3 = *entitiesIt2;
          *entitiesIt2 = sortProperty;
          ++entitiesIt2;
          ++entitiesIt3;
        }
        shift::Entity* auxEntity = _entitiesData.at( index1 );
        _entitiesData.at( index1 ) = _entitiesData.at( sortIndex );
        _entitiesData.at( sortIndex ) = auxEntity;
      }
    }
    _tableWidget->hide( );
    _tableWidget->show( );
  }

  shift::Entity* ConnectionsTableModel::entity( ) const
  {
    return _entity;
  }

  bool ConnectionsTableModel::isAggregated( ) const
  {
    return _isAggregated;
  }

  bool ConnectionsTableModel::entityIsOrigin( ) const
  {
    return _entityIsOrigin;
  }

  shift::Entity* ConnectionsTableModel::connectedEntityAt( const unsigned int index_ ) const
  {
    if(index_ < _entitiesData.size())
    {
      return _entitiesData.at( index_ );
    }

    return nullptr;
  }

  bool ConnectionsTableModel::removeRows( int row_, int count_,
    const QModelIndex& /*parent_*/ )
  {
    if( ( row_ + count_ ) > _numConnections )
    {
      return false;
    }
    beginRemoveRows( QModelIndex( ), row_, row_ + count_ -1 );
    const int row = row_ *_numProperties;
    const int count = count_ *_numProperties;
    auto eraseEtitiesBegin = _entitiesData.begin( ) + row_;
    _entitiesData.erase( eraseEtitiesBegin, eraseEtitiesBegin + count_ );
    auto eraseTableBegin = _tableData.begin( ) + row;
    _tableData.erase( eraseTableBegin, eraseTableBegin + count );
    _numConnections = _numConnections - count_;
    if( _numConnections == 0 )
    {
      _rowCount = 1;
      updateColumnCount( );
    }
    else
    {
      _rowCount = _numConnections;
    }
    endRemoveRows( );
    _tableWidget->autoResize( );
    return true;
  }


  void ConnectionTableWidget::autoResize( )
  {
    this->horizontalHeader( )->setDefaultSectionSize( 150 );
    this->horizontalHeader( )->setSectionResizeMode(
      QHeaderView::ResizeToContents );
    int lastColumn = _model->columnCount( ) - 1;
    if( lastColumn != 0 )
    {
      this->horizontalHeader( )
        ->setSectionResizeMode( lastColumn, QHeaderView::Fixed );
    }
    this->resizeRowsToContents( );
    int height =
      std::min( 250, this->rowHeight( 0 ) * ( _model->rowCount( ) + 2 ));
    this->setMaximumHeight( height );
    this->setMinimumHeight( height );
    _gridLayout->setRowMinimumHeight( _gridPos+1, height );
    _gridLayout->setRowStretch(_gridPos+1, height );
  }

  ConnectionsTableDelegate::ConnectionsTableDelegate(
    ConnectionTableWidget* tableWidget_, ConnectionsTableModel* model_,
    EntityConnectionListWidget* listWidget_, QWidget* parentWidget_ )
    : QItemDelegate( parentWidget_ )
    , _tableWidget( tableWidget_ )
    , _model( model_ )
    , _listWidget(listWidget_)
  {
  }

  void ConnectionsTableDelegate::paint( QPainter* painter_,
    const QStyleOptionViewItem& option_, const QModelIndex& index_ ) const
  {
    QVariant data = index_.data( );
    if( data.canConvert< QString >( ) )
    {
      QItemDelegate::paint( painter_, option_, index_ );
    }
    else
    {
      const QRect rect = option_.rect;
      QStyleOptionButton editButtonStyle;
      editButtonStyle.text = "Edit";
      editButtonStyle.state = QStyle::State_Enabled;
      if( _model->isAggregated( ))
      {
        editButtonStyle.rect = QRect( rect.left( ), rect.top( ),
          rect.width( ), rect.height( ) );
      }
      else
      {
        const int buttonWidth = static_cast<int>( 0.5f * rect.width( ));
        editButtonStyle.rect =QRect( rect.left( ), rect.top( ), buttonWidth,
          rect.height( ));

        QStyleOptionButton deleteButtonStyle;
        deleteButtonStyle.rect = QRect( rect.left( ) + buttonWidth, rect.top( ),
          buttonWidth, rect.height( ));
        deleteButtonStyle.text = "Delete";
        deleteButtonStyle.state = QStyle::State_Enabled;
        QApplication::style( )->drawControl( QStyle::CE_PushButton,
          &deleteButtonStyle, painter_ );
      }
      QApplication::style( )->drawControl( QStyle::CE_PushButton,
        &editButtonStyle, painter_ );
      }
  }

  bool ConnectionsTableDelegate::editorEvent( QEvent* event_,
    QAbstractItemModel* model_, const QStyleOptionViewItem& option_,
    const QModelIndex& index_ )
  {
    if( index_.isValid( ) && index_.column( ) == model_->columnCount( ) - 1 )
    {
      if( event_->type( ) == QEvent::MouseButtonRelease )
      {
        auto mouseEvent = dynamic_cast<QMouseEvent*>( event_ );
        const QRect rect = option_.rect;

        if( rect.contains(mouseEvent->pos(), true) )
        {
          const unsigned int rowIndex = index_.row( );
          auto entity = _model->entity( );
          auto connectedEntity = _model->connectedEntityAt( rowIndex );
          if(!connectedEntity) return false;

          const bool isAggregated = _model->isAggregated( );
          if( isAggregated || mouseEvent->x() < rect.left() +
            static_cast< int >( rect.width() * 0.5f ))
          {
            auto type = isAggregated
              ? ConnectionRelationshipEditWidget::TConnectionType::AGGREGATED
              : ConnectionRelationshipEditWidget::TConnectionType::SIMPLE;
            if( _model->entityIsOrigin( ))
            {
              InteractionManager::createConnectionRelationship( entity,
                connectedEntity, type );
            }
            else
            {
              InteractionManager::createConnectionRelationship( connectedEntity,
                entity, type );
            }
            return true;
          }
          else
          {
            auto& relAggregatedConnectsTo = *( DataManager::entities( )
              .relationships( )[ "aggregatedConnectsTo" ]
              ->asAggregatedOneToN( ));
            auto& relAggregatedConnectedBy = *( DataManager::entities( )
              .relationships( )[ "aggregatedConnectedBy" ]
              ->asAggregatedOneToN( ));
            if( _model->rowCount( ) == 1 )
            {
             _model->updateData( ( shift::AggregatedOneToNAggregatedDests* )
               nullptr, _model->entity( ));
            }
            else
            {
              _model->removeRow( rowIndex );
            }

            if( _model->entityIsOrigin( ))
            {
              shift::Relationship::BreakAnAggregatedRelation(
                relAggregatedConnectsTo, relAggregatedConnectedBy,
                entity, connectedEntity );
              InteractionManager::updateConnectionRelationship( entity,
                connectedEntity );
            }
            else
            {
              shift::Relationship::BreakAnAggregatedRelation(
                relAggregatedConnectsTo, relAggregatedConnectedBy,
                connectedEntity, entity );
              InteractionManager::updateConnectionRelationship( connectedEntity,
                entity );
            }
            //Recalcs aggregated relations
            InteractionManager::updateEntityConnectionList( 0, 0, true, true );
            for( auto pane : PaneManager::panes( ))
            {
              pane->resizeEvent( nullptr );
            }
          }
          _listWidget->checkClose( );
        }
      }
      return true;
    }
    else
    {
      return QItemDelegate::editorEvent( event_, model_, option_, index_ );
    }
  }

  ConnectionTableWidget::ConnectionTableWidget( QString tableTitle_,
    bool entityIsOrigin_, bool isAggregated_, QString emptyString_,
    shift::RelationshipProperties* propConnectionType_,
    fires::PropertyCaster* entityNameCaster_, QString entityNameLabel_,
    fires::PropertySorter* entitySorter_, QGridLayout* gridLayout_,
    int gridPos_, EntityConnectionListWidget* listWidget_ )
    : QTableView( gridLayout_->widget( ))
    , _model( new ConnectionsTableModel( entityIsOrigin_, isAggregated_,
        emptyString_, propConnectionType_, entityNameCaster_,
        entityNameLabel_, entitySorter_, this ))
    , _tableTitle( new QLabel( tableTitle_ ))
    , _gridPos( gridPos_ )
    , _gridLayout( gridLayout_ )
  {
    this->setModel( _model );
    this->setItemDelegate( new ConnectionsTableDelegate(
      this, _model, listWidget_, _gridLayout->widget( )));
    this->setSortingEnabled( true );
    auto sizePolicy = QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    this->setSizePolicy( sizePolicy );
    _tableTitle->setSizePolicy( sizePolicy );
    _tableTitle->setStyleSheet( "font-weight:bold" );
    _gridLayout->addWidget( _tableTitle, _gridPos, 0, 1, 2 );
    _gridLayout->addWidget( this, _gridPos + 1, 0, 1, 2 );
  }

  void ConnectionTableWidget::setTableData(
    shift::RelationshipOneToNMapDest* connectsMap_, shift::Entity* entity_,
    bool allProperties_ )
  {
    _model->updateData( connectsMap_, entity_ );
    _model->allProperties( allProperties_ );
    this->autoResize( );
  }

  void ConnectionTableWidget::setTableData(
    shift::AggregatedOneToNAggregatedDests* connectsMap_,
    shift::Entity* entity_, bool allProperties_ )
  {
    _model->updateData( connectsMap_, entity_ );
    _model->allProperties( allProperties_ );
    this->autoResize( );
  }

  ConnectionsTableModel* ConnectionTableWidget::conectionModel( ) const
  {
    return _model;
  }
}
