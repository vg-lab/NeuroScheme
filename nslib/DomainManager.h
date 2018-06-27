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
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NSLIB__DOMAIN_MANAGER__
#define __NSLIB__DOMAIN_MANAGER__

#include <nslib/api.h>
#include <shift/shift.h>
#include <Eigen/Dense>
#include <QMenuBar>
#include <QMainWindow>
#include "DataLoader.h"
#include "DataManager.h"

namespace nslib
{
  using Matrix4f = ::Eigen::Matrix4f;
  using Vector4f = ::Eigen::Vector4f;

  class NSLIB_API Domain
  {

  public:
    Domain( void )
      : _dataLoader( nullptr )
      , _entitiesTypes( nullptr )
    {
    }
    virtual ~Domain( void ) {}
    virtual bool isSelectableEntity( shift::Entity* entity ) const = 0;
    virtual unsigned int selectableEntityId( shift::Entity* entity ) const = 0;
    virtual const Vector4f entity3DPosition ( shift::Entity* entity ) const = 0;
    DataLoader* dataLoader( void )
    {
      return _dataLoader;
    }
    const shift::EntitiesTypes& entitiesTypes( void ) const
    {
      return *_entitiesTypes;
    }
    const shift::RelationshipPropertiesTypes&
      relationshipPropertiesTypes( void ) const
    {
      return *_relationshipPropertiesTypes;
    }

    virtual void exportJSON ( std::ostream& outputStream )
    {
      auto entities = nslib::DataManager::entities( ).vector( );
      SHIFT_CHECK_THROW( !entities.empty( ),
        "ERROR: Exporting scene without entities." );

      outputStream << "{" << std::endl << "  \"domain\": \"congen\","
        << std::endl << "  \"entities\":" << std::endl << "    ["
        << std::endl;

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
        outputStream << "      {" << std::endl << "        \"EntityType\": \""
          << entity->entityName( ) << "\"," << std::endl
          << "        \"EntityGID\": \"" << entity->entityGid( )
          << "\"," << std::endl << "        \"EntityData\":"
          << std::endl;
        entity->serialize( outputStream, "          " );
      }
      outputStream << "      }" << std::endl << "    ]," << std::endl
        << "  \"relationships\":" << std::endl << "    ["
        << std::endl;


      std::vector < std::string > exportRelations = { "connectsTo" };
      if ( !exportRelations.empty( ) )
      {
        firstIteration = true;
        for ( auto relationName : exportRelations )
        {
          if ( firstIteration )
          {
            firstIteration = false;
          }
          else
          {
            outputStream << "      }," << std::endl;
          }
          addRelationTypeToJSON( relationName, outputStream );
        }
        outputStream << "      }" << std::endl;
      }
      outputStream << "    ]" << std::endl;
      outputStream << "}" << std::endl;

    }

    /*void importJSON ( std::istream inputStream )
    {

    }*/



    virtual void createGUI( QMainWindow* /* mw */, QMenuBar* /* menubar */ ) {}

  protected:
    DataLoader* _dataLoader;
    shift::EntitiesTypes* _entitiesTypes;
    shift::RelationshipPropertiesTypes* _relationshipPropertiesTypes;
    void addRelationTypeToJSON(std::string relationName,
      std::ostream& outputStream  )
    {
      const auto& relationOneToN = *( DataManager::entities( ).
        relationships( )[ relationName ]->asOneToN( ));

      outputStream << "      {" << std::endl << "        \"relationType\": \""
        << relationName << "\"," << std::endl << "        \"relations\":"
        << std::endl << "          [" << std::endl;

      bool firstIteration = true;
      for ( auto relOrigIt = relationOneToN.begin( );
        relOrigIt != relationOneToN.end( ); ++relOrigIt)
      {
        for ( auto relDestIt = relOrigIt->second.begin( );
          relDestIt != relOrigIt->second.end( ); ++relDestIt)
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
            << "              \"Source\" : \"" << relOrigIt->first << "\","
            << std::endl << "              \"Dest\" : \"" << relDestIt->first
            << "\"," << std::endl << "              \"RelationData\":"
            << std::endl;
          relDestIt->second->serialize( outputStream, "              " );
        }
      }
        if ( !firstIteration )
        {
          outputStream << "            }" << std::endl;
        }
      outputStream << "          ]" << std::endl;

    }




  };

  class NSLIB_API DomainManager
  {
  public:

    static Domain* getActiveDomain( void )
    {
      return _domain;
    }

    static Domain* setActiveDomain( Domain* domain_ )
    {
      return _domain = domain_;
    }

  protected:
    // static unsigned int _activeDomain;
    static Domain* _domain;
    //static unsigned int _storedDomainNextId;
  };
}

#endif
