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
#include <nslib/Loggers.h>
#include "RepresentationCreator.h"
#include <shift_NeuronPop.h>
#include <shift_ConnectsWith.h>
#include "NeuronPopRep.h"
#include "ConnectionArrowRep.h"
#include "AutoConnectionArrowRep.h"
#include <algorithm>

namespace nslib
{
  namespace congen
  {

    RepresentationCreator::RepresentationCreator( void )
      : _maxNeuronsPerPopulation( 1 )
      , _maxAbsoluteWeight( 0.0f )
    {
    }

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
      nslib::Loggers::get( )->log( "create",
                                   LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

      scoop::CategoricalColorMap< shiftgen::NeuronPop::TNeuronModel >
        neuronModelColorMap;
      neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::iaf_psc_alpha,
        scoop::Color( "#5c8daa" ));
      neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::undefined,
        scoop::Color( "#e7ba83" ));

      MapperFloatToFloat
        neuronsToPercentage( 0, _maxNeuronsPerPopulation, 0.0f, 1.0f );

      // scoop::SequentialColorMap neuronTypeColorMapper(
      //   scoop::ColorPalette::colorBrewerSequential(
      //     scoop::ColorPalette::ColorBrewerSequential::PuBu, 6 ),
      //   0.0f, _maxNeuronsPerPopulation );

      for ( const auto entity : entities.vector( ))
      {
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
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
            neuronModelColorMap.getColor(
              neuronPop->getProperty( "Neuron model" ).
              value< shiftgen::NeuronPop::TNeuronModel >( )));
          neuronPopRep->setProperty(
            "line perc",
            neuronsToPercentage.map(
              neuronPop->getProperty( "Nb of neurons" ).value< uint >( )));

          // neuronPopRep->setProperty(
          //   "color",
          //   neuronTypeColorMapper.getColor(
          //     neuronPop->getProperty( "Nb of neurons" ).value< uint >( )));
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
        0, _maxAbsoluteWeight == 0 ? 0.1f : _maxAbsoluteWeight,
        1.0f, 5.0f );

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
          auto otherRep = gidsToEntitiesReps.find( other->entityGid( ) );
          if( otherRep == gidsToEntitiesReps.end( ) )
            continue;

          auto numberOfConnections =
              entityRelations->second.count( other->entityGid( ) );

          if( numberOfConnections == 0 )
            continue;

          // TODO: Change to equal_range whenever multiple relationships between
          // the same elements are imported. Then, create a loop to iterate
          // over the given results and create a new one if not found.
          auto combinedKey = std::make_pair( entity->entityGid( ),
                                             other->entityGid( ) );
          auto alreadyConnected =
              relatedEntitiesReps.find( combinedKey );

          if( alreadyConnected == relatedEntitiesReps.end( ) )
          {
            ConnectionArrowRep* relationRep;
            if( srcEntityRep->second.second == otherRep->second.second )
            {
              //SLDEBUG
              std::cout << "AUTO - Connection:" << srcEntityRep->second.second
                        << " - "
                        << otherRep->second.second << std::endl;
              //END
              relationRep =
                  new AutoConnectionArrowRep( srcEntityRep->second.second );
            }
            else
            {
              relationRep =
                  new ConnectionArrowRep( srcEntityRep->second.second,
                                          otherRep->second.second );
            }
            const std::unordered_multimap<shift::Entity::EntityGid,
                shift::RelationshipProperties*>& relMMap =
                ( *relatedElements )[ entity->entityGid( ) ];
            auto relMMapIt = relMMap.find( other->entityGid( ) );
            if( relMMapIt !=
                ( *relatedElements )[ entity->entityGid( ) ].end( ) )
            {
              // If fixed weight over zero or if gaussian and mean over zero
              // then circle
              if( ( relMMapIt->second->getProperty( "Weight Type" ).
                  value<shiftgen::ConnectsWith::TFixedOrDistribution>( ) ==
                    shiftgen::ConnectsWith::TFixedOrDistribution::Fixed &&
                    relMMapIt->second->getProperty( "Weight" ).value<float>( )
                    < 0.0f ) ||
                  ( relMMapIt->second->getProperty( "Weight Type" ).
                      value<shiftgen::ConnectsWith::TFixedOrDistribution>( ) ==
                    shiftgen::ConnectsWith::TFixedOrDistribution::Gaussian &&
                    relMMapIt->second->getProperty( "Weight Gaussian Mean" ).
                        value<float>( )
                    < 0.0f ) )
              {
                relationRep->setProperty(
                    "head",
                    shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE );
              }
              else
              {
                relationRep->setProperty(
                    "head",
                    shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
              }

              relationRep->setProperty(
                  "width", ( unsigned int ) roundf(
                      nbConnectionsToWidth.map( fabsf(
                          relMMapIt->second->getProperty(
                              "Weight" ).value<float>( ) ) ) ) );
            }

            alreadyConnected = relatedEntitiesReps.insert(
                std::make_pair( combinedKey,
                                std::make_tuple( relationRep,
                                                 entity,
                                                 other,
                                                 srcEntityRep->second.second,
                                                 otherRep->second.second ) ) );
          }
          relatedEntities.push_back( std::get< 0 >( alreadyConnected->second ));

        }


      }

    } // generateRelations


    bool RepresentationCreator::entityUpdatedOrCreated( shift::Entity* entity )
    {
      bool needToClear = false;

      if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
      {
        auto oldMaxNeuronsPerPopulation = _maxNeuronsPerPopulation;
        auto newNeuronsPerPopulation =
          entity->getProperty( "Nb of neurons" ).value< unsigned int >( );

        _maxNeuronsPerPopulation= std::max( newNeuronsPerPopulation,
                                            _maxNeuronsPerPopulation );

        needToClear = ( _maxNeuronsPerPopulation != oldMaxNeuronsPerPopulation );

      }

      if ( needToClear )
        this->clear( );

      return needToClear;
    }


    bool RepresentationCreator::relationshipUpdatedOrCreated(
      shift::RelationshipProperties* relProperties )
    {
      bool needToClear = false;

      if ( dynamic_cast< shiftgen::ConnectsWith* >( relProperties ))
      {
        auto oldMaxAbsoluteWeight = _maxAbsoluteWeight;
        auto newAbsoluteWeight = fabsf(
          ( relProperties->getProperty( "Weight Type" ).
            value< shiftgen::ConnectsWith::TFixedOrDistribution >( ) ==
            shiftgen::ConnectsWith::TFixedOrDistribution::Fixed  ?
            relProperties->getProperty( "Weight" ).value< float >( ) :
            relProperties->getProperty( "Weight Gaussian Mean" ).value< float >( )));

        _maxAbsoluteWeight= std::max( newAbsoluteWeight,
                                      _maxAbsoluteWeight );

        needToClear = ( _maxAbsoluteWeight != oldMaxAbsoluteWeight );
      }

      return needToClear;
    }

  } // namespace congen
} // namespace nslib
