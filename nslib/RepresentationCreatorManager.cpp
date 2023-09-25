/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
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
#include "RepresentationCreatorManager.h"
#include "reps/QGraphicsItemRepresentation.h"
#include "DataManager.h"
#include "Loggers.h"

namespace nslib
{
  std::unordered_map< unsigned int, shift::RepresentationCreator* >
  RepresentationCreatorManager::_repCreators =
    std::unordered_map< unsigned int, shift::RepresentationCreator* >( );

  std::unordered_map< unsigned int, shift::TEntitiesToReps >
  RepresentationCreatorManager::_entitiesToReps =
    std::unordered_map< unsigned int, shift::TEntitiesToReps >( );

  std::unordered_map< unsigned int, shift::TRepsToEntities>
  RepresentationCreatorManager::_repsToEntities =
    std::unordered_map< unsigned int, shift::TRepsToEntities>( );

  std::unordered_map< unsigned int, shift::TRelatedEntitiesReps >
  RepresentationCreatorManager::_relatedEntitiesReps =
    std::unordered_map< unsigned int, shift::TRelatedEntitiesReps >( );

  std::unordered_map< unsigned int, shift::TGidToEntitiesReps >
  RepresentationCreatorManager::_gidsToEntitiesReps =
    std::unordered_map< unsigned int, shift::TGidToEntitiesReps >( );

  void RepresentationCreatorManager::addCreator(
    shift::RepresentationCreator* repCreator,
    unsigned int repCreatorId )
  {
    if(_repCreators.find(repCreatorId) == _repCreators.end())
    {
      _repCreators[ repCreatorId ] = repCreator;
    }
    else
    {
      Loggers::get( )->log(
        std::string("Representation creator already exists:") + std::to_string(repCreatorId),
         LOG_LEVEL_WARNING );
    }
  }

  shift::RepresentationCreator* RepresentationCreatorManager::getCreator(
    unsigned int idx )
  {
    if ( _repCreators.count( idx ) == 1 )
      return _repCreators[ idx ];
    else
      return nullptr;
  }

  RepresentationCreatorManager::TCreatorsMap&
  RepresentationCreatorManager::creators( void )
  {
    return _repCreators;
  }

  void RepresentationCreatorManager::create(
    const shift::Entities& entities,
    shift::Representations& representations,
    // TEntitiesToReps& entitiesToReps,
    // TRepsToEntities& repsToEntities,
    bool linkEntitiesToReps,
    bool linkRepsToObjs,
    unsigned int repCreatorId )
  {
    //Check if exists performed in the repCreators
    if ( _repCreators.count( repCreatorId ) == 1 )
      _repCreators[ repCreatorId ]->create( entities, representations,
        _entitiesToReps[ repCreatorId ],
        _repsToEntities[ repCreatorId ],
        _gidsToEntitiesReps[ repCreatorId ],
        linkEntitiesToReps,
        linkRepsToObjs );
  }

  void RepresentationCreatorManager::generateRelations(
      const shift::Entities& entities,
      shift::Representations& representations,
      const std::string& name,
      const bool aggregated,
      unsigned int repCreatorId )
  {
    if( _repCreators.count( repCreatorId ) == 1 )
    {
      if ( aggregated )
      {
        auto relations =  DataManager::entities( ).
          relationships( )[ name ]->asAggregatedOneToN( );
        _repCreators[ repCreatorId ]
          ->generateRelations( entities, _gidsToEntitiesReps[ repCreatorId ],
          _relatedEntitiesReps[ repCreatorId ],
          representations, relations );
      }
      else
      {
        auto relations =  DataManager::entities( ).
          relationships( )[ name ]->asOneToN( );
        _repCreators[ repCreatorId ]
          ->generateRelations( entities, _gidsToEntitiesReps[ repCreatorId ],
          _relatedEntitiesReps[ repCreatorId ],
          representations, relations );
      }
    }
  }

  const shift::TEntitiesToReps& RepresentationCreatorManager::entitiesToReps(
    unsigned int repCreatorId )
  {
    return _entitiesToReps[ repCreatorId ];
  }

  const shift::TRepsToEntities& RepresentationCreatorManager::repsToEntities(
    unsigned int repCreatorId )
  {
    return _repsToEntities[ repCreatorId ];
  }

  const shift::TRelatedEntitiesReps& RepresentationCreatorManager::relatedEntities(
    unsigned int repCreatorId )
  {
    return _relatedEntitiesReps[ repCreatorId ];
  }

  const shift::TGidToEntitiesReps& RepresentationCreatorManager::gidsToEntitiesReps(
    unsigned int repCreatorId )
  {
    return _gidsToEntitiesReps[ repCreatorId ];
  }

  void RepresentationCreatorManager::deleteItemsOfCanvas( Canvas* canvas )
  {
    for ( auto& entitiesToReps : _entitiesToReps )
      for ( auto& entityToReps : entitiesToReps.second )
        for ( auto& rep : entityToReps.second )
        {
          auto qGraphicsRep =
            dynamic_cast< QGraphicsItemRepresentation* >( rep );
          if ( qGraphicsRep )
            qGraphicsRep->deleteItem( &canvas->scene( ));
        }
  }

  void RepresentationCreatorManager::updateEntitiyRepresentations(
    const shift::Entity* entity_,
    std::set< shift::Representation* > entityReps_,
    unsigned int repCreatorId,
    const bool freeLayoutInUse_)
  {
    std::vector<QGraphicsScene*> scenes;
    for( auto canvas : PaneManager::panes( ))
    {
      scenes.push_back(&canvas->scene( ));
    }
    shift::RepresentationCreator* creatorRep_ =
      RepresentationCreatorManager::getCreator( repCreatorId );

    for ( shift::Representation* rep : entityReps_ )
    {
      creatorRep_->updateRepresentation( entity_, rep );
      auto graphicsItemRep =
        dynamic_cast< QGraphicsItemRepresentation* >( rep );
      auto graphicsItems = graphicsItemRep->items( );

      if( freeLayoutInUse_ )
      {
        for( auto itemIt : graphicsItems )
        {
          auto item = itemIt.second;
          auto scene = itemIt.first;
          if ( item && scene )
          {
            if( std::find( scenes.begin( ), scenes.end( ), scene ) ==
              scenes.end( ))
            {
              graphicsItemRep->items( ).erase( scene );
            }
            else
            {
              auto items = scene->items( );
              auto pos = item->pos( );
              auto scale = item->scale( );
              if( items.contains( item ))
              {
                scene->removeItem( item );
                graphicsItemRep->deleteItem( scene );
                auto newItem = graphicsItemRep->item( scene );
                scene->addItem( newItem );
                newItem->setScale( scale );
                newItem->setPos( pos );
              }
              else
              {
                graphicsItemRep->deleteItem( scene );
              }
            }
          }
        }
      }
      else
      {
        graphicsItemRep->clearItems( );
      }
    }
  }

  void RepresentationCreatorManager::updateEntities(
    const shift::Entities& updatedEntities_,
    const unsigned int repCreatorId,
    const bool freeLayoutInUse_ )
  {
    bool representationUpdated = false;
    auto creator = RepresentationCreatorManager::getCreator( repCreatorId );
    for (const auto updatedEntity : updatedEntities_.vector( ))
    {
      representationUpdated = creator->entityUpdatedOrCreated( updatedEntity )
        || representationUpdated;
    }
    if( representationUpdated )
    {
      RepresentationCreatorManager::clearEntitiesCache(
        repCreatorId, freeLayoutInUse_ );
    }
    else
    {
      auto entitiesToReps =
        RepresentationCreatorManager::entitiesToReps( repCreatorId);
      for( const auto& entity : updatedEntities_.vector( ))
      {
        const auto entityReps = entitiesToReps.find( entity );
        if( entityReps == entitiesToReps.end( ))
        {
          Loggers::get( )->log(
            "Not found the representation of the edited entity.",
             LOG_LEVEL_WARNING );
        }
        else
        {
          RepresentationCreatorManager::updateEntitiyRepresentations( entity,
            entityReps->second, repCreatorId, freeLayoutInUse_ );
        }
      }
    }
    RepresentationCreatorManager::clearRelationshipsCache( repCreatorId );
  }

  void RepresentationCreatorManager::removeEntity(
    shift::Entity* entity, unsigned int repCreatorId )
  {
    auto repsToEntities = _repsToEntities[ repCreatorId ];
    auto entitiesToReps = _entitiesToReps[ repCreatorId ];
    auto entityReps = entitiesToReps.find(entity);
    if ( entityReps != entitiesToReps.end( ))
    {
      for ( const auto rep : entityReps->second )
      {
        repsToEntities.erase( rep );
      }
      entitiesToReps.erase( entity );
    }
    auto gidToReps = _gidsToEntitiesReps[ repCreatorId ];

    auto entityGid = entity->entityGid( );
    if (gidToReps.find( entityGid ) != gidToReps.end( ))
    {
      gidToReps.erase( entityGid );
    }
  }

  void RepresentationCreatorManager::clearEntitiesToReps(
    unsigned int repCreatorId )
  {
    //_repCreators[ repCreatorId ]->clear( );
    for ( auto& repPair : _repsToEntities[ repCreatorId ] )
      delete repPair.first;
    _entitiesToReps[ repCreatorId ].clear( );
    _repsToEntities[ repCreatorId ].clear( );
    _gidsToEntitiesReps[ repCreatorId ].clear( );
  }

  void RepresentationCreatorManager::clearEntitiesCache(
    unsigned int repCreatorId,
    const bool freeLayoutInUse )
  {
    if ( freeLayoutInUse )
    {
      auto entitiesToReps =
        RepresentationCreatorManager::entitiesToReps( repCreatorId );
      for(const auto& entityReps : entitiesToReps )
      {
        RepresentationCreatorManager::updateEntitiyRepresentations(
          entityReps.first, entityReps.second, repCreatorId, freeLayoutInUse );
      }
    }
    else
    {
      RepresentationCreatorManager::clearEntitiesToReps( repCreatorId );
    }
    RepresentationCreatorManager::getCreator( repCreatorId )->clear( );
  }

  void RepresentationCreatorManager::clearRelationshipsCache(
    unsigned int repCreatorId )
  {
    if( Config::showConnectivity( ))
    {
      for( const auto& pane : PaneManager::panes( ))
      {
        if( pane->activeLayoutIndex( ) == Layout::TLayoutIndexes::FREE )
        {
          FreeLayout* freeLayout = dynamic_cast< FreeLayout* >(
            pane->layouts( ).getLayout( Layout::TLayoutIndexes::FREE ));
          freeLayout->removeRelationshipsReps( );
        }
      }
    }
    for ( auto& relPair : _relatedEntitiesReps[ repCreatorId ] )
    {
      delete std::get< 0 >( relPair.second );
    }
    _relatedEntitiesReps[ repCreatorId ].clear( );
  }

  void RepresentationCreatorManager::clearCaches( )
  {
    const bool freeLayoutInUse = PaneManager::freeLayoutInUse( );
    for ( const auto& creatorPair : _repCreators )
    {
      clearEntitiesCache( creatorPair.first, freeLayoutInUse );
      clearRelationshipsCache( creatorPair.first );
    }
  }
  void RepresentationCreatorManager::clearMaximums( )
  {
    for ( const auto& creatorPair : _repCreators )
    {
      creatorPair.second->reset( );
    }
  }
}
