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

#include <scoop/scoop.h>
#include <nslib/mappers/VariableMapper.h>
#include <nslib/error.h>
#include <nslib/DataManager.h>
#include <nslib/Loggers.h>
#include "RepresentationCreator.h"
#include <shift_NeuronSuperPop.h>
#include <shift_ConnectsWith.h>
#include "CongenPopRep.h"
#include "NeuronSuperPopRep.h"
#include "ConnectionArrowRep.h"
#include "AutoConnectionArrowRep.h"
#include "NeuronSuperPopItem.h"
#include <algorithm>
#include <shift_AggregatedConnectsWith.h>

namespace nslib
{
  namespace congen
  {

    RepresentationCreator::RepresentationCreator( void )
      : _maxNeuronsPerPopulation( 1 )
      , _maxLevelsPerSuperPop( 1 )
      , _maxAbsoluteWeight( 0.1f )
      , _superPopSeparation( 0.5f )//1.0f / float( _maxLevelsPerSuperPop + 1 ))
      , _superPopLevelSeparation( NeuronSuperPopItem::rangeCircle
          * _superPopSeparation)
      , _nbConnectionsToWidth( 0, _maxAbsoluteWeight, 1.0f, 5.0f )
      ,  _neuronsToPercentage( 0, _maxNeuronsPerPopulation, 0.0f, 1.0f )
      , _superPopLevelColorMap( 0.0f, scoop::Color( "#b6d7a8" ),
          1.0f, scoop::Color( "#b075f0" ))
      , _superPopColor( "#b6d7a8" )
    {
      _neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::iaf_psc_alpha,
        scoop::Color( "#9fc5e8" ));
      _neuronModelColorMap.setColor(
        shiftgen::NeuronPop::TNeuronModel::undefined,
        scoop::Color( "#ea9999" ));

      _inputModelColorMap.setColor(
        shiftgen::Input::TInputType::Pulse_input, scoop::Color( "#ddf231" ));
      _inputModelColorMap.setColor(
        shiftgen::Input::TInputType::Random_stim, scoop::Color( "#f9f190" ));

      _outputModelColorMap.setColor(
        shiftgen::Output::TOutputModel::Multimeter, scoop::Color( "#d7b5fc" ));
      _outputModelColorMap.setColor(
        shiftgen::Output::TOutputModel::Voltmeter, scoop::Color( "#b075f0" ));
      _outputModelColorMap.setColor(
        shiftgen::Output::TOutputModel::Spike_detector,
        scoop::Color( "#4c1c7c" ));

    }

    void RepresentationCreator::updateRepresentation(
      const shift::Entity* entity_, shift::Representation* entityRep_
    )
    {
      if ( dynamic_cast< const shiftgen::NeuronPop* >( entity_ ))
      {
        updateNeuronPopRep( entity_, entityRep_ );
      }
      else if(  dynamic_cast< const shiftgen::Input* >( entity_ ))
      {
        updateInputRep( entity_, entityRep_ );
      }
      else if( dynamic_cast< const shiftgen::NeuronSuperPop* >( entity_ ))
      {
        updateSuperPopRep( entity_, entityRep_ );
      }
      else if( dynamic_cast< const shiftgen::Output* >( entity_ ))
      {
        updateOutputRep( entity_, entityRep_ );
      }
    }

    void RepresentationCreator::updateNeuronPopRep(
      const shift::Entity* entity_, shift::Representation* entityRep_ )
    {
      entityRep_->setProperty( "Entity name", entity_->getPropertyValue
        <std::string>( "Entity name", " " ));

      entityRep_->setProperty( "color", _neuronModelColorMap.getColor(
        entity_->getPropertyValue< shiftgen::NeuronPop::TNeuronModel >
        ( "Neuron model", shiftgen::NeuronPop::TNeuronModel::undefined )));

      entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
        entity_->getPropertyValue< uint >( "Nb of neurons", 0u )));
    }

    void RepresentationCreator::updateSuperPopRep( const shift::Entity* entity_,
      shift::Representation* entityRep_ )
    {
      entityRep_->setProperty( "color", _superPopColor );
      entityRep_->setProperty( "circles separation", _superPopLevelSeparation );
      entityRep_->setProperty( "circles color separation",
        _superPopSeparation );
      entityRep_->setProperty( "circles color map", _superPopLevelColorMap );

      entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
        entity_->getPropertyValue< uint >( "Nb of neurons Mean", 0u )));

      entityRep_->setProperty( "num circles", entity_->
        getPropertyValue<unsigned int>( "child depth", 0u ));

      entityRep_->setProperty( "Entity name",
        entity_->getPropertyValue< std::string >( "Entity name", " " ));
    }

    void RepresentationCreator::updateInputRep(
      const shift::Entity* entity_, shift::Representation* entityRep_ )
    {
      entityRep_->setProperty( "color", _inputModelColorMap.getColor(
        entity_->getPropertyValue< shiftgen::Input::TInputType >(
        "Stimulator type", shiftgen::Input::TInputType::Random_stim )));

      entityRep_->setProperty( "line perc", _neuronsToPercentage.map(
        entity_->getPropertyValue< uint >( "Nb of neurons", 0u )));

      entityRep_->setProperty( "Entity name",
        entity_->getPropertyValue< std::string >( "Entity name", " " ));
    }

    void RepresentationCreator::updateOutputRep( const shift::Entity* entity_,
      shift::Representation* entityRep_ )
    {
      entityRep_->setProperty( "color", _outputModelColorMap.getColor(
        entity_->getPropertyValue< shiftgen::Output::TOutputModel >(
        "Output model", shiftgen::Output::TOutputModel::Multimeter )));

      entityRep_->setProperty( "Entity name",
        entity_->getPropertyValue< std::string >( "Entity name", " " ));
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
          entityRep = new CongenPopRep( );
          updateNeuronPopRep( entity, entityRep );
        }
        else if( dynamic_cast< shiftgen::NeuronSuperPop* >( entity ))
        {
          entityRep = new NeuronSuperPopRep( );
          updateSuperPopRep( entity, entityRep );
        }
        else if( dynamic_cast< shiftgen::Input* >( entity ) )
        {
          entityRep = new CongenPopRep( );
          updateInputRep( entity, entityRep );
        }
        else if( dynamic_cast< shiftgen::Output* >( entity ) )
        {
          entityRep = new CongenPopRep( );
          updateOutputRep( entity, entityRep );
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
                otherRep->second.second, false );
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
                weightPropertyValue = relProps->getPropertyValue< float >
                  ( "Weight", .0f );
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
              auto weightType = relProps->getPropertyValue
                < shiftgen::ConnectsWith::TFixedOrDistribution >( "Weight Type",
                shiftgen::ConnectsWith::TFixedOrDistribution::Fixed );
              relationRep->setProperty( "head", ( ( weightType ==
                shiftgen::ConnectsWith::TFixedOrDistribution::Fixed &&
                weightPropertyValue < 0.0f ) || ( weightType ==
                shiftgen::ConnectsWith::TFixedOrDistribution::Gaussian &&
                relProps->getPropertyValue< float >( "Weight Gaussian", 0.0f )
                < 0.0f )) ? shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE
                : shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
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
                srcEntityRep->second.second, otherRep->second.second, true );
            }

            shift::RelationshipProperties* relationProperties =
              otherEntityConnection->second
              .relationshipAggregatedProperties.get( );
            if ( relationProperties )
            {
              float weightPropertyValue = relationProperties
                ->getPropertyValue< float >( "Weight mean", .0f );
              relationRep->setProperty(
                "width", ( unsigned int ) roundf(
                _nbConnectionsToWidth.map( fabsf( weightPropertyValue ))));

              // If fixed weight over zero or if gaussian and mean over zero
              // then circle

              relationRep->setProperty( "head",
                ( weightPropertyValue < 0.0f || relationProperties->
                getPropertyValue< float >( "Weight Gaussian Mean mean", 0.0f )
                < 0.0f ) ? shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE
                : shiftgen::ConnectionArrowRep::TArrowHead::TRIANGLE );
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

    bool RepresentationCreator::entityUpdatedOrCreated( const shift::Entity* entity )
    {
      bool updatedValues = false;
      unsigned int newNeuronsPerPopulation = 0;
      if ( dynamic_cast< const shiftgen::NeuronPop* >( entity )
        || dynamic_cast< const shiftgen::Input* >( entity ))
      {
        if( entity->hasProperty( "Nb of neurons" ))
        {
          newNeuronsPerPopulation =
            entity->getPropertyValue< unsigned int >( "Nb of neurons", 0u );
        }
        else
        {
          Loggers::get( )->log("Expected property Nb of neurons.",
            LOG_LEVEL_WARNING );
        }
      }
      else if ( dynamic_cast< const shiftgen::NeuronSuperPop* >( entity ))
      {
        if(entity->hasProperty( "Nb of neurons Mean"))
        {
          newNeuronsPerPopulation =
            entity->getPropertyValue< unsigned int >( "Nb of neurons Mean", 0u );
        }
        else
        {
          Loggers::get( )->log("Expected property Nb of neurons Mean.",
            LOG_LEVEL_WARNING );
        }
        if( entity->hasProperty( "child depth"))
        {
          unsigned  int newLevel =
            entity->getPropertyValue< unsigned int >( "child depth", 0u );
          if( newLevel > _maxLevelsPerSuperPop )
          {
            this->clear( );
            maxLevelsPerSuperPop( newLevel, false );
            updatedValues = true;
          }
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
        updatedValues =  true;
      }
      return updatedValues;
    }

    bool RepresentationCreator::relationshipUpdatedOrCreated(
      const shift::RelationshipProperties* relProperties )
    {
      float newAbsoluteWeight = 0.0f;
      if ( dynamic_cast< const shiftgen::ConnectsWith* >( relProperties ))
      {
        newAbsoluteWeight = fabsf( relProperties
          ->getPropertyValue< shiftgen::ConnectsWith::TFixedOrDistribution >
          ( "Weight Type", shiftgen::ConnectsWith::TFixedOrDistribution::Fixed )
          == shiftgen::ConnectsWith::TFixedOrDistribution::Fixed ?
          relProperties->getPropertyValue< float >( "Weight", 0.0f ) :
          relProperties->getPropertyValue< float >( "Weight Gaussian Mean",
          0.0f ));
      }
      else if ( dynamic_cast
        < const shiftgen::AggregatedConnectsWith* >( relProperties ))
      {
        if( relProperties->hasProperty( "Weight mean" ))
        {
          newAbsoluteWeight = fabsf(
            relProperties->getPropertyValue< float >( "Weight mean", .0f ));
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

    void RepresentationCreator::maxLevelsPerSuperPop(
        unsigned int maxLevelsPerSuperPop_, bool compare )
    {
      if ( !compare || maxLevelsPerSuperPop_ > _maxLevelsPerSuperPop )
      {
        _maxLevelsPerSuperPop = maxLevelsPerSuperPop_;
        _superPopSeparation = 1.0f / float( _maxLevelsPerSuperPop + 1 );
        _superPopLevelSeparation = NeuronSuperPopItem::rangeCircle
          * _superPopSeparation;
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

    unsigned int RepresentationCreator::maxLevelsPerSuperPop( void ) const
    {
      return _maxLevelsPerSuperPop;
    }

    void RepresentationCreator::reset( void )
    {
      maxNeuronsPerPopulation( 1 );
      maxLevelsPerSuperPop( 1 );
      maxAbsoluteWeight( 0.1f );
    }

  } // namespace congen
} // namespace nslib
