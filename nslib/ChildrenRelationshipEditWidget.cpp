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
      setWindowTitle( tr( "Children added:" ) );
      int posGrid = 1;
      for(auto entity : _childrenEntities){
        std::string stringLabel =  "Added " + entity->getProperty("Entity name").value<std::string>() + " as child (ID " + std::to_string( entity->entityGid() ) + ")";
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
    shift::RelationshipProperties* propObject;

    auto& relConnectsTo =
      *( DataManager::entities( ).relationships( )["connectsTo"]->asOneToN( ));
    auto& relConnectedBy =
      *( DataManager::entities( ).relationships( )["connectedBy"]->asOneToN( ));

    auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
      ->relationshipPropertiesTypes( );
    auto connectsToIt = relConnectsTo.find( _parentEntity->entityGid( ));
    if( !( connectsToIt != relConnectsTo.end( )))
    {
      propObject = relationshipPropertiesTypes.getRelationshipProperties(
        "connectsTo" )->create( );
      relConnectsTo[ _parentEntity->entityGid( )].insert(
        std::make_pair( _parentEntity->entityGid( ), propObject ));
      relConnectedBy[ _parentEntity->entityGid( )].insert(
        std::make_pair( _parentEntity->entityGid( ), nullptr ));
    }
    else
    {
      auto connectsToMMIt =
        connectsToIt->second.find( _parentEntity->entityGid( ));
      if( !( connectsToMMIt != connectsToIt->second.end( )))
      {
        propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" )->create( );
        relConnectsTo[ _parentEntity->entityGid( )].insert(
          std::make_pair( _parentEntity->entityGid( ), propObject ));
        relConnectedBy[ _parentEntity->entityGid( )].insert(
          std::make_pair( _parentEntity->entityGid( ), nullptr ));
      }
      else
      {
        propObject = connectsToMMIt->second;
      }
    }


    for ( const auto& propParam: _propParamCont )
    {
      const auto& labelWidget = std::get< TEditTuple::LABEL >( propParam );
      const auto& label = labelWidget->text( ).toStdString( );

      const auto& editType = std::get< TEditTuple::WIDGET_TYPE >( propParam );
      const auto& widget = std::get< TEditTuple::WIDGET >( propParam );
      QString paramString;
      if ( editType ==  TWidgetType::COMBO )
      {
        auto comboWidget = dynamic_cast< QComboBox* >( widget );
        paramString = comboWidget->currentText( );
      }
      else if ( editType == TWidgetType::LINE_EDIT )
      {
        auto lineEditwidget = dynamic_cast< QLineEdit* >( widget );
        paramString = lineEditwidget->text( );
      }
      else
        assert( false );

      auto caster = fires::PropertyManager::getPropertyCaster( label );
      auto& prop = propObject->getProperty( label );
      assert ( caster );

      caster->fromString( prop, paramString.toStdString( ));
    }

    bool needToClearCache = false;
    for ( const auto& creatorPair :
            nslib::RepresentationCreatorManager::creators( ))
    {
      needToClearCache = needToClearCache ||
        creatorPair.second->relationshipUpdatedOrCreated( propObject );
    }

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

  void ChildrenRelationshipEditWidget::refreshSubproperties( void )
  {
    // First set all values via caster
    // TODO this code is the same as in validate
    for ( const auto& propParam: _propParamCont )
    {
      const auto& labelWidget = std::get< TEditTuple::LABEL >( propParam );
      const auto& label = labelWidget->text( ).toStdString( );
      const auto& editType = std::get< TEditTuple::WIDGET_TYPE >( propParam );
      const auto& widget = std::get< TEditTuple::WIDGET >( propParam );
      QString paramString;

      if ( editType ==  TWidgetType::COMBO )
      {
        auto comboWidget = dynamic_cast< QComboBox* >( widget );
        paramString = comboWidget->currentText( );
      }
      else if ( editType == TWidgetType::LINE_EDIT )
      {
        auto lineEditwidget = dynamic_cast< QLineEdit* >( widget );
        paramString = lineEditwidget->text( );
      }
      else
        assert( false );

      auto caster = fires::PropertyManager::getPropertyCaster( label );
      auto& prop = _propObject->getProperty( label );
      assert ( caster );

      caster->fromString( prop, paramString.toStdString( ));
    }

    // Now hide/show widgets depending on the values set before
    for ( const auto& propParam: _propParamCont )
    {
      const auto& labelWidget = std::get< TEditTuple::LABEL >( propParam );
      const auto& label = labelWidget->text( ).toStdString( );
      const auto& widget = std::get< TEditTuple::WIDGET >( propParam );

      if ( !_propObject->evalConstraint( shift::Properties::SUBPROPERTY,
                                         label ))
      {
        labelWidget->hide( );
        widget->hide( );
      }
      else
      {
        labelWidget->show( );
        widget->show( );
      }
    }
  }

}
