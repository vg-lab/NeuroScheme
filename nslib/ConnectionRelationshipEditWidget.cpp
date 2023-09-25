/*
 * Copyright (c) 2017 VG-Lab/URJC/UPM.
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
#include "InteractionManager.h"
#include "Loggers.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include <assert.h>

namespace nslib
{
  QDockWidget* ConnectionRelationshipEditWidget::_parentDock = nullptr;
  bool ConnectionRelationshipEditWidget::_autoCloseChecked = false;

  ConnectionRelationshipEditWidget::ConnectionRelationshipEditWidget(
    QWidget* parentWidget_ )
    : QWidget ( parentWidget_ )
    , _originEntity( nullptr )
    , _destEntity( nullptr )
    , _updateOriginEntity( nullptr )
    , _updateDestEntity( nullptr )
    , _updateConnectionType( TConnectionType::AUTO )
    , _labelTitleRel( new QLabel )
    , _labelEntitiesRel( new QLabel )
    , _autoCloseCheck( new QCheckBox( ))
    , _validationButton( new QPushButton )
    , _eraseButton( new QPushButton( QString( tr( "Delete" ))))
    , _cancelButton( new QPushButton( QString( tr( "Cancel" ))))
    , _gridLayout( new QGridLayout( ))
    , _gridPropertiesLayout( new QGridLayout( ))
  {
    unsigned int numProp = 0u;
    _gridLayout->setAlignment( Qt::AlignTop );
    _gridLayout->setColumnStretch( 0, 1 );
    _gridLayout->setColumnStretch( 1, 1 );
    _labelTitleRel->setStyleSheet( "font-weight:bold" );
    _labelEntitiesRel->setStyleSheet( "font-weight:bold" );
    _gridLayout->addWidget( _labelTitleRel, numProp, 0, 1, 0 );
    _gridLayout->addWidget( _labelEntitiesRel, ++numProp, 0, 1, 0 );

    _gridLayout->addLayout(_gridPropertiesLayout, ++numProp, 0, 1, 2 );

    auto separation = new QFrame( );
    separation->setFrameShape( QFrame::HLine );
    _gridLayout->addWidget( separation, ++numProp, 0, 1, 2 );

    _gridLayout->addWidget( _cancelButton, ++numProp, 0 );

    _gridLayout->addWidget( _validationButton, numProp, 1 );
    _gridLayout->addWidget( _eraseButton, ++numProp, 0, 1, 2);
    auto autoCloseLabel =  new QLabel( tr( "Auto-close" ));
    _gridLayout->addWidget( autoCloseLabel, ++numProp, 0 );

    _autoCloseCheck->setChecked( _autoCloseChecked );
    _gridLayout->addWidget( _autoCloseCheck, numProp, 1 );

    connect( _eraseButton, SIGNAL( clicked( )), this,
      SLOT( breakDialog( )));
    connect( _validationButton, SIGNAL( clicked( )),
      this, SLOT( validateDialog( )));
    connect( _cancelButton, SIGNAL( clicked( )), this,
      SLOT( cancelDialog( )));
    connect( _autoCloseCheck, SIGNAL( clicked( )),
      this, SLOT( toggleAutoClose( )));

    setLayout( _gridLayout );

    if(_parentDock)
      _parentDock->setWidget( this );
  }

  void ConnectionRelationshipEditWidget::updateWidget(
    shift::Entity* originEntity_, shift::Entity* destinationEntity_,
    TConnectionType connectionType_ )
  {
    _updateOriginEntity = originEntity_;
    _updateDestEntity = destinationEntity_;
    _updateConnectionType = connectionType_;

    refreshEntities();
  }

  void ConnectionRelationshipEditWidget::validateDialog( void )
  {
    refreshValues( );

    bool needToClearCache = false;
    for ( const auto& creatorPair : RepresentationCreatorManager::creators( ))
    {
      needToClearCache = needToClearCache ||
        creatorPair.second->relationshipUpdatedOrCreated( _propObject );
    }
    auto& relAggregatedConnectsTo = *( DataManager::entities( )
      .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
    auto& relAggregatedConnectedBy = *( DataManager::entities( )
      .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));

    const auto originGid = _originEntity->entityGid( );
    const auto destGid = _destEntity->entityGid( );
    if( _isNew )
    {
      shift::Relationship::EstablishAndAggregate( relAggregatedConnectsTo,
        relAggregatedConnectedBy, DataManager::entities( ), _originEntity,
        _destEntity, _propObject, _propObject );
      InteractionManager::updateEntityConnectionList( originGid, destGid );
      checkClose( );
    }
    else
    {
      InteractionManager::refreshEntityConnectionList( );
      relAggregatedConnectsTo.updateDependentRelations( originGid, destGid );
      relAggregatedConnectedBy.updateDependentRelations( destGid, originGid );
      if( _autoCloseCheck->isChecked( ) )
      {
        this->hide( );
        if(_parentDock)
          _parentDock->close( );
      }
    }

    // TODO: improvement: check if cache needs to be cleared or if just the
    // items related to the entity under edition
    // if ( needToClearCache ) {
    RepresentationCreatorManager::clearRelationshipsCache( );
    // }

    for ( auto &pane : PaneManager::panes( ))
    {
      pane->displayEntities( false, true );
    }
  }

  void ConnectionRelationshipEditWidget::refreshValues( )
  {
    for ( const auto& propParam: _propParamCont )
    {
      const auto& labelWidget = std::get< TEditTuple::LABEL >( propParam );
      const auto& label = labelWidget->text( ).toStdString( );

      const auto& editType = std::get< TEditTuple::WIDGET_TYPE >( propParam );
      const auto& widget = std::get< TEditTuple::WIDGET >( propParam );
      QString paramString;
      if ( editType ==  TWidgetType::COMBO )
      {
        const auto comboWidget = dynamic_cast< QComboBox* >( widget );
        paramString = comboWidget->currentText( );
      }
      else if ( editType == TWidgetType::LINE_EDIT )
      {
        const auto lineEditwidget = dynamic_cast< QLineEdit* >( widget );
        paramString = lineEditwidget->text( );
      }
      else
      {
        assert( false );
      }

      const auto caster = fires::PropertyManager::getPropertyCaster( label );
      auto& prop = _propObject->getProperty( label );
      assert ( caster );

      caster->fromString( prop, paramString.toStdString( ));
    }
  }

  void ConnectionRelationshipEditWidget::cancelDialog( void )
  {
    this->hide( );
    if(_parentDock)
      _parentDock->close( );
  }

  ConnectionRelationshipEditWidget::~ConnectionRelationshipEditWidget( void )
  {
    if ( _isNew && _destEntity && _originEntity )
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
      relAggregatedConnectedBy, _originEntity, _destEntity );

    const auto origGid = _originEntity->entityGid( );
    const auto destGid = _destEntity->entityGid( );

    for ( auto &pane : PaneManager::panes( ))
    {
      pane->resizeEvent( nullptr );
    }
    InteractionManager::updateEntityConnectionList( origGid, destGid );
    checkClose( );
  }

  void ConnectionRelationshipEditWidget::refreshSubproperties( void )
  {
    // First set all values via caster
    refreshValues( );

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
      if(_parentDock)
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
      auto relationshipPropertiesTypes =
          DomainManager::getActiveDomain( )->relationshipPropertiesTypes( );
      _propObject = relationshipPropertiesTypes.getRelationshipProperties(
          "connectsTo" )->create( );
      _isNew = true;
      _validationButton->setText( tr( "Create" ));
      setWindowTitle( tr( "Edit connection relationship" ));
      _eraseButton->hide( );
    }
  }

  shift::Entity* ConnectionRelationshipEditWidget::originEntity( ) const
  {
    return _originEntity;
  }

  shift::Entity*
  ConnectionRelationshipEditWidget::destEntity( ) const
  {
    return _destEntity;
  }

  bool ConnectionRelationshipEditWidget::isAggregated( ) const
  {
    return _isAggregated;
  }

  void ConnectionRelationshipEditWidget::refreshEntities( void )
  {
    if( _updateOriginEntity && _updateDestEntity )
    {
      if( _updateConnectionType != TConnectionType::AGGREGATED )
      {
        if( !shift::RelationshipPropertiesTypes::isConstrained( "ConnectedTo",
           _updateOriginEntity->typeName( ), _updateDestEntity->typeName( )))
        {
          if ( _updateConnectionType == TConnectionType::SIMPLE )
          {
            Loggers::get( )->log( "A " + _updateOriginEntity->typeName( ) +
              " cannot be connected to a " + _updateDestEntity->typeName( ) +
              '.', LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE );
            this->hide( );
            if(_parentDock)
              _parentDock->hide( );
            return;
          }
          else
          {
            _isAggregated = true;
            Loggers::get( )->log( "A " + _updateOriginEntity->typeName( ) +
              " cannot be connected to a " + _updateDestEntity->typeName( ) +
              ". Searching aggregated connections.", LOG_LEVEL_VERBOSE,
              NEUROSCHEME_FILE_LINE );
          }
        }
        else
        {
          _isAggregated = false;
        }
      }
      const std::string originName = _updateOriginEntity
        ->getPropertyValue< std::string >( "Entity name", " " );
      const std::string destName = _updateDestEntity
        ->getPropertyValue< std::string >( "Entity name", " " );
      if( _isAggregated )
      {
        auto& relAggregatedConnectsTo = *( DataManager::entities( )
          .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
        _propObject = relAggregatedConnectsTo.getRelationProperties(
          _updateOriginEntity->entityGid( ), _updateDestEntity->entityGid( ));
        if( !_propObject )
        {
          Loggers::get( )->log( "Entity " + originName +
            " cannot be aggregated connected to entity " + destName + '.',
            LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE );
          cancelDialog( );
          return;
        }
        _labelTitleRel->setText(
            tr( "Editing Aggregated Relationship Parameters: " ));
        _isNew = false;
      }
      else
      {
        auto& relConnectsTo = *( DataManager::entities( )
          .relationships( )[ "connectsTo" ]->asOneToN( ));
        _propObject = relConnectsTo.getRelationProperties(
          _updateOriginEntity->entityGid( ), _updateDestEntity->entityGid( ));
        if( _propObject )
        {
          _isNew = false;
          _labelTitleRel->setText( tr( "Editing Relationship Parameters: " ));
        }
        else
        {
          auto relationshipPropertiesTypes = DomainManager::getActiveDomain( )
            ->relationshipPropertiesTypes( );
          _propObject = relationshipPropertiesTypes.getRelationshipProperties(
            "connectsTo" )->create( );
          _propObject->setProperty( "Name",
            "R:" + originName + "-" + destName );
          _isNew = true;
          _labelTitleRel->setText( tr( "Creating Relationship Parameters: " ));
        }
      }
      assert( _propObject );
      _labelEntitiesRel->setText(
          QString::fromStdString( originName + " - " + destName ));

      unsigned int numProp = 0u;
      QLayoutItem* item;
      while( ( item = _gridPropertiesLayout->takeAt( 0 )) != nullptr )
      {
        delete item->widget( );
        delete item;
      }
      _propParamCont.clear( );
      for( const auto& propPair: _propObject->properties( ))
      {
        const auto prop = propPair.first;
        const auto caster = fires::PropertyManager::getPropertyCaster( prop );
        const auto propName =
          fires::PropertyGIDsManager::getPropertyLabel( prop );
        if ( caster  && !_propObject->hasPropertyFlag( propName,
          shift::Properties::TPropertyFlag::HIDE ))
        {
          const auto label = new QLabel( QString::fromStdString( propName ));
          _gridPropertiesLayout->addWidget( label, numProp, 0 );

          const auto& categories = caster->categories( );
          TWidgetType widgetType;
          QWidget* widget;
          const bool isEditable = _propObject->hasPropertyFlag(
              propName, shift::Properties::TPropertyFlag::EDITABLE );
          if( !categories.empty( ))
          {
            widgetType = TWidgetType::COMBO;
            auto comboBoxWidget = new QComboBox;
            widget = comboBoxWidget;
            auto currentCategory = caster->toString( propPair.second );
            unsigned int index = 0;
            for( const auto& category : categories )
              comboBoxWidget->addItem( QString::fromStdString( category ));
            for( const auto& category : categories )
            {
              if( category != currentCategory )
                ++index;
              else
                break;
            }
            comboBoxWidget->setCurrentIndex( index );
            comboBoxWidget->setEnabled( isEditable );
            connect( comboBoxWidget, SIGNAL( currentIndexChanged( int )),
              this, SLOT( refreshSubproperties( )));
          }
          else
          {
            widgetType = TWidgetType::LINE_EDIT;
            auto lineEditwidget = new QLineEdit;
            widget = lineEditwidget;
            lineEditwidget->setText( QString::fromStdString(
                caster->toString( propPair.second )));

            lineEditwidget->setEnabled( isEditable );
          }
          _gridPropertiesLayout->addWidget( widget, numProp, 1 );

          if( !_propObject->evalConstraint( shift::Properties::SUBPROPERTY,
            propName ))
          {
            label->hide( );
            widget->hide( );
          }
          ++numProp;
          _propParamCont.push_back(
              std::make_tuple( widgetType, label, widget ));
        }
      }

      if( _isNew )
      {
        _validationButton->setText( tr( "Create" ));
        _eraseButton->hide( );
      }
      else
      {
        if( _isAggregated )
        {
          _eraseButton->hide( );
        }
        else
        {
          _eraseButton->show( );
        }
        _validationButton->setText( tr( "Save" ));
      }

      if(_parentDock)
        _parentDock->show( );
      this->show( );
      _originEntity = _updateOriginEntity;
      _destEntity = _updateDestEntity;
      _updateOriginEntity = _updateDestEntity = nullptr;
    }
  }
}
