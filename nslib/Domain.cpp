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
    auto relationsSize = _exportRelations.size( );
    SHIFT_CHECK_THROW( relationsSize == _exportAggregatedRelations.size( ),
      "ERROR: Not concordance between export relations size." );

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

     auto oldGIDToEntity =
       new std::unordered_map <unsigned int,shift::Entity*>( );

    importEntititiesJSON( entities, oldGIDToEntity );

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

    importRelationshipsJSON( relationships, oldGIDToEntity );

    auto canvas = nslib::PaneManager::activePane( );
    canvas->displayEntities( nslib::DataManager::rootEntities( ), false, true );
  }

  void Domain::exportRelationTypeToJSON( const std::string& relationName,
    std::ostream &outputStream, const bool minimizeStream,
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
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting relationType for JSON: "
          + std::string( ex.what( )));
      };

      if ( relationType == relationName )
      {
        try
        {
          return ( boost::property_tree::ptree& )
            relation.second.get_child( "relations" );
        }
        catch ( std::exception const& ex )
        {
          SHIFT_THROW( "ERROR: getting relations array for JSON: "
            + std::string( ex.what( )));
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
      unsigned int origGID = relation.get< unsigned int >( "Source" );
      auto search = oldGIDToEntity->find( origGID );
      SHIFT_CHECK_THROW( search != oldGIDToEntity->end( ),
        "ERROR: old origGID doesn't exist");
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
      auto search = oldGIDToEntity->find( destGID );
      SHIFT_CHECK_THROW( search != oldGIDToEntity->end( ),
        "ERROR: old destGID doesn't exist");
      destEntity = search->second;
    }
    catch ( std::exception const& ex )
    {
      SHIFT_THROW( "ERROR: getting Dest for JSON: "
        + std::string( ex.what( )));
    };
    if( checkConstrained )
    {
      SHIFT_CHECK_THROW( shift::RelationshipPropertiesTypes::isConstrained(
        relationName, origEntity->typeName( ), destEntity->typeName( )),
        "ERROR: relation: " + relationName + " not supported between: " +
        origEntity->typeName( ) +" - " + destEntity->typeName( ));
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
      shift::Entity* origEntity;
      shift::Entity* destEntity;

      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, "ConnectedTo", true );
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
      propObject->deserialize( firesData );
      shift::Relationship::EstablishAndAggregate(
        relAggregatedConnectsTo,relAggregatedConnectedBy,
        DataManager::entities( ), origEntity, destEntity, propObject,
        propObject, false );
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
    auto& relAggregatedConnectsTo = *(entities
      .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
    auto& relAggregatedConnectedBy = *( entities
      .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));

    for ( const auto& relation : relations )
    {
      shift::Entity* origEntity;
      shift::Entity* destEntity;
      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, "ParentOf", true );
      shift::Relationship::EstablishWithHierarchy( relParentOf, relChildOf,
        relAggregatedConnectsTo, relAggregatedConnectedBy,
        origEntity, destEntity );
    }
  }

  void Domain::importEntititiesJSON(
    const boost::property_tree::ptree& entities,
    std::unordered_map < unsigned int, shift::Entity* >* oldGIDToEntity )
  {
    for ( const auto& entityJSON : entities )
    {
      shift::Entity* entity ;
      bool isRootEntity;
      unsigned int oldGID;
      importEntityJSON( entityJSON.second, entity, isRootEntity, oldGID );
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
      shift::Entity* origEntity;
      shift::Entity* destEntity;

      importJSONRelationGIDS( relation.second, oldGIDToEntity, origEntity,
        destEntity, name, false );
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
      shift::RelationshipProperties* propObject = relAggregatedOneToN
        .getRelationProperties(origEntity->entityGid( ),
        destEntity->entityGid( ));
      propObject->deserialize( firesData );
      propObject->autoUpdateProperties( );
    }
  }

}
