/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
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
#ifndef __NSLIB__REPRESENTATION_CREATOR_MANAGER__
#define __NSLIB__REPRESENTATION_CREATOR_MANAGER__

#include <nslib/api.h>
#include <shift/shift.h>
#include <nslib/layouts/FreeLayout.h>
#include "Canvas.h"
#include "PaneManager.h"
#include "Config.h"
// #include "domains/domains.h"

namespace nslib
{


  class NSLIB_API RepresentationCreatorManager
  {
  public:
    typedef std::unordered_map< unsigned int, shift::RepresentationCreator* >
      TCreatorsMap;

    static void addCreator( shift::RepresentationCreator* repCreator,
                            unsigned int repCreatorId = 0 );

    static void create( const shift::Entities& entities,
                        shift::Representations& representations,
                        // TEntitiesToReps& entitiesToReps,
                        // TRepsToEntities& repsToEntities,
                        bool linkEntitiesToReps = false,
                        bool linkRepsToObjs = false,
                        unsigned int repCreatorId = 0 );

    static void generateRelations( const shift::Entities& entities,
      shift::Representations& representations,
      const std::string& name, bool aggregated = false,
      unsigned int repCreatorId = 0 );

    static const shift::TEntitiesToReps& entitiesToReps(
        unsigned int repCreatorId = 0 );

    static const shift::TRepsToEntities& repsToEntities(
        unsigned int repCreatorId = 0 );

    static void clearEntitiesToReps( unsigned int repCreatorId = 0 )
    {
      //_repCreators[ repCreatorId ]->clear( );
      for ( auto& repPair : _repsToEntities[ repCreatorId ] )
          delete repPair.first;
      _entitiesToReps[ repCreatorId ].clear( );
      _repsToEntities[ repCreatorId ].clear( );
      _gidsToEntitiesReps[ repCreatorId ].clear( );
    }

    static void clearRelationshipsCache( unsigned int repCreatorId = 0 )
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

    static const shift::TRelatedEntitiesReps& relatedEntities(
        unsigned int repCreatorID = 0 );

    static const shift::TGidToEntitiesReps& gidsToEntitiesReps(
        unsigned int repCreatorID = 0 );

    static void deleteItemsOfCanvas( Canvas* canvas );

    static shift::RepresentationCreator* getCreator( unsigned int idx = 0 );

    static TCreatorsMap& creators( void );

    // static const shift::Representation& objectToRep(

  protected:

    static TCreatorsMap _repCreators;
    static std::unordered_map< unsigned int, shift::TEntitiesToReps > _entitiesToReps;
    static std::unordered_map< unsigned int, shift::TRepsToEntities > _repsToEntities;

    static std::unordered_map< unsigned int, shift::TRelatedEntitiesReps > _relatedEntitiesReps;
    static std::unordered_map< unsigned int, shift::TGidToEntitiesReps > _gidsToEntitiesReps;
  };

} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__
