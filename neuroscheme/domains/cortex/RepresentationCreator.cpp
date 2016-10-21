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
#include "mappers/ColorMapper.h"
#include "mappers/VariableMapper.h"
#include "domains/domains.h"

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

      std::cout << "--------------------" << _maxNeurons << std::endl;
      MapperFloatToFloat neuronsToPercentage( 0, _maxNeurons, 0.0f, 1.0f );


      for ( const auto entityPair : entities )
      {
        auto& entity = entityPair.second;
        if ( dynamic_cast< Neuron* >( entity ))
        {
          auto neuron = dynamic_cast< Neuron* >( entity );
          auto neuronRep = new NeuronRep( );

          std::cout
            << fires::PropertyManager::getPropertyCaster(
              "morphoType" )->toString(
              neuron->getProperty( "morphoType" )) << std::endl;
          switch ( neuron->getProperty( "morphoType" ).
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
              "funcType" ).value< Neuron::TFunctionalType >( ))
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
                  neuron->getProperty( "somaArea" ).value< float >( )))));
          greenMapper.value( ) =
            neuron->getProperty( "somaVolume" ).value< float >( );
          somaRing.setProperty( "color", greenMapper.map( ));
          rings.push_back( somaRing );

          shiftgen::Ring dendRing;
          dendRing.setProperty(
            "angle",
            int(
              roundf(
                dendAreaToAngle.map(
                  neuron->getProperty( "dendArea" ).value< float >( )))));
          redMapper.value( ) =
            neuron->getProperty( "dendVolume" ).value< float >( );
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

        if ( dynamic_cast< Column* >( entity ))
        {
          auto column = dynamic_cast< Column* >( entity );
          auto columnRep = new ColumnRep( );
          _CreateColumnOrMiniColumn( column, columnRep,
                                     somaAreaToAngle,
                                     dendAreaToAngle,
                                     greenMapper,
                                     redMapper,
                                     neuronsToPercentage );
          representations.push_back( columnRep );

          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( columnRep );
          if ( linkRepsToEntities )
            repsToEntities[ columnRep ].insert( entity );

        } // it its MiniColumn entity
        if ( dynamic_cast< MiniColumn* >( entity ))
        {
          std::cout << "creating minicolumn rep" << std::endl;
          auto miniColumn = dynamic_cast< MiniColumn* >( entity );
          auto miniColumnRep = new MiniColumnRep( );
          _CreateColumnOrMiniColumn( miniColumn, miniColumnRep,
                                     somaAreaToAngle,
                                     dendAreaToAngle,
                                     greenMapper,
                                     redMapper,
                                     neuronsToPercentage );
          representations.push_back( miniColumnRep );

          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( miniColumnRep );
          if ( linkRepsToEntities )
            repsToEntities[ miniColumnRep ].insert( entity );

        } // if its Column entity
      } // for all entities
    } // create

    void RepresentationCreator::_CreateColumnOrMiniColumn(
      shift::Entity *entity,
      shift::Representation* rep,
      MapperFloatToFloat& somaAreaToAngle,
      MapperFloatToFloat& dendAreaToAngle,
      ColorMapper& somaVolumeToColor,
      ColorMapper& dendVolumeToColor,
      MapperFloatToFloat& neuronsToPercentage )
    {
      NeuronRep meanNeuronRep;

      meanNeuronRep.setProperty( "symbol", NeuronRep::NO_SYMBOL );
      meanNeuronRep.setProperty( "bg", Color( 200, 200, 200 ));

      shiftgen::NeuronAggregationRep::Rings rings;


      somaVolumeToColor.value( ) =
        entity->getProperty( "meanSomaVolume" ).value< float >( );
      std::cout << "!!!!"
                << entity->getProperty( "meanSomaVolume" ).value< float >( )
                << "-->"
                << somaVolumeToColor.map( )
                << std::endl;

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
      shiftgen::LayerRep layerRep;

      (void) neuronsToPercentage;
      layerRep.setProperty(
        "leftPerc",
        roundf(
          neuronsToPercentage.map(
            entity->getProperty( "Num Pyramidals" ).
            value< float >( ))));
      layerRep.setProperty(
        "rightPerc",
        roundf(
          neuronsToPercentage.map(
            entity->getProperty( "Num Interneurons" ).
            value< float >( ))));
      layersReps.push_back( layerRep );

      for ( unsigned int layer = 1; layer <= 6; ++layer )
      {
        layerRep.setProperty(
          "leftPerc",
          roundf(
            neuronsToPercentage.map(
              entity->getProperty(
                std::string( "Num Pyr Layer " ) +
                std::to_string( layer )).
              value< float >( ))));
        layerRep.setProperty(
          "rightPerc",
          roundf(
            neuronsToPercentage.map(
              entity->getProperty(
                std::string( "Num Inter Layer " ) +
                std::to_string( layer )).
              value< float >( ))));
        layersReps.push_back( layerRep );
      }
      rep->registerProperty( "layers", layersReps );

    }


  } // namespace cortex
} // namespace neuroscheme
