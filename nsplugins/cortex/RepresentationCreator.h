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
#ifndef __NSLIB__REPRESENTATION_CREATOR__
#define __NSLIB__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include <nslib/mappers/VariableMapper.h>
#include <scoop/scoop.h>

#include <unordered_map>
#include <set>

namespace nslib
{
  namespace cortex
  {

    class LayerRep;
    class NeuronTypeAggregationRep;

    class RepresentationCreator : public shift::RepresentationCreator
    {
    public:
      virtual ~RepresentationCreator( void ) {};

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        shift::TGidToEntitiesReps& gidsToEntitiesReps,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false );

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        const std::string& relationName );

      void setMaximums( float maxNeuronSomaVolume_,
                        float maxNeuronSomaArea_,
                        float maxNeuronDendsVolume_,
                        float maxNeuronDendsArea_,
                        unsigned int maxNeurons_,
                        unsigned int maxNeuronsPerColumn_,
                        unsigned int maxNeuronsPerMiniColumn_,
                        unsigned int maxConnectionsPerEntity_ )
      {
        _maxNeuronSomaVolume = maxNeuronSomaVolume_;
        _maxNeuronSomaArea = maxNeuronSomaArea_;
        _maxNeuronDendsVolume = maxNeuronDendsVolume_;
        _maxNeuronDendsArea = maxNeuronDendsArea_;
        _maxNeurons = maxNeurons_;
        _maxNeuronsPerColumn = maxNeuronsPerColumn_;
        _maxNeuronsPerMiniColumn = maxNeuronsPerMiniColumn_;
        _maxConnectionsPerEntity = maxConnectionsPerEntity_;
      }

      virtual void clear( void ) final
      {
        _layersMap.clear( );
        _neuronTypeAggsMap.clear( );
      }

      void entityUpdatedOrCreated( shift::Entity* entity ) override;

    protected:
#define TripleKey( x, y, z ) std::make_pair( x, std::make_pair( y, z ))
      typedef std::map<
        std::pair< shift::Entity::EntityGid,
                   std::pair< unsigned int,
                              std::pair< unsigned int, unsigned int>>>,
      LayerRep* > LayersMap;

#define QuadKey( x, y, z, w )                                         \
      std::make_pair( x, std::make_pair( y, std::make_pair( z, w )))
#define PentaKey( a, b, c, d, e )                                       \
      std::make_pair( a, std::make_pair( b, std::make_pair(             \
                                         c, std::make_pair( d, e ))))

      typedef std::map<
        std::pair< shift::Entity::EntityGid,
                   std::pair< unsigned int,
                              std::pair< unsigned int,
                                         std::pair< unsigned int,
                                                    unsigned int >>>>,
        NeuronTypeAggregationRep* > NeuronTypeAggsMaps;

      void _createColumnOrMiniColumn(
        shift::Entity *obj,
        shift::Representation* rep,
        shift::Entity::EntityGid entityGid,
        unsigned int id,
        unsigned int columnOrMiniColumn,
        MapperFloatToFloat& somaAreaToAngle,
        MapperFloatToFloat& dendAreaToAngle,
        scoop::SequentialColorMap& somaVolumeToColor,
        scoop::SequentialColorMap& dendVolumeToColor,
        MapperFloatToFloat& neuronsToPercentage,
        MapperFloatToFloat& layerNeuronsToPercentage,
        LayersMap& layersMap,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        bool linkEntitiesToReps,
        bool linkRepsToEntities );

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
    };

  } // namespace cortex
} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__
