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
#include "ColumnRep.h"
#include "LayerRep.h"
#include "MiniColumnRep.h"
#include "Neuron.h"
#include "NeuronRep.h"
#include "NeuronTypeAggregationRep.h"
#include "ConnectionArrowRep.h"
#include "RepresentationCreator.h"

namespace nslib
{
  namespace cortex
  {
    void RepresentationCreator::updateRepresentation(
      const shift::Entity* entity,
      shift::Representation* entityRep )
    {
      if ( dynamic_cast< const Neuron* >( entity ))
      {
        updateNeuronRep( entity, entityRep );
      }
      else if ( dynamic_cast< const Column* >( entity ))
      {
        updateColumnOrMiniColumnRep( entity, entityRep, 0 );
      }
      else if ( dynamic_cast< const MiniColumn* >( entity ))
      {
        updateColumnOrMiniColumnRep( entity, entityRep, 1 );
      }
    }

    RepresentationCreator::RepresentationCreator( void )
    : _maxNeuronSomaVolume( 0.1f )
    , _maxNeuronSomaArea( 0.1f )
    , _maxNeuronDendsVolume( 0.1f )
    , _maxNeuronDendsArea( 0.1f )
    , _maxNeurons( 1u )
    , _maxNeuronsPerColumn( 1u )
    , _maxNeuronsPerMiniColumn( 1u )
    , _maxConnectionsPerEntity( 1u )
    , _greenMapper( scoop::ColorPalette::colorBrewerSequential(
        scoop::ColorPalette::ColorBrewerSequential::Greens, 6 ),
        0.0f,  _maxNeuronSomaVolume )
    , _redMapper( scoop::ColorPalette::colorBrewerSequential(
        scoop::ColorPalette::ColorBrewerSequential::Reds, 6 ),
        0.0f, _maxNeuronDendsVolume )
    , _somaAreaToAngle( 0, _maxNeuronSomaArea, 0, -360 )
    , _dendAreaToAngle( 0, _maxNeuronDendsArea, 0, -360 )
    , _neuronsToPercentage( 0, _maxNeurons, 0.0f, 1.0f )
    , _columnNeuronsToPercentage( 0, _maxNeuronsPerColumn, 0.0f, 1.0f )
    , _miniColumnNeuronsToPercentage(
        0, _maxNeuronsPerMiniColumn, 0.0f, 1.0f )
    , _nbConnectionsToWidth( 0, _maxConnectionsPerEntity , 1.0f, 3.0f )
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

      for ( const auto entity : entities.vector( ))
      {
        // if the entity has already a rep(s) don't create it
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
          for ( const auto rep : entitiesToReps[ entity ] )
            representations.push_back( rep );

          continue;
        }

        shift::Representation* entityRep = nullptr;
        if ( dynamic_cast< Neuron* >( entity ))
        {
          entityRep = new NeuronRep( );
          updateNeuronRep( entity, entityRep );

        } // end if its Neuron entity
        else if ( dynamic_cast< Column* >( entity ))
        {
          entityRep = new ColumnRep( );
          updateColumnOrMiniColumnRep( entity, entityRep, 0 );

        } // it its MiniColumn entity
        else if ( dynamic_cast< MiniColumn* >( entity ))
        {
          entityRep = new MiniColumnRep( );
          updateColumnOrMiniColumnRep( entity, entityRep, 1 );

        } // if Column entity
        else if ( dynamic_cast< Layer* >( entity ))
        {
          entityRep = getLayerRep( entity );
        } // if Layer
        else if ( dynamic_cast< NeuronTypeAggregation* >( entity ))
        {
          entityRep = getNeuronTypeAggregationRep( entity );

        } // if NeuronTypeAggregationRep

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
      } // for all entities

      // Create subentities
      const auto& relSuperEntityOf =
        *( DataManager::entities( ).
           relationships( )[ "isSuperEntityOf" ]->asOneToN( ));
      shift::Entities subEntities;

      for ( const auto& entity : entities.vector( ))
      {
        const auto entityGid = entity->entityGid( );
        if ( relSuperEntityOf.count( entityGid ) > 0 )
          for ( const auto& subEntity : relSuperEntityOf.at( entityGid ))
            subEntities.add( DataManager::entities( ).at( subEntity.first ));
      }

      if ( subEntities.size( ) > 0 )
      {
        this->create( subEntities, representations,
          entitiesToReps, repsToEntities, gidsToEntitiesReps,
          linkEntitiesToReps, linkRepsToEntities );
      }
    }

    shift::Representation* RepresentationCreator::getLayerRep(
      const shift::Entity* entity_ )
    {
      shift::Representation* entityRep;
      const bool correctProperties =
        entity_->hasProperty( "Parent gid" ) &&
        entity_->hasProperty( "Parent Id" ) &&
        entity_->hasProperty( "Parent Type" ) &&
        entity_->hasProperty( "Layer" );

      if ( !correctProperties )
      {
        Loggers::get( )->log( "Expected properties.", LOG_LEVEL_WARNING );
      }

      const LayersMapKey layerKey = correctProperties ? QuadKey(
        entity_->getPropertyValue< unsigned int >( "Parent gid", 0u ),
        entity_->getPropertyValue< unsigned int >( "Parent Id", 0u ),
        entity_->getPropertyValue< Layer::TLayerParentType >( "Parent Type" ),
        entity_->getPropertyValue< unsigned int >( "Layer", 0u ))
        : QuadKey( 0u, 0u, 0u, 0u );
      auto layerPair = _layersMap.find( layerKey );
      if ( layerPair == _layersMap.end( ))
      {
        LayerRep* layerRep =
            new LayerRep( );
        _layersMap.insert( std::make_pair( layerKey,layerRep ));
        entityRep = layerRep;
      }
      else
      {
        entityRep = layerPair->second;
      }

      return entityRep;
    }

    shift::Representation* RepresentationCreator::getNeuronTypeAggregationRep(
        const shift::Entity* entity_ )
    {
      shift::Representation* entityRep;
      const bool correctProperties =
        entity_->hasProperty( "Parent gid" ) &&
        entity_->hasProperty( "Parent Id" ) &&
        entity_->hasProperty( "Parent Type" ) &&
        entity_->hasProperty( "Layer" ) &&
        entity_->hasProperty( "Morpho Type" );

      if ( !correctProperties )
      {
        Loggers::get( )->log( "Expected properties.", LOG_LEVEL_WARNING );
      }

      NeuronTypeAggsMapKey neuronTypeAggKey = correctProperties ? PentaKey(
        entity_->getPropertyValue< unsigned int >( "Parent gid", 0u ),
        entity_->getPropertyValue< unsigned int >( "Parent Id", 0u ),
        entity_->getPropertyValue< Layer::TLayerParentType >( "Parent Type" ),
        entity_->getPropertyValue< unsigned int >( "Layer", 0u ),
        uint( entity_->getPropertyValue
        < Neuron::TMorphologicalType >( "Morpho Type" )))
        : PentaKey( 0u, 0u, 0u, 0u, 0u );

      auto neuronTypeAggPair = _neuronTypeAggsMap.find( neuronTypeAggKey );
      if ( neuronTypeAggPair == _neuronTypeAggsMap.end( ))
      {
        NeuronTypeAggregationRep* neuronTypeAggRep =
          new NeuronTypeAggregationRep( );
        _neuronTypeAggsMap.insert(
          std::make_pair( neuronTypeAggKey, neuronTypeAggRep ));
        entityRep = neuronTypeAggRep;
      }
      else
      {
        entityRep = neuronTypeAggPair->second;
      }

      return entityRep;
    }

    void RepresentationCreator::updateNeuronRep( const shift::Entity* entity_,
       shift::Representation* entityRep_ )
    {
      switch ( entity_->getPropertyValue< Neuron::TMorphologicalType >(
        "Morpho Type", Neuron::UNDEFINED_MORPHOLOGICAL_TYPE ))
      {
        case Neuron::UNDEFINED_MORPHOLOGICAL_TYPE:
          entityRep_->setProperty( "symbol", NeuronRep::NO_SYMBOL );
          break;
        case Neuron::INTERNEURON:
          entityRep_->setProperty( "symbol", NeuronRep::CIRCLE );
          break;
        case Neuron::PYRAMIDAL:
          entityRep_->setProperty( "symbol", NeuronRep::TRIANGLE );
          break;
        default:
          Loggers::get( )->log( "Unexpected value of Morpho Type.",
            LOG_LEVEL_WARNING );
          break;
      }

      switch ( entity_->getPropertyValue< Neuron::TFunctionalType >(
        "Funct Type", Neuron::UNDEFINED_FUNCTIONAL_TYPE ))
      {
        case Neuron::UNDEFINED_FUNCTIONAL_TYPE:
          entityRep_->setProperty( "bg", Color( 100, 100, 100 ));
          break;
        case Neuron::INHIBITORY:
          entityRep_->setProperty( "bg", Color( 200, 100, 100 ));
          break;
        case Neuron::EXCITATORY:
          entityRep_->setProperty( "bg", Color( 100, 100, 200 ));
          break;
        default:
          Loggers::get( )->log( "Unexpected value of Funct Type.",
            LOG_LEVEL_WARNING );
          break;
      }

      NeuronRep::Rings rings;

      shiftgen::Ring somaRing;
      somaRing.setProperty( "angle", int( roundf( _somaAreaToAngle.map(
        entity_->getPropertyValue< float >( "Soma Surface", .0f )))));

      somaRing.setProperty( "color", _greenMapper.getColor(
        entity_->getPropertyValue< float >( "Soma Volume", .0f )));
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      dendRing.setProperty( "angle", int( roundf( _dendAreaToAngle.map(
        entity_->getPropertyValue< float >( "Dendritic Surface", .0f )))));

      dendRing.setProperty( "color", _redMapper.getColor(
        entity_->getPropertyValue< float >( "Dendritic Volume", .0f )));
      rings.push_back( dendRing );
      entityRep_->setProperty( "Entity name", entity_->getPropertyValue
        <std::string>( "Entity name", " " ));
      entityRep_->setProperty( "rings", rings );
    } // create

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

            std::unordered_multimap< shift::EntityGid,
                                     shift::RelationshipProperties* > relMMap =
              ( *relatedElements )[ entity->entityGid( ) ];
            auto relMMapIt = relMMap.find( other->entityGid( ));
            if ( relMMapIt != ( *relatedElements )[ entity->entityGid( ) ].end( ) )
            {
              relationRep->setProperty( "width", ( unsigned int )
              roundf( _nbConnectionsToWidth.map(relMMapIt->second->
              getPropertyValue< unsigned int >( "count", 0u ))));
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

        auto entityRelations = relatedElements->mapAggregatedRels( ).
          find( entity->entityGid( ));

        if( entityRelations == relatedElements->mapAggregatedRels( ).end( ))
          continue;

        for( auto& other : entities.vector( ))
        {
          if( other->entityGid( ) == entity->entityGid( ))
            continue;

          auto otherRep = gidsToEntitiesReps.find( other->entityGid( ));
          if( otherRep == gidsToEntitiesReps.end( ))
            continue;

          auto otherEntityConnection = entityRelations->second->find(
            other->entityGid( ));
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
            ConnectionArrowRep* relationRep =
              new ConnectionArrowRep( srcEntityRep->second.second,
                                      otherRep->second.second );

            shift::RelationshipProperties* relationProperties =
              otherEntityConnection->second
              .relationshipAggregatedProperties.get( );
            if ( relationProperties )
            {
              relationRep->setProperty( "width", ( unsigned int ) roundf(
                _nbConnectionsToWidth.map( relationProperties->
                getPropertyValue< unsigned int >( "count sum", 0u ))));
            }

            alreadyConnected = relatedEntitiesReps.insert(
              std::make_pair( combinedKey, std::make_tuple( relationRep,
              entity, other, srcEntityRep->second.second,
              otherRep->second.second )));
          }
          relatedEntities.push_back( std::get< 0 >( alreadyConnected->second ));
        }
      }
    } // generateRelations

    void RepresentationCreator::updateColumnOrMiniColumnRep(
      const shift::Entity* entity_,
      shift::Representation* entityRep_,
      unsigned int columnOrMiniColumn )
    {
      NeuronRep* meanNeuronRep = new NeuronRep( );

      meanNeuronRep->setProperty( "symbol", NeuronRep::NO_SYMBOL );
      meanNeuronRep->setProperty( "bg", Color( 200, 200, 200 ));

      shiftgen::NeuronAggregationRep::Rings rings;

      shiftgen::Ring somaRing;
      somaRing.setProperty( "angle", int( roundf( _somaAreaToAngle.map(
        entity_->getPropertyValue< float >( "meanSomaArea", .0f )))));
      somaRing.setProperty( "color", _redMapper.getColor(
        entity_->getPropertyValue< float >( "meanSomaVolume", .0f )));
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      dendRing.setProperty( "angle", int( roundf( _dendAreaToAngle.map(
        entity_->getPropertyValue< float >( "meanDendArea", .0f )))));
      dendRing.setProperty( "color", _greenMapper.getColor(
        entity_->getPropertyValue< float >( "meanDendVolume", .0f )));
      rings.push_back( dendRing );

      meanNeuronRep->registerProperty( "rings", rings );

      entityRep_->registerProperty( "meanNeuron", *meanNeuronRep );

      shiftgen::NeuronAggregationRep::Layers layersReps;

      auto layerRep = new LayerRep;
      layerRep->setProperty( "leftPerc", roundf( _neuronsToPercentage.map(
        entity_->getPropertyValue<uint>( "Num Pyramidals", 0u ))));

      layerRep->setProperty( "rightPerc", roundf( _neuronsToPercentage.map(
        entity_->getPropertyValue<uint>( "Num Interneurons", 0u ))));
      layersReps.push_back( layerRep );

      uint entityID = entity_->getPropertyValue< uint >( "Id", 0u );

      unsigned int entityGid = entity_->entityGid( );
      for ( unsigned int layer = 1; layer <= 6; ++layer )
      {
        auto layerKey = QuadKey(
          entityGid,
          entityID,
          columnOrMiniColumn,
          layer );

        if ( _layersMap.count( layerKey ) == 0 )
        {
          _layersMap[ layerKey ] = new LayerRep( );
          assert( _layersMap.count( layerKey ) == 1);
        }

        layerRep = _layersMap[ layerKey ]; //new LayerRep;

        layerRep->setProperty( "leftPerc", _columnNeuronsToPercentage.map(
          entity_->getPropertyValue< uint >( std::string( "Num Pyr Layer " )
          + std::to_string( layer ))));

        layerRep->setProperty( "rightPerc", _columnNeuronsToPercentage.map(
          entity_->getPropertyValue< uint >( std::string( "Num Inter Layer " )
          + std::to_string( layer ), 0u )));
        layersReps.push_back( layerRep );
      }
      entityRep_->registerProperty( "layers", layersReps );

      NeuronTypeAggregationRep* neuronTypeAggRep;
      shiftgen::NeuronAggregationRep::NeuronTypeAggregations neuronTypeAggsReps;
      std::vector< Neuron::TMorphologicalType>
        neuronTypes =
        {
          Neuron::PYRAMIDAL,
          Neuron::INTERNEURON
        };
      for ( const auto& neuronType : neuronTypes )
      {
        for ( unsigned int layer = 0; layer <= 6; ++layer )
        {
          auto neuronTypeAggKey = PentaKey( entityGid,
            entityID, columnOrMiniColumn, layer, uint( neuronType ));

          if ( _neuronTypeAggsMap.count( neuronTypeAggKey ) == 0 )
            _neuronTypeAggsMap[ neuronTypeAggKey ] =
              new NeuronTypeAggregationRep( );

          neuronTypeAggRep = _neuronTypeAggsMap[ neuronTypeAggKey ];

          neuronTypeAggRep->setProperty(
            "symbol",
            neuronType == Neuron::PYRAMIDAL ?
            NeuronTypeAggregationRep::TSymbol::TRIANGLE :
            NeuronTypeAggregationRep::TSymbol::CIRCLE );
          neuronTypeAggsReps.push_back( neuronTypeAggRep );
        }
      }
      entityRep_->setProperty( "Entity name", entity_->getPropertyValue
        <std::string>( "Entity name", " " ));
      entityRep_->registerProperty( "neuronTypeAggregations", neuronTypeAggsReps );
    }

    bool RepresentationCreator::entityUpdatedOrCreated(
      const shift::Entity* entity )
    {
      bool needToClearCache = false;
      if ( dynamic_cast< const Neuron* >( entity ))
      {
        const float newMaxSomaVolume =
            entity->getPropertyValue< float >( "Soma Volume", .0f );
        if ( newMaxSomaVolume > _maxNeuronSomaVolume )
        {
          needToClearCache = true;
          maxNeuronSomaVolume( newMaxSomaVolume, false );
        }

        const float newMaxSomaArea =
            entity->getPropertyValue< float >( "Soma Surface", .0f );
        if ( newMaxSomaArea > _maxNeuronSomaArea )
        {
          needToClearCache = true;
          maxNeuronSomaArea( newMaxSomaArea, false );
        }

        const float newMaxDendriticVolume =
            entity->getPropertyValue< float >( "Dendritic Volume", .0f );
        if ( newMaxDendriticVolume > _maxNeuronDendsVolume )
        {
          needToClearCache = true;
          maxNeuronDendsVolume( newMaxDendriticVolume, false );
        }

        const float newMaxDendsArea =
            entity->getPropertyValue< float >( "Dendritic Surface", .0f );
        if ( newMaxDendsArea > _maxNeuronDendsArea )
        {
          needToClearCache = true;
          maxNeuronDendsArea( newMaxDendsArea, false );
        }
      }
      else if ( dynamic_cast< const Layer* >( entity ))
      {
        const unsigned int newMaxNeurons = std::max(
            entity->getPropertyValue< unsigned int > ( "Num Pyramidals", 0u ),
            entity->getPropertyValue< unsigned int >( "Num Interneurons", 0u ));
        if ( newMaxNeurons > _maxNeurons )
        {
          needToClearCache = true;
          maxNeurons( newMaxNeurons, false );
        }
      }

      if ( needToClearCache )
      {
        this->clear( );
        return true;
      }
      else
      {
        return false;
      }
    }

    void RepresentationCreator::reset( void )
    {
      clear( );
      setMaximums( 0.1f, 0.1f, 0.1f, 0.1f, 1u, 1u, 1u, 1u );
    }

    void RepresentationCreator::clear( void )
    {
      _layersMap.clear( );
      _neuronTypeAggsMap.clear( );
    }

    float RepresentationCreator::maxNeuronSomaVolume( void ) const
    {
      return _maxNeuronSomaVolume;
    }

    float RepresentationCreator::maxNeuronSomaArea( void ) const
    {
      return _maxNeuronSomaArea;
    }

    float RepresentationCreator::maxNeuronDendsVolume( void ) const
    {
      return _maxNeuronDendsVolume;
    }

    float RepresentationCreator::maxNeuronDendsArea( void ) const
    {
      return _maxNeuronDendsArea;
    }

    unsigned int RepresentationCreator::maxNeurons( void ) const
    {
      return _maxNeurons;
    }

    unsigned int RepresentationCreator::maxNeuronsPerColumn( void ) const
    {
      return _maxNeuronsPerColumn;
    }

    unsigned int RepresentationCreator::maxNeuronsPerMiniColumn( void ) const
    {
      return _maxNeuronsPerMiniColumn;
    }

    unsigned int RepresentationCreator::maxConnectionsPerEntity( void ) const
    {
      return _maxConnectionsPerEntity;
    }

    void RepresentationCreator::maxNeuronSomaVolume(
      float maxNeuronSomaVolume_, bool compare )
    {
      if ( !compare || maxNeuronSomaVolume_ > _maxNeuronSomaVolume )
      {
        _maxNeuronSomaVolume = maxNeuronSomaVolume_;
        _greenMapper = scoop::SequentialColorMap(
          scoop::ColorPalette::colorBrewerSequential(
          scoop::ColorPalette::ColorBrewerSequential::Greens, 6 ),
          0.0f,  _maxNeuronSomaVolume );
      }
    }

    void RepresentationCreator::maxConnectionsPerEntity(
      unsigned int maxConnectionsPerEntity_, bool compare )
    {
      if ( !compare || maxConnectionsPerEntity_ > _maxConnectionsPerEntity  )
      {
        _maxConnectionsPerEntity = maxConnectionsPerEntity_;
         _nbConnectionsToWidth = MapperFloatToFloat(
          0, _maxConnectionsPerEntity, 1.0f, 3.0f );
      }
    }

    void RepresentationCreator::maxNeuronsPerMiniColumn(
      unsigned int maxNeuronsPerMiniColumn_, bool compare )
    {
      if ( !compare || maxNeuronsPerMiniColumn_ > _maxNeuronsPerMiniColumn )
      {
        _maxNeuronsPerMiniColumn = maxNeuronsPerMiniColumn_;
        _miniColumnNeuronsToPercentage =
          MapperFloatToFloat( 0, _maxNeuronsPerMiniColumn, 0.0f, 1.0f );
      }
    }

    void RepresentationCreator::maxNeuronsPerColumn(
      unsigned int maxNeuronsPerColumn_, bool compare )
    {
      if ( !compare || maxNeuronsPerColumn_ > _maxNeuronsPerColumn )
      {
        _maxNeuronsPerColumn = maxNeuronsPerColumn_;
        _columnNeuronsToPercentage =
          MapperFloatToFloat( 0, _maxNeuronsPerColumn, 0.0f, 1.0f );
      }
    }

    void RepresentationCreator::maxNeurons(
      unsigned int maxNeurons_, bool compare )
    {
      if ( !compare || maxNeurons_ > _maxNeurons )
      {
        _maxNeurons = maxNeurons_;
        _neuronsToPercentage = MapperFloatToFloat( 0, _maxNeurons, 0.0f, 1.0f );
      }
    }

    void RepresentationCreator::maxNeuronDendsArea(
      float maxNeuronDendsArea_, bool compare )
    {
      if ( !compare || maxNeuronDendsArea_ > _maxNeuronDendsArea )
      {
        _maxNeuronDendsArea = maxNeuronDendsArea_;
        _dendAreaToAngle =
          MapperFloatToFloat( 0, _maxNeuronDendsArea, 0, -360 );
      }
    }

    void RepresentationCreator::maxNeuronDendsVolume(
      float maxNeuronDendsVolume_, bool compare )
    {
      if ( !compare || maxNeuronDendsVolume_ > _maxNeuronDendsVolume )
      {
        _maxNeuronDendsVolume = maxNeuronDendsVolume_;
        _redMapper = scoop::SequentialColorMap(
          scoop::ColorPalette::colorBrewerSequential(
          scoop::ColorPalette::ColorBrewerSequential::Reds, 6 ),
          0.0f, _maxNeuronDendsVolume );
      }
    }

    void RepresentationCreator::maxNeuronSomaArea(
      float maxNeuronSomaArea_, bool compare )
    {
      if ( !compare || maxNeuronSomaArea_ > _maxNeuronSomaArea )
      {
        _maxNeuronSomaArea = maxNeuronSomaArea_;
        _somaAreaToAngle = MapperFloatToFloat( 0, _maxNeuronSomaArea, 0, -360 );
      }
    }

    void RepresentationCreator::setMaximums( float maxNeuronSomaVolume_,
      float maxNeuronSomaArea_,
      float maxNeuronDendsVolume_,
      float maxNeuronDendsArea_,
      unsigned int maxNeurons_,
      unsigned int maxNeuronsPerColumn_,
      unsigned int maxNeuronsPerMiniColumn_,
      unsigned int maxConnectionsPerEntity_ )
    {
      maxNeuronSomaVolume( maxNeuronSomaVolume_ );
      maxNeuronSomaArea( maxNeuronSomaArea_ );
      maxNeuronDendsVolume( maxNeuronDendsVolume_ );
      maxNeuronDendsArea( maxNeuronDendsArea_ );
      maxNeurons( maxNeurons_ );
      maxNeuronsPerColumn( maxNeuronsPerColumn_ );
      maxNeuronsPerMiniColumn( maxNeuronsPerMiniColumn_ );
      maxConnectionsPerEntity( maxConnectionsPerEntity_ );
    }

  } // namespace cortex
} // namespace nslib
