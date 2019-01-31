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
#include <shift_NeuronSuperPop.h>
#include <shift_ConnectsWith.h>
#include "NeuronPopRep.h"
#include "ConnectionArrowRep.h"
#include "AutoConnectionArrowRep.h"
#include <algorithm>
#include <shift_AggregatedConnectsWith.h>

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
        shiftgen::NeuronPop* neuronPopEntity =
          dynamic_cast< shiftgen::NeuronPop* >( entity );
        shiftgen::NeuronSuperPop* neuronSuperPopEntity;
        NeuronPopRep* neuronRep = nullptr;

        if ( neuronPopEntity )
        {
          neuronRep = new NeuronPopRep( );
          if ( neuronPopEntity->hasProperty( "Neuron model" ))
          {
            neuronRep->setProperty(
              "color", neuronModelColorMap.getColor(
              neuronPopEntity->getProperty( "Neuron model" ).
              value< shiftgen::NeuronPop::TNeuronModel >( )));
          }
          else
          {
            neuronRep->setProperty( "color", neuronModelColorMap.getColor(
              shiftgen::NeuronPop::TNeuronModel::undefined ) );
          }
          neuronRep->setProperty( "line perc", neuronsToPercentage.map(
            neuronPopEntity->getProperty( "Nb of neurons" ).value< uint >( )));
        }
        else
        {
          neuronSuperPopEntity =
            dynamic_cast< shiftgen::NeuronSuperPop* >( entity );
          if ( neuronSuperPopEntity ){
            neuronRep = new NeuronPopRep( );
            if ( neuronSuperPopEntity->hasProperty( "Neuron model" ))
            {
              neuronRep->setProperty(
                "color", neuronModelColorMap.getColor(
                  neuronSuperPopEntity->getProperty( "Neuron model" ).
                    value< shiftgen::NeuronPop::TNeuronModel >( )));
            }
            else
            {
              //TODO: neuron superPop color
              neuronRep->setProperty( "color", neuronModelColorMap.getColor(
                shiftgen::NeuronPop::TNeuronModel::undefined ) );
            }
            neuronRep->setProperty( "line perc", neuronsToPercentage.map(
              neuronSuperPopEntity->getProperty( "Nb of neurons Mean" ).value< uint >( )));
          }
        }
        if ( neuronPopEntity || neuronSuperPopEntity )
        {
          representations.push_back( neuronRep );
          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( neuronRep );
          if ( linkRepsToEntities )
            repsToEntities[ neuronRep ].insert( entity );

          gidsToEntitiesReps
            .insert( TripleKey( entity->entityGid( ), entity, neuronRep ) );
        }
      }
    }

    void RepresentationCreator::generateRelations(
      const shift::Entities& entities,
      const shift::TGidToEntitiesReps& gidsToEntitiesReps,
      shift::TRelatedEntitiesReps& relatedEntitiesReps,
      shift::Representations& relatedEntities,
      shift::RelationshipOneToN* relatedElements )
    {
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
            ConnectionArrowRep* relationRep;
            if( srcEntityRep->second.second == otherRep->second.second )
            {
              relationRep =
                new AutoConnectionArrowRep( srcEntityRep->second.second );
            }
            else
            {
              relationRep = new ConnectionArrowRep( srcEntityRep->second.second,
                otherRep->second.second );
            }
            const std::unordered_multimap< shift::EntityGid,
              shift::RelationshipProperties* >& relMMap =
              ( *relatedElements )[ entity->entityGid( ) ];

            auto relMMapIt = relMMap.find( other->entityGid( ));
            if ( relMMapIt != relMMap.end( ))
            {
              float weightPropertyValue = 0.0f;
              if ( relMMapIt->second->hasProperty( "Weight" ))
              {
                weightPropertyValue = relMMapIt->second
                  ->getProperty( "Weight" ).value< float >( );
              }
              else
              {
                Loggers::get( )->log("Expected propery Weight.",
                  LOG_LEVEL_WARNING );
              }

              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                 nbConnectionsToWidth.map( fabsf( weightPropertyValue ))));

              // If fixed weight over zero or if gaussian and mean over zero
              // then circle
              relationRep->setProperty(
                "head", shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
              if ( relMMapIt->second->hasProperty( "Weight Type" ))
              {
                auto weightType = relMMapIt->second->getProperty("Weight Type" )
                  .value< shiftgen::ConnectsWith::TFixedOrDistribution >( );
                if ( weightType ==
                  shiftgen::ConnectsWith::TFixedOrDistribution::Fixed
                  && weightPropertyValue < 0.0f )
                {
                  relationRep->setProperty( "head",
                    shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE );
                }
                else if ( weightType
                  == shiftgen::ConnectsWith::TFixedOrDistribution::Gaussian )
                {
                  if ( relMMapIt->second->hasProperty( "Weight Gaussian" ))
                  {
                    if ( relMMapIt->second->getProperty( "Weight Gaussian" )
                      .value< float >( ) < 0.0f )
                    {
                      relationRep->setProperty( "head",
                        shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE );
                    }
                  }
                  else
                  {
                    Loggers::get( )->log("Expected propery Weight Gaussian.",
                      LOG_LEVEL_WARNING );
                  }
                }
              }
            }

            alreadyConnected = relatedEntitiesReps.insert(
              std::make_pair( combinedKey,
              std::make_tuple( relationRep, entity, other,
              srcEntityRep->second.second, otherRep->second.second )));
          }
          relatedEntities.push_back( std::get< 0 >( alreadyConnected->second ));
        }
      }
    } // generateRelations

    void RepresentationCreator::generateRelations(
      const shift::Entities& entities,
      const shift::TGidToEntitiesReps& gidsToEntitiesReps,
      shift::TRelatedEntitiesReps& relatedEntitiesReps,
      shift::Representations& relatedEntities,
      shift::RelationshipAggregatedOneToN* relatedElements )
    {
      MapperFloatToFloat nbConnectionsToWidth(
        0, _maxAbsoluteWeight == 0 ? 0.1f : _maxAbsoluteWeight,
        1.0f, 5.0f );

      for( auto& entity : entities.vector( ))
      {
        auto srcEntityRep = gidsToEntitiesReps.find( entity->entityGid( ));
        if( srcEntityRep == gidsToEntitiesReps.end( ))
          continue;

        auto entityRelations = relatedElements->mapAggregatedRels( )
          .find( entity->entityGid( ));

        if( entityRelations == relatedElements->mapAggregatedRels( ).end( ))
          continue;

        for( auto& other : entities.vector( ))
        {
          auto otherRep = gidsToEntitiesReps.find( other->entityGid( ));
          if( otherRep == gidsToEntitiesReps.end( ))
            continue;

          auto otherEntityConnection = entityRelations->second
            ->find( other->entityGid( ));
          if( otherEntityConnection == entityRelations->second->end( ))
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
            ConnectionArrowRep* relationRep;
            if( srcEntityRep->second.second == otherRep->second.second )
            {
              relationRep =
                new AutoConnectionArrowRep( srcEntityRep->second.second );
            }
            else
            {
              relationRep = new ConnectionArrowRep(
                srcEntityRep->second.second, otherRep->second.second );
            }

            shift::RelationshipProperties* relationProperties =
              otherEntityConnection->second
              .relationshipAggregatedProperties.get( );
            if ( relationProperties )
            {
              float weightPropertyValue = 0.0f;
              if ( relationProperties->hasProperty( "Weight mean" ))
              {
                weightPropertyValue = relationProperties
                  ->getProperty( "Weight mean" ).value< float >( );
              }
              else
              {
                Loggers::get( )->log("Expected propery Weight mean.",
                  LOG_LEVEL_WARNING );
              }
              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                nbConnectionsToWidth.map( fabsf( weightPropertyValue ))));

              // If fixed weight over zero or if gaussian and mean over zero
              // then circle
              if ( weightPropertyValue < 0.0f  || ( relationProperties
                ->hasProperty( "Weight Gaussian Mean mean" )
                &&  relationProperties->getProperty(
                "Weight Gaussian Mean mean" ).value< float >( ) < 0.0f ))
              {
                relationRep->setProperty(
                  "head", shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE );
              }
              else
              {
                relationRep->setProperty(
                  "head", shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
              }
            }

            alreadyConnected = relatedEntitiesReps.insert(
              std::make_pair( combinedKey,
                std::make_tuple( relationRep,
                entity, other, srcEntityRep->second.second,
                otherRep->second.second )));
          }
          relatedEntities.push_back( std::get< 0 >( alreadyConnected->second ));
        }
      }
    } // generateRelations

    bool RepresentationCreator::entityUpdatedOrCreated( shift::Entity* entity )
    {
      auto oldMaxNeuronsPerPopulation = _maxNeuronsPerPopulation;
      unsigned int newNeuronsPerPopulation = 0;
      if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
      {
        if( entity->hasProperty( "Nb of neurons" ))
        {
          newNeuronsPerPopulation =
            entity->getProperty( "Nb of neurons" ).value< unsigned int >( );
        }
        else
        {
          Loggers::get( )->log("Expected propery Nb of neurons.",
            LOG_LEVEL_WARNING );
        }
      }
      else if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
      {
        if( entity->hasProperty( "Nb of neurons Mean" ))
        {
          newNeuronsPerPopulation = entity
              ->getProperty( "Nb of neurons Mean" ).value< unsigned int >( );
        }
        else
        {
          Loggers::get( )->log("Expected propery Nb of neurons Mean.",
             LOG_LEVEL_WARNING );
        }
      }
      _maxNeuronsPerPopulation = std::max( newNeuronsPerPopulation,
         _maxNeuronsPerPopulation );
      if ( _maxNeuronsPerPopulation != oldMaxNeuronsPerPopulation )
      {
        this->clear( );
        return true;
      }
      else
      {
        return false;
      }

    }

    bool RepresentationCreator::relationshipUpdatedOrCreated(
      shift::RelationshipProperties* relProperties )
    {
      auto oldMaxAbsoluteWeight = _maxAbsoluteWeight;
      float newAbsoluteWeight = 0.0f;
      if ( dynamic_cast< shiftgen::ConnectsWith* >( relProperties ))
      {
        if( relProperties->hasProperty( "Weight Type" )
          &&  relProperties->hasProperty( "Weight" )
          && relProperties->hasProperty( "Weight Gaussian Mean" ))
        {
          newAbsoluteWeight = fabsf( ( relProperties
            ->getProperty( "Weight Type" )
            .value< shiftgen::ConnectsWith::TFixedOrDistribution >( ) ==
            shiftgen::ConnectsWith::TFixedOrDistribution::Fixed ?
            relProperties->getProperty( "Weight" ).value< float >( ) :
            relProperties->getProperty( "Weight Gaussian Mean" )
            .value< float >( )));
        }
        else
        {
          Loggers::get( )->log("Expected properies in connects with.",
            LOG_LEVEL_WARNING );
        }
      }
      else if ( dynamic_cast
        < shiftgen::AggregatedConnectsWith* >( relProperties ))
      {
        if( relProperties->hasProperty( "Weight mean" ))
        {
          newAbsoluteWeight = fabsf( (
            relProperties->getProperty( "Weight mean" ).value< float >( )));
        }
        else
        {
          Loggers::get( )->log("Expected properies in connects with.",
            LOG_LEVEL_WARNING );
        }
      }
      _maxAbsoluteWeight= std::max( newAbsoluteWeight, _maxAbsoluteWeight );
      return _maxAbsoluteWeight != oldMaxAbsoluteWeight;
    }

    void RepresentationCreator::maxAbsoluteWeight(
      float maxAbsoluteWeight_, bool compare )
    {
      if ( !compare || maxAbsoluteWeight_ > _maxAbsoluteWeight )
      {
        _maxAbsoluteWeight = maxAbsoluteWeight_;
      }
    }

    void RepresentationCreator::maxNeuronsPerPopulation(
      unsigned int maxNeuronsPerPopulation_, bool compare )
    {
      if ( !compare || maxNeuronsPerPopulation_ > _maxNeuronsPerPopulation )
      {
        _maxNeuronsPerPopulation = maxNeuronsPerPopulation_;
      }
    }

    float RepresentationCreator::maxAbsoluteWeight( void ) const
    {
      return _maxAbsoluteWeight;
    }

    unsigned int RepresentationCreator::maxNeuronsPerPopulation( void ) const
    {
      return _maxNeuronsPerPopulation;
    }

  } // namespace congen
} // namespace nslib
