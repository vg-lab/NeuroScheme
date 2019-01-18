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


namespace nslib
{

  FreeLayout::FreeLayout( void )
    : Layout( "Free", 0 )
  {
    //todo iago resize
    //todo iago pan
    //todo iago new entities position
    //todo iago padding
    //todo iago relationships
    _isGrid = false;

  }

  void FreeLayout::_arrangeItems( const shift::Representations& reps,
    const bool animate, const shift::Representations& postFilterReps )
  {
    //todo iago debug
    std::cout << "arranging free layout" << std::endl;//todo resize
    std::cout << "reps " << reps.size( ) <<std::endl;
    std::cout << "animate " << animate <<std::endl;
    std::cout << "filterreps " << postFilterReps.size( ) <<std::endl;
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
    _movedItem->setPos( newPos_ - _moveStart );
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
/**/
  void FreeLayout::display( shift::Entities& entities,
                        shift::Representations& representations,
                        bool animate )
  {
    //todo iago debug
    std::cout << "display: " << animate << std::endl;
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
    _addRepresentations( representations );
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
      _addRepresentations( _relationshipReps );

      OpConfig opConfig( &_canvas->scene( ), false, _isGrid );
      for ( auto& relationshipRep : _relationshipReps )
      {
        relationshipRep->preRender( &opConfig );
      }
    }
  }//*/

  void FreeLayout::_addRepresentations( const shift::Representations& reps )
  {
    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    qreal repsScale = _canvas->repsScale( );
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
            Loggers::get( )->log(
                "No entities associated to representation",
                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

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
                shapeItem->setPen( SelectableItem::selectedPen( ));
              else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
                shapeItem->setPen(
                    SelectableItem::partiallySelectedPen( ));
            }
          }
        }
        //std::cout << &scene << " add item " << std::endl;
        // std::cout << "Adding item" << item << std::endl;

        if ( !item->parentItem( )
          && !_canvas->scene( ).items( ).contains( item ))
        {
          _canvas->scene( ).addItem( item );
          item->setScale( repsScale );
          std::cout << repsScale << std::endl;
          //todo iago check if position center; and move to mouse position
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
    return new FreeLayout( );
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

}