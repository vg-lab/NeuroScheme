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

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <assert.h>

namespace nslib
{

  QDockWidget* EntityConnectionListWidget::_parentDock = nullptr;

  EntityConnectionListWidget::EntityConnectionListWidget(
    shift::Entity* entity_,shift::RelationshipOneToNMapDest*  connectsToMap_,
    shift::RelationshipOneToNMapDest*  connectedByMap_,
    shift::AggregatedOneToNAggregatedDests* aggregatedConnectsToMap_,
    shift::AggregatedOneToNAggregatedDests* aggregatedConnectedByMap_,
    QWidget* parentWidget_ )
    : QFrame ( parentWidget_ )
{
  QGridLayout* gridLayout = new QGridLayout( );
  unsigned  int numProp = 0u;
  QPushButton* cancelButton = new QPushButton( QString( tr( "Cancel" )));
  gridLayout->setAlignment( Qt::AlignTop );
  gridLayout->setColumnStretch( 1, 1 );
  gridLayout->setColumnMinimumWidth( 1, 150 );
  QString editButtonText = QString( tr( "Edit" ));
  QString entityName = QString::fromStdString(
    entity_->getProperty( "Entity name" ).value<std::string>( ));

  auto labelConnectsTo = new QLabel( QString( tr( "Connected to entities:" )));
  labelConnectsTo->setStyleSheet( "font-weight:bold" );
  gridLayout->addWidget( labelConnectsTo, 0, 0, 1, 2 );

  if( connectsToMap_ )
  {
    auto iterator = connectsToMap_->begin( );
    while( iterator != connectsToMap_->end( ))
    {
      auto connectedEntity = DataManager::entities( ).at( iterator->first );
      auto entitiesLabel = new QLabel( entityName + QString( " - " )
        + QString::fromStdString( connectedEntity->getProperty( "Entity name" )
        .value<std::string>( )));
      auto connectionNameLabel = new QLabel( QString::fromStdString(
        iterator++->second->getProperty( "Name" ).value< std::string >( )));
      auto editButton = new QPushButton( editButtonText );

      gridLayout->addWidget( entitiesLabel, ++numProp, 0 );
      gridLayout->addWidget( connectionNameLabel, numProp, 1 );
      gridLayout->addWidget( editButton, numProp, 2 );

      _connectionsCont.push_back( std::make_tuple(
        entitiesLabel, connectionNameLabel, editButton ));
      connect( editButton, &QPushButton::clicked, this,
        [ this, entity_, connectedEntity ]
        {
          connectionDialog( entity_, connectedEntity );
        });
    }
  }
  else
  {
    auto labelNotConnectedTo = new QLabel( QString( tr(
      "Entity not connected to." )));
    gridLayout->addWidget( labelNotConnectedTo, ++numProp, 0, 1, 2 );
  }

  auto labelConnectBy = new QLabel( QString( tr( "Connected by entities:" )));
  labelConnectBy->setStyleSheet( "font-weight:bold" );
  gridLayout->addWidget( labelConnectBy, ++numProp, 0, 1, 2 );

  if( connectedByMap_ )
  {
    auto iterator = connectedByMap_->begin( );
    while( iterator != connectedByMap_->end( ))
    {
      auto connectedEntity = DataManager::entities( ).at( iterator->first );
      auto entitiesLabel = new QLabel( QString::fromStdString(
        connectedEntity->getProperty( "Entity name" )
        .value<std::string>( ))  + QString( " - " ) + entityName );
      auto connectionNameLabel = new QLabel( QString::fromStdString(
          iterator++->second->getProperty( "Name" ).value< std::string >( )));
      auto editButton = new QPushButton( editButtonText );

      gridLayout->addWidget( entitiesLabel, ++numProp, 0 );
      gridLayout->addWidget( connectionNameLabel, numProp, 1 );
      gridLayout->addWidget( editButton, numProp, 2 );

      _connectionsCont.push_back( std::make_tuple(
          entitiesLabel, connectionNameLabel, editButton ));
      connect( editButton, &QPushButton::clicked, this,
        [ this, entity_, connectedEntity ]
        {
          connectionDialog( connectedEntity, entity_ );
        });
    }
  }
  else
  {
    auto labelNotConnectBy = new QLabel( QString( tr(
      "Entity not connected by." )));
    gridLayout->addWidget( labelNotConnectBy, ++numProp, 0, 1, 2 );
  }

  auto labelAggregatedConnectTo =
      new QLabel( QString( tr( "Aggregated connected to entities:" )));
  labelAggregatedConnectTo->setStyleSheet( "font-weight:bold" );
  gridLayout->addWidget( labelAggregatedConnectTo, ++numProp, 0, 1, 2 );

  if( aggregatedConnectsToMap_ )
  {
    auto iterator = aggregatedConnectsToMap_->begin( );
    while( iterator != aggregatedConnectsToMap_->end( ))
    {
      auto connectedEntity = DataManager::entities( ).at( iterator->first );
      auto entitiesLabel = new QLabel( entityName + QString( " - " )
        + QString::fromStdString( connectedEntity->getProperty( "Entity name" )
        .value<std::string>( ) ) );
      auto connectionNameLabel = new QLabel( QString::fromStdString(
        iterator++->second.relationshipAggregatedProperties
        ->getProperty( "Name" ).value< std::string >( )));
      auto editButton = new QPushButton( editButtonText );

      gridLayout->addWidget( entitiesLabel, ++numProp, 0 );
      gridLayout->addWidget( connectionNameLabel, numProp, 1 );
      gridLayout->addWidget( editButton, numProp, 2 );

      _connectionsCont.push_back( std::make_tuple(
          entitiesLabel, connectionNameLabel, editButton ) );
      /*connect( editButton, &QPushButton::clicked, this,
          [ this, entity_, connectedEntity ]
          {
            connectionDialog( connectedEntity, entity_ );
          });*/
    }
  }
  else
  {
    auto labelNotAggregatedConnectTo = new QLabel( QString( tr(
      "Entity not aggregated connected to." )));
    gridLayout->addWidget( labelNotAggregatedConnectTo, ++numProp, 0, 1, 2 );
  }

  auto labelAggregatedConnectBy =
    new QLabel( QString( tr( "Aggregated connected by entities:" )));
  labelAggregatedConnectBy->setStyleSheet( "font-weight:bold" );
  gridLayout->addWidget( labelAggregatedConnectBy, ++numProp, 0, 1, 2 );

  if( aggregatedConnectedByMap_ )
  {
    auto iterator = aggregatedConnectedByMap_->begin( );
    while( iterator != aggregatedConnectedByMap_->end( ))
    {
      auto connectedEntity = DataManager::entities( ).at( iterator->first );
      auto entitiesLabel = new QLabel( QString::fromStdString(
        connectedEntity->getProperty( "Entity name" ).value<std::string>( ))
        + QString( " - " ) + entityName );
      auto connectionNameLabel = new QLabel( QString::fromStdString(
          iterator++->second.relationshipAggregatedProperties->
          getProperty( "Name" ).value< std::string >( )));
      auto editButton = new QPushButton( editButtonText );

      gridLayout->addWidget( entitiesLabel, ++numProp, 0 );
      gridLayout->addWidget( connectionNameLabel, numProp, 1 );
      gridLayout->addWidget( editButton, numProp, 2 );

      _connectionsCont.push_back( std::make_tuple(
          entitiesLabel, connectionNameLabel, editButton ) );
      /*connect( editButton, &QPushButton::clicked, this,
          [ this, entity_, connectedEntity ]
          {
            connectionDialog( connectedEntity, entity_ );
          });*/
    }
  }
  else
  {
    auto labelNotAggregatedConnectBy = new QLabel( QString( tr(
      "Entity not aggregated connected by." )));
    gridLayout->addWidget( labelNotAggregatedConnectBy, ++numProp, 0, 1, 2 );
  }

  gridLayout->addWidget( cancelButton, 1+numProp, 0 );
  connect( cancelButton, &QPushButton::clicked, this,
    [ this ]
    {
      cancelDialog( );
    });

  setLayout( gridLayout );
  _parentDock->setWidget( this );
  _parentDock->show( );
  this->show( );
  }

  void EntityConnectionListWidget::connectionDialog(
      shift::Entity* originEntity_, shift::Entity* destinationEntity_ )
  {
    nslib::InteractionManager::createConnectionRelationship(
      originEntity_, destinationEntity_ );
    cancelDialog( );
  }

  void EntityConnectionListWidget::cancelDialog(  )
  {
    this->hide( );
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
}
