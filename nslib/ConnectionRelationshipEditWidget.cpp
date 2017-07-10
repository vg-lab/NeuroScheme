/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
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

#include "ConnectionRelationshipEditWidget.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "PaneManager.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>


namespace nslib
{

  ConnectionRelationshipEditWidget::ConnectionRelationshipEditWidget(
    std::vector< shift::Entity* > entities_, QWidget* parent )
    : QWidget( parent )
    , _entities( entities_ )
  {
    QGridLayout* layout = new QGridLayout;

    _originEntityComboBox = new QComboBox( );
    _destinationEntityComboBox = new QComboBox( );

    for ( auto entity: _entities )
    {
      _originEntityComboBox->addItem(
        QString::fromStdString( std::to_string( entity->entityGid( ))));
      _destinationEntityComboBox->addItem(
        QString::fromStdString( std::to_string( entity->entityGid( ))));
    }

    _originEntityComboBox->setCurrentIndex( 0 );
    _destinationEntityComboBox->setCurrentIndex( 1 );

    connect( _originEntityComboBox, SIGNAL( currentIndexChanged( int )),
             this, SLOT( originEntityComboBoxChanged( int )));
    connect( _destinationEntityComboBox, SIGNAL( currentIndexChanged( int )),
             this, SLOT( destinationEntityComboBoxChanged( int  )));

    auto label = new QLabel( QString( "Origin Entity Id" ));
    layout->addWidget( label, 0, 0 );
    layout->addWidget( _originEntityComboBox, 0, 1 );

    label = new QLabel( QString( "Destination Entity Id" ));
    layout->addWidget( label, 1, 0 );
    layout->addWidget( _destinationEntityComboBox, 1, 1 );

    QPushButton* cancelButton = new QPushButton( tr( "Cancel" ));
    layout->addWidget( cancelButton, 2, 0 );

    QPushButton* validationButton = new QPushButton( tr( "Save" ));
    layout->addWidget( validationButton, 2, 1 );

    connect( cancelButton, SIGNAL( clicked( )), this, SLOT( cancelDialog( )));
    connect( validationButton, SIGNAL( clicked( )),
             this, SLOT( validateDialog( )));

    setLayout( layout );
    setWindowTitle( tr( "Create connection relationship" ));
  }

  void ConnectionRelationshipEditWidget::validateDialog( void )
  {
    auto originEntity =
      _entities[_originEntityComboBox->currentIndex( )]->entityGid( );
    auto destinationEntity =
      _entities[ _destinationEntityComboBox->currentIndex( )]->entityGid( );

    auto& relConnectsTo =
      *( DataManager::entities( ).relationships( )["connectsTo"]->asOneToN( ));
    auto& relConnectedBy =
      *( DataManager::entities( ).relationships( )["connectedBy"]->asOneToN( ));

    auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
      ->relationshipPropertiesTypes( );
    auto connectsToIt = relConnectsTo.find( originEntity );
    if( connectsToIt != relConnectsTo.end( ))
    {
      auto connectsToMMIt = connectsToIt->second.find( destinationEntity );
      if( connectsToMMIt != connectsToIt->second.end( ))
      {
        auto& connectsToProperty =
          connectsToMMIt->second->getProperty( "count" );

        unsigned int value = connectsToProperty.value< unsigned int >( ) + 1;
        connectsToProperty.set< unsigned int >( value );
      }
      else
      {
        auto relationshipProperties = relationshipPropertiesTypes.
          getRelationshipProperties( "connectsTo" );
        auto connectsWith = relationshipProperties->create( );
        connectsWith->getProperty( "count" ).set< unsigned int >(
          (unsigned int ) 1 );
        relConnectsTo[ originEntity ].insert(
          std::make_pair( destinationEntity, connectsWith ));
        relConnectedBy[ destinationEntity ].insert(
          std::make_pair( originEntity, nullptr ));
      }
    }
    else
    {
      auto relationshipProperties = relationshipPropertiesTypes.
          getRelationshipProperties( "connectsTo" );
        auto connectsWith = relationshipProperties->create( );
        connectsWith->getProperty( "count" ).set< unsigned int >(
          (unsigned int ) 1 );
      relConnectsTo[ originEntity ].insert(
        std::make_pair( destinationEntity, connectsWith ));
      relConnectedBy[ destinationEntity ].insert(
        std::make_pair( originEntity, nullptr ));
    }

    for ( auto pane : nslib::PaneManager::panes( ))
    {
      pane->resizeEvent( nullptr );
    }

    this->hide( );
  }

  void ConnectionRelationshipEditWidget::cancelDialog(  )
  {
    this->hide( );
  }

  void ConnectionRelationshipEditWidget::originEntityComboBoxChanged( int )
  {
    int originIndex = _originEntityComboBox->currentIndex( );
    int destinationIndex = _destinationEntityComboBox->currentIndex( );
    if( originIndex == destinationIndex )
    {
      destinationIndex ++;
      destinationIndex %= _entities.size( );
      _destinationEntityComboBox->setCurrentIndex( destinationIndex );
    }
  }

  void ConnectionRelationshipEditWidget::destinationEntityComboBoxChanged( int )
  {
    int originIndex = _originEntityComboBox->currentIndex( );
    int destinationIndex = _destinationEntityComboBox->currentIndex( );
    if( originIndex == destinationIndex )
    {
      originIndex ++;
      originIndex %= _entities.size( );
      _originEntityComboBox->setCurrentIndex( originIndex );
    }
  }

}
