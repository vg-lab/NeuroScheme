/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
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

#include "Domain.h"
#include "DataManager.h"
#include "PaneManager.h"

namespace nslib
{
  Domain::Domain( void )
    : _dataLoader( nullptr ), _entitiesTypes( nullptr )
  {
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
    auto entities = nslib::DataManager::entities( ).vector( );
    SHIFT_CHECK_THROW( !entities.empty( ),
      "ERROR: Exporting scene without entities." );

    std::string domainLabel;
    std::string entitiesLabel;
    std::string maximumsLabel;
    std::string continueBracket;
    std::string relationshipsLabel;
    
    if ( minimizeStream )
    {
      domainLabel = "{\"domain\":\"";
      maximumsLabel = "\",\"maximums\":{";
      entitiesLabel = "},\"entities\":[";
      continueBracket = "},";
      relationshipsLabel =  "}],\"relationships\":[";
    }
    else
    {
      domainLabel = "{\n  \"domain\": \"";
      maximumsLabel = "\",\n  \"maximums\": {\n";
      entitiesLabel = "  },\n  \"entities\": [\n";
      continueBracket = "\n    },\n";
      relationshipsLabel =  "    }\n  ],\n  \"relationships\": [\n";

    }

    outputStream << domainLabel << _domainName
       << maximumsLabel;
    exportRepresentationMaxMin( outputStream, minimizeStream );

    outputStream << entitiesLabel;
    exportEntitiesJSON( outputStream, minimizeStream );

    outputStream << relationshipsLabel;

    if ( !_exportRelations.empty( ) )
    {
      bool firstIteration = true;
      for ( auto relationName : _exportRelations )
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << continueBracket;
        }
        exportRelationTypeToJSON( relationName, outputStream, minimizeStream );
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

  void Domain::importJSON( std::istream& inputStream )
  {
    boost::property_tree::ptree root;
    try
    {
      boost::property_tree::read_json( inputStream, root );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: reading JSON: " + std::string( ex.what( )));
    };

    SHIFT_CHECK_THROW( !root.empty( ),"ERROR: empty JSON file" );
    try
    {
      std::string domainValue = root.get< std::string >( "domain" );
      SHIFT_CHECK_THROW( domainValue == _domainName, "ERROR parsing object:"
         "the domain must specify a " + _domainName + " domain." );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting Domain from JSON: "
        + std::string( ex.what( )));
    };

    boost::property_tree::ptree maximums;
    try
    {
      maximums = root.get_child( "maximums" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting maximums object from JSON: "
        + std::string( ex.what( )));
    };
    importMaximumsJSON( maximums );

    boost::property_tree::ptree entities;
    try
    {
      entities = root.get_child( "entities" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting entities Array from JSON: "
        + std::string( ex.what( )));
    };

     auto oldGUIToEntity =
       new std::unordered_map <unsigned int,shift::Entity*>( );

    importEntititiesJSON( entities, oldGUIToEntity );

    boost::property_tree::ptree relationships;
    try
    {
      relationships = root.get_child( "relationships" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting relationships Array from JSON: "
        + std::string( ex.what( )));
    };

    importRelationshipsJSON( relationships, oldGUIToEntity );

    auto canvas = nslib::PaneManager::activePane( );
    canvas->displayEntities( nslib::DataManager::rootEntities( ), false, true );
  }

  void Domain::exportRelationTypeToJSON( const std::string& relationName,
    std::ostream &outputStream, bool minimizeStream ) const
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
    const auto &relationOneToN = *( DataManager::entities( )
      .relationships( )[ relationName ]->asOneToN( ));

    outputStream << relationTypeLabel
      << relationName << relationsLabel;

    bool firstIteration = true;
    for ( auto relOrigIt = relationOneToN.begin( );
      relOrigIt != relationOneToN.end( ); ++relOrigIt )
    {
      for ( auto relDestIt = relOrigIt->second.begin( );
        relDestIt != relOrigIt->second.end( ); ++relDestIt )
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << continueBracket;
        }
        outputStream << sourceLabel << relOrigIt->first
          << destLabel
          << relDestIt->first;
        if ( relationName == "connectsTo" )
        {
          outputStream << relationDataLabel;
          relDestIt->second->serialize( outputStream,
            minimizeStream, "            " );
          if ( !minimizeStream )
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
    shift::Entity*& entity, bool& isRootEntity, unsigned int& entityGID )
  {
    try
    {
      std::string entityType = entityJSON.get< std::string >( "EntityType" );
      entity = ( *_entitiesTypes ).getEntityObject( entityType )->create( );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting EntityType from JSON: "
        + std::string( ex.what( )));
    };

    try
    {
      isRootEntity = entityJSON.get< std::string >( "RootEntity" )[ 0 ] == 't';
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting RootEntity from JSON: "
        + std::string( ex.what( )));
    };

    try
    {
      entityGID = entityJSON.get< unsigned int >( "EntityGID" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting EntityGID from JSON: "
        + std::string( ex.what( )));
    };

    try
    {
      entityGID = entityJSON.get< unsigned int >( "EntityGID" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting EntityGID from JSON: "
        + std::string( ex.what( )));
    };
    boost::property_tree::ptree firesJSON;
    try
    {
      firesJSON = entityJSON.get_child( "EntityData" );
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting EntityData from JSON: "
        + std::string( ex.what( )));
    };
    entity->deserialize( firesJSON );
  }

  void Domain::importRelationshipsJSON(
    const boost::property_tree::ptree& relationships,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {

    for ( const auto& relation : relationships )
    {
      std::string relationType;
      try
      {
        relationType = relation.second.get< std::string >( "relationType");
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting relationType for JSON: "
          + std::string( ex.what( )));
      };

      boost::property_tree::ptree relations;
      try
      {
        relations = relation.second.get_child( "relations" );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting relations array for JSON: "
          + std::string( ex.what( )));
      };
      addRelationsOfType( relations, relationType, oldGUIToEntity );
    }
  }

  void Domain::importJSONRelationGIDS(
    const  boost::property_tree::ptree& relation,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity,
    shift::Entity*& origEntity, shift::Entity*& destEntity,
    const std::string& relationName )
  {
    try
    {
      unsigned int origGID = relation.get< unsigned int >( "Source" );
      auto search = oldGUIToEntity->find( origGID );
      SHIFT_CHECK_THROW( search != oldGUIToEntity->end( ),
        "ERROR: old origGUI doesn't exist");
      origEntity = search->second;
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting Source for JSON: "
        + std::string( ex.what( )));
    };

    try
    {
      unsigned int destGID = relation.get< unsigned int >( "Dest");
      auto search = oldGUIToEntity->find( destGID );
      SHIFT_CHECK_THROW( search != oldGUIToEntity->end( ),
        "ERROR: old destGUI doesn't exist");
      destEntity = search->second;
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting Dest for JSON: "
        + std::string( ex.what( )));
    };

    SHIFT_CHECK_THROW( shift::RelationshipPropertiesTypes::isConstrained(
      relationName, origEntity->entityName( ), destEntity->entityName( )),
      "ERROR: relation: " + relationName + " not supported between: " +
      origEntity->entityName( ) +" - " + destEntity->entityName( ));
  }

  void Domain::addConnectsToRelationsToJSON(
    const boost::property_tree::ptree& relations,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    auto& relConnectsTo = *( DataManager::entities( )
      .relationships( )[ "connectsTo" ]->asOneToN( ));
    auto& relConnectedBy = *( DataManager::entities( )
      .relationships( )[ "connectedBy" ]->asOneToN( ));

    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity;
      shift::Entity* destEntity;

      importJSONRelationGIDS( relation.second, oldGUIToEntity, origEntity,
        destEntity, "ConnectedTo" );
      boost::property_tree::ptree firesData;
      try
      {
        firesData = relation.second.get_child( "RelationData" );
      }
      catch( std::exception& ex )
      {
        SHIFT_THROW( "ERROR: getting RelationData for JSON: "
         + std::string( ex.what( )));
      }
      shift::RelationshipProperties* propObject = _relationshipPropertiesTypes
        ->getRelationshipProperties( "connectsTo" )->create( );
      shift::Relationship::Establish( relConnectsTo, relConnectedBy,
        origEntity, destEntity, propObject );
      propObject->deserialize( firesData );
    }
  }

  void Domain::addIsParentOfRelationshipsToJSON(
    const boost::property_tree::ptree& relations,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    auto& relParentOf = *( DataManager::entities( )
      .relationships( )[ "isParentOf" ]->asOneToN( ));
    auto& relChildOf = *( DataManager::entities( )
      .relationships( )[ "isChildOf" ]->asOneToOne( ));

    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity;
      shift::Entity* destEntity;
      importJSONRelationGIDS( relation.second, oldGUIToEntity, origEntity,
        destEntity, "ParentOf" );
      shift::Relationship::Establish( relParentOf, relChildOf,
        origEntity, destEntity );
    }
  }

  void Domain::importEntititiesJSON(
    const boost::property_tree::ptree& entities,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    for ( const auto& entityJSON : entities )
    {
      shift::Entity* entity ;
      bool isRootEntity;
      unsigned int oldGUI;
      importEntityJSON( entityJSON.second, entity, isRootEntity, oldGUI );
      oldGUIToEntity->insert( std::make_pair( oldGUI, entity ));
      DataManager::entities( ).add( entity );
      if ( isRootEntity )
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
    auto entities = nslib::DataManager::entities( ).vector( );
    auto rootEntitiesMap = DataManager::rootEntities( ).map( );
    bool firstIteration = true;
    for ( auto entity : entities )
    {
      if ( firstIteration )
      {
        firstIteration = false;
      }
      else
      {
        outputStream << continueBracket;
      }
      auto entityGID = entity->entityGid( );
      char isRoot = ( rootEntitiesMap.find( entityGID )
        != rootEntitiesMap.end( )) ? 't' : 'f';

      outputStream << entityTypeLabel  << entity->entityName( )
        << rootEntityLabel << isRoot << entityGIDLabel
        << entityGID << entityDataLabel;

      entity->serialize( outputStream, minimizeStream, "        " );
    }
  }

}