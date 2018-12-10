/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito <juanpedro.brito@upm.es>
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
#include "EntityEditWidget.h"
#include "DataManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "DomainManager.h"

#include <QtGui>
#include <QInputDialog>

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>

#include <memory>
#include <shift/Relationship.h>

//TODO: Add realtion of parent of to entities

namespace nslib
{
  QDockWidget* EntityEditWidget::_parentDock = nullptr;
  bool EntityEditWidget::_autoCloseChecked = false;
  bool EntityEditWidget::_checkUniquenessChecked = true;

  EntityEditWidget::EntityEditWidget( QWidget *parentWidget_)
    : QWidget( parentWidget_ )
    , _gridLayout ( new QGridLayout )
    , _gridLayoutProps ( new QGridLayout )
    , _numNewEntities ( new QLineEdit )
    , _autoCloseCheck( new QCheckBox )
    , _checkUniquenessCheck( new QCheckBox )
    , _eraseButton( new QPushButton( tr( "Delete" )))
    , _cancelButton( new QPushButton( tr( "Close" )))
    , _validationButton( new QPushButton )
    , _titleLabel( new QLabel )
    , _numEntitiesLabel( new QLabel( tr( "Number of entities" )))
    , _separation( new QFrame( ))
  {
    _gridLayout->setAlignment( Qt::AlignTop );
    _gridLayout->setColumnStretch( 1, 1 );
    _gridLayout->setColumnMinimumWidth( 1, 150 );

    _titleLabel->setStyleSheet( "font-weight:bold" );

    unsigned  int rowNum = 0;

    _gridLayout->addWidget( _titleLabel, rowNum, 0, 1, 2 );
    _gridLayout->addLayout( _gridLayoutProps, ++rowNum, 0, 1, 2 );

    _separation->setFrameShape( QFrame::HLine );
    _gridLayout->addWidget( _separation, ++rowNum, 0, 1, 2 );

    _gridLayout->addWidget( _numEntitiesLabel, ++rowNum, 0 );

    _numNewEntities->setEnabled( true );

    _gridLayout->addWidget( _numNewEntities, rowNum, 1 );

    _gridLayout->addWidget( _cancelButton, ++rowNum, 0 );


    _gridLayout->addWidget( _validationButton, rowNum, 1 );

    _gridLayout->addWidget( _eraseButton, ++rowNum, 0, 1, 2 );

    QLabel* autoCloseLabel = new QLabel( tr( "Auto-close" ));

    _gridLayout->addWidget( autoCloseLabel, ++rowNum, 0 );

    _autoCloseCheck->setChecked( _autoCloseChecked );
    connect( _autoCloseCheck, SIGNAL( clicked( )),
             this, SLOT( toggleAutoClose( )));
    _gridLayout->addWidget( _autoCloseCheck, rowNum, 1 );

    QLabel* checkUniquenessLabel = new QLabel( tr( "Check uniqueness" ));
    _gridLayout->addWidget( checkUniquenessLabel, ++rowNum, 0 );

    _checkUniquenessCheck->setChecked( _checkUniquenessChecked );
    connect( _checkUniquenessCheck, SIGNAL( clicked( )),
      this, SLOT( toggleCheckUniqueness( )));
    _gridLayout->addWidget( _checkUniquenessCheck, rowNum, 1 );

    connect( _cancelButton, SIGNAL( clicked( )), this, SLOT( cancelDialog( )));
    connect( _validationButton, SIGNAL( clicked( )),
      this, SLOT( validateDialog( )));

    setLayout( _gridLayout );

    _parentDock->setWidget( this );
  }

  void EntityEditWidget::updateEntity( shift::Entity* entity_,
    TEntityEditWidgetAction action_,
    shift::Entity* parentEntity_, bool addToScene_)
  {
    _entity = entity_;
    _parentEntity = parentEntity_;
    _action = action_;
    _isNew = action_ == TEntityEditWidgetAction::NEW_ENTITY;
    _addToScene = addToScene_;

    unsigned int element = 0;
    QLayoutItem* item;
    while (( item = _gridLayoutProps->takeAt( 0 )) != nullptr )
    {
      delete item->widget();
      delete item;
    }
    _entityParamCont.clear( );

    if ( _entity )
    {
      if ( _action == DUPLICATE_ENTITY || _isNew )
      {
        _separation->setVisible( true );
        _numEntitiesLabel->setVisible( true );
        _numNewEntities->setText( "1" );
        _numNewEntities->setVisible( true );

        if( _isNew )
        {
          _eraseButton->setVisible( false );
          _validationButton->setText( tr( "New" ));
          _titleLabel->setText( tr( "Creating new: " )
             + QString::fromStdString( _entity->entityName( )));
        }
        else
        {
          _eraseButton->setVisible( false );
          _validationButton->setText( tr( "Duplicate" ));
          _titleLabel->setText( tr( "Duplicate: " )
            + QString::fromStdString( _entity->entityName( )));
        }
      }
      else
      {
        _eraseButton->setVisible( true );
        _separation->setVisible( false );
        _numEntitiesLabel->setVisible( false );
        _numNewEntities->setVisible( false );
        _validationButton->setText( tr( "Save" ));
        _titleLabel->setText( tr("Editing: ")
          + QString::fromStdString( _entity->entityName( )));
      }

      TWidgetType widgetType;
      QWidget* widget;

      for ( const auto& propPair : _entity->properties( ))
      {
        const auto prop = propPair.first;
        auto caster = fires::PropertyManager::getPropertyCaster( prop );
        if ( caster )
        {
          auto propName = fires::PropertyGIDsManager::getPropertyLabel( prop );
          auto label = new QLabel(
            QString::fromStdString( propName ));

          _gridLayoutProps->addWidget( label, element, 0 );

          const auto& categories = caster->categories( );

          bool isEditable = _entity->hasPropertyFlag( propName,
            shift::Entity::TPropertyFlag::EDITABLE );

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
              {
                ++index;
              }
              else
              {
                break;
              }
            }
            comboBoxWidget->setCurrentIndex( index );
            comboBoxWidget->setEnabled( isEditable );
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
          _gridLayoutProps->addWidget( widget, element, 1 );
          ++element;

          _entityParamCont.push_back( std::make_tuple(
            widgetType, label, widget ));
        }
      }
    }
    _parentDock->show( );
    this->show( );
  }

  void EntityEditWidget::validateDialog( void )
  {
    unsigned int numEles = 1;

    if ( _action == DUPLICATE_ENTITY || _isNew )
    {
      numEles = _numNewEntities->text( ).toUInt( );
    }

    for ( unsigned int i = 0; i < numEles; ++i )
    {
      if ( _action == DUPLICATE_ENTITY || _isNew )
      {
        _entity = _entity->create( );
      }

      QList< QString > errorMessages;

      assert ( _entity );

      for ( const auto& entityParam: _entityParamCont )
      {
        const auto& labelWidget = std::get< TEditTuple::LABEL >( entityParam );
        const auto& label = labelWidget->text( ).toStdString( );

        const bool isEditable = _entity->hasPropertyFlag(
          label, shift::Entity::TPropertyFlag::EDITABLE );

        if ( ( _action == EDIT_ENTITY ) && !isEditable )
        {
          continue;
        }
        const auto& editType =
          std::get< TEditTuple::WIDGET_TYPE >( entityParam );
        const auto& widget = std::get< TEditTuple::WIDGET >( entityParam );
        QString paramString;
        if ( editType == TWidgetType::COMBO )
        {
          const auto comboWidget = dynamic_cast< QComboBox* >( widget );
          paramString = comboWidget->currentText( );
        }
        else if ( editType == TWidgetType::LINE_EDIT )
        {
          const auto lineEditwidget = dynamic_cast< QLineEdit* >( widget );
          paramString = lineEditwidget->text( );
          //change name if multiple are created silmultaneus
          if ( numEles > 1 && label == "Entity name" )
          {
            paramString = paramString + QString( "_" ) + QString::number( i );
          }
        }
        else
        {
          assert( false );
        }

        const auto caster = fires::PropertyManager::getPropertyCaster( label );
        if ( !_entity->hasProperty( label ))
        {
          // WAR: This is to force create/copy properties not defined in the
          // JSON but defined in when loading data, but does not solve all cases
          auto prop = _entity->getProperty( label );
          _entity->registerProperty( label, prop );
        }
        auto& prop = _entity->getProperty( label );
        assert( caster );

        bool saveNewPropValue = true;

        if ( _checkUniquenessCheck->isChecked( ))
        {
          const bool isUnique = _entity->hasPropertyFlag(
            label, shift::Entity::TPropertyFlag::UNIQUE );
          if ( isUnique )
          {
            std::string pStr = paramString.toStdString( );
            if ( _isNew ||
              caster->toString( prop ) != pStr ) // If value changed
            {
              const auto &entities = nslib::DataManager::entities( ).vector( );
              for( const auto entity: entities )
              {
                if ( entity->isSameEntityType( _entity ) &&
                     entity->hasProperty( label ))
                {
                  if ( caster->toString( entity->getProperty( label )) == pStr )
                  {
                    errorMessages.push_back(QString( "Property '" ) +
                       QString::fromStdString( label ) +
                       QString( "' is repeated" ));
                    saveNewPropValue = false;
                    break;
                  }
                }
              }
            }
          }
        }
        if ( saveNewPropValue )
        {
          caster->fromString( prop, paramString.toStdString( ));
        }
      }

      if ( !errorMessages.empty( ))
      {
        QString errors = "<p><strong>Errors: </strong></p><ul>";
        for ( const auto& err: errorMessages )
        {
          errors += QString( "<li>" ) + err + QString( "</li>" );
        }
        errors += "</ul>";
        QMessageBox::warning( this, "Errors", errors );
        return;
      }
      else
      {
        if ( _autoCloseCheck->isChecked( ))
        {
          this->hide( );
          _parentDock->close( );
        }
      }

      bool needToClearCache = false;
      for ( const auto& creatorPair :
        nslib::RepresentationCreatorManager::creators( ))
      {
        needToClearCache = needToClearCache ||
          creatorPair.second->entityUpdatedOrCreated( _entity );
      }

      // TODO improvemente: check if cache needs to be cleared or if just the
      // items related to the entity under edition
      // if ( needToClearCache ) {
      nslib::RepresentationCreatorManager::clearEntitiesToReps( );
      nslib::RepresentationCreatorManager::clearRelationshipsCache( );
      // }

      if ( _action == DUPLICATE_ENTITY || _isNew )
      {
        nslib::DataManager::entities( ).add( _entity );
        if ( _addToScene )
        {
          nslib::PaneManager::activePane( )->entities( ).add( _entity );
        }

        std::vector< shift::Entity* > subentities;
        _entity->createSubEntities( subentities );

        auto& _entities = nslib::DataManager::entities( );
        auto& relSuperEntityOf =
          *( _entities.relationships( )[ "isSuperEntityOf" ]->asOneToN( ));
        auto& relSubEntityOf =
          *( _entities.relationships( )[ "isSubEntityOf" ]->asOneToOne( ));

        //TODO: improve this set of adds which will do push_backs
        for ( const auto& subentity : subentities )
        {
          shift::Relationship::Establish( relSuperEntityOf, relSubEntityOf,
            _entity, subentity );
          nslib::DataManager::entities( ).add( subentity );
          nslib::PaneManager::activePane( )->entities( ).add( subentity );
        }

        // nslib::PaneManager::activePane( )->refreshProperties(
        // nslib::PaneManager::activePane( )->entities( ));
        // nslib::PaneManager::activePane( )->resizeEvent( nullptr );
        nslib::PaneManager::activePane( )->displayEntities(
          nslib::PaneManager::activePane( )->entities( ), false, true );

        if ( _parentEntity )
        {

          auto& entities = nslib::DataManager::entities( );
          auto& relParentOf =
            *( entities.relationships( )[ "isParentOf" ]->asOneToN( ));
          auto& relChildOf =
            *( entities.relationships( )[ "isChildOf" ]->asOneToOne( ));
          auto& relAggregatedConnectsTo = *( entities.relationships( )
            [ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
          auto& relAggregatedConnectedBy = *( entities.relationships( )
            [ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));

          shift::Relationship::EstablishWithHierarchy( relParentOf, relChildOf,
            relAggregatedConnectsTo, relAggregatedConnectedBy,
            _parentEntity, _entity );
          nslib::PaneManager::activePane( )->refreshProperties (
            nslib::PaneManager::activePane( )->entities( ));
        }
        else
        {
          nslib::DataManager::rootEntities( ).add( _entity );
        }
      }
      if ( _action == EDIT_ENTITY )
      {
        auto& relChildOf = *( DataManager::entities( ).relationships( )
          [ "isChildOf" ]->asOneToOne( ));
        auto parentID = relChildOf[ _entity->entityGid( ) ].entity;
        if ( parentID > 0 )
        {
          auto parent = DataManager::entities( ).at( parentID );
          parent->autoUpdateProperties( );
        }
        for ( const auto& repPair :
          nslib::RepresentationCreatorManager::repsToEntities( ))
        {
          shift::Representation* rep = repPair.first;
          delete rep;
        }
        for ( auto pane : nslib::PaneManager::panes( ))
        {
          pane->reps( ).clear( );
          // pane->resizeEvent( nullptr );
          pane->displayEntities(
            nslib::PaneManager::activePane( )->entities( ), false, true );
        }
      }
    }
  }


  void EntityEditWidget::cancelDialog( void )
  {
    this->hide( );
    _parentDock->close( );
  }

  void EntityEditWidget::parentDock( QDockWidget* parentDock_ )
  {
    _parentDock = parentDock_;
  }

  QDockWidget* EntityEditWidget::parentDock( void )
  {
    return _parentDock;
  }

  void EntityEditWidget::toggleAutoClose( void )
  {
    _autoCloseChecked = _autoCloseCheck->isChecked( );
  }

  void EntityEditWidget::toggleCheckUniqueness( void )
  {
    _checkUniquenessChecked = _checkUniquenessCheck->isChecked( );
  }

  EntityEditWidget::~EntityEditWidget( void )
  {
  }

  shift::Entity* EntityEditWidget::entity( )
  {
    return _entity;
  }

} // namespace