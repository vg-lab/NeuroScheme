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
#include <QPushButton>
#include <QGridLayout>
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

  EntityEditWidget::EntityEditWidget(
    shift::Entity* entity, TEntityEditWidgetAction action, QWidget *parentWidget_,
    bool addToScene_, shift::Entity* parentEntity_)
    : QWidget( parentWidget_ )
    , _entity( nullptr )
    , _parentEntity( parentEntity_ )
    , _addToScene( addToScene_ )
    , _action( action )
    , _isNew( action == TEntityEditWidgetAction::NEW_ENTITY )
    , _autoCloseCheck( new QCheckBox )
    , _checkUniquenessCheck( new QCheckBox )
  {
    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->setAlignment( Qt::AlignTop );
    gridLayout->setColumnStretch( 1, 1 );
    gridLayout->setColumnMinimumWidth( 1, 150 );

    unsigned int element = 0;

    if ( entity )
    {
      TWidgetType widgetType;
      QWidget* widget;

      for ( const auto& propPair : entity->properties( ))
      {
        const auto prop = propPair.first;
        auto caster = fires::PropertyManager::getPropertyCaster( prop );
        if ( caster )
        {
          auto propName = fires::PropertyGIDsManager::getPropertyLabel( prop );
          auto label = new QLabel(
            QString::fromStdString( propName ));

          gridLayout->addWidget( label, element, 0 );

          const auto& categories = caster->categories( );

          bool isEditable = entity->hasPropertyFlag(
            propName, shift::Entity::TPropertyFlag::EDITABLE );

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
          gridLayout->addWidget( widget, element, 1 );
          ++element;

          _entityParamCont.push_back(
            std::make_tuple( widgetType, label, widget ));
        }
      }

      if ( _action == DUPLICATE_ENTITY || _isNew )
      {
        QFrame* sep3 = new QFrame( );
        QFrame* sep4 = new QFrame( );
        sep3->setFrameShape( QFrame::HLine );
        sep4->setFrameShape( QFrame::HLine );

        gridLayout->addWidget( sep3,element,0 );
        gridLayout->addWidget( sep4,element,1 );
        ++element;

        widgetType = TWidgetType::LINE_EDIT;
        auto label = new QLabel(
          QString::fromStdString( "Number of entities" ));
        gridLayout->addWidget( label, element, 0 );

        _numNewEntities.reset( new QLineEdit );
        widget = _numNewEntities.get( );
        _numNewEntities->setText( "1" );

        _numNewEntities->setEnabled( true );
        gridLayout->addWidget( widget, element, 1 );
      }
      ++element;
    }

    QPushButton* cancelButton = new QPushButton( tr( "Close" ));
    gridLayout->addWidget( cancelButton, element, 0 );

    QPushButton* validationButton = new QPushButton(
      ( _isNew ? tr( "New" ) : tr( "Save" )));
    gridLayout->addWidget( validationButton, element, 1 );

    ++element;
    auto autoCloseLabel = new QLabel( tr( "Auto-close" ));
    gridLayout->addWidget( autoCloseLabel, element, 0 );

    _autoCloseCheck->setChecked( _autoCloseChecked );
    connect( _autoCloseCheck.get( ), SIGNAL( clicked( )),
             this, SLOT( toggleAutoClose( )));
    gridLayout->addWidget( _autoCloseCheck.get( ), element, 1 );

    ++element;
    auto checkUniquenessLabel = new QLabel( tr( "Check uniqueness" ));
    gridLayout->addWidget( checkUniquenessLabel, element, 0 );

    //_checkUniquenessCheck.reset( ); // = new QCheckBox( );
    _checkUniquenessCheck->setChecked( _checkUniquenessChecked );
    connect( _checkUniquenessCheck.get( ), SIGNAL( clicked( )),
             this, SLOT( toggleCheckUniqueness( )));
    gridLayout->addWidget( _checkUniquenessCheck.get( ), element, 1 );

    connect( cancelButton, SIGNAL( clicked( )), this, SLOT( cancelDialog( )));
    connect( validationButton, SIGNAL( clicked( )),
             this, SLOT( validateDialog( )));

    setLayout( gridLayout );
    setWindowTitle( tr( "Entity inspector" ));

    _entity = entity;
  }

  void EntityEditWidget::validateDialog( void )
  {
    auto origEntity = _entity;
    unsigned int numEles = 1;

    if ( _action == DUPLICATE_ENTITY || _isNew )
    {
      numEles = _numNewEntities->text( ).toInt( );
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

        bool isEditable = origEntity->hasPropertyFlag(
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
          auto comboWidget = dynamic_cast< QComboBox* >( widget );
          paramString = comboWidget->currentText( );
        }
        else if ( editType == TWidgetType::LINE_EDIT )
        {
          auto lineEditwidget = dynamic_cast< QLineEdit* >( widget );
          paramString = lineEditwidget->text( );
          //change name if multiple are created silmultaneus
          if ( numEles > 1 && label == "Entity name" )
          {
            paramString = paramString + QString( "_" ) + QString::number( i );
          }
        }
        else
          assert( false );

        auto caster = fires::PropertyManager::getPropertyCaster( label );
        if ( !_entity->hasProperty( label ))
        {
          // WAR: This is to force create/copy properties not defined in the
          // JSON but defined in when loading data, but does not solve all cases
          auto prop = origEntity->getProperty( label );
          _entity->registerProperty( label, prop );
        }
        auto& prop = _entity->getProperty( label );
        assert( caster );

        bool saveNewPropValue = true;

        if ( _checkUniquenessCheck->isChecked( ))
        {
          bool isUnique = _entity->hasPropertyFlag(
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

} // namespace