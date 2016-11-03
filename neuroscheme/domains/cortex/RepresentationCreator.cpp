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
#include "RepresentationCreator.h"
#include "mappers/ColorMapper.h"
#include "mappers/VariableMapper.h"
#include "domains/domains.h"
#include "error.h"
#include "../../DataManager.h"

namespace neuroscheme
{

  namespace cortex
  {

    void RepresentationCreator::create(
      const shift::Entities& entities,
      shift::Representations& representations,
      shift::TEntitiesToReps& entitiesToReps,
      shift::TRepsToEntities& repsToEntities,
      bool linkEntitiesToReps,
      bool linkRepsToEntities
      )
    {

      // if ( linkEntitiesToReps )
      //   entitiesToReps.clear( );
      // if ( linkRepsToEntities )
      //   repsToEntities.clear( );

      DiscreteColorMapper greenMapper;
      DiscreteColorMapper redMapper;

#define ColorF( r, g, b )                             \
      Color( uint8_t( roundf( r * 255 )),             \
             uint8_t( roundf( g * 255 )),             \
             uint8_t( roundf( b * 255 )))
      greenMapper.push_back( ColorF( 0.62f, 1.00f, 0.75f ));
      greenMapper.push_back( ColorF( 0.55f, 0.88f, 0.65f ));
      greenMapper.push_back( ColorF( 0.46f, 0.75f, 0.56f ));
      greenMapper.push_back( ColorF( 0.40f, 0.63f, 0.47f ));
      greenMapper.push_back( ColorF( 0.31f, 0.51f, 0.37f ));
      greenMapper.push_back( ColorF( 0.22f, 0.38f, 0.27f ));

      redMapper.push_back( ColorF( 1.00f, 0.62f, 0.75f ));
      redMapper.push_back( ColorF( 0.88f, 0.55f, 0.65f ));
      redMapper.push_back( ColorF( 0.75f, 0.46f, 0.56f ));
      redMapper.push_back( ColorF( 0.63f, 0.40f, 0.47f ));
      redMapper.push_back( ColorF( 0.51f, 0.31f, 0.37f ));
      redMapper.push_back( ColorF( 0.38f, 0.22f, 0.27f ));

      greenMapper.max( ) =
        _maxNeuronSomaVolume == 0 ? 0.1f : _maxNeuronSomaVolume;
      redMapper.max( ) =
        _maxNeuronDendsVolume == 0 ? 0.1f : _maxNeuronDendsVolume;

      MapperFloatToFloat somaAreaToAngle(
        0, _maxNeuronSomaArea == 0 ? 0.1f : _maxNeuronSomaArea, 0, -360 );
      MapperFloatToFloat dendAreaToAngle(
        0, _maxNeuronDendsArea == 0 ? 0.1f : _maxNeuronDendsArea, 0, -360 );

      // std::cout << "--------------------" << _maxNeurons << std::endl;
      // std::cout << "max neurons per layer " <<  " " << _maxNeuronsPerColumn
      //           << " " << _maxNeuronsPerMiniColumn << std::endl;
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
            neuronRep->setProperty( "symbol", NeuronRep::TRIANGLE );
            break;
          case Neuron::PYRAMIDAL:
            neuronRep->setProperty( "symbol", NeuronRep::CIRCLE );
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
          greenMapper.value( ) =
            neuron->getProperty( "Soma Volume" ).value< float >( );
          somaRing.setProperty( "color", greenMapper.map( ));
          rings.push_back( somaRing );

          shiftgen::Ring dendRing;
          dendRing.setProperty(
            "angle",
            int(
              roundf(
                dendAreaToAngle.map(
                  neuron->getProperty(
                    "Dendritic Surface" ).value< float >( )))));
          redMapper.value( ) =
            neuron->getProperty( "Dendritic Volume" ).value< float >( );
          dendRing.setProperty( "color", redMapper.map( ));
          rings.push_back( dendRing );


          neuronRep->setProperty( "rings", rings );

          representations.push_back( neuronRep );

          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( neuronRep );
          if ( linkRepsToEntities )
            repsToEntities[ neuronRep ].insert( entity );
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
            // std::cout << ", Creating layer "
            //           << entity->getProperty( "Parent Id" ).value< uint >( )
            //           << ", "
            //           << entity->getProperty( "Parent Type" ).value< uint >( )
            //           << ", "
            //           << entity->getProperty( "Layer" ).value< uint >( )
            //           << std::endl;

            _layersMap[ layerKey ] = new LayerRep( );
          }
          // std::cout << "Linking " << _layersMap.at( layerKey )
          //           << " to " << entity << std::endl;
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( _layersMap.at( layerKey ));
          if ( linkRepsToEntities )
            repsToEntities[ _layersMap.at( layerKey ) ].insert( entity );
          representations.push_back( _layersMap[ layerKey ] );
        } // if Layer


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
            subEntities.add( DataManager::entities( ).at( subEntity ));
      }
      if ( subEntities.size( ) > 0 )
        this->create( subEntities, representations,
                      entitiesToReps, repsToEntities,
                      linkEntitiesToReps, linkRepsToEntities );
    } // create

    void RepresentationCreator::_createColumnOrMiniColumn(
      shift::Entity *entity,
      shift::Representation* rep,
      unsigned int id,
      unsigned int columnOrMiniColumn,
      MapperFloatToFloat& somaAreaToAngle,
      MapperFloatToFloat& dendAreaToAngle,
      ColorMapper& somaVolumeToColor,
      ColorMapper& dendVolumeToColor,
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


      somaVolumeToColor.value( ) =
        entity->getProperty( "meanSomaVolume" ).value< float >( );

      shiftgen::Ring somaRing;
      somaRing.setProperty(
        "angle",
        int(
          roundf(
            somaAreaToAngle.map(
              entity->getProperty( "meanSomaArea" ).
              value< float >( )))));
      somaVolumeToColor.value( ) =
        entity->getProperty( "meanSomaVolume" ).value< float >( );
      somaRing.setProperty( "color", somaVolumeToColor.map( ));
      rings.push_back( somaRing );

      shiftgen::Ring dendRing;
      dendRing.setProperty(
        "angle",
        int(
          roundf(
            dendAreaToAngle.map(
              entity->getProperty( "meanDendArea" ).
              value< float >( )))));
      dendVolumeToColor.value( ) =
        entity->getProperty( "meanDendVolume" ).value< float >( );
      dendRing.setProperty( "color", dendVolumeToColor.map( ));
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
        // std::cout << ", Creating layer "
        //           << id
        //           << ", "
        //           << columnOrMiniColumn
        //           << ", "
        //           << layer
        //           << std::endl;

          layersMap_[ layerKey ] = new LayerRep( );
          assert( layersMap_.count( layerKey ) == 1);
          // if ( linkEntitiesToReps )
          //   entitiesToReps[ entity ].insert( _layersMap.at( layerKey ));
          // if ( linkRepsToEntities )
          //   repsToEntities[ _layersMap.at( layerKey ) ].insert( entity );
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

      // std::cout << ", End of _createColumnOrMiniColumn LayersMap size = "
      //           << layersMap_.size( ) << std::endl;
    }


  } // namespace cortex
} // namespace neuroscheme
