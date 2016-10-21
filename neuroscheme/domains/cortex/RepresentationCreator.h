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
#ifndef __NEUROSCHEME__REPRESENTATION_CREATOR__
#define __NEUROSCHEME__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include "mappers/VariableMapper.h"
#include "mappers/ColorMapper.h"

#include <unordered_map>
#include <set>

namespace neuroscheme
{

  namespace cortex
  {

    class RepresentationCreator : public shift::RepresentationCreator
    {
    public:
      virtual ~RepresentationCreator( void ) {};

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false );


      void setMaximums( float maxNeuronSomaVolume_,
                        float maxNeuronSomaArea_,
                        float maxNeuronDendsVolume_,
                        float maxNeuronDendsArea_,
                        unsigned int maxNeurons_ )
      {
        _maxNeuronSomaVolume = maxNeuronSomaVolume_;
        _maxNeuronSomaArea = maxNeuronSomaArea_;
        _maxNeuronDendsVolume = maxNeuronDendsVolume_;
        _maxNeuronDendsArea = maxNeuronDendsArea_;
        _maxNeurons = maxNeurons_;
      }

    protected:
      void _CreateColumnOrMiniColumn(
        shift::Entity *obj,
        shift::Representation* rep,
        MapperFloatToFloat& somaAreaToAngle,
        MapperFloatToFloat& dendAreaToAngle,
        ColorMapper& somaVolumeToColor,
        ColorMapper& dendVolumeToColor,
        MapperFloatToFloat& neuronsToPercentage );

      float _maxNeuronSomaVolume;
      float _maxNeuronSomaArea;
      float _maxNeuronDendsVolume;
      float _maxNeuronDendsArea;
      unsigned int _maxNeurons;
    };

    // class RepresentationCreatorManager
    // {
    // public:
    //   static void addCreator( RepresentationCreator* repCreator,
    //                           unsigned int repCreatorId = 0 )
    //   {
    //     //TODO check if exists
    //     _repCreators[ repCreatorId ] = repCreator;
    //   }
    //   static void create( const shift::Entities& entities,
    //                       shift::Representations& representations,
    //                       // TEntitiesToReps& entitiesToReps,
    //                       // TRepsToEntities& repsToEntities,
    //                       bool linkEntitiesToReps = false,
    //                       bool linkRepsToObjs = false,
    //                       unsigned int repCreatorId = 0 )
    //   {
    //     //TODO check if exists
    //     _repCreators[ repCreatorId ]->create( entities, representations,
    //                                           _entitiesToReps[ repCreatorId ],
    //                                           _repsToEntities[ repCreatorId ],
    //                                           linkEntitiesToReps,
    //                                           linkRepsToObjs );
    //   }

    //   const TEntitiesToReps& entitiesToReps( repCreatorId = 0 )
    //   {
    //     return _entitiesToReps[ repCreatorId ];
    //   }
    //   const TRepsToEntities& repsToEntities( repCreatorId = 0 )
    //   {
    //     return _repsToEntities[ repCreatorId ];
    //   }

    // protected:
    //   static std::map< unsigned int, RepresentationCreator* > _repCreators;
    //   static std::map< unsigned int, TEntitiesToReps> _entitiesToReps;
    //   static std::map< unsigned int, TRepsToEntities> _repsToEntities;

    // };

  } // namespace cortex
} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
