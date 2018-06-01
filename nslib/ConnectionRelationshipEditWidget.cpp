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
#include "RepresentationCreatorManager.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <assert.h>


namespace nslib
{

  ConnectionRelationshipEditWidget::ConnectionRelationshipEditWidget(
    shift::Entity* originEntity_, shift::Entity* destinationEntity_,
    QWidget* parent_, bool modal )
    : QDialog ( parent_ )
    , _originEntity( originEntity_ )
    , _destinationEntity( destinationEntity_ )
  {

    this->setModal( modal );
    auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
      ->relationshipPropertiesTypes( );
    shift::Properties* propObject;

    bool propertyObjExists = true;

    auto& relConnectsTo =
      *( DataManager::entities( ).relationships( )["connectsTo"]->asOneToN( ));
    auto connectsToIt = relConnectsTo.find( _originEntity->entityGid( ));
    if( connectsToIt != relConnectsTo.end( ))
    {
      auto connectsToMMIt =
        connectsToIt->second.find( _destinationEntity->entityGid( ));
      if( connectsToMMIt != connectsToIt->second.end( ))
      {
        propObject = connectsToMMIt->second;
      }
      else
      {
        propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" );
        propertyObjExists = false;
      }
    }
    else
    {
      propObject = relationshipPropertiesTypes.getRelationshipProperties(
        "connectsTo" );
      propertyObjExists = false;
    }
    _propObject = propObject;

    assert( propObject );

    QGridLayout* myLayout = new QGridLayout;
    myLayout->setAlignment( Qt::AlignTop );

    auto labelRel = new QLabel( QString( "Relationship Parameters" ));
    myLayout->addWidget( labelRel, 0, 0, 1, 0 );

    unsigned int numProp = 1;

    for( const auto& propPair: propObject->properties( ))
    {
      const auto prop = propPair.first;
      auto caster = fires::PropertyManager::getPropertyCaster( prop );
      if ( caster )
      {
        auto propName = fires::PropertyGIDsManager::getPropertyLabel( prop );
        auto label = new QLabel( QString::fromStdString( propName ));
        myLayout->addWidget( label, numProp, 0 );

        const auto& categories = caster->categories( );
        TWidgetType widgetType;
        QWidget* widget;
        if ( !categories.empty( ) )
        {
          widgetType = TWidgetType::COMBO;
          auto comboBoxWidget = new QComboBox;
          widget = comboBoxWidget;
          auto currentCategory = caster->toString( propPair.second );
          unsigned int index = 0;
          for ( const auto& category : categories )
            comboBoxWidget->addItem( QString::fromStdString( category ));
          for ( const auto& category : categories )
          {
            if ( category != currentCategory )
              ++index;
            else
              break;
          }
          comboBoxWidget->setCurrentIndex( index );
          comboBoxWidget->setEnabled( true );
          connect( comboBoxWidget, SIGNAL( currentIndexChanged( int )),
            this, SLOT( refreshSubproperties( )));
        }
        else
        {
          widgetType = TWidgetType::LINE_EDIT;
          auto lineEditwidget = new QLineEdit;
          widget = lineEditwidget;
          if( propName == "Name" )
          {
            QString connectionName( QString::fromStdString(
              ( propertyObjExists ? propObject->getProperty(
                "Name" ).value<std::string>( ) : originEntity_->getProperty(
                "Entity name" ).value<std::string>( ) + "-" +
                destinationEntity_->getProperty(
                "Entity name" ).value<std::string>( ))));
            lineEditwidget->setText( connectionName );
          }
          else
          {
            lineEditwidget->setText( QString::fromStdString(
              caster->toString( propPair.second )));
          }
          lineEditwidget->setEnabled( true );
        }

        myLayout->addWidget( widget, numProp, 1 );

        if ( !propObject->evalConstraint( shift::Properties::SUBPROPERTY,
          propName ))
        {
          label->hide( );
          widget->hide( );
        }
        numProp++;
        _propParamCont.push_back( std::make_tuple( widgetType, label, widget ));
      }
    }

    QPushButton* cancelButton = new QPushButton( QString( "Cancel" ));
    myLayout->addWidget( cancelButton, numProp, 0 );
    QPushButton* validationButton = new QPushButton( QString( "Create" ));
    myLayout->addWidget( validationButton, numProp, 1 );

    connect( cancelButton, SIGNAL( clicked( )), this, SLOT( cancelDialog( )));
    connect( validationButton, SIGNAL( clicked( )),
             this, SLOT( validateDialog( )));

    setLayout( myLayout );
    setWindowTitle( tr( "Create connection relationship" ));
  }

  void ConnectionRelationshipEditWidget::validateDialog( void )
  {
    shift::RelationshipProperties* propObject;

    auto& relConnectsTo =
      *( DataManager::entities( ).relationships( )["connectsTo"]->asOneToN( ));
    auto& relConnectedBy =
      *( DataManager::entities( ).relationships( )["connectedBy"]->asOneToN( ));

    auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
      ->relationshipPropertiesTypes( );
    auto connectsToIt = relConnectsTo.find( _originEntity->entityGid( ));
    if( !( connectsToIt != relConnectsTo.end( )))
    {

      propObject = relationshipPropertiesTypes.getRelationshipProperties(
        "connectsTo" )->create( );
      relConnectsTo[ _originEntity->entityGid( )].insert(
        std::make_pair( _destinationEntity->entityGid( ), propObject ));
      relConnectedBy[ _destinationEntity->entityGid( )].insert(
        std::make_pair( _originEntity->entityGid( ), nullptr ));
    }
    else
    {
      auto connectsToMMIt =
        connectsToIt->second.find( _destinationEntity->entityGid( ));
      if( !( connectsToMMIt != connectsToIt->second.end( )))
      {
        propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" )->create( );
        relConnectsTo[ _originEntity->entityGid( )].insert(
          std::make_pair( _destinationEntity->entityGid( ), propObject ));
        relConnectedBy[ _destinationEntity->entityGid( )].insert(
          std::make_pair( _originEntity->entityGid( ), nullptr ));
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

  void ConnectionRelationshipEditWidget::cancelDialog(  )
  {
    this->hide( );
  }

  void ConnectionRelationshipEditWidget::refreshSubproperties( void )
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
