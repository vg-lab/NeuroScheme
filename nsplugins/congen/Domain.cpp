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

#include "Domain.h"
#include "RepresentationCreator.h"
#include <nslib/DataManager.h>
#include <nslib/RepresentationCreatorManager.h>
#include <shift_NeuronPop.h>
#include <shift_congen_entities.h>
#include <shift_congen_relationshipProperties.h>

namespace nslib
{
  namespace congen
  {
    using NeuronPop = shiftgen::NeuronPop;

    Domain::Domain( void )
    {
      this->_dataLoader = new DataLoader;
      this->_entitiesTypes = new nslib::congen::shiftgen::EntitiesTypes;
      this->_relationshipPropertiesTypes =
        new nslib::congen::shiftgen::RelationshipPropertiesTypes( );
      auto repCreator = new RepresentationCreator( );
      nslib::RepresentationCreatorManager::addCreator( repCreator );
      auto& _entities = nslib::DataManager::entities( );
      //fires::PropertyManager::clear( );
      _entities.clear( );

      _entities.relationships( )[ "isParentOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "isAGroupOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isPartOf" ] =
        new shift::RelationshipOneToN;

      _entities.relationships( )[ "isSuperEntityOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isSubEntityOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "connectsTo" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "connectedBy" ] =
        new shift::RelationshipOneToN;
    }

    bool Domain::isSelectableEntity( shift::Entity* entity ) const
    {
      return dynamic_cast< NeuronPop* >( entity );
    }

    unsigned int Domain::selectableEntityId( shift::Entity* /* entity */ ) const
    {
      return 0;
    }

    const Vector4f Domain::entity3DPosition ( shift::Entity* /* entity */ ) const
    {
      return Vector4f( 0.f, 0.f, 0.f, 0.f  );
    }

    void Domain::usageMessage( void )
    {
    }

  }
}
