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
#ifndef __NSPLUGINS_CORTEX__REPRESENTATION_CREATOR__
#define __NSPLUGINS_CORTEX__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include <nslib/mappers/VariableMapper.h>
#include <scoop/scoop.h>
#include <nslibcortex/api.h>

#include <unordered_map>
#include <set>

namespace nslib
{
  namespace cortex
  {

    class LayerRep;
    class NeuronTypeAggregationRep;

    class NSLIBCORTEX_API RepresentationCreator
        : public shift::RepresentationCreator
    {
    public:
      RepresentationCreator( void );

      virtual ~RepresentationCreator( void ) {};

      void updateRepresentation(
          const shift::Entity* entity,
          shift::Representation* representation
      ) final;

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        shift::TGidToEntitiesReps& gidsToEntitiesReps,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false ) final;

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        shift::RelationshipAggregatedOneToN* relatedElements ) final;

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        shift::RelationshipOneToN* relatedElements ) final;

      void setMaximums(
        float maxNeuronSomaVolume_,
        float maxNeuronSomaArea_,
        float maxNeuronDendsVolume_,
        float maxNeuronDendsArea_,
        unsigned int maxNeurons_,
        unsigned int maxNeuronsPerColumn_,
        unsigned int maxNeuronsPerMiniColumn_,
        unsigned int maxConnectionsPerEntity_ );

      virtual void clear( void ) final;
      virtual void reset( void ) final;

      bool entityUpdatedOrCreated( const shift::Entity* entity ) final;

      float maxNeuronSomaVolume( void ) const;

      float maxNeuronSomaArea( void ) const;

      float maxNeuronDendsVolume( void ) const;

      float maxNeuronDendsArea( void ) const;

      unsigned int maxNeurons( void ) const;

      unsigned int maxNeuronsPerColumn( void ) const;

      unsigned int maxNeuronsPerMiniColumn( void ) const;

      unsigned int maxConnectionsPerEntity( void ) const;

      void maxNeuronSomaVolume(
        float maxNeuronSomaVolume_, bool compare = false );

      void maxNeuronSomaArea(
        float maxNeuronSomaArea_, bool compare = false );

      void maxNeuronDendsVolume(
        float maxNeuronDendsVolume_, bool compare = false );

      void maxNeuronDendsArea(
        float maxNeuronDendsArea_, bool compare = false );

      void maxNeurons( unsigned int maxNeurons_, bool compare = false );

      void maxNeuronsPerColumn(
        unsigned int maxNeuronsPerColumn_, bool compare = false );

      void maxNeuronsPerMiniColumn(
        unsigned int maxNeuronsPerMiniColumn_, bool compare = false );

      void maxConnectionsPerEntity(
        unsigned int maxConnectionsPerEntity_, bool compare = false );

    protected:
#define TripleKey( x, y, z ) std::make_pair( x, std::make_pair( y, z ))

#define QuadKey( x, y, z, w )                                         \
      std::make_pair( x, std::make_pair( y, std::make_pair( z, w )))

#define PentaKey( a, b, c, d, e )                                       \
      std::make_pair( a, std::make_pair( b, std::make_pair(             \
                                         c, std::make_pair( d, e ))))

      typedef std::pair< shift::EntityGid, std::pair< unsigned int,
        std::pair< unsigned int, unsigned int>>>
        LayersMapKey;

      typedef std::map< std::pair< shift::EntityGid, std::pair< unsigned int,
        std::pair< unsigned int, unsigned int>>>, LayerRep* > LayersMap;

      typedef std::pair< shift::EntityGid, std::pair< unsigned int,
        std::pair< unsigned int, std::pair< unsigned int,  unsigned int >>>>
        NeuronTypeAggsMapKey;

      typedef std::map< NeuronTypeAggsMapKey,
        NeuronTypeAggregationRep* > NeuronTypeAggsMaps;

      void updateColumnOrMiniColumnRep(
        const shift::Entity* entity_,
        shift::Representation* entityRep_,
        unsigned int columnOrMiniColumn );

      void updateNeuronRep( const shift::Entity* entity_,
        shift::Representation* entityRep_ );

      shift::Representation*
        getNeuronTypeAggregationRep( const shift::Entity* entity_ );

      shift::Representation* getLayerRep( const shift::Entity* entity_ );

      float _maxNeuronSomaVolume;
      float _maxNeuronSomaArea;
      float _maxNeuronDendsVolume;
      float _maxNeuronDendsArea;
      unsigned int _maxNeurons;
      unsigned int _maxNeuronsPerColumn;
      unsigned int _maxNeuronsPerMiniColumn;
      unsigned int _maxConnectionsPerEntity;
      LayersMap _layersMap;
      NeuronTypeAggsMaps _neuronTypeAggsMap;

      scoop::SequentialColorMap _greenMapper;
      scoop::SequentialColorMap _redMapper;
      MapperFloatToFloat _somaAreaToAngle;
      MapperFloatToFloat _dendAreaToAngle;
      MapperFloatToFloat _neuronsToPercentage;
      MapperFloatToFloat _columnNeuronsToPercentage;
      MapperFloatToFloat _miniColumnNeuronsToPercentage;
      MapperFloatToFloat _nbConnectionsToWidth;
    };

  } // namespace cortex
} // namespace nslib

#endif // __NSPLUGINS_CORTEX__REPRESENTATION_CREATOR__
