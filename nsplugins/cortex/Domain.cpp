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
#include "Neuron.h"
#include <nslib/RepresentationCreatorManager.h>
#include <nslib/DataManager.h>
#include "RepresentationCreator.h"
#include <shift_cortex_entities.h>

namespace nslib
{
  namespace cortex
  {
    Domain::Domain( void )
    {
      this->_dataLoader = new DataLoader;
      this->_entitiesTypes = new nslib::cortex::shiftgen::EntitiesTypes;
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
      return dynamic_cast< Neuron* >( entity );
    }

    unsigned int Domain::selectableEntityId( shift::Entity* entity ) const
    {
      assert( dynamic_cast< Neuron* >( entity ));
      return dynamic_cast< Neuron* >( entity )->
        getProperty( "gid" ).value< uint >( );
    }

    const Vector4f Domain::entity3DPosition ( shift::Entity* entity ) const
    {
      // std::cout << entity->getProperty( "Position3D" ).type( ) << std::endl;
      return entity->getProperty( "Position3D" ).value< Vector4f >( );
    }

    void Domain::usageMessage( void )
    {
      std::cerr << "\t\t[ -bc blue_config_path " // | " -swc swc_file_list "
                << " | -xml scene_xml ] "
                << std::endl
                << "\t\t[ -target target_label (*1) ] "
                << std::endl
                << "\t\t[ --no-morphologies | -nm ] "
                << std::endl
                << "\t\t[ -cns | --csv-neuron-stats ] csv_file"
                << std::endl
                << "\t\t[ -lc | --load-connectivity ] "
                << std::endl << std::endl
                << "\t\t(*1) only for BlueConfig files" << std::endl;

    }

  }
}
