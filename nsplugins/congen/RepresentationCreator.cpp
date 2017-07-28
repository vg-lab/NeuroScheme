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
#include <scoop/scoop.h>
#include <nslib/mappers/VariableMapper.h>
#include <nslib/error.h>
#include <nslib/DataManager.h>
#include "RepresentationCreator.h"
#include <shift_NeuronPop.h>
#include "NeuronPopRep.h"
#include "ConnectionArrowRep.h"

namespace nslib
{
  namespace congen
  {

    void RepresentationCreator::create(
      const shift::Entities& entities,
      shift::Representations& representations,
      shift::TEntitiesToReps& entitiesToReps,
      shift::TRepsToEntities& repsToEntities,
      shift::TGidToEntitiesReps& gidsToEntitiesReps,
      bool linkEntitiesToReps,
      bool linkRepsToEntities
      )
    {
      nslib::Log::log( "congen::Representationcreator::create",
                       LOG_LEVEL_VERBOSE );

      scoop::SequentialColorMap neuronTypeColorMapper(
        scoop::ColorPalette::colorBrewerDiverging(
          scoop::ColorPalette::ColorBrewerDiverging::RdYlGn, 10 ),
        0.0f, 100.0f );

      for ( const auto entity : entities.vector( ))
      {
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
          // std::cout << "Entity with already a rep" << std::endl;
          for ( const auto rep : entitiesToReps[ entity ] )
            representations.push_back( rep );
          continue;
        }
        if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
        {
          auto neuronPop = dynamic_cast< shiftgen::NeuronPop* >( entity );
          auto neuronPopRep = new NeuronPopRep( );

          neuronPopRep->setProperty(
            "color",
            neuronTypeColorMapper.getColor(
              neuronPop->getProperty( "Perc Inhibitory" ).value< float >( )));

          representations.push_back( neuronPopRep );
          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( neuronPopRep );
          if ( linkRepsToEntities )
            repsToEntities[ neuronPopRep ].insert( entity );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, neuronPopRep ));

        }
      }
    }


        void RepresentationCreator::generateRelations(
      const shift::Entities& entities,
      const shift::TGidToEntitiesReps& gidsToEntitiesReps,
      shift::TRelatedEntitiesReps& relatedEntitiesReps,
      shift::Representations& relatedEntities,
      const std::string& relationName )
    {

      const auto& relatedElements =
          DataManager::entities( ).relationships( )[ relationName ]->asOneToN( );

      MapperFloatToFloat nbConnectionsToWidth(
        0, _maxConnectionsPerEntity == 0 ? 0.1f : _maxConnectionsPerEntity,
        1.0f, 3.0f );

      for( auto& entity : entities.vector( ))
      {
        auto srcEntityRep = gidsToEntitiesReps.find( entity->entityGid( ));
        if( srcEntityRep == gidsToEntitiesReps.end( ))
          continue;

        auto entityRelations = relatedElements->find( entity->entityGid( ));

        if( entityRelations == relatedElements->end( ))
          continue;

        for( auto& other : entities.vector( ))
        {
          if( other->entityGid( ) == entity->entityGid( ))
            continue;

          auto otherRep = gidsToEntitiesReps.find( other->entityGid( ));
          if( otherRep == gidsToEntitiesReps.end( ))
            continue;

          auto numberOfConnections =
              entityRelations->second.count( other->entityGid( ));

          if( numberOfConnections == 0 )
            continue;

          // TODO: Change to equal_range whenever multiple relationships between
          // the same elements are imported. Then, create a loop to iterate
          // over the given results and create a new one if not found.
          auto combinedKey = std::make_pair( entity->entityGid( ),
                                             other->entityGid( ));
          auto alreadyConnected =
              relatedEntitiesReps.find( combinedKey );

          if( alreadyConnected == relatedEntitiesReps.end( ))
          {
            ConnectionArrowRep* relationRep =
              new ConnectionArrowRep( srcEntityRep->second.second,
                                      otherRep->second.second );

            std::unordered_multimap< shift::Entity::EntityGid,
                                     shift::RelationshipProperties* > relMMap =
              ( *relatedElements )[ entity->entityGid( ) ];
            auto relMMapIt = relMMap.find( other->entityGid( ));
            if ( relMMapIt != ( *relatedElements )[ entity->entityGid( ) ].end( ) )
            {
              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                  nbConnectionsToWidth.map(
                    relMMapIt->second->getProperty( "count" ).value< unsigned int >( ))));
            }

            alreadyConnected = relatedEntitiesReps.insert(
              std::make_pair( combinedKey,
                              std::make_tuple( relationRep,
                                               entity,
                                               other,
                                               srcEntityRep->second.second,
                                               otherRep->second.second )));
          }

          relatedEntities.push_back( std::get< 0 >( alreadyConnected->second ));

        }


      }

    } // generateRelations


  } // namespace congen
} // namespace nslib
