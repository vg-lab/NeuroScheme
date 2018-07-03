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

#ifndef __NSLIB__DOMAIN__
#define __NSLIB__DOMAIN__

#include <nslib/api.h>
#include <shift/Entity.h>
#include <shift/EntitiesTypes.h>
#include <shift/RelationshipPropertiesTypes.h>

#include <QMainWindow>
#include <QMenuBar>
#include <Eigen/Dense>

#include "DataLoader.h"


namespace nslib
{
  using Matrix4f = ::Eigen::Matrix4f;
  using Vector4f = ::Eigen::Vector4f;

  class NSLIB_API Domain
  {

  public:
    Domain( void );

    virtual ~Domain( void )
    {
    }

    virtual bool isSelectableEntity( shift::Entity* entity ) const = 0;

    virtual unsigned int selectableEntityId( shift::Entity* entity ) const = 0;

    virtual const Vector4f entity3DPosition( shift::Entity* entity ) const = 0;

    DataLoader* dataLoader( void );

    const shift::EntitiesTypes &entitiesTypes( void ) const;

    const shift::RelationshipPropertiesTypes &
    relationshipPropertiesTypes( void ) const;

    virtual void exportJSON( std::ostream &outputStream );

    virtual void importJSON( std::istream &inputStream );


    virtual void createGUI( QMainWindow* /* mw */, QMenuBar* /* menubar */ )
    {
    }

  protected:
    std::vector <std::string> _exportRelations;
    DataLoader* _dataLoader;
    shift::EntitiesTypes* _entitiesTypes;
    shift::RelationshipPropertiesTypes* _relationshipPropertiesTypes;
    std::string _domainName;

    virtual void exportRelationTypeToJSON( std::string relationName,
      std::ostream& outputStream );
    virtual void importEntityJSON( boost::property_tree::ptree entityJSON,
      shift::Entity*& entity, bool& isRootEntity, unsigned int& entityGID );


    virtual void importRelationshipsJSON(
      boost::property_tree::ptree relationships,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );

    virtual void addRelationsOfType( boost::property_tree::ptree  /*relations*/,
      std::string /*relationName*/, std::unordered_map
      < unsigned int, shift::Entity* >* /*oldGUIToEntity*/ ) = 0;

    virtual void importJSONRelationGIDS( boost::property_tree::ptree relation,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity,
      shift::Entity*& origEntity, shift::Entity*& destEntity,
      const std::string& /*relationName*/ );

    virtual void addConnectsToRelationsToJSON(
      boost::property_tree::ptree relations,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );

    virtual void addIsParentOfRelationshipsToJSON(
      boost::property_tree::ptree relations,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );

    void importEntititiesJSON( boost::property_tree::ptree entities,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );
  };
}

#endif //__NSLIB__DOMAIN__
