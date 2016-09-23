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

  typedef std::unordered_map< shift::Object*,
                              std::set< shift::Representation* >>
  TObjectsToReps;
  typedef std::unordered_map< shift::Representation*,
                              std::set< shift::Object* >>
  TRepsToObjects;

  class RepresentationCreator //: shift::RepresentationCreator
  {
  public:
    virtual ~RepresentationCreator( void ) {};

    void create(
      const shift::Objects& objects,
      shift::Representations& representations,
      TObjectsToReps& objectsToReps,
      TRepsToObjects& repsToObjects,
      bool linkObjectsToReps = false,
      bool linkRepsToObjs = false );


  protected:
    void _CreateColumnOrMiniColumn(
      shift::Object *obj,
      shift::Representation* rep,
      MapperFloatToFloat& somaAreaToAngle,
      MapperFloatToFloat& dendAreaToAngle,
      ColorMapper& somaVolumeToColor,
      ColorMapper& dendVolumeToColor,
      MapperFloatToFloat& neuronsToPercentage );
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
  //   static void create( const shift::Objects& objects,
  //                       shift::Representations& representations,
  //                       // TObjectsToReps& objectsToReps,
  //                       // TRepsToObjects& repsToObjects,
  //                       bool linkObjectsToReps = false,
  //                       bool linkRepsToObjs = false,
  //                       unsigned int repCreatorId = 0 )
  //   {
  //     //TODO check if exists
  //     _repCreators[ repCreatorId ]->create( objects, representations,
  //                                           _objectsToReps[ repCreatorId ],
  //                                           _repsToObjects[ repCreatorId ],
  //                                           linkObjectsToReps,
  //                                           linkRepsToObjs );
  //   }

  //   const TObjectsToReps& objectsToReps( repCreatorId = 0 )
  //   {
  //     return _objectsToReps[ repCreatorId ];
  //   }
  //   const TRepsToObjects& repsToObjects( repCreatorId = 0 )
  //   {
  //     return _repsToObjects[ repCreatorId ];
  //   }

  // protected:
  //   static std::map< unsigned int, RepresentationCreator* > _repCreators;
  //   static std::map< unsigned int, TObjectsToReps> _objectsToReps;
  //   static std::map< unsigned int, TRepsToObjects> _repsToObjects;

  // };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
