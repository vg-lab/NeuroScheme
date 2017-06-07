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

      // if ( linkEntitiesToReps )
      //   entitiesToReps.clear( );
      // if ( linkRepsToEntities )
      //   repsToEntities.clear( );

      scoop::SequentialColorMap greenMapper(
        scoop::ColorPalette::colorBrewerSequential(
          scoop::ColorPalette::ColorBrewerSequential::Greens, 6 ),
        0.0f, ( _maxNeuronSomaVolume == 0 ? 0.1f : _maxNeuronSomaVolume ));

      scoop::SequentialColorMap redMapper(
        scoop::ColorPalette::colorBrewerSequential(
          scoop::ColorPalette::ColorBrewerSequential::Reds, 6 ),
        0.0f, ( _maxNeuronDendsVolume == 0 ? 0.1f : _maxNeuronDendsVolume ));


      MapperFloatToFloat somaAreaToAngle(
        0, _maxNeuronSomaArea == 0 ? 0.1f : _maxNeuronSomaArea, 0, -360 );
      MapperFloatToFloat dendAreaToAngle(
        0, _maxNeuronDendsArea == 0 ? 0.1f : _maxNeuronDendsArea, 0, -360 );

      MapperFloatToFloat
        neuronsToPercentage( 0, _maxNeurons, 0.0f, 1.0f );
      MapperFloatToFloat
        columnNeuronsToPercentage( 0, _maxNeuronsPerColumn, 0.0f, 1.0f );
      MapperFloatToFloat
        miniColumnNeuronsToPercentage(
          0, _maxNeuronsPerMiniColumn, 0.0f, 1.0f );

      for ( const auto entity : entities.vector( ))
      {
        // std::cout << ", new entity " << entity->entityGid( )
        //           << " --> LayersMap size = "
        //             << _layersMap.size( ) << std::endl;
        // if the entity has already a rep(s) don't create it
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
          // std::cout << "Entity with already a rep" << std::endl;
          for ( const auto rep : entitiesToReps[ entity ] )
            representations.push_back( rep );
          continue;
        }
//        auto& entity = entityPair.second;
        if ( dynamic_cast< Neuron* >( entity ))
        {
          auto neuron = dynamic_cast< Neuron* >( entity );
          auto neuronRep = new NeuronRep( );

          // std::cout
          //   << fires::PropertyManager::getPropertyCaster(
          //     "morphoType" )->toString(
          //     neuron->getProperty( "morphoType" )) << std::endl;
          switch ( neuron->getProperty( "Morpho Type" ).
                   value< Neuron::TMorphologicalType >( ))
          {
          case Neuron::UNDEFINED_FUNCTIONAL_TYPE:
            neuronRep->setProperty( "symbol", NeuronRep::NO_SYMBOL );
            break;
          case Neuron::INTERNEURON:
            neuronRep->setProperty( "symbol", NeuronRep::CIRCLE );
            break;
          case Neuron::PYRAMIDAL:
            neuronRep->setProperty( "symbol", NeuronRep::TRIANGLE );
            break;
          default:
            break;
          }

          switch (
            neuron->getProperty(
              "Funct Type" ).value< Neuron::TFunctionalType >( ))
          {
          case Neuron::UNDEFINED_FUNCTIONAL_TYPE:
            neuronRep->setProperty( "bg", Color( 100, 100, 100 ));
            break;
          case Neuron::INTERNEURON:
            neuronRep->setProperty( "bg", Color( 200, 100, 100 ));
            break;
          case Neuron::PYRAMIDAL:
            neuronRep->setProperty( "bg", Color( 100, 100, 200 ));
            break;
          default:
            break;
          }

          shiftgen::NeuronRep::Rings rings;

          shiftgen::Ring somaRing;
          somaRing.setProperty(
            "angle",
            int(
              roundf(
                somaAreaToAngle.map(
                  neuron->getProperty( "Soma Surface" ).value< float >( )))));
          // greenMapper.value( ) =
          //   neuron->getProperty( "Soma Volume" ).value< float >( );
          // somaRing.setProperty( "color", greenMapper.map( ));
          somaRing.setProperty(
            "color",
            greenMapper.getColor(
              neuron->getProperty( "Soma Volume" ).value< float >( )));
          rings.push_back( somaRing );

          shiftgen::Ring dendRing;
          dendRing.setProperty(
            "angle",
            int(
              roundf(
                dendAreaToAngle.map(
                  neuron->getProperty(
                    "Dendritic Surface" ).value< float >( )))));
          // redMapper.value( ) =
          //   neuron->getProperty( "Dendritic Volume" ).value< float >( );
          // dendRing.setProperty( "color", redMapper.map( ));
          dendRing.setProperty(
            "color",
            redMapper.getColor(
              neuron->getProperty( "Dendritic Volume" ).value< float >( )));
          rings.push_back( dendRing );


          neuronRep->setProperty( "rings", rings );

          representations.push_back( neuronRep );

          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( neuronRep );
          if ( linkRepsToEntities )
            repsToEntities[ neuronRep ].insert( entity );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, neuronRep ));
        } // end if its Neuron entity
        else
        if ( dynamic_cast< Column* >( entity ))
        {
          // entity->registerProperty( "tt", uint( 0 ));
          // std::cout << " id type: "
          //           << entity->getProperty( "tt" ).type( ) << " --> "
          //           << demangleType( entity->getProperty( "tt" ).type( )) << "  "
          //           << entity->getProperty( "Id" ).type( ) << " --> "
          //           << demangleType( entity->getProperty( "Id" ).type( )) << " "
          //           << std::endl;
          auto column = dynamic_cast< Column* >( entity );
          auto columnRep = new ColumnRep( );
       // std::cout << ", RepresentationCreator::_createColumnOrMiniColumn" << std::endl;
         _createColumnOrMiniColumn(
            column, columnRep,
            entity->getProperty( "Id" ).value< uint >( ),
            0,
            somaAreaToAngle,
            dendAreaToAngle,
            greenMapper,
            redMapper,
            neuronsToPercentage,
            columnNeuronsToPercentage,
            _layersMap,
            entitiesToReps, repsToEntities,
            linkEntitiesToReps, linkRepsToEntities );
          representations.push_back( columnRep );
          // std::cout << ", POST _createColumnOrMiniColumn LayersMap size = "
          //           << _layersMap.size( ) << std::endl;


          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( columnRep );
          if ( linkRepsToEntities )
            repsToEntities[ columnRep ].insert( entity );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, columnRep ));

        } // it its MiniColumn entity
        else
        if ( dynamic_cast< MiniColumn* >( entity ))
        {
          // std::cout << "creating minicolumn rep" << std::endl;
          auto miniColumn = dynamic_cast< MiniColumn* >( entity );
          auto miniColumnRep = new MiniColumnRep( );
          _createColumnOrMiniColumn(
            miniColumn, miniColumnRep,
            entity->getProperty( "Id" ).value< unsigned int >( ),
            1,
            somaAreaToAngle,
            dendAreaToAngle,
            greenMapper,
            redMapper,
            neuronsToPercentage,
            miniColumnNeuronsToPercentage,
            _layersMap,
            entitiesToReps, repsToEntities,
            linkEntitiesToReps, linkRepsToEntities);

          representations.push_back( miniColumnRep );

          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( miniColumnRep );
          if ( linkRepsToEntities )
            repsToEntities[ miniColumnRep ].insert( entity );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, miniColumnRep ));
        } // if Column entity
        else
        if ( dynamic_cast< Layer* >( entity ))
        {
          // std::cout << ", RepresentationCreator::Layer "
          //           << _layersMap.size( ) << std::endl;
          auto layerKey = TripleKey(
              entity->getProperty( "Parent Id" ).value< unsigned int >( ),
              entity->getProperty( "Parent Type" ).value< unsigned int >( ),
              entity->getProperty( "Layer" ).value< unsigned int >( ));
          if ( _layersMap.count( layerKey ) == 0 )
          {
            _layersMap[ layerKey ] = new LayerRep( );
          }
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( _layersMap.at( layerKey ));
          if ( linkRepsToEntities )
            repsToEntities[ _layersMap.at( layerKey ) ].insert( entity );

          auto layerRep = _layersMap[ layerKey ];
          representations.push_back( layerRep );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, layerRep ));
        } // if Layer
        else
        if ( dynamic_cast< NeuronTypeAggregation* >( entity ))
        {
          // std::cout << ", RepresentationCreator "
          //           << _neuronTypeAggsMap.size( ) << std::endl;
          auto neuronTypeAggregationKey = QuadKey(
              entity->getProperty( "Parent Id" ).value< unsigned int >( ),
              entity->getProperty( "Parent Type" ).value< unsigned int >( ),
              entity->getProperty( "Layer" ).value< unsigned int >( ),
              uint( entity->getProperty( "Morpho Type" ).value<
                    Neuron::TMorphologicalType >( )));

          // std::cout << "-Quadkey"
          //           << entity->getProperty( "Parent Id" ).value< unsigned int >( ) << ","
          //           << entity->getProperty( "Parent Type" ).value< unsigned int >( ) << ","
          //           << entity->getProperty( "Layer" ).value< unsigned int >( ) << ","
          //           << uint( entity->getProperty( "Morpho Type" ).value<
          //                    Neuron::TMorphologicalType >( )) 
          //           << std::endl;

          if ( _neuronTypeAggsMap.count( neuronTypeAggregationKey ) == 0 )
          {
            // std::cout << "+++ Doesnt exist" << std::endl;
            _neuronTypeAggsMap[ neuronTypeAggregationKey ] =
              new NeuronTypeAggregationRep( );
          }
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert(
              _neuronTypeAggsMap.at( neuronTypeAggregationKey ));
          if ( linkRepsToEntities )
            repsToEntities[ _neuronTypeAggsMap.at( neuronTypeAggregationKey ) ].
              insert( entity );

          auto aggTypeAggRep = _neuronTypeAggsMap[ neuronTypeAggregationKey ];
          representations.push_back( aggTypeAggRep );

          gidsToEntitiesReps.insert( TripleKey( entity->entityGid( ), entity, aggTypeAggRep ));
        } // if NeuronTypeAggregationRep


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

    } // create

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
        1, 10 );

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

    void RepresentationCreator::_createColumnOrMiniColumn(
      shift::Entity *entity,
      shift::Representation* rep,
      unsigned int id,
      unsigned int columnOrMiniColumn,
      MapperFloatToFloat& somaAreaToAngle,
      MapperFloatToFloat& dendAreaToAngle,
      scoop::SequentialColorMap& somaVolumeToColor,
      scoop::SequentialColorMap& dendVolumeToColor,
      MapperFloatToFloat& neuronsToPercentage,
      MapperFloatToFloat& layerNeuronsToPercentage,
      RepresentationCreator::LayersMap& layersMap_,
      shift::TEntitiesToReps& // entitiesToReps
      ,
      shift::TRepsToEntities& // repsToEntities
      ,
      bool // linkEntitiesToReps
      ,
      bool // linkRepsToEntities
      )
    {
      NeuronRep meanNeuronRep;

      meanNeuronRep.setProperty( "symbol", NeuronRep::NO_SYMBOL );
      meanNeuronRep.setProperty( "bg", Color( 200, 200, 200 ));

      shiftgen::NeuronAggregationRep::Rings rings;


      // somaVolumeToColor.value( ) =
      //   entity->getProperty( "meanSomaVolume" ).value< float >( );

      shiftgen::Ring somaRing;
      somaRing.setProperty(
        "angle",
        int(
          roundf(
            somaAreaToAngle.map(
              entity->getProperty( "meanSomaArea" ).
              value< float >( )))));
      // somaVolumeToColor.value( ) =
      //   entity->getProperty( "meanSomaVolume" ).value< float >( );
      // somaRing.setProperty( "color", somaVolumeToColor.map( ));
      somaRing.setProperty(
        "color",
        somaVolumeToColor.getColor(
          entity->getProperty( "meanSomaVolume" ).value< float >( )));
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      dendRing.setProperty(
        "angle",
        int(
          roundf(
            dendAreaToAngle.map(
              entity->getProperty( "meanDendArea" ).
              value< float >( )))));
      // dendVolumeToColor.value( ) =
      //   entity->getProperty( "meanDendVolume" ).value< float >( );
      // dendRing.setProperty( "color", dendVolumeToColor.map( ));
      dendRing.setProperty(
        "color",
        dendVolumeToColor.getColor(
          entity->getProperty( "meanDendVolume" ).value< float >( )));
      rings.push_back( dendRing );

      meanNeuronRep.registerProperty( "rings", rings );

      rep->registerProperty( "meanNeuron", meanNeuronRep );

      shiftgen::NeuronAggregationRep::Layers layersReps;

      auto layerRep = new LayerRep;
      layerRep->setProperty(
        "leftPerc",
        roundf(
          neuronsToPercentage.map(
            entity->getProperty( "Num Pyramidals" ).
            value< float >( ))));
      layerRep->setProperty(
        "rightPerc",
        roundf(
          neuronsToPercentage.map(
            entity->getProperty( "Num Interneurons" ).
            value< float >( ))));
      layersReps.push_back( layerRep );


      for ( unsigned int layer = 1; layer <= 6; ++layer )
      {
        auto layerKey = TripleKey(
          id,
          columnOrMiniColumn,
          layer );


        if ( layersMap_.count( layerKey ) == 0 )
        {
          layersMap_[ layerKey ] = new LayerRep( );
          assert( layersMap_.count( layerKey ) == 1);
        }

        layerRep = layersMap_[ layerKey ]; //new LayerRep;

        //layerRep = new LayerRep;
        layerRep->setProperty(
          "leftPerc",
            layerNeuronsToPercentage.map(
              entity->getProperty(
                std::string( "Num Pyr Layer " ) +
                std::to_string( layer )).
              value< float >( )));
        layerRep->setProperty(
          "rightPerc",
            layerNeuronsToPercentage.map(
              entity->getProperty(
                std::string( "Num Inter Layer " ) +
                std::to_string( layer )).
              value< float >( )));
        layersReps.push_back( layerRep );
      }
      rep->registerProperty( "layers", layersReps );

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
          auto neuronTypeAggKey = QuadKey(
            id, columnOrMiniColumn, layer, uint( neuronType ));

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
      rep->registerProperty( "neuronTypeAggregations", neuronTypeAggsReps );
    }


  } // namespace cortex
} // namespace nslib
