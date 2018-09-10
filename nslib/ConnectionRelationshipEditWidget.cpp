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
#include <QLabel>
#include <QLineEdit>
#include <assert.h>

namespace nslib
{

  QDockWidget* ConnectionRelationshipEditWidget::_parentDock = nullptr;
  bool ConnectionRelationshipEditWidget::_autoCloseChecked = false;

  ConnectionRelationshipEditWidget::ConnectionRelationshipEditWidget(
    shift::Entity* originEntity_, shift::Entity* destinationEntity_,
    QWidget* parentWidget_ )
    : QFrame ( parentWidget_ )
    , _originEntity( originEntity_ )
    , _destinationEntity( destinationEntity_ )
    , _autoCloseLabel( new QLabel( tr( "Auto-close" )))
    , _autoCloseCheck( new QCheckBox( ))
    , _eraseButton( new QPushButton( QString( tr( "Break" ))))
    , _cancelButton( new QPushButton( QString( tr( "Cancel" ))))
    , _gridLayout( new QGridLayout( ))
{
  unsigned int numProp = 1u;
  _gridLayout->setAlignment( Qt::AlignTop );
  _gridLayout->setColumnStretch( 1, 1 );
  _gridLayout->setColumnMinimumWidth( 1, 150 );

  auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
      ->relationshipPropertiesTypes( );

  auto& relConnectsTo = *( DataManager::entities( )
      .relationships( )[ "connectsTo" ]->asOneToN( ));
  auto connectsToIt = relConnectsTo.find( _originEntity->entityGid( ));
  if( connectsToIt != relConnectsTo.end( ))
  {
    auto connectsToMMIt =
        connectsToIt->second.find( _destinationEntity->entityGid( ));
    if( connectsToMMIt != connectsToIt->second.end( ))
    {
      _propObject = connectsToMMIt->second;
      _isNew = false;
    }
    else
    {
      _propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" )->create( );
      _isNew = true;
    }
  }
  else
  {
    _propObject = relationshipPropertiesTypes.getRelationshipProperties(
        "connectsTo" )->create( );
    _isNew = true;
  }
  assert( _propObject );

  auto labelRel = new QLabel( QString( tr( "Relationship Parameters" )));
  _gridLayout->addWidget( labelRel, 0, 0, 1, 0 );

  for( const auto& propPair: _propObject->properties( ))
  {
    const auto prop = propPair.first;
    auto caster = fires::PropertyManager::getPropertyCaster( prop );
    if ( caster )
    {
      auto propName = fires::PropertyGIDsManager::getPropertyLabel( prop );
      auto label = new QLabel( QString::fromStdString( propName ));
      _gridLayout->addWidget( label, numProp, 0 );

      const auto& categories = caster->categories( );
      TWidgetType widgetType;
      QWidget* widget;
      if ( !categories.empty( ))
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
              ( _isNew ? _originEntity->getProperty( "Entity name" )
              .value< std::string >( ) + "-" + _destinationEntity->
              getProperty( "Entity name" ).value<std::string>( )
              : _propObject->getProperty( "Name" ).value< std::string >( ))));
          lineEditwidget->setText( connectionName );
        }
        else
        {
          lineEditwidget->setText( QString::fromStdString(
              caster->toString( propPair.second )));
        }
        lineEditwidget->setEnabled( true );
      }

      _gridLayout->addWidget( widget, numProp, 1 );

      if ( !_propObject->evalConstraint( shift::Properties::SUBPROPERTY,
        propName ))
      {
        label->hide( );
        widget->hide( );
      }
      ++numProp;
      _propParamCont.push_back( std::make_tuple( widgetType, label, widget ));
    }
  }


  _gridLayout->addWidget( _cancelButton.get( ), numProp, 0 );

  if ( _isNew )
  {
    setWindowTitle( tr( "Create connection relationship" ));
    _validationButton.reset( new QPushButton( QString( tr( "Create" ))));
    _eraseButton->hide();
  }
  else
  {
    setWindowTitle( tr( "Edit connection relationship" ));
    _validationButton.reset( new QPushButton( QString( tr( "Save" ))));
  }
  _gridLayout->addWidget( _validationButton.get( ), numProp, 1 );
  _gridLayout->addWidget( _eraseButton.get( ), ++numProp, 0, 1, 2);
  _gridLayout->addWidget( _autoCloseLabel.get( ), ++numProp, 0 );

  _autoCloseCheck->setChecked( _autoCloseChecked );
  _gridLayout->addWidget( _autoCloseCheck.get( ), numProp, 1 );

  connect( _eraseButton.get( ), SIGNAL( clicked( )), this,
    SLOT( breakDialog( )));
  connect( _validationButton.get( ), SIGNAL( clicked( )),
           this, SLOT( validateDialog( )));
  connect( _cancelButton.get( ), SIGNAL( clicked( )), this,
           SLOT( cancelDialog( )));
  connect( _autoCloseCheck.get( ), SIGNAL( clicked( )),
      this, SLOT( toggleAutoClose( )));

  setLayout( _gridLayout.get( ) );

  _parentDock->setWidget( this );
  _parentDock->show( );
  this->show( );
  }

  void ConnectionRelationshipEditWidget::validateDialog( void )
  {
    if( _isNew )
    {
      auto& relAggregatedConnectsTo = *( DataManager::entities( )
        .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
      auto& relAggregatedConnectedBy = *( DataManager::entities( )
        .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));
      shift::Relationship::EstablishAndAggregate( relAggregatedConnectsTo,
        relAggregatedConnectedBy, DataManager::entities( ), _originEntity,
        _destinationEntity, _propObject, _propObject );
      checkClose( );
    }
    else if ( _autoCloseCheck->isChecked( ))
    {
      this->hide( );
      _parentDock->close( );
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
      auto& prop = _propObject->getProperty( label );
      assert ( caster );

      caster->fromString( prop, paramString.toStdString( ));
    }

    bool needToClearCache = false;
    for ( const auto& creatorPair : RepresentationCreatorManager::creators( ))
    {
      needToClearCache = needToClearCache ||
        creatorPair.second->relationshipUpdatedOrCreated( _propObject );
    }

    // TODO improvement: check if cache needs to be cleared or if just the
    // items related to the entity under edition
    // if ( needToClearCache ) {
    RepresentationCreatorManager::clearRelationshipsCache( );
    // }

    for ( auto pane : PaneManager::panes( ))
    {
      pane->resizeEvent( nullptr );
    }
  }

  void ConnectionRelationshipEditWidget::cancelDialog(  )
  {
    this->hide( );
    _parentDock->close( );
  }

  ConnectionRelationshipEditWidget::~ConnectionRelationshipEditWidget( void )
  {
    if ( _isNew )
    {
      delete _propObject;
    }
  };

  void ConnectionRelationshipEditWidget::parentDock( QDockWidget* parentDock_ )
  {
    _parentDock = parentDock_;
  }

  QDockWidget* ConnectionRelationshipEditWidget::parentDock( void )
  {
    return _parentDock;
  }

  void ConnectionRelationshipEditWidget::toggleAutoClose( void )
  {
    _autoCloseChecked = _autoCloseCheck->isChecked( );
  }

  void ConnectionRelationshipEditWidget::breakDialog(  )
  {
    auto& relAggregatedConnectsTo = *( DataManager::entities( )
        .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
    auto& relAggregatedConnectedBy = *( DataManager::entities( )
        .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));
    shift::Relationship::BreakAnAggregatedRelation( relAggregatedConnectsTo,
      relAggregatedConnectedBy, DataManager::entities( ), _originEntity,
      _destinationEntity );

    for ( auto pane : PaneManager::panes( ))
    {
      pane->resizeEvent( nullptr );
    }
    checkClose( );
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

      if ( !_propObject->evalConstraint(
        shift::Properties::SUBPROPERTY, label ))
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

  void ConnectionRelationshipEditWidget::checkClose( )
  {
    if ( _autoCloseCheck->isChecked( ))
    {
      this->hide( );
      _parentDock->close( );
    }
    else if( _isNew )
    {
      _isNew = false;
      _validationButton->setText( tr( "Save" ));
      setWindowTitle( tr( "Create connection relationship" ));
      _eraseButton->show();
    }
    else
    {
      auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
          ->relationshipPropertiesTypes( );
      _propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" )->create( );
      _isNew = true;
      _validationButton->setText( tr( "Create" ));
      setWindowTitle( tr( "Edit connection relationship" ));
      _eraseButton->hide();
    }
  }
}
