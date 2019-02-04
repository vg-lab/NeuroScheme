/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *          Iago Calvo <i.calvol@alumnos.urjc.es>
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
#include "FreeLayout.h"
#include "../reps/Item.h"
#include "../reps/QGraphicsItemRepresentation.h"
#include "../error.h"
#include "../RepresentationCreatorManager.h"
#include <QToolBox>
#include <nslib/reps/SelectableItem.h>
#include <nslib/reps/QGraphicsItemRepresentation.h>
#include <nslib/Config.h>
#include <nslib/SelectionManager.h>
#include <QtWidgets/QMainWindow>


namespace nslib
{

  FreeLayout::FreeLayout( QStatusBar* statusBar_ )
    : Layout( "Free", 0, new QWidget )
    , _moveNewCheckBox( new QCheckBox )
    , _statusBar(statusBar_)
  {
    _isGrid = false;
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    _layoutSpecialProperties->setLayout( layout_ );

    const auto labelMoveNewEntities =
        new QLabel( QString::fromStdString( "Move new entities:" ));
    _moveNewCheckBox->setChecked( true );
    _moveNewCheckBox->setEnabled( true );

    layout_->addWidget( labelMoveNewEntities, 0, 0 );
    layout_->addWidget( _moveNewCheckBox, 0, 1 );
  }

  void FreeLayout::_arrangeItems( const shift::Representations& /*reps*/,
    const bool /*animate*/, const shift::Representations& /*postFilterReps*/ )
  {
    Loggers::get( )->log( "Free Layout unable to arrange items.",
      LOG_LEVEL_WARNING );
  }

  void FreeLayout::stopMoveActualRepresentation( void )
  {
    _movedItem = nullptr;
    _statusBar->showMessage( "",5 );
  }

  void FreeLayout::startMoveRepresentation( QGraphicsItem* item_,
    const QPointF clickPos_ )
  {
    auto parentItem = item_->parentItem( );
    while( parentItem )
    {
      auto selectableItem = dynamic_cast< SelectableItem* >( item_ );
      if( selectableItem )
        break;
      item_ = parentItem;
      parentItem = item_->parentItem( );
    }
    auto nslibItem = dynamic_cast< Item* >( item_ );
    auto itemRepresentation =
      dynamic_cast< QGraphicsItemRepresentation* >( nslibItem->parentRep( ));
    _movedItem = itemRepresentation->item( &_canvas->scene( ));
    _moveStart = item_->pos( )- clickPos_;
    if( Config::showConnectivity( ))
    {
      preRenderOpConfig = OpConfig( &_canvas->scene( ), false, _isGrid );
    }
  }

  void FreeLayout::moveRepresentation( const QPointF newPos_ )
  {
    QPointF newItemPos = newPos_ - _moveStart;
    _movedItem->setPos( newItemPos );
    _statusBar->showMessage(
      QString( "X: " ) + QString::number( newItemPos.x( )) +
      QString( " Y: " ) + QString::number( newItemPos.y( )));
    if( Config::showConnectivity( ))
    {
      for ( auto& relationshipRep : _relationshipReps )
      {
        relationshipRep->preRender( &preRenderOpConfig );
      }
    }
  }

  void FreeLayout::_updateOptionsWidget( void )
  {

  }

  void FreeLayout::display( shift::Entities& entities,
    shift::Representations& representations, bool /*animate*/ )
  {
    Loggers::get( )->log( "display "
      + std::to_string( entities.size( )),
      LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

    representations.clear( );
    RepresentationCreatorManager::create(
      entities, representations,
      true, true );

    shift::Representations removeReps;
    for( const auto& oldRep : _entitiesReps )
    {
      if( std::find( representations.begin( ), representations.end( ),
        oldRep ) == representations.end( ))
      {
        removeReps.push_back( oldRep );
      }
    }
    _removeRepresentations( removeReps );
    _addRepresentations( representations, true );
    _entitiesReps = representations;

    removeRelationshipsReps( );
    if ( Config::showConnectivity( ))
    {
      shift::Representations newRepresentations;
      RepresentationCreatorManager::generateRelations( entities,
        newRepresentations, "connectsTo", false );
      RepresentationCreatorManager::generateRelations( entities,
        newRepresentations, "aggregatedConnectsTo", true );
      _relationshipReps = newRepresentations;
      _addRepresentations( _relationshipReps, false );

      OpConfig opConfig( &_canvas->scene( ), false, _isGrid );
      for ( auto& relationshipRep : _relationshipReps )
      {
        relationshipRep->preRender( &opConfig );
      }
    }
  }

  void FreeLayout::_addRepresentations( const shift::Representations& reps,
    const bool isEntity )
  {
    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    qreal repsScale = _canvas->repsScale( );
    auto view = _canvas->scene( ).views( ).first( );
    const bool moveNewEntities = _moveNewCheckBox->isChecked( );

    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >( representation );
      if ( !graphicsItemRep )
      {
        Loggers::get( )->log( "Item null", LOG_LEVEL_WARNING );
      }
      else
      {
        // std::cout << "+++++ Retrieving item" << std::endl;
        auto item = graphicsItemRep->item( &_canvas->scene( ));

        if ( item->parentItem( ))
        {
          continue;
        }

        // Find out if its entity is selected
        // and if so set its pen
        // const auto& repsToEntities =
        //   RepresentationCreatorManager::repsToEntities( );

        if ( repsToEntities.count( representation ) > 0 )
        {
          const auto entities = repsToEntities.at( representation );
          if ( entities.empty( ))
          {
            Loggers::get( )->log(
              "No entities associated to representation",
              LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          }
          auto selectableItem = dynamic_cast< SelectableItem* >( item );
          if ( selectableItem )
          {
            auto selectedState = SelectionManager::getSelectedState(
              *entities.begin( ));

            // if ( selectedState == selectedStateSelectedState::SELECTED )
            selectableItem->setSelected( selectedState );

            auto shapeItem =
                dynamic_cast< QAbstractGraphicsShapeItem* >( item );
            if ( shapeItem )
            {
              if ( selectedState == SelectedState::SELECTED )
              {
                shapeItem->setPen( SelectableItem::selectedPen( ));
              }
              else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
              {
                shapeItem->setPen(
                  SelectableItem::partiallySelectedPen( ) );
              }
            }
          }
        }
        //std::cout << &scene << " add item " << std::endl;
        // std::cout << "Adding item" << item << std::endl;

        if ( !item->parentItem( )
          && !_canvas->scene( ).items( ).contains( item ))
        {
          if ( isEntity )
          {
            if( moveNewEntities )
            {
              qreal posX = view->x( );
              qreal posY = item->y( );
              qreal glyphRadius =
                2.05f * float( item->scale( ))
                * float( item->boundingRect( ).width( ));
              auto viewItem = view->itemAt( view->mapFromScene( posX, posY ));
              while( viewItem )
              {
                posX += glyphRadius;
                if( posX > view->width( ))
                {
                  posX = item->x( );
                  posY += glyphRadius;
                }
                viewItem = view->itemAt( view->mapFromScene( posX, posY ));
              }
              item->setPos( posX, posY );
            }
            item->setScale( repsScale );
          }
          _canvas->scene( ).addItem( item );
        }
      }
    }
  } // _addRepresentations

  void FreeLayout::_removeRepresentations( const shift::Representations& reps )
  {
    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >( representation );
      if ( !graphicsItemRep )
      {
        Loggers::get( )->log( "Item null", LOG_LEVEL_WARNING );
      }
      else
      {
        // std::cout << "+++++ Retrieving item" << std::endl;
        auto item = graphicsItemRep->item( &_canvas->scene( ));

        if ( item->parentItem( ))
        {
          continue;
        }
        if ( !item->parentItem( )
             && _canvas->scene( ).items( ).contains( item ))
        {
          _canvas->scene( ).removeItem( item );
        }

      }
    }
  } // _removeRepresentations

  Layout* FreeLayout::clone( void ) const
  {
    return new FreeLayout( _statusBar );
  }

  void FreeLayout::removeRelationshipsReps( )
  {
    _removeRepresentations( _relationshipReps );
    _relationshipReps.clear( );
  }

  void FreeLayout::init( )
  {
    _relationshipReps.clear( );
    _entitiesReps.clear( );
  }

  void FreeLayout::_addRepresentations( const shift::Representations& reps )
  {
    FreeLayout::_addRepresentations( reps, true );
  }

}