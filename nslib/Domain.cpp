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

  void Domain::exportJSON( std::ostream &outputStream )
  {
    auto entities = nslib::DataManager::entities( ).vector( );
    SHIFT_CHECK_THROW( !entities.empty( ),
      "ERROR: Exporting scene without entities." );

    outputStream << "{" << std::endl << "  \"domain\": \"" << _domainName
       << "\"," << std::endl << "  \"entities\":" << std::endl
       << "    [" << std::endl;

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
        outputStream << "      }," << std::endl;
      }
      auto entityGID = entity->entityGid( );
      char isRoot = ( rootEntitiesMap.find( entityGID )
        != rootEntitiesMap.end( )) ? 't' : 'f';

      outputStream << "      {" << std::endl << "        \"EntityType\": \""
        << entity->entityName( ) << "\"," << std::endl
        << "        \"RootEntity\": \"" << isRoot << "\","
        << std::endl << "        \"EntityGID\": \"" << entityGID
        << "\"," << std::endl << "        \"EntityData\":" << std::endl;

      entity->serialize( outputStream, "          " );
    }
    outputStream << "      }" << std::endl << "    ]," << std::endl
                 << "  \"relationships\":" << std::endl << "    [" << std::endl;

    if ( !_exportRelations.empty( ) )
    {
      firstIteration = true;
      for ( auto relationName : _exportRelations )
      {
        if ( firstIteration )
        {
          firstIteration = false;
        }
        else
        {
          outputStream << "      }," << std::endl;
        }
        exportRelationTypeToJSON( relationName, outputStream );
      }
      outputStream << "      }" << std::endl;
    }
    outputStream << "    ]" << std::endl << "}" << std::endl;

  }

  void Domain::importJSON( std::istream &inputStream )
  {
    std::string line;
    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: Empty File." );

    auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
      "ERROR parsing object: First line must be a '{' for start the "
      "JSONobject, instead does: \"" + line + "\"." );
    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find_last_not_of( "  \r\t" ),
      "ERROR parsing object: in line \"" + line +
      "\" nothing must come after '{'." );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: next line after initial '{' must"
      " specify the domain but it doesn't exist" );

    std::string labelString = std::string( "\"domain\": \"" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: next line after initial '{'"
      " must specify the domain, instead does: " + line );

    auto startString = firstNotWhiteSpace + labelString.size( );
    auto endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );
    std::string objectLabel =
      line.substr( startString, endString - startString );

    SHIFT_CHECK_THROW( objectLabel == _domainName,
      "ERROR parsing object: line: \"" + line +
      "\", must specify a congen domain." );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: next line after the declaration of the domain "
      "must declare the entities, but it doesn't exist" );

    labelString = std::string( "\"entities\":" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: next line after the declaration of "
      "the domain must declare the entities, instead does: " + line );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) + 1 ==
      firstNotWhiteSpace + labelString.size( ), "ERROR parsing object: line: \""
      + line + "\", it must end in: '\"entities\":'." );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: Next line after the declaration of entities must"
      " declare the  JSONarray of the entities, instead it doesn't exit." );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '[',
      "ERROR parsing object: Next line after the declaration of entities must"
      " declare the  JSONarray of the entities, instead does: \""
      + line + "\"." );

     auto oldGUIToEntity =
       new std::unordered_map <unsigned int,shift::Entity*>( );

    while ( std::getline( inputStream, line ) )
    {
      firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
      if ( line[ firstNotWhiteSpace ] == ']' )
      {
        break;
      }
      else
      {
        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
          "ERROR parsing object: this line must be a '{' for start the "
          "JSONobject of the entity or an ] to close the JSONArray of the "
          "entites, instead does: \"" + line + "\"." );

        shift::Entity* entity = nullptr;
        bool isRootEntity = false;
        unsigned int oldGUI = 0;

        importEntityJSON( inputStream, entity, isRootEntity, oldGUI );

        oldGUIToEntity->insert( std::make_pair( oldGUI, entity ));
        DataManager::entities( ).add( entity );
        if ( isRootEntity )
        {
          DataManager::rootEntities( ).add( entity );
        }
      }
    }

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: this line  must declare the relationships,"
      " but it doesn't exist." );

    labelString = std::string( "\"relationships\":" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: this line must declare the relationships,"
      " instead does: " + line );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) + 1 ==
      firstNotWhiteSpace + labelString.size( ), "ERROR parsing object: line: \""
      + line + "\", it must end in: '\"relationships\":'." );



    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: Next line after the declaration of "
      "relationships must  declare the  JSONarray of the relationships,"
      " instead it doesn't exit." );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '[',
       "ERROR parsing object: Next line after the declaration of relationships "
       "must declare the  JSONarray of the relationships, instead does: \"" +
       line + "\"." );


    while ( std::getline( inputStream, line ) )
    {
      firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
      if ( line[ firstNotWhiteSpace ] == ']' )
      {
        break;
      }
      else
      {
        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
          "ERROR parsing object: this line must be a '{' for start the "
          "JSONobject of the entity or an ] to close the JSONArray of the"
          " entites, instead does: \"" + line + "\"." );

        importRelationshipType( inputStream, oldGUIToEntity );
      }

    }

    auto canvas = nslib::PaneManager::activePane( );
    canvas->displayEntities( nslib::DataManager::rootEntities( ), false, true );

  }


  void Domain::exportRelationTypeToJSON( std::string relationName,
                                 std::ostream &outputStream )
  {
    const auto &relationOneToN =
      *( DataManager::entities( ).relationships( )[ relationName ]
        ->asOneToN( ) );

    outputStream << "      {" << std::endl << "        \"relationType\": \""
      << relationName << "\"," << std::endl
      << "        \"relations\":" << std::endl << "          [" << std::endl;

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
          outputStream << "            }," << std::endl;
        }
        outputStream << "            {" << std::endl
          << "              \"Source\": \"" << relOrigIt->first
          << "\"," << std::endl << "              \"Dest\": \""
          << relDestIt->first;
        if ( relationName == "connectsTo" )
        {
          outputStream << "\"," << std::endl
            << "              \"RelationData\":" << std::endl;
          relDestIt->second->serialize( outputStream, "              " );
        }
        else
        {
          outputStream << "\"" << std::endl;
        }
      }
    }
    if ( !firstIteration )
    {
      outputStream << "            }" << std::endl;
    }
    outputStream << "          ]" << std::endl;

  }

  void Domain::importEntityJSON( std::istream &inputStream,
    shift::Entity*& entity, bool& isRootEntity, unsigned int& entityGID )
  {

    std::string line;
    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after the property '{'"
      " must specify the entityType, instead it doesn't exit." );

    std::string labelString = std::string( "\"EntityType\": \"" );
    auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: this line must specify the entityType,"
      " instead it does: " + line );

    auto startString = firstNotWhiteSpace + labelString.size( );
    auto endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );
    std::string entityType =
      line.substr( startString, endString - startString );



    entity = _entitiesTypes[ 0 ].getEntityObject( entityType )->create( );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after entityType"
      " must specify if isRoot, instead it doesn't exit." );
    labelString = std::string( "\"RootEntity\": \"" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: next line after entityType"
      " must specify if isRoot, instead it does: " + line );

    startString = firstNotWhiteSpace + labelString.size( );
    endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );

    isRootEntity =
      line.substr( startString, endString - startString )[ 0 ] == 't';

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after isRoot"
      " must specify the entityGID, instead it doesn't exit." );

    labelString = std::string( "\"EntityGID\": \"" );
    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "ERROR parsing object: this line must specify the entityGID,"
      " instead it does: " + line );

    startString = firstNotWhiteSpace + labelString.size( );
    endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );
    entityGID = boost::lexical_cast< unsigned int >
      ( line.substr( startString, endString - startString ));


    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: next line after the declaration of the GID "
      "must declare the entityData, but it doesn't exist" );

    labelString = std::string( "\"EntityData\":" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: next line after the declaration of "
      "the GID must declare the entityData, instead does: " + line );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) + 1 ==
      firstNotWhiteSpace + labelString.size( ),
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\"EntityData\":'." );


    entity->deserialize( inputStream );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: This line must"
      " close the  JSONObjec of the EntityData, instead it doesn't exit." );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
      "ERROR parsing object: This line must close the  "
      "JSONObject of the EntityData, instead does: \"" + line + "\"." );
  }

  void Domain::importRelationshipType( std::istream &inputStream,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    std::string line;
    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after the relationShip '{'"
      " must specify the relationType, instead it doesn't exit." );

    std::string labelString = std::string( "\"relationType\": \"" );
    auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "ERROR parsing object: this line must specify the relationType,"
      " instead it does: " + line );

    auto startString = firstNotWhiteSpace + labelString.size( );
    auto endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );

    std::string relationType =
      line.substr( startString, endString - startString );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: next line after the declaration of the domain "
      "must declare the entities, but it doesn't exist" );

    labelString = std::string( "\"relations\":" );
    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: next line after the declaration of "
      "the relationType must declare the relations, instead does: " + line );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) + 1 ==
       firstNotWhiteSpace + labelString.size( ),
       "ERROR parsing object: line: \"" + line +
       "\", it must end in: '\"entities\":'." );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: Next line after the declaration"
      " of the relationType must declare the  JSONarray of the relations,"
      " instead it doesn't exit." );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '[',
      "ERROR parsing object: Next line after the declaration of the"
      " relationType must declare the  JSONarray of the relations,"
      " instead does: \"" + line + "\"." );

    addRelationsOfType( inputStream, relationType, oldGUIToEntity );

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "ERROR parsing object: this line must close the JSONObject"
      " but instead it doesn't exit" );

    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
      "ERROR parsing object: this line must be a '}' for close the JSONObject,"
      " instead does: \"" + line + "\"." );
  }

  void Domain::importJSONRelationGIDS( std::istream &inputStream,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity,
    shift::Entity*& origEntity, shift::Entity*& destEntity )
  {
    std::string line;

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after the relation '{'"
      " must specify the Source, instead it doesn't exit." );

    std::string labelString = std::string( "\"Source\": \"" );
    auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: this line must specify the Source,"
      " instead it does: " + line );
    auto startString = firstNotWhiteSpace + labelString.size( );
    auto endString = line.rfind( "\"," );

    SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) - 1 == endString,
      "ERROR parsing object: line: \"" + line +
      "\", it must end in: '\",'." );
    unsigned int origGID = boost::lexical_cast< unsigned int >
      ( line.substr( startString, endString - startString ));

    SHIFT_CHECK_THROW( std::getline( inputStream, line ),
      "\"ERROR parsing object: next line after the relation '{'"
      " must specify the Source, instead it doesn't exit." );
    labelString = std::string( "\"Dest\": \"" );
    firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
    SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
      "\"ERROR parsing object: this line must specify the Dest,"
      " instead it does: " + line );
    startString = firstNotWhiteSpace + labelString.size( );
    endString = line.rfind( "\"" );

    unsigned int destGID = boost::lexical_cast< unsigned int >
      ( line.substr( startString, endString - startString ));

    auto search = oldGUIToEntity->find( origGID );
    SHIFT_CHECK_THROW( search != oldGUIToEntity->end( ),
      "ERROR: old GUI doesn't exist");
    origEntity = search->second;

    search = oldGUIToEntity->find( destGID );
    SHIFT_CHECK_THROW( search != oldGUIToEntity->end( ),
      "ERROR: old GUI doesn't exist");
    destEntity = search->second;
  }

  void Domain::addConnectsToRelationsToJSON( std::istream &inputStream,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    auto& relConnectsTo =
      *( DataManager::entities( ).relationships( )["connectsTo"]->asOneToN( ));
    auto& relConnectedBy =
      *( DataManager::entities( ).relationships( )["connectedBy"]->asOneToN( ));

    std::string line;
    while ( std::getline( inputStream, line ) )
    {
      auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
      if ( line[ firstNotWhiteSpace ] == ']' )
      {
        break;
      }
      else
      {
        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
          "ERROR parsing object: this line must be a '{' for start the "
          "JSONobject of the entity or an ] to close the JSONArray of the"
          " entites, instead does: \"" + line + "\"." );

        shift::Entity* origEntity = nullptr;
        shift::Entity* destEntity = nullptr;

        importJSONRelationGIDS(inputStream, oldGUIToEntity,
           origEntity, destEntity);

        shift::RelationshipProperties* propObject = _relationshipPropertiesTypes
          ->getRelationshipProperties( "connectsTo" )->create( );

        shift::Relationship::Establish( relConnectsTo,relConnectedBy,
          origEntity,destEntity,propObject );

        SHIFT_CHECK_THROW( std::getline( inputStream, line ),
          "ERROR parsing object: this line "
          "must declare the RelationData, but it doesn't exist" );

        std::string labelString = std::string( "\"RelationData\":" );

        firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

        SHIFT_CHECK_THROW( firstNotWhiteSpace == line.find( labelString ),
          "\"ERROR parsing object: this line must declare the RelationData,"
          " instead does: " + line );

        SHIFT_CHECK_THROW( line.find_last_not_of( "  \r\t" ) + 1 ==
          firstNotWhiteSpace + labelString.size( ),
          "ERROR parsing object: line: \"" + line +
          "\", it must end in: '\"RelationData\":'." );

        propObject->deserialize( inputStream );

        SHIFT_CHECK_THROW( std::getline( inputStream, line ),
          "ERROR parsing object: this line must close the JSONObject"
          " but instead it doesn't exit" );

        firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
          "ERROR parsing object: this line must be a '}' for close the JSONObject"
          ", instead does: \"" + line + "\"." );
      }
    }
  }

  void Domain::addIsParentOfRelationshipsToJSON( std::istream &inputStream,
    std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
  {
    auto& relParentOf = *( DataManager::entities( )
      .relationships( )[ "isParentOf" ]->asOneToN( ) );
    auto& relChildOf = *( DataManager::entities( )
      .relationships( )[ "isChildOf" ]->asOneToOne( ) );


    std::string line;
    while ( std::getline( inputStream, line ) )
    {
      auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
      if ( line[ firstNotWhiteSpace ] == ']' )
      {
        break;
      }
      else
      {
        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
          "ERROR parsing object: this line must be a '{' for start the"
          " JSONobject of the entity or an ] to close the JSONArray of the"
          " entites, instead does: \"" + line + "\"." );

        shift::Entity* origEntity = nullptr;
        shift::Entity* destEntity = nullptr;

        importJSONRelationGIDS(inputStream, oldGUIToEntity, origEntity, destEntity);

        shift::Relationship::Establish( relParentOf, relChildOf,
          origEntity,destEntity );

        SHIFT_CHECK_THROW( std::getline( inputStream, line ),
          "ERROR parsing object: this line must close the JSONObject"
          " but instead it doesn't exit" );

        firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

        SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
          "ERROR parsing object: this line must be a '}' for close the JSONObject"
          ", instead does: \"" + line + "\"." );
      }
    }
  }
}