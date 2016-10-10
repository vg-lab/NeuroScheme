#include "DataManager.h"
#include "RepresentationCreatorManager.h"

namespace neuroscheme
{
  shift::EntitiesWithRelationships DataManager::_entities =
    shift::EntitiesWithRelationships( );

  shift::Representations DataManager::_representations =
    shift::Representations( );;

  shift::EntitiesWithRelationships& DataManager::entities( void )
  {
    return _entities;
  }

  shift::Representations& DataManager::representations( void )
  {
    return _representations;
  }

  void DataManager::loadData( void )
    {
      //Temp function for testing
      // auto relHierarchyChild = ;
      _entities.relationships( )[ "isParentOf" ] =
        new shift::RelationshipOneToN;

      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne;

      shift::Entity* entity =
        new neuroscheme::Column(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added column gid " << entity->entityGid( ) << std::endl;
      auto colId1 = entity->entityGid( );

      entity =
        new neuroscheme::Column(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added column gid " << entity->entityGid( ) << std::endl;
      auto colId2 = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol0Id = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol1Id = entity->entityGid( );

            entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol2Id = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol3Id = entity->entityGid( );

      auto& relParentOf =
        *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      auto& relChildOf =
        *( _entities.relationships( )[ "isChildOf" ]->asOneToOne( ));
      relParentOf[ 0 ].insert( colId1 );
      relParentOf[ 0 ].insert( colId2 );
      relChildOf[ colId1 ] = 0;
      relChildOf[ colId2 ] = 0;

      relParentOf[ colId1 ].insert( miniCol0Id );
      relParentOf[ colId1 ].insert( miniCol1Id );
      relChildOf[ miniCol0Id ] = colId1;
      relChildOf[ miniCol1Id ] = colId1;

      relParentOf[ colId2 ].insert( miniCol2Id );
      relParentOf[ colId2 ].insert( miniCol3Id );
      relChildOf[ miniCol2Id ] = colId2;
      relChildOf[ miniCol3Id ] = colId2;

      unsigned int neuronGid = 0;

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol0Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol0Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol0Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol0Id;

      }

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol1Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol1Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol1Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol1Id;

      }
      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol2Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol2Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol2Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol2Id;

      }

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol3Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol3Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol3Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol3Id;

      }

      // Display root reps
      shift::Entities rootEntities;
      // auto& relParentOf =
      //   *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      // auto& relParentOf = *( neuroscheme::DataManager::entities( ).
      //                        relationships( )[ "isParentOf" ]->asOneToN( ));

      const auto& childrenIds = relParentOf[ 0 ];
      std::cout << "-- Root entities " << childrenIds.size( ) << std::endl;
      for ( const auto& child : childrenIds )
        rootEntities[child] =
          neuroscheme::DataManager::entities( )[child];

      neuroscheme::RepresentationCreatorManager::addCreator(
        new neuroscheme::cortex::RepresentationCreator );
      neuroscheme::RepresentationCreatorManager::create(
        rootEntities, _representations,
        true, true );

    } // loadData
} // namespace neuroscheme
