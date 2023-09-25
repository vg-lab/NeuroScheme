/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *          Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <nslib/reps/QGraphicsItemRepresentation.h>
#include "Domain.h"
#include "DataManager.h"
#include "PaneManager.h"
#include "Loggers.h"
#include "RepresentationCreatorManager.h"

namespace nslib
{
  Domain::Domain( void )
    : _dataLoader( nullptr )
    , _entitiesTypes( nullptr )
    , _relationshipPropertiesTypes ( nullptr )
  {
  }

  Domain::~Domain( void )
  {
    if(_dataLoader)
      delete _dataLoader;

    if(_entitiesTypes)
      delete _entitiesTypes;

    if(_relationshipPropertiesTypes)
      delete _relationshipPropertiesTypes;
  }

  std::string& Domain::domainName( void )
  {
    return _domainName;
  }

  DataLoader* Domain::dataLoader( void )
  {
    return _dataLoader;
  }

  const shift::EntitiesTypes& Domain::entitiesTypes( void ) const
  {
    return *_entitiesTypes;
  }

  const shift::RelationshipPropertiesTypes &
  Domain::relationshipPropertiesTypes( void ) const
  {
    return *_relationshipPropertiesTypes;
  }

  void Domain::exportJSON( std::ostream& outputStream,
    bool minimizeStream )  const
  {
    auto entities = DataManager::entities( ).vector( );
    if ( entities.empty( ))
    {
      Loggers::get( )->log( "ERROR: Exporting scene without entities.",
        LOG_LEVEL_WARNING );
    }
    auto relationsSize = _exportRelations.size( );
    if ( relationsSize != _exportAggregatedRelations.size( ))
    {
      Loggers::get( )->log( "Not concordance between export relations size.",
        LOG_LEVEL_WARNING );
    }

    std::string domainLabel;
    std::string entitiesLabel;
    std::string maximumsLabel;
    std::string layoutLabel;
    std::string continueBracket;
    std::string relationsLabel;

    if ( minimizeStream )
    {
      domainLabel = "{\"domain\":\"";
      maximumsLabel = "\",\"maximums\":{";
      layoutLabel = "},\"layout\":{";
      entitiesLabel = "},\"entities\":[";
      continueBracket = "},";
      relationsLabel = entities.empty( ) ? "],\"relationships\":["
        : "}],\"relationships\":[";
    }
    else
    {
      domainLabel = "{\n  \"domain\": \"";
      maximumsLabel = "\",\n  \"maximums\": {\n";
      layoutLabel = "  },\n  \"layout\": {\n";
      entitiesLabel = "  },\n  \"entities\": [\n";
      continueBracket = "\n    },\n";
      relationsLabel = entities.empty( ) ? "  ],\n  \"relationships\": [\n"
        : "    }\n  ],\n  \"relationships\": [\n";

    }

    outputStream << domainLabel << _domainName
       << maximumsLabel;
    exportRepresentationMaxMin( outputStream, minimizeStream );

    outputStream << layoutLabel;
    exportLayoutJSON( outputStream, minimizeStream );

    outputStream << entitiesLabel;
    exportEntitiesJSON( outputStream, minimizeStream );

    outputStream << relationsLabel;

    if ( !_exportRelations.empty( ) )
    {
      bool firstIteration = true;
      auto isAggregated = _exportAggregatedRelations.begin( );
      for ( const auto& relationName : _exportRelations )
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << continueBracket;
        }
        exportRelationTypeToJSON( relationName, outputStream, minimizeStream,
          *isAggregated++ );
      }
      if ( minimizeStream )
      {
        outputStream << "}";

      }
      else
      {
        outputStream << "    }" << std::endl;
      }
    }
    if ( minimizeStream )
    {
      outputStream << "]}";

    }
    else
    {
      outputStream << "  ]" << std::endl << "}" << std::endl;
    }
  }

  void Domain::importJSON( std::istream& inputStream, const bool replaceGIDs )
  {
    boost::property_tree::ptree root;
    try
    {
      boost::property_tree::read_json( inputStream, root );
    }
    catch ( const std::exception & ex )
    {
      Loggers::get( )->log( "ERROR: reading JSON: " + std::string( ex.what( )),
        LOG_LEVEL_ERROR );
      return;
    };

    if ( root.empty( ))
    {
      Loggers::get( )->log( "ERROR: empty JSON file",
        LOG_LEVEL_WARNING );
      return;
    }
    try
    {
      const std::string domainValue = root.get< std::string >( "domain" );
      if( domainValue != _domainName )
      {
        Loggers::get( )->log( "ERROR parsing object: the domain must specify a "
          + _domainName + " domain.", LOG_LEVEL_ERROR );
      }
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting Domain from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      boost::property_tree::ptree maximums = root.get_child( "maximums" );
      importMaximumsJSON( maximums );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting maximums object from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    const auto oldGIDToEntity =
      new std::unordered_map <unsigned int,shift::Entity*>( );

    try
    {
      boost::property_tree::ptree entities = root.get_child( "entities" );
      importEntititiesJSON( entities, oldGIDToEntity, replaceGIDs );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting entities Array from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      boost::property_tree::ptree relationships =
        root.get_child( "relationships" );
      importRelationshipsJSON( relationships, oldGIDToEntity );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting relationships Array from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      boost::property_tree::ptree layout = root.get_child( "layout" );
      importLayoutJSON( layout, oldGIDToEntity );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting layout Object from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
      auto canvas = PaneManager::activePane( );
      canvas->displayEntities( DataManager::rootEntities( ), false, true );
    };


  }

  void Domain::exportRelationTypeToJSON( const std::string& relationName,
    std::ostream& outputStream, const bool minimizeStream,
    const bool aggregated ) const
  {
    std::string relationTypeLabel;
    std::string relationsLabel;
    std::string sourceLabel;
    std::string destLabel;
    std::string relationDataLabel;
    std::string continueBracket;
    std::string closeQuotations;

    if ( minimizeStream )
    {
      relationTypeLabel = "{\"relationType\":\"";
      relationsLabel = "\",\"relations\":[";
      sourceLabel =  "{\"Source\":\"";
      destLabel = "\",\"Dest\":\"";
      relationDataLabel = "\",\"RelationData\":";
      continueBracket =  "},";
      closeQuotations =  "\"";
    }
    else
    {
      relationTypeLabel = "      {\n      \"relationType\": \"";
      relationsLabel = "\",\n      \"relations\": [\n";
      sourceLabel =  "          {\n            \"Source\": \"";
      destLabel = "\",\n            \"Dest\": \"";
      relationDataLabel = "\",\n            \"RelationData\":\n";
      continueBracket =  "\n          },\n";
      closeQuotations =  "\"\n";
    }

    outputStream << relationTypeLabel
      << relationName << relationsLabel;

    bool firstIteration = true;
    if ( aggregated )
    {
      const auto& relationAggregatedOneToN = DataManager::entities( )
        .relationships( )[ relationName ]->asAggregatedOneToN( )
        ->mapAggregatedRels( );
      for( auto relOrigIt = relationAggregatedOneToN.begin( );
        relOrigIt != relationAggregatedOneToN.end( ); ++relOrigIt )
      {
        for( auto relDestIt = relOrigIt->second->begin( );
             relDestIt != relOrigIt->second->end( ); ++relDestIt )
        {
          if( firstIteration )
          {
            firstIteration = false;
          }
          else
          {
            outputStream << continueBracket;
          }
          outputStream << sourceLabel << relOrigIt->first << destLabel
            << relDestIt->first;
          outputStream << relationDataLabel;
          relDestIt->second.relationshipAggregatedProperties->serialize(
            outputStream, minimizeStream, "            " );
          if( !minimizeStream )
          {
            outputStream << std::endl;
          }
        }
      }
    }
    else
    {
      const auto& relationOneToN = *( DataManager::entities( )
        .relationships( )[ relationName ]->asOneToN( ));
      for( auto relOrigIt = relationOneToN.begin( );
        relOrigIt != relationOneToN.end( ); ++relOrigIt )
      {
        for( auto relDestIt = relOrigIt->second.begin( );
          relDestIt != relOrigIt->second.end( ); ++relDestIt )
        {
          if( firstIteration )
          {
            firstIteration = false;
          }
          else
          {
            outputStream << continueBracket;
          }
          outputStream << sourceLabel << relOrigIt->first << destLabel
            << relDestIt->first;
          auto props = relDestIt->second;
          if( props )
          {
            outputStream << relationDataLabel;
            props->serialize( outputStream,
              minimizeStream, "            " );
            if( !minimizeStream )
            {
              outputStream << std::endl;
            }
          }
          else
          {
            outputStream << closeQuotations;
          }
        }
      }
    }
    if ( minimizeStream )
    {
      if ( !firstIteration )
      {
        outputStream << "}]";
      }
      else
      {
        outputStream << "]";
      }
    }
    else
    {
      if ( !firstIteration )
      {
        outputStream << "            }" << std::endl
          << "          ]" << std::endl;
      }
      else
      {
        outputStream <<  "          ]" << std::endl;
      }
    }
  }

  void Domain::importEntityJSON( const boost::property_tree::ptree& entityJSON,
    shift::Entity*& entity, bool& isRootEntity, unsigned int& entityGID,
    const bool replaceGIDs )
  {
    try
    {
      const std::string entityType = entityJSON.get< std::string >( "EntityType" );
      if(!_entitiesTypes) throw std::runtime_error("Uninitialized _entitiesTypes.");
      entity = _entitiesTypes->getEntityObject( entityType )->create( );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting EntityType from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      isRootEntity = entityJSON.get< std::string >( "RootEntity" )[ 0 ] == 't';
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting RootEntity from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      entityGID = entityJSON.get< unsigned int >( "EntityGID" );
      if ( replaceGIDs )
      {
        entity->entityGid( entityGID );
        shift::Entity::shiftEntityGid( entityGID, true );
      }
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting EntityGID from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      boost::property_tree::ptree firesJSON =
        entityJSON.get_child( "EntityData" );
      entity->deserialize( firesJSON );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting EntityData from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };
  }

  boost::property_tree::ptree& Domain::getRelationsOfType(
    const std::string& relationName,
    const boost::property_tree::ptree& relationships )
  {

    for ( const auto& relation : relationships )
    {
      std::string relationType;
      try
      {
        relationType = relation.second.get< std::string >( "relationType");
      }
      catch ( const std::exception &ex )
      {
        Loggers::get( )->log( "ERROR: getting relationType from JSON: "
          + std::string( ex.what( )), LOG_LEVEL_WARNING );
      };

      if ( relationType == relationName )
      {
        try
        {
          return ( boost::property_tree::ptree& )
            relation.second.get_child( "relations" );
        }
        catch ( const std::exception &ex )
        {
          Loggers::get( )->log( "ERROR: getting relations array from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_WARNING );
        };
      }
    }
    return *new boost::property_tree::ptree( );
  }

  void Domain::importJSONRelationGIDS(
    const  boost::property_tree::ptree& relation,
    std::unordered_map < unsigned int, shift::Entity* >* oldGIDToEntity,
    shift::Entity*& origEntity, shift::Entity*& destEntity,
    const std::string& relationName, const bool checkConstrained )
  {
    try
    {
      const unsigned int origGID = relation.get< unsigned int >( "Source" );
      auto search = oldGIDToEntity->find( origGID );
      if( search == oldGIDToEntity->end( ))
      {
        Loggers::get( )->log( "ERROR: old origGID doesn't exist",
          LOG_LEVEL_ERROR );
        origEntity = destEntity = nullptr;
        return;
        }
      origEntity = search->second;
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting Source from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    try
    {
      const unsigned int destGID = relation.get< unsigned int >( "Dest");
      auto search = oldGIDToEntity->find( destGID );
      if( search == oldGIDToEntity->end( ))
      {
        Loggers::get( )->log( "ERROR: old destGID doesn't exist",
          LOG_LEVEL_ERROR );
        origEntity = destEntity = nullptr;
        return;
      }
      destEntity = search->second;
    }
    catch ( const std::exception &ex )
    {
      origEntity = destEntity = nullptr;
      Loggers::get( )->log( "ERROR: getting Dest from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
      return;
    };

    if( checkConstrained && !shift::RelationshipPropertiesTypes::isConstrained(
        relationName, origEntity->typeName( ), destEntity->typeName( )) )
    {
      Loggers::get( )->log( "ERROR: relation: " + relationName +
        " not supported between: " + origEntity->typeName( ) +" - " +
        destEntity->typeName( ), LOG_LEVEL_ERROR );
      origEntity = destEntity = nullptr;
      return;
    }
  }

  void Domain::addConnectsToRelationsFromJSON(
    const boost::property_tree::ptree& relations,
    std::unordered_map< unsigned int, shift::Entity* >* oldGIDToEntity )
  {
    auto& relAggregatedConnectsTo = *( DataManager::entities( )
      .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
    auto& relAggregatedConnectedBy = *( DataManager::entities( )
      .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));

    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity = nullptr;
      shift::Entity* destEntity = nullptr;

      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, "ConnectedTo", true );
      if ( origEntity != nullptr && destEntity != nullptr )
      {
        boost::property_tree::ptree firesData;
        try
        {
          firesData = relation.second.get_child( "RelationData" );
        }
        catch( const std::exception &ex )
        {
          Loggers::get( )->log( "ERROR: getting RelationData from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_WARNING );
        }

        try
        {
          if(!_relationshipPropertiesTypes) throw std::runtime_error("Uninitialized _relationshipPropertiesTypes.");
          shift::RelationshipProperties* propObject = _relationshipPropertiesTypes
            ->getRelationshipProperties( "connectsTo" )->create( );
          propObject->deserialize( firesData );
          shift::Relationship::EstablishAndAggregate(
            relAggregatedConnectsTo,relAggregatedConnectedBy,
            DataManager::entities( ), origEntity, destEntity, propObject,
            propObject, false );
        }
        catch( const std::exception &ex)
        {
          Loggers::get( )->log( "ERROR: getting RelationshipPropertiesTypes from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_WARNING );
        }
      }
    }
  }

  void Domain::addIsParentOfRelationshipsFromJSON(
    const boost::property_tree::ptree& relations,
    std::unordered_map<unsigned int, shift::Entity*>* oldGIDToEntity )
  {
    auto entities = DataManager::entities( );
    auto& relParentOf = *( entities
      .relationships( )[ "isParentOf" ]->asOneToN( ));
    auto& relChildOf = *( DataManager::entities( )
      .relationships( )[ "isChildOf" ]->asOneToOne( ));

    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity = nullptr;
      shift::Entity* destEntity = nullptr;
      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, "ParentOf", true );
      if ( origEntity != nullptr && destEntity != nullptr )
      {
        shift::Relationship::Establish( relParentOf, relChildOf,
          origEntity, destEntity );
      }
    }
  }

  void Domain::importEntititiesJSON(
    const boost::property_tree::ptree& entities,
    std::unordered_map < unsigned int, shift::Entity* >* oldGIDToEntity,
    const bool replaceGIDs )
  {
    for ( const auto& entityJSON : entities )
    {
      shift::Entity* entity = nullptr;
      bool isRootEntity;
      unsigned int oldGID;
      importEntityJSON( entityJSON.second, entity, isRootEntity,
        oldGID, replaceGIDs );
      oldGIDToEntity->insert( std::make_pair( oldGID, entity ));
      DataManager::entities( ).add( entity );
      if( entity->isNotHierarchy( ))
      {
        DataManager::noHierarchyEntities( ).add( entity );
      }
      else if ( isRootEntity )
      {
        DataManager::rootEntities( ).add( entity );
      }
    }
  }

  void Domain::exportEntitiesJSON( std::ostream &outputStream,
    bool minimizeStream ) const
  {
    std::string continueBracket;
    std::string entityTypeLabel;
    std::string rootEntityLabel;
    std::string entityGIDLabel;
    std::string entityDataLabel;
    if ( minimizeStream )
    {
      continueBracket = "},";
      entityTypeLabel = "{\"EntityType\":\"";
      rootEntityLabel = "\",\"RootEntity\":\"";
      entityGIDLabel = "\",\"EntityGID\":\"";
      entityDataLabel = "\",\"EntityData\":";
    }
    else
    {
      continueBracket = "\n    },\n";
      entityTypeLabel = "    {\n      \"EntityType\": \"";
      rootEntityLabel = "\",\n      \"RootEntity\": \"";
      entityGIDLabel = "\",\n      \"EntityGID\": \"";
      entityDataLabel = "\",\n      \"EntityData\":\n";
    }
    auto entities = DataManager::entities( ).vector( );
    auto rootEntitiesMap = DataManager::rootEntities( ).map( );
    bool firstIteration = true;
    for ( const auto &entity : entities )
    {
      if ( firstIteration )
      {
        firstIteration = false;
      }
      else
      {
        outputStream << continueBracket;
      }
      const auto entityGID = entity->entityGid( );
      const char isRoot = ( rootEntitiesMap.find( entityGID )
        != rootEntitiesMap.end( )) ? 't' : 'f';

      outputStream << entityTypeLabel  << entity->typeName( )
        << rootEntityLabel << isRoot << entityGIDLabel
        << entityGID << entityDataLabel;

      entity->serialize( outputStream, minimizeStream, "        " );
    }
  }

  void Domain::addAggregatedConnectionFromJSON(
    const boost::property_tree::ptree& relations, const std::string& name,
    std::unordered_map< unsigned int, shift::Entity* >* oldGIDToEntity )
  {
    auto& relAggregatedOneToN = *( DataManager::entities( )
      .relationships( )[ name ]->asAggregatedOneToN( ));
    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity = nullptr;
      shift::Entity* destEntity = nullptr;

      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, name, false );

      if ( origEntity != nullptr && destEntity != nullptr )
      {
        boost::property_tree::ptree firesData;
        try
        {
          firesData = relation.second.get_child( "RelationData" );
        }
        catch( const std::exception &ex )
        {
          Loggers::get( )->log( "ERROR: getting RelationData from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_WARNING );
        }
        shift::RelationshipProperties* propObject =
          relAggregatedOneToN.getRelationProperties( origEntity->entityGid( ),
          destEntity->entityGid( ));
        propObject->deserialize( firesData );
        propObject->autoUpdateProperties( );
      }
    }
  }

  void Domain::exportLayoutJSON( std::ostream& outputStream,
    const bool minimizeStream ) const
  {
    std::string continueBracket;
    std::string layoutTypeLabel;
    std::string entitiesLabel;
    std::string closeEntitiesLabel;
    std::string entityGIDLabel;
    std::string entityPosXLabel;
    std::string entityPosYLabel;
    if ( minimizeStream )
    {
      continueBracket = ",";
      layoutTypeLabel = "\"LayoutType\":\"";
      entitiesLabel = "\",\"SceneEntities\":[";
      closeEntitiesLabel = "]";
      entityGIDLabel = "{\"EntityGID\":";
      entityPosXLabel = ",\"PosX\":";
      entityPosYLabel = ",\"PosY\":";
    }
    else
    {
      continueBracket = ", ";
      layoutTypeLabel = "    \"LayoutType\": ";
      entitiesLabel = ",\n    \"SceneEntities\":[ ";
      closeEntitiesLabel = " ]\n";
      entityGIDLabel = "\n      {\n        \"EntityGID\": ";
      entityPosXLabel = ",\n        \"PosX\": ";
      entityPosYLabel = ",\n        \"PosY\": ";
    }

    auto currentCanvas = PaneManager::activePane( );
    const int layoutIndex = currentCanvas->activeLayoutIndex( );
    shift::Entities& paneEntities = currentCanvas->sceneEntities( );
    outputStream << layoutTypeLabel << layoutIndex << entitiesLabel;
    bool firstIteration = true;
    if (layoutIndex == Layout::TLayoutIndexes::FREE )
    {
      if ( minimizeStream )
      {
        continueBracket = "},";
        closeEntitiesLabel = "}]";
      }
      else
      {
        continueBracket = "\n    },";
        closeEntitiesLabel = "\n    }\n    ]\n";
      }

      auto gidsToEntitiesReps =
        RepresentationCreatorManager::gidsToEntitiesReps( );
      for (auto entity : paneEntities.vector( ))
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << continueBracket;
        }

        const auto entityGid = entity->entityGid( );
        float posx = 0.0f;
        float posy = 0.0f;
        auto rep =
          gidsToEntitiesReps.find( entityGid );

        if ( rep == gidsToEntitiesReps.end( ))
        {
          Loggers::get( )->log( "Representation not found", LOG_LEVEL_WARNING );
        }
        else
        {
          auto graphicsItemRep =
            dynamic_cast< QGraphicsItemRepresentation* >( rep->second.second );
          if ( graphicsItemRep)
          {
            auto item = graphicsItemRep->item( &currentCanvas->scene( ));
            auto itemPos = item->pos( );
            posx = static_cast<float>(itemPos.x( ));
            posy = static_cast<float>(itemPos.y( ));
          }
          else
          {
            Loggers::get( )->log( "GraphicsItemRep not found",
              LOG_LEVEL_WARNING );
          }
        }

        outputStream << entityGIDLabel << entityGid << entityPosXLabel
          << posx << entityPosYLabel << posy;
      }
    }
    else
    {
      for (auto entity : paneEntities.vector( ))
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << continueBracket;
        }
        outputStream << entity->entityGid( );
      }
    }
    outputStream << closeEntitiesLabel;
  }

  void Domain::importLayoutJSON(
    const boost::property_tree::ptree& layoutObject,
    std::unordered_map<unsigned int, shift::Entity*>* oldGIDToEntity )
  {
    auto canvas = PaneManager::activePane( );
    int layoutTypeIndex = Layout::TLayoutIndexes::GRID;
    try
    {
      layoutTypeIndex = layoutObject.get<int>( "LayoutType" );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting LayoutType from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };
    canvas->layoutChanged( layoutTypeIndex );
    boost::property_tree::ptree entities;
    try
    {
      entities = layoutObject.get_child( "SceneEntities" );
    }
    catch ( const std::exception &ex )
    {
      Loggers::get( )->log( "ERROR: getting Scene entities Array from JSON: "
        + std::string( ex.what( )), LOG_LEVEL_WARNING );
    };

    shift::Entities entitiesNewScene;
    if ( layoutTypeIndex == Layout::TLayoutIndexes::FREE )
    {
      FreeLayout* freeLayout = dynamic_cast< FreeLayout* >(
        canvas->layouts( ) .getLayout( Layout::TLayoutIndexes::FREE ));
      bool moveNewEntities = freeLayout->moveNewEntitiesChecked( );
      freeLayout->moveNewEntitiesChecked( false );
      for ( auto entityJSON : entities )
      {
        shift::Entity* entity = nullptr;
        try
        {
          const auto entiyGID = entityJSON.second.get<int>( "EntityGID" );
          auto search = oldGIDToEntity->find( entiyGID );
          if ( search == oldGIDToEntity->end( ))
          {
            Loggers::get( )->log( "ERROR: old scene EntityGID doesn't exist",
              LOG_LEVEL_ERROR );
          }
          else
          {
            entity = search->second;
          }
        }
        catch( const std::exception &ex )
        {
          entity = nullptr;
          Loggers::get( )->log( "ERROR: getting scene EntityGID from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_ERROR );
        };
        if ( entity )
        {
          float posx = 0.0f;
          float posy = 0.0f;
          try
          {
            posx = entityJSON.second.get<float>( "PosX" );
          }
          catch( const std::exception &ex )
          {
            Loggers::get( )->log(
              "ERROR: getting entity pos x from JSON: "
              + std::string( ex.what( )), LOG_LEVEL_WARNING );
          };
          try
          {
            posy = entityJSON.second.get<float>( "PosY" );
          }
          catch( const std::exception &ex )
          {
            Loggers::get( )->log(
              "ERROR: getting entity pos y from JSON: "
              + std::string( ex.what( )), LOG_LEVEL_WARNING );
          };
          entitiesNewScene.add( entity );
          shift::Entities loadEntity;
          loadEntity.add( entity );
          shift::Representations loadRep;
          RepresentationCreatorManager::create(loadEntity,loadRep,true,true);
          if( loadRep.empty( ))
          {
            Loggers::get( )->log(
              "ERROR: Unable to create entity representation for: "
              + std::to_string( entity->entityGid( )), LOG_LEVEL_WARNING );
          }
          else
          {
            auto graphicsItemRep =
              dynamic_cast< QGraphicsItemRepresentation* >( loadRep.at( 0 ));
            if ( graphicsItemRep)
            {
              auto item = graphicsItemRep->item( &canvas->scene( ));
              item->setPos(posx, posy );
            }
            else
            {
              Loggers::get( )->log( "GraphicsItemRep not found",
                LOG_LEVEL_WARNING );
            }
          }
        }
      }
      canvas->displayEntities(entitiesNewScene, false, true );
      freeLayout->moveNewEntitiesChecked( moveNewEntities );
    }
    else
    {
      for( auto entityJSON : entities )
      {
        try
        {
          auto entiyGID = entityJSON.second.get<int>( "" );
          auto search = oldGIDToEntity->find( entiyGID );
          if( search == oldGIDToEntity->end( ))
          {
            Loggers::get( )->log( "ERROR: old scene EntityGID doesn't exist",
              LOG_LEVEL_ERROR );
          }
          else
          {
            shift::Entity* entity = search->second;
            entitiesNewScene.add( entity );
          }
        }
        catch( const std::exception &ex )
        {
          Loggers::get( )->log( "ERROR: getting scene EntityGID from JSON: "
            + std::string( ex.what( )), LOG_LEVEL_ERROR );
        };
      }
      canvas->displayEntities( entitiesNewScene, false, true );
    }
  }
}
