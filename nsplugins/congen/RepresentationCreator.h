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
#ifndef __NSPLUGINS_CONGEN__REPRESENTATION_CREATOR__
#define __NSPLUGINS_CONGEN__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include <nslib/mappers/VariableMapper.h>
#include <scoop/scoop.h>
#include <shift_NeuronPop.h>
#include <shift_Input.h>
#include <shift_Output.h>

#include <unordered_map>
#include <set>
#include <nslibcongen/api.h>

namespace nslib
{
  namespace congen
  {

    class NSLIBCONGEN_API RepresentationCreator
        : public shift::RepresentationCreator
    {
    public:

      RepresentationCreator( void );

      virtual ~RepresentationCreator( void ) {};

      void updateRepresentation(
          const shift::Entity* entity_,
          shift::Representation* entityRep_
      ) final;

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        shift::TGidToEntitiesReps& gidsToEntitiesReps,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false ) final;

        void maxAbsoluteWeight(
          float maxAbsoluteWeight_, bool compare = false );
        void maxNeuronsPerPopulation(
          unsigned int maxNeuronsPerPopulation_, bool compare = false ) ;
        float maxAbsoluteWeight( void ) const;
        unsigned int maxNeuronsPerPopulation( void ) const;

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        shift::RelationshipOneToN* relations ) override;

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        shift::RelationshipAggregatedOneToN* relatedElements ) override ;

      void reset( void ) final;

#define TripleKey( x, y, z ) std::make_pair( x, std::make_pair( y, z ))

      bool entityUpdatedOrCreated( const shift::Entity* entity ) final;
      bool relationshipUpdatedOrCreated(
        const shift::RelationshipProperties* relProperties ) final;

      unsigned int maxLevelsPerSuperPop( void ) const;

      void maxLevelsPerSuperPop( unsigned int _maxLevelsPerSuperPop,
        bool compare = false );

      protected:
      unsigned int _maxNeuronsPerPopulation;
      unsigned int _maxLevelsPerSuperPop;
      float _maxAbsoluteWeight;

      float _superPopSeparation;
      float _superPopLevelSeparation;

      MapperFloatToFloat _nbConnectionsToWidth;
      MapperFloatToFloat _neuronsToPercentage;
      scoop::CategoricalColorMap< shiftgen::NeuronPop::TNeuronModel >
        _neuronModelColorMap;
      scoop::CategoricalColorMap< shiftgen::Input::TInputType >
        _inputModelColorMap;
      scoop::CategoricalColorMap< shiftgen::Output::TOutputModel >
        _outputModelColorMap;
      scoop::SequentialColorMap _superPopLevelColorMap;
      scoop::Color _superPopColor;

      void updateNeuronPopRep( const shift::Entity* entity_,
        shift::Representation* entityRep_ );

      void updateSuperPopRep( const shift::Entity* entity_,
        shift::Representation* entityRep_ );

      void updateInputRep( const shift::Entity* entity_,
        shift::Representation* entityRep_ );

      void updateOutputRep( const shift::Entity* entity_,
        shift::Representation* entityRep_ );
};

  } // namespace congen
} // namespace nslib

#endif //__NSPLUGINS_CONGEN__REPRESENTATION_CREATOR__
