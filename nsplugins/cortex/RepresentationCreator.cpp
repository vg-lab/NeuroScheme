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
      // if ( linkEntitiesToReps )
      //   entitiesToReps.clear( );
      // if ( linkRepsToEntities )
      //   repsToEntities.clear( );

      for ( const auto entity : entities.vector( ))
      {
        // std::cout << ", new entity " << entity->entityGid( )
                  // << " --> LayersMap size = "
                  //   << _layersMap.size( ) 
                  //<< std::endl;
        // if the entity has already a rep(s) don't create it
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
          // std::cout << "Entity with already a rep" << std::endl;
          for ( const auto rep : entitiesToReps[ entity ] )
            representations.push_back( rep );
          continue;
        }
//        auto& entity = entityPair.second;
        shift::Representation* entityRep = nullptr;
        if ( dynamic_cast< Neuron* >( entity ))
        {
          entityRep = new NeuronRep( );
          updateNeuronRep( entity, entityRep );

        } // end if its Neuron entity
        else if ( dynamic_cast< Column* >( entity ))
        {
          // entity->registerProperty( "tt", uint( 0 ));
          // std::cout << " id type: "
          //           << entity->getProperty( "tt" ).type( ) << " --> "
          //           << demangleType( entity->getProperty( "tt" ).type( )) << "  "
          //           << entity->getProperty( "Id" ).type( ) << " --> "
          //           << demangleType( entity->getProperty( "Id" ).type( )) << " "
          //           << std::endl;
          entityRep = new ColumnRep( );
       // std::cout << ", RepresentationCreator::updateColumnOrMiniColumnRep" << std::endl;

          updateColumnOrMiniColumnRep( entity, entityRep, 0 );
          // std::cout << ", POST updateColumnOrMiniColumnRep LayersMap size = "
          //           << _layersMap.size( ) << std::endl;

        } // it its MiniColumn entity
        else if ( dynamic_cast< MiniColumn* >( entity ))
        {
          // std::cout << "creating minicolumn rep" << std::endl;
          entityRep = new MiniColumnRep( );
          updateColumnOrMiniColumnRep( entity, entityRep, 1 );

        } // if Column entity
        else if ( dynamic_cast< Layer* >( entity ))
        {
          entityRep = getLayerRep( entity );
        } // if Layer
        else if ( dynamic_cast< NeuronTypeAggregation* >( entity ))
        {
          // std::cout << ", RepresentationCreator "
          //           << _neuronTypeAggsMap.size( ) << std::endl;
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

        // std::cout << ", end entity --> LayersMap size = "
        //           << _layersMap.size( ) << std::endl;
      } // for all entities

      // Create subentities
      const auto& relSuperEntityOf =
        *( DataManager::entities( ).
           relationships( )[ "isSuperEntityOf" ]->asOneToN( ));
      shift::Entities subEntities;
      for ( const auto& entity : entities.vector( ))
      {
        auto entityGid = entity->entityGid( );
        if ( relSuperEntityOf.count( entityGid ) > 0 )
          for ( const auto& subEntity : relSuperEntityOf.at( entityGid ))
            subEntities.add( DataManager::entities( ).at( subEntity.first ));
      }
      if ( subEntities.size( ) > 0 )
        this->create( subEntities, representations,
          entitiesToReps, repsToEntities, gidsToEntitiesReps,
          linkEntitiesToReps, linkRepsToEntities );

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
        entity_->getProperty( "Parent gid" ).value< unsigned int >( ),
        entity_->getProperty( "Parent Id" ).value< unsigned int >( ),
        entity_->getProperty( "Parent Type" ).value< Layer::TLayerParentType >( ),
        entity_->getProperty( "Layer" ).value< unsigned int >( ))
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
        entity_->getProperty( "Parent gid" ).value< unsigned int >( ),
        entity_->getProperty( "Parent Id" ).value< unsigned int >( ),
        entity_->getProperty( "Parent Type" ).value< Layer::TLayerParentType >( ),
        entity_->getProperty( "Layer" ).value< unsigned int >( ),
        uint( entity_->getProperty( "Morpho Type" ).value<
        Neuron::TMorphologicalType >( )))
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
      // std::cout
      //   << fires::PropertyManager::getPropertyCaster(
      //     "morphoType" )->toString(
      //     neuron->getProperty( "morphoType" )) << std::endl;
      if ( entity_->hasProperty( "Morpho Type" ))
      {
        switch ( entity_->getProperty( "Morpho Type" ).
          value< Neuron::TMorphologicalType >( ))
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
      }
      else
      {
        entityRep_->setProperty( "symbol", NeuronRep::NO_SYMBOL );
        Loggers::get( )->log( "Expected propery Morpho Type.",
          LOG_LEVEL_WARNING );
      }
      if( entity_->hasProperty( "Funct Type" ))
      {
        switch ( entity_->getProperty(
          "Funct Type" ).value< Neuron::TFunctionalType >( ))
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
      }
      else
      {
        Loggers::get( )->log( "Expected property Funct Type.",
          LOG_LEVEL_WARNING );
        entityRep_->setProperty( "bg", Color( 100, 100, 100 ));
      }

      NeuronRep::Rings rings;

      shiftgen::Ring somaRing;
      if ( entity_->hasProperty( "Soma Surface" ))
      {
        somaRing.setProperty( "angle",
          int( roundf( _somaAreaToAngle.map(
          entity_->getProperty( "Soma Surface" ).value< float >( )))));
      }
      else
      {
        Loggers::get( )->log( "Expected property Soma Surface.",
          LOG_LEVEL_WARNING );
        somaRing.setProperty( "angle",
          int( roundf( _somaAreaToAngle.map( 0.0f ))));
      }
      // greenMapper.value( ) =
      //   neuron->getProperty( "Soma Volume" ).value< float >( );
      // somaRing.setProperty( "color", greenMapper.map( ));

      if ( entity_->hasProperty( "Soma Volume" ))
      {
        somaRing.setProperty( "color",
          _greenMapper.getColor(
          entity_->getProperty( "Soma Volume" ).value< float >( )));
      }
      else
      {
        Loggers::get( )->log( "Expected property Soma Volume.",
          LOG_LEVEL_WARNING );
        somaRing.setProperty( "color", _greenMapper.getColor( 0.0f ));
      }
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      if ( entity_->hasProperty( "Dendritic Surface" ))
      {
        dendRing.setProperty( "angle",
          int( roundf( _dendAreaToAngle.map(
          entity_->getProperty( "Dendritic Surface" ).value< float >( )))));
      }
      else
      {
        Loggers::get( )->log( "Expected property Dendritic Surface.",
                              LOG_LEVEL_WARNING );
        dendRing.setProperty( "angle",
          int( roundf( _dendAreaToAngle.map( 0.0f ))));
      }
      // _redMapper.value( ) =
      //   neuron->getProperty( "Dendritic Volume" ).value< float >( );
      // dendRing.setProperty( "color", _redMapper.map( ));

      if ( entity_->hasProperty( "Dendritic Volume" ))
      {
        dendRing.setProperty( "color",
          _redMapper.getColor(
          entity_->getProperty( "Dendritic Volume" ).value< float >( )));
      }
      else
      {
        Loggers::get( )->log( "Expected property Dendritic Volume.",
          LOG_LEVEL_WARNING );
        dendRing.setProperty( "color", _redMapper.getColor( 0.0f ));
      }
      rings.push_back( dendRing );
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
              if ( relMMapIt->second->hasProperty( "count" ))
              {
                relationRep->setProperty(
                  "width", ( unsigned int ) roundf( _nbConnectionsToWidth.map(
                  relMMapIt->second->getProperty( "count" )
                  .value< unsigned int >( ))));
              }
              else
              {
                relationRep->setProperty( "width",
                  ( unsigned int ) roundf( _nbConnectionsToWidth.map( 0u )));
                Loggers::get( )->log("Expected property count.",
                  LOG_LEVEL_WARNING );
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

          //auto numberOfConnections = 1
          //  //entityRelations->count( other->entityGid( ));

          //if( numberOfConnections == 0 )
          //  continue;

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
              if ( relationProperties->hasProperty( "count sum" ))
              {
                relationRep->setProperty(
                  "width", ( unsigned int ) roundf(
                  _nbConnectionsToWidth.map(
                  relationProperties->getProperty( "count sum" )
                  .value< unsigned int >( ))));
              }
              else
              {
                relationRep->setProperty( "width",
                  ( unsigned int ) roundf( _nbConnectionsToWidth.map( 0u )));
                Loggers::get( )->log("Expected property count sum.",
                  LOG_LEVEL_WARNING );
              }
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
      NeuronRep meanNeuronRep;

      meanNeuronRep.setProperty( "symbol", NeuronRep::NO_SYMBOL );
      meanNeuronRep.setProperty( "bg", Color( 200, 200, 200 ));

      shiftgen::NeuronAggregationRep::Rings rings;


      // somaVolumeToColor.value( ) =
      //   entity->getProperty( "meanSomaVolume" ).value< float >( );

      shiftgen::Ring somaRing;
      if ( entity_->hasProperty( "meanSomaArea" ))
      {
        somaRing.setProperty( "angle",
          int( roundf( _somaAreaToAngle.map(
          entity_->getProperty( "meanSomaArea" ).value< float >( )))));
      }
      else
      {
        somaRing.setProperty( "angle",
          int( roundf( _somaAreaToAngle.map( 0.0f ))));
        Loggers::get( )->log("Expected property meanSomaArea.",
          LOG_LEVEL_WARNING );
      }
      // somaVolumeToColor.value( ) =
      //   entity->getProperty( "meanSomaVolume" ).value< float >( );
      // somaRing.setProperty( "color", somaVolumeToColor.map( ));
      if ( entity_->hasProperty( "meanSomaVolume" ))
      {
        somaRing.setProperty( "color", _redMapper.getColor(
          entity_->getProperty( "meanSomaVolume" ).value< float >( )));
      }
      else
      {
        somaRing.setProperty( "color",_redMapper.getColor( 0.0f ));
        Loggers::get( )->log("Expected property meanSomaVolume.",
          LOG_LEVEL_WARNING );
      }
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      if ( entity_->hasProperty( "meanDendArea" ))
      {
        dendRing.setProperty( "angle",
          int( roundf( _dendAreaToAngle.map(
          entity_->getProperty( "meanDendArea" ).value< float >( )))));
      }
      else
      {
        dendRing.setProperty( "angle",
          int( roundf( _dendAreaToAngle.map( 0.0f ))));
        Loggers::get( )->log("Expected property meanDendArea.",
          LOG_LEVEL_WARNING );
      }
      // dendVolumeToColor.value( ) =
      //   entity->getProperty( "meanDendVolume" ).value< float >( );
      // dendRing.setProperty( "color", dendVolumeToColor.map( ));
      if ( entity_->hasProperty( "meanDendVolume" ))
      {
        dendRing.setProperty( "color",
          _greenMapper.getColor(
          entity_->getProperty( "meanDendVolume" ).value< float >( )));
      }
      else
      {
        dendRing.setProperty( "color", _greenMapper.getColor( 0.0f ));
        Loggers::get( )->log("Expected property meanDendVolume.",
          LOG_LEVEL_WARNING );
      }
      rings.push_back( dendRing );

      meanNeuronRep.registerProperty( "rings", rings );

      entityRep_->registerProperty( "meanNeuron", meanNeuronRep );

      shiftgen::NeuronAggregationRep::Layers layersReps;

      auto layerRep = new LayerRep;
      if ( entity_->hasProperty( "Num Pyramidals" ))
      {
        layerRep->setProperty( "leftPerc",
          roundf( _neuronsToPercentage.map(
          entity_->getProperty( "Num Pyramidals" ).value< uint >( ))));
      }
      else
      {
        layerRep->setProperty( "leftPerc",
          roundf( _neuronsToPercentage.map( 0u )));
        Loggers::get( )->log("Expected property Num Pyramidals.",
          LOG_LEVEL_WARNING );
      }

      if ( entity_->hasProperty( "Num Interneurons" ))
      {
        layerRep->setProperty( "rightPerc",
          roundf( _neuronsToPercentage.map(
          entity_->getProperty( "Num Interneurons" ).value< uint >( ))));
      }
      else
      {
        layerRep->setProperty( "rightPerc",
          roundf( _neuronsToPercentage.map( 0u )));
        Loggers::get( )->log("Expected property Num Interneurons.",
          LOG_LEVEL_WARNING );
      }
      layersReps.push_back( layerRep );

      uint entityID = 0;
      if ( entity_->hasProperty( "Id" ))
      {
        entityID = entity_->getProperty( "Id" ).value< uint >( );
      }
      else
      {
        Loggers::get( )->log("Expected property Id.", LOG_LEVEL_WARNING );
      }

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

        //layerRep = new LayerRep;
        std::string layerLabel =
          std::string( "Num Pyr Layer " ) + std::to_string( layer );
        if ( entity_->hasProperty( layerLabel ))
        {
          layerRep->setProperty( "leftPerc",
            _columnNeuronsToPercentage.map(
            entity_->getProperty( layerLabel ).value< uint >( )));
        }
        else
        {
          layerRep->setProperty( "leftPerc",
            _columnNeuronsToPercentage.map( 0u ));
          Loggers::get( )->log( std::string( "Expected property " )
            + layerLabel, LOG_LEVEL_WARNING );
        }

        layerLabel =
            std::string( "Num Inter Layer " ) + std::to_string( layer );
        if ( entity_->hasProperty( layerLabel ))
        {
          layerRep->setProperty( "rightPerc",
            _columnNeuronsToPercentage.map(
            entity_->getProperty( layerLabel ).value< uint >( )));
        }
        else
        {
          layerRep->setProperty( "rightPerc",
            _columnNeuronsToPercentage.map( 0u ));
          Loggers::get( )->log( std::string( "Expected property " )
            + layerLabel, LOG_LEVEL_WARNING );
        }
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
      entityRep_->registerProperty( "neuronTypeAggregations", neuronTypeAggsReps );
    }


    bool RepresentationCreator::entityUpdatedOrCreated( shift::Entity* entity )
    {
      bool needToClearCache = false;
      if ( dynamic_cast< Neuron* >( entity ))
      {
        if( entity->hasProperty( "Soma Volume" ))
        {
          float newMaxSomaVolume =
            entity->getProperty( "Soma Volume" ).value< float >( );
          if ( newMaxSomaVolume > _maxNeuronSomaVolume )
          {
            needToClearCache = true;
            maxNeuronSomaVolume( newMaxSomaVolume, false );
          }
        }
        else
        {
          Loggers::get( )->log("Expected property Soma Volume.",
            LOG_LEVEL_WARNING );
        }
        if( entity->hasProperty( "Soma Surface" ))
        {
          float newMaxSomaArea =
              entity->getProperty( "Soma Surface" ).value< float >( );
          if ( newMaxSomaArea > _maxNeuronSomaArea )
          {
            needToClearCache = true;
            maxNeuronSomaArea( newMaxSomaArea, false );
          }
        }
        else
        {
          Loggers::get( )->log("Expected property Soma Surface.",
                               LOG_LEVEL_WARNING );
        }
        if( entity->hasProperty( "Dendritic Volume" ))
        {
          float newMaxDendriticVolume =
              entity->getProperty( "Dendritic Volume" ).value< float >( );
          if ( newMaxDendriticVolume > _maxNeuronDendsVolume )
          {
            needToClearCache = true;
            maxNeuronDendsVolume( newMaxDendriticVolume, false );
          }
        }
        else
        {
          Loggers::get( )->log("Expected property Dendritic Volume.",
                               LOG_LEVEL_WARNING );
        }
        if( entity->hasProperty( "Dendritic Surface" ))
        {
          float newMaxDendsArea =
            entity->getProperty( "Dendritic Surface" ).value< float >( );
          if ( newMaxDendsArea > _maxNeuronDendsArea )
          {
            needToClearCache = true;
            maxNeuronDendsArea( newMaxDendsArea, false );
          }
        }
        else
        {
          Loggers::get( )->log("Expected property Dendritic Surface.",
            LOG_LEVEL_WARNING );
        }
      }
      else if ( dynamic_cast< Layer* >( entity ))
      {
        unsigned int newMaxNeurons = 0u;
        if( entity->hasProperty( "Num Pyramidals" ))
        {
          newMaxNeurons = entity->getProperty( "Num Pyramidals" )
            .value< unsigned int >( );
        }
        else
        {
          Loggers::get( )->log("Expected property Num Pyramidals.",
            LOG_LEVEL_WARNING );
        }
        if( entity->hasProperty( "Num Interneurons" ))
        {
          newMaxNeurons = std::max( newMaxNeurons,
            entity->getProperty( "Num Interneurons" ).value< unsigned int >( ));
        }
        else
        {
          Loggers::get( )->log("Expected property Num Interneurons.",
            LOG_LEVEL_WARNING );
        }
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
