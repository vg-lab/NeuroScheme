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
#include <shift_NeuronSuperPop.h>
#include <shift_ConnectsWith.h>
#include "NeuronPopRep.h"
#include "NeuronSuperPopRep.h"
#include "StimulatorRep.h"
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
      , _maxAbsoluteWeight( 0.1f )
      , _nbConnectionsToWidth( 0, _maxAbsoluteWeight, 1.0f, 5.0f )
      ,  _neuronsToPercentage( 0, _maxNeuronsPerPopulation, 0.0f, 1.0f )
      , _superPopColor( "#a8f7ac" )
    {
      _neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::iaf_psc_alpha,
        scoop::Color( "#5c8daa" ));
      _neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::undefined,
        scoop::Color( "#e7ba83" ));

      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::AC_generator,
        scoop::Color( "#721346" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::DC_generator,
        scoop::Color( "#af2f3c" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::MIP_generator,
        scoop::Color( "#af4a1f" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::Noise_generator,
        scoop::Color( "#b23431" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::Poisson_generator,
        scoop::Color( "#a12320" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::Spike_generator,
        scoop::Color( "#a33250" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::Step_current_generator,
        scoop::Color( "#8e3766" ));
      _neuronStimulatorModelColorMap.setColor(
        shiftgen::Stimulator::TStimulatorModel::undefined_generator,
        scoop::Color( "#a88459" ));
    }

    void RepresentationCreator::updateRepresentation(
      const shift::Entity* entity_, shift::Representation* entityRep_
    )
    {
      if ( dynamic_cast< const shiftgen::NeuronPop* >( entity_ ))
      {
        updateNeuronPopRep( entity_, entityRep_ );
      }
      else if( dynamic_cast< const shiftgen::NeuronSuperPop* >( entity_ ))
      {
        updateSuperPopRep( entity_, entityRep_ );
      }
      else if(  dynamic_cast< const shiftgen::Stimulator* >( entity_ ))
      {
        updateStimulator( entity_, entityRep_ );
      }
    }

    void RepresentationCreator::updateNeuronPopRep(
      const shift::Entity* entity_, shift::Representation* entityRep_ )
    {
      if ( entity_->hasProperty( "Neuron model" ))
      {
        entityRep_->setProperty(
          "color", _neuronModelColorMap.getColor(
          entity_->getProperty( "Neuron model" )
          .value< shiftgen::NeuronPop::TNeuronModel >( )));
      }
      else
      {
        entityRep_->setProperty( "color", _neuronModelColorMap.getColor(
          shiftgen::NeuronPop::TNeuronModel::undefined ) );
      }
      if ( entity_->hasProperty( "Nb of neurons" ))
      {
        entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
          entity_->getProperty( "Nb of neurons" ).value< uint >( )));
      }
      else
      {
        Loggers::get( )->log( "Expected property Nb of neurons.",
          LOG_LEVEL_WARNING );
        entityRep_->setProperty( "line perc", _neuronsToPercentage.map( 0u ));
      }
    }

    void RepresentationCreator::updateSuperPopRep( const shift::Entity* entity_,
      shift::Representation* entityRep_ )
    {
      entityRep_->setProperty( "color", _superPopColor );
      if ( entity_->hasProperty( "Nb of neurons Mean" ))
      {
        entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
          entity_->getProperty( "Nb of neurons Mean" ).value< uint >( )));
      }
      else
      {
        Loggers::get( )->log( "Expected property Nb of neurons Mean.",
          LOG_LEVEL_WARNING );
        entityRep_->setProperty( "line perc",
          _neuronsToPercentage.map( 0u ));
      }
    }

    void RepresentationCreator::updateStimulator( const shift::Entity* entity_,
      shift::Representation* entityRep_ )
    {
      if( entity_->hasProperty( "Stimulator model" ))
      {
        entityRep_->setProperty(
          "color", _neuronStimulatorModelColorMap.getColor(
          entity_->getProperty( "Stimulator model" )
          .value< shiftgen::Stimulator::TStimulatorModel >( )));
      }
      else
      {
        entityRep_->setProperty( "color",
          _neuronStimulatorModelColorMap.getColor(
          shiftgen::Stimulator::TStimulatorModel::undefined_generator ));
      }
      if ( entity_->hasProperty( "Nb of neurons" ))
      {
        entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
          entity_->getProperty( "Nb of neurons" ).value< uint >( )));
      }
      else
      {
        Loggers::get( )->log( "Expected property Nb of neurons.",
          LOG_LEVEL_WARNING );
        entityRep_->setProperty( "line perc", _neuronsToPercentage.map( 0u ));
      }
    }

    void RepresentationCreator::create(
      const shift::Entities& entities,
      shift::Representations& representations,
      shift::TEntitiesToReps& entitiesToReps,
      shift::TRepsToEntities& repsToEntities,
      shift::TGidToEntitiesReps& gidsToEntitiesReps,
      bool linkEntitiesToReps,
      bool linkRepsToEntities )
    {
      nslib::Loggers::get( )->log( "create",
        LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );


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
        shift::Representation* entityRep = nullptr;

        if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
        {
          entityRep = new NeuronPopRep( );
          updateNeuronPopRep( entity, entityRep );
        }
        else if( dynamic_cast< shiftgen::NeuronSuperPop* >( entity ))
        {
          entityRep = new NeuronSuperPopRep( );
          updateSuperPopRep( entity, entityRep );
        }
        else if( dynamic_cast< shiftgen::Stimulator* >( entity ) )
        {
          entityRep = new StimulatorRep( );
          updateStimulator( entity, entityRep );
        }
        if ( entityRep )
        {
          representations.push_back( entityRep );
          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( entityRep );
          if ( linkRepsToEntities )
            repsToEntities[ entityRep ].insert( entity );

          gidsToEntitiesReps
            .insert( TripleKey( entity->entityGid( ), entity, entityRep ));
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
              auto relProps = relMMapIt->second;
              float weightPropertyValue = 0.0f;
              if ( relProps->hasProperty( "Weight" ))
              {
                weightPropertyValue =
                  relProps->getProperty( "Weight" ).value< float >( );
              }
              else
              {
                Loggers::get( )->log( "Expected property Weight.",
                  LOG_LEVEL_WARNING );
              }

              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                 _nbConnectionsToWidth.map( fabsf( weightPropertyValue ))));

              // If fixed weight over zero or if gaussian and mean over zero
              // then circle
              relationRep->setProperty(
                "head", shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
              if ( relProps->hasProperty( "Weight Type" ))
              {
                auto weightType = relProps->getProperty( "Weight Type" )
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
                  if ( relProps->hasProperty( "Weight Gaussian" ))
                  {
                    if ( relProps->getProperty( "Weight Gaussian" )
                      .value< float >( ) < 0.0f )
                    {
                      relationRep->setProperty( "head",
                        shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE );
                    }
                  }
                  else
                  {
                    Loggers::get( )->log("Expected property Weight Gaussian.",
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
                Loggers::get( )->log("Expected property Weight mean.",
                  LOG_LEVEL_WARNING );
              }
              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                _nbConnectionsToWidth.map( fabsf( weightPropertyValue ))));

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
      unsigned int newNeuronsPerPopulation = 0;
      if ( dynamic_cast< shiftgen::NeuronPop* >( entity )
        || dynamic_cast< shiftgen::Stimulator* >( entity ))
      {
        if( entity->hasProperty( "Nb of neurons" ))
        {
          newNeuronsPerPopulation =
            entity->getProperty( "Nb of neurons" ).value< unsigned int >( );
        }
        else
        {
          Loggers::get( )->log("Expected property Nb of neurons.",
            LOG_LEVEL_WARNING );
        }
      }
      else if ( dynamic_cast< shiftgen::NeuronSuperPop* >( entity ))
      {
        if(entity->hasProperty( "Nb of neurons Mean"))
        {
          newNeuronsPerPopulation =
            entity->getProperty( "Nb of neurons Mean" ).value< unsigned int >( );
        }
        else
        {
          Loggers::get( )->log("Expected property Nb of neurons Mean.",
            LOG_LEVEL_WARNING );
        }
      }
      if ( newNeuronsPerPopulation > _maxNeuronsPerPopulation )
      {
        this->clear( );
        maxNeuronsPerPopulation( newNeuronsPerPopulation, false );
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
          Loggers::get( )->log("Expected properties in connects with.",
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
      if( newAbsoluteWeight > _maxAbsoluteWeight )
      {
        maxAbsoluteWeight( newAbsoluteWeight );
        return true;
      }
      else
      {
        return false;
      }
    }

    void RepresentationCreator::maxAbsoluteWeight(
      float maxAbsoluteWeight_, bool compare )
    {
      if ( !compare || maxAbsoluteWeight_ > _maxAbsoluteWeight )
      {
        _maxAbsoluteWeight = maxAbsoluteWeight_;
        _nbConnectionsToWidth =
          MapperFloatToFloat( 0, _maxAbsoluteWeight, 1.0f, 5.0f );
      }
    }

    void RepresentationCreator::maxNeuronsPerPopulation(
      unsigned int maxNeuronsPerPopulation_, bool compare )
    {
      if ( !compare || maxNeuronsPerPopulation_ > _maxNeuronsPerPopulation )
      {
        _maxNeuronsPerPopulation = maxNeuronsPerPopulation_;
        _neuronsToPercentage =
          MapperFloatToFloat ( 0, _maxNeuronsPerPopulation, 0.0f, 1.0f );
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
