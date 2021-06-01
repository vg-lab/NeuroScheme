/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
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

#include "EntityConnectionListWidget.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "InteractionManager.h"
#include "Loggers.h"

#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QLineEdit>
#include <assert.h>

namespace nslib
{

  QDockWidget* EntityConnectionListWidget::_parentDock = nullptr;
  bool EntityConnectionListWidget::_autoCloseChecked = false;
  bool EntityConnectionListWidget::_allPropsChecked = false;

  EntityConnectionListWidget::EntityConnectionListWidget(
    QWidget* parentWidget_ )
    : QWidget ( parentWidget_ )
    , _entity( nullptr )
    , _gridLayout( new QGridLayout( ))
    , _entityNameLabel( new QLabel )
    , _autoCloseLabel( new QLabel( tr( "Auto-close" )))
    , _autoCloseCheck( new QCheckBox( ))
    , _allPropertiesLabel( new QLabel( tr( "All-Properties" )))
    , _allPropertiesCheck( new QCheckBox( ))
    , _cancelButton( new QPushButton( QString( tr( "Cancel" ))))
  {
    unsigned int element = 0;
    _gridLayout->addWidget( _entityNameLabel, element, 0, 1, 2 );
    _entityNameLabel->setStyleSheet( "font-weight:bold" );
    auto types = DomainManager::getActiveDomain( )->entitiesTypes( );
    shift::Entity* entityType = std::get< shift::EntitiesTypes::OBJECT >
      ( *types.entitiesTypes( ).begin( ));
    auto entityNamePropertyGid = entityType->properties( ).begin( )->first;
    std::string entityNameString =
      fires::PropertyGIDsManager::getPropertyLabel( entityNamePropertyGid );
    QString entityNameLabel = QString::fromStdString( entityNameString );
    fires::PropertyCaster* entityNameCaster =
      fires::PropertyManager::getPropertyCaster( entityNamePropertyGid );
    fires::PropertySorter* entityNameSorter =
      fires::PropertyManager::getSorter( entityNamePropertyGid );

    auto relationshipPropertiesTypes =
      DomainManager::getActiveDomain( )-> relationshipPropertiesTypes( );
    auto propConnectionType = relationshipPropertiesTypes
      .getRelationshipProperties( "connectsTo" );
    _connectsToTable = new ConnectionTableWidget( QString( tr(
      "Connected to entities:" )), true, false, QString( tr(
      "Entity not connected to." )), propConnectionType,
      entityNameCaster, entityNameLabel, entityNameSorter,
      _gridLayout, ++element, this );

    element += 2;
    _connectedByTable = new ConnectionTableWidget( QString( tr(
      "Connected by entities:" )), false, false, QString( tr(
      "Entity not connected by." )), propConnectionType,
      entityNameCaster, entityNameLabel, entityNameSorter,
      _gridLayout, element, this );

    element += 2;
    propConnectionType = relationshipPropertiesTypes
        .getRelationshipProperties( "aggregatedConnectsTo" );
    _aggregatedConnectsToTable = new ConnectionTableWidget( QString( tr(
      "Aggregated connected to entities:" )), true, true, QString( tr(
      "Entity not aggregated connected to." )), propConnectionType,
      entityNameCaster, entityNameLabel, entityNameSorter,
      _gridLayout, element, this );

    element += 2;
    _aggregatedConnectedByTable = new ConnectionTableWidget( QString( tr(
      "Aggregated connected by entities:" )), false, true, QString( tr(
      "Entity not aggregated connected by." )), propConnectionType,
      entityNameCaster, entityNameLabel, entityNameSorter,
      _gridLayout, element, this );

    _gridLayout->setAlignment( Qt::AlignTop );
    _gridLayout->setColumnStretch( 1, 1 );
    _gridLayout->setColumnStretch( 0, 1 );
    _gridLayout->setVerticalSpacing( 1 );

    _autoCloseCheck->setChecked( _autoCloseChecked );
    _allPropertiesCheck->setChecked( _allPropsChecked );

    element += 2;
    auto separation = new QFrame( );
    separation->setFrameShape( QFrame::HLine );
    _gridLayout->addWidget( separation, element, 0, 1, 2 );

    _gridLayout->addWidget( _cancelButton, ++element, 0, 1, 2 );
    _gridLayout->addWidget( _allPropertiesCheck, ++element, 1 );
    _gridLayout->addWidget( _allPropertiesLabel, element, 0 );
    _gridLayout->addWidget( _autoCloseCheck, ++element, 1 );
    _gridLayout->addWidget( _autoCloseLabel, element, 0 );

    auto sizePolicy = QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    _entityNameLabel->setSizePolicy( sizePolicy );
    _cancelButton->setSizePolicy( sizePolicy );
    _allPropertiesCheck->setSizePolicy( sizePolicy );
    _allPropertiesLabel->setSizePolicy( sizePolicy );
    _autoCloseCheck->setSizePolicy( sizePolicy );
    _autoCloseLabel->setSizePolicy( sizePolicy );

    connect( _cancelButton, &QPushButton::clicked, this,
      [ this ]
      {
        cancelDialog( );
      });
    connect( _allPropertiesCheck, &QPushButton::clicked, this,
      [ this ]
      {
        toggleAllProperties( );
      });
    connect( _autoCloseCheck, &QPushButton::clicked, this,
      [ this ]
      {
        toggleAutoClose( );
      });
    setLayout( _gridLayout );
    if(_parentDock)
      _parentDock->setWidget( this );
  }

  void EntityConnectionListWidget::setConnections( shift::Entity*  entity_,
    shift::RelationshipOneToNMapDest*  connectsToMap_,
    shift::RelationshipOneToNMapDest*  connectedByMap_,
    shift::AggregatedOneToNAggregatedDests* aggregatedConnectsToMap_,
    shift::AggregatedOneToNAggregatedDests* aggregatedConnectedByMap_ )
  {
    _entity = entity_;
    _entityNameLabel->setText(QString::fromStdString( "Connections of entity: "
      + _entity->getPropertyValue< std::string >( "Entity name", " ")));
    _aggregatedConnectedByTable->setTableData(
      aggregatedConnectedByMap_, _entity, _allPropsChecked );
    _aggregatedConnectsToTable->setTableData(
      aggregatedConnectsToMap_, _entity, _allPropsChecked );
    _connectedByTable->setTableData( connectedByMap_, _entity,
      _allPropsChecked );
    _connectsToTable->setTableData( connectsToMap_, _entity, _allPropsChecked );
    if(_parentDock)
      _parentDock->show( );
    this->show( );
  }

  void EntityConnectionListWidget::cancelDialog()
  {
    this->hide( );
    if(_parentDock)
      _parentDock->close( );
  }

  void EntityConnectionListWidget::parentDock( QDockWidget* parentDock_ )
  {
    _parentDock = parentDock_;
  }

  QDockWidget* EntityConnectionListWidget::parentDock( void )
  {
    return _parentDock;
  }

  void EntityConnectionListWidget::toggleAutoClose( void )
  {
    _autoCloseChecked = _autoCloseCheck->isChecked( );
  }

  void EntityConnectionListWidget::toggleAllProperties( void )
  {
    _allPropsChecked = _allPropertiesCheck->isChecked( );
    _connectsToTable->conectionModel( )->allProperties( _allPropsChecked );
    _connectedByTable->conectionModel( )->allProperties( _allPropsChecked );
    _aggregatedConnectedByTable->conectionModel( )
      ->allProperties( _allPropsChecked );
    _aggregatedConnectsToTable->conectionModel( )
      ->allProperties( _allPropsChecked );
  }

  void
  EntityConnectionListWidget::updateConnections( shift::EntityGid origEntity_,
    shift::EntityGid destEntity_, const bool updateAggregatedTo_,
    const bool updateAggregatedBy_ )
  {
    if( _entity )
    {
      const auto entityGid = _entity->entityGid( );
      auto dataRelations = DataManager::entities( ).relationships( );
      auto relConnectsTo = *( dataRelations[ "connectsTo" ]->asOneToN( ));
      auto relConnectedBy = *( dataRelations[ "connectedBy" ]->asOneToN( ));
      auto relAggregatedConnectsTo = dataRelations[ "aggregatedConnectsTo" ]
        ->asAggregatedOneToN( )->mapAggregatedRels( );
      auto relAggregatedConnectBy = dataRelations[ "aggregatedConnectedBy" ]
        ->asAggregatedOneToN( )->mapAggregatedRels( );

      if( entityGid == origEntity_ )
      {
        auto connectsIt = relConnectsTo.find( entityGid );
        shift::RelationshipOneToNMapDest* connectsMap =
          ( connectsIt == relConnectsTo.end( )
          || connectsIt->second.empty( ))
          ? nullptr : &connectsIt->second;
        _connectsToTable->setTableData( connectsMap, _entity );
      }
      if( entityGid == destEntity_ )
      {
        auto connectsIt = relConnectedBy.find( entityGid );
        shift::RelationshipOneToNMapDest* connectsMap =
          ( connectsIt == relConnectedBy.end( )
          || connectsIt->second.empty( ))
          ? nullptr : &connectsIt->second;
        _connectedByTable->setTableData( connectsMap, _entity );
      }
      if( updateAggregatedBy_ )
      {
        auto connectsIt = relAggregatedConnectBy.find( entityGid );
        shift::AggregatedOneToNAggregatedDests* connectsMap =
          ( connectsIt == relAggregatedConnectBy.end( )
          || connectsIt->second->empty( ))
          ? nullptr : connectsIt->second.get( );
        _aggregatedConnectedByTable->setTableData( connectsMap, _entity );
      }
      if( updateAggregatedTo_ )
      {
        auto connectsIt = relAggregatedConnectsTo.find( entityGid );
        shift::AggregatedOneToNAggregatedDests* connectsMap =
          ( connectsIt == relAggregatedConnectsTo.end( )
          || connectsIt->second->empty( ))
          ? nullptr : connectsIt->second.get( );
        _aggregatedConnectsToTable->setTableData( connectsMap, _entity );
      }
    }
  }

  void EntityConnectionListWidget::updateConnections( void )
  {
    if ( _entity )
    {
      const auto entityGid = _entity->entityGid( );
      auto dataRelations = DataManager::entities( ).relationships( );
      auto relConnectsTo = *( dataRelations[ "connectsTo" ]->asOneToN( ) );
      auto relConnectedBy = *( dataRelations[ "connectedBy" ]->asOneToN( ) );
      auto relAggregatedConnectsTo = dataRelations[ "aggregatedConnectsTo" ]
        ->asAggregatedOneToN( )->mapAggregatedRels( );
      auto relAggregatedConnectBy = dataRelations[ "aggregatedConnectedBy" ]
        ->asAggregatedOneToN( )->mapAggregatedRels( );

      auto connectsIt = relConnectsTo.find( entityGid );
      shift::RelationshipOneToNMapDest* connectsMap =
        ( connectsIt == relConnectsTo.end( ) || connectsIt->second.empty( ) )
        ? nullptr : &connectsIt->second;
      _connectsToTable->setTableData( connectsMap, _entity );

      auto connectsIt1 = relConnectedBy.find( entityGid );
      shift::RelationshipOneToNMapDest* connectsMap1 =
        ( connectsIt1 == relConnectedBy.end( ) ||
        connectsIt1->second.empty( ))
        ? nullptr : &connectsIt1->second;
      _connectedByTable->setTableData( connectsMap1, _entity );

      auto connectsIt2 = relAggregatedConnectBy.find( entityGid );
      shift::AggregatedOneToNAggregatedDests* connectsMap2 =
        ( connectsIt2 == relAggregatedConnectBy.end( )
        || connectsIt2->second->empty( )) ? nullptr
        : connectsIt2->second.get( );
      _aggregatedConnectedByTable->setTableData( connectsMap2, _entity );

      auto connectsIt3 = relAggregatedConnectsTo.find( entityGid );
      shift::AggregatedOneToNAggregatedDests* connectsMap3 =
        ( connectsIt3 == relAggregatedConnectsTo.end( )
        || connectsIt3->second->empty( ))
        ? nullptr : connectsIt3->second.get( );
      _aggregatedConnectsToTable->setTableData( connectsMap3, _entity );
    }

  }

  void EntityConnectionListWidget::checkClose( void )
  {
    if ( _autoCloseCheck->isChecked( ))
    {
      this->hide( );
      if(_parentDock)
        _parentDock->close( );
    }
  }

  shift::Entity* EntityConnectionListWidget::entity( )
  {
    return _entity;
  }
}
