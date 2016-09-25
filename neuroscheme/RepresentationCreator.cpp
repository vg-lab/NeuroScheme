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
#include "entities/Neuron.h"
#include "reps/NeuronRep.h"
#include "reps/ColumnRep.h"
#include "reps/MiniColumnRep.h"
#include "mappers/ColorMapper.h"
#include "mappers/VariableMapper.h"

namespace neuroscheme
{

  void RepresentationCreator::create(
    const shift::Entities& entities,
    shift::Representations& representations,
    TEntitiesToReps& entitiesToReps,
    TRepsToEntities& repsToEntities,
    bool linkEntitiesToReps,
    bool linkRepsToEntities
    )
  {

    if ( linkEntitiesToReps )
      entitiesToReps.clear( );
    if ( linkRepsToEntities )
      repsToEntities.clear( );

    auto _greenMapper = new DiscreteColorMapper( );
    auto _redMapper = new DiscreteColorMapper( );

    _greenMapper->push_back( Color( 0.62 * 255, 1.00 * 255, 0.75 * 255 ));
    _greenMapper->push_back( Color( 0.55 * 255, 0.88 * 255, 0.65 * 255 ));
    _greenMapper->push_back( Color( 0.46 * 255, 0.75 * 255, 0.56 * 255 ));
    _greenMapper->push_back( Color( 0.40 * 255, 0.63 * 255, 0.47 * 255 ));
    _greenMapper->push_back( Color( 0.31 * 255, 0.51 * 255, 0.37 * 255 ));
    _greenMapper->push_back( Color( 0.22 * 255, 0.38 * 255, 0.27 * 255 ));

    _redMapper->push_back( Color( 1.00 * 255, 0.62 * 255, 0.75 * 255 ));
    _redMapper->push_back( Color( 0.88 * 255, 0.55 * 255, 0.65 * 255 ));
    _redMapper->push_back( Color( 0.75 * 255, 0.46 * 255, 0.56 * 255 ));
    _redMapper->push_back( Color( 0.63 * 255, 0.40 * 255, 0.47 * 255 ));
    _redMapper->push_back( Color( 0.51 * 255, 0.31 * 255, 0.37 * 255 ));
    _redMapper->push_back( Color( 0.38 * 255, 0.22 * 255, 0.27 * 255 ));

    _greenMapper->max( ) = 100.0f;
    _redMapper->max( ) = 100.0f;

    const float maxNeuronSomaArea = 100.0f;
    const float maxNeuronDendArea = 100.0f;
    auto _somaAreaToAngle =
      new MapperFloatToFloat( 0, maxNeuronSomaArea, 0.0f, -360.0f);
    auto _dendAreaToAngle =
      new MapperFloatToFloat( 0, maxNeuronDendArea, 0.0f, -360.0f );

    const float maxNeurons = 100.0f;
    auto _neuronsToPercentage =
      new MapperFloatToFloat( 0, maxNeurons, 0.0f, 1.0f );


    for ( const auto entity : entities )
    {
      if ( dynamic_cast< Neuron* >( entity ))
      {
        auto neuron = dynamic_cast< Neuron* >( entity );
        auto neuronRep = new NeuronRep( );

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
          neuron->getProperty( "funcType" ).value< Neuron::TFunctionalType >( ))
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
              _somaAreaToAngle->map(
                neuron->getProperty( "somaVolume" ).value< float >( )))));
        _greenMapper->value( ) =
          neuron->getProperty( "somaArea" ).value< float >( );
        somaRing.setProperty( "color", _greenMapper->map( ));
        rings.push_back( somaRing );

        shiftgen::Ring dendRing;
        dendRing.setProperty(
          "angle",
          int(
            roundf(
              _dendAreaToAngle->map(
                neuron->getProperty( "dendVolume" ).value< float >( )))));
        _redMapper->value( ) =
          neuron->getProperty( "dendArea" ).value< float >( );
        dendRing.setProperty( "color", _redMapper->map( ));
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
                                   *_somaAreaToAngle,
                                   *_dendAreaToAngle,
                                   *_greenMapper,
                                   *_redMapper,
                                   *_neuronsToPercentage );
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
                                   *_somaAreaToAngle,
                                   *_dendAreaToAngle,
                                   *_greenMapper,
                                   *_redMapper,
                                   *_neuronsToPercentage );
        representations.push_back( miniColumnRep );

        if ( linkEntitiesToReps )
          entitiesToReps[ entity ].insert( miniColumnRep );
        if ( linkRepsToEntities )
          repsToEntities[ miniColumnRep ].insert( entity );

      } // it its Column entity
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

    shiftgen::Ring somaRing;
    somaRing.setProperty(
      "angle",
      int(
        roundf(
          somaAreaToAngle.map(
            entity->getProperty( "meanSomaVolume" ).
            value< float >( )))));
    somaVolumeToColor.value( ) =
      entity->getProperty( "meanSomaArea" ).value< float >( );
    somaRing.setProperty( "color", dendVolumeToColor.map( ));
    rings.push_back( somaRing );

    shiftgen::Ring dendRing;
    dendRing.setProperty(
      "angle",
      int(
        roundf(
          dendAreaToAngle.map(
            entity->getProperty( "meanDendVolume" ).
            value< float >( )))));
    dendVolumeToColor.value( ) =
      entity->getProperty( "meanDendArea" ).value< float >( );
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


} // namespace neuroscheme
