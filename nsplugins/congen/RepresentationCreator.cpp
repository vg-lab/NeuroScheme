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
#include "RepresentationCreator.h"
#include <shift_NeuronPop.h>
#include "NeuronPopRep.h"

namespace nslib
{
  namespace congen
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
      nslib::Log::log( "congen::Representationcreator::create",
                       LOG_LEVEL_VERBOSE );

      scoop::SequentialColorMap neuronTypeColorMapper(
        scoop::ColorPalette::colorBrewerDiverging(
          scoop::ColorPalette::ColorBrewerDiverging::RdYlGn, 10 ),
        0.0f, 100.0f );

      for ( const auto entity : entities.vector( ))
      {
        if ( entitiesToReps.find( entity ) != entitiesToReps.end( ))
        {
          // std::cout << "Entity with already a rep" << std::endl;
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
            neuronTypeColorMapper.getColor(
              neuronPop->getProperty( "Perc Inhibitory" ).value< float >( )));

          representations.push_back( neuronPopRep );
          // Link entity and rep
          if ( linkEntitiesToReps )
            entitiesToReps[ entity ].insert( neuronPopRep );
          if ( linkRepsToEntities )
            repsToEntities[ neuronPopRep ].insert( entity );

        }
      }
    }

  } // namespace congen
} // namespace nslib
