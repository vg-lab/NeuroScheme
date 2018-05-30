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

#include "ChildrenRelationshipEditWidget.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <assert.h>


namespace nslib
{

  ChildrenRelationshipEditWidget::ChildrenRelationshipEditWidget(
    shift::Entity* parentEntity_,
    std::vector< shift::Entity* > childrenEntities_,
    QWidget* parent_, bool modal )
    : QDialog ( parent_ )
    , _parentEntity( parentEntity_ )
    , _childrenEntities( childrenEntities_ )
  {
    this->setModal( modal );
    auto& relConnectsTo =
        *( DataManager::entities( ).relationships( )[ "isParentOf" ]->asOneToN( ) );
    auto connectsToIt = relConnectsTo.find( _parentEntity->entityGid( ) );
    if( connectsToIt != relConnectsTo.end( ) )
    {
      QGridLayout* myLayout = new QGridLayout;
      myLayout->setAlignment( Qt::AlignTop );

      auto labelRel = new QLabel( QString( "ParentShip Parameters" ) );
      myLayout->addWidget( labelRel, 0, 0, 1, 0 );



      setLayout( myLayout );
      setWindowTitle( tr( "Children to add:" ) );
      int posGrid = 1;
      for(auto entity : _childrenEntities){
        std::string stringLabel =  "Add " + entity->getProperty("Entity name").value<std::string>() + " as child (ID " + std::to_string( entity->entityGid() ) + ")";
        auto label = new QLabel( QString::fromStdString( stringLabel));
        myLayout->addWidget( label, posGrid++, 0 );
      }

      QPushButton* cancelButton = new QPushButton( QString( "Cancel" ) );
      myLayout->addWidget( cancelButton, posGrid, 0 );
      QPushButton* validationButton = new QPushButton( QString( "Create" ) );
      myLayout->addWidget( validationButton, posGrid, 1 );

      connect( cancelButton, SIGNAL( clicked( ) ), this,
               SLOT( cancelDialog( ) ) );
      connect( validationButton, SIGNAL( clicked( ) ),
               this, SLOT( validateDialog( ) ) );
    }
  }

  void ChildrenRelationshipEditWidget::validateDialog( void )
  {
    auto& entities = nslib::DataManager::entities( );
    auto& relParentOf=
      *( entities.relationships( )[ "isParentOf" ]->asOneToN( ));
    auto& relChildOf =
      *( entities.relationships( )[ "isChildOf" ]->asOneToOne( ));
    for(auto childEntity : _childrenEntities )
    {
      shift::Relationship::Establish( relParentOf, relChildOf, _parentEntity,childEntity );
    }

//    bool needToClearCache = false;
//    for ( const auto& creatorPair :
//            nslib::RepresentationCreatorManager::creators( ))
//    {
//      needToClearCache = needToClearCache ||
//        creatorPair.second->relationshipUpdatedOrCreated( propObject );
//    }

    // TODO improvemente: check if cache needs to be cleared or if just the
    // items related to the entity under edition
    // if ( needToClearCache ) {
    nslib::RepresentationCreatorManager::clearRelationshipsCache( );
    // }

    for ( auto pane : nslib::PaneManager::panes( ))
    {
      pane->resizeEvent( nullptr );
    }

    this->hide( );
  }

  void ChildrenRelationshipEditWidget::cancelDialog(  )
  {
    this->hide( );
  }

}
