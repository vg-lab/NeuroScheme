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
#ifndef __NEUROSCHEME__CORTEX_DOMAIN_MANAGER__
#define __NEUROSCHEME__CORTEX_DOMAIN_MANAGER__

#include "../../DomainManager.h"
#include "Neuron.h"

namespace neuroscheme
{

  namespace cortex
  {
    class Domain
      : public ::neuroscheme::Domain
    {

    public:
      bool isSelectableEntity( shift::Entity* entity ) const
      {
        return dynamic_cast< Neuron* >( entity );
      }

      unsigned int selectableEntityId( shift::Entity* entity ) const
      {
        assert( dynamic_cast< Neuron* >( entity ));
        return dynamic_cast< Neuron* >( entity )->
          getProperty( "gid" ).value< uint >( );
      }

      const Vector4f entity3DPosition ( shift::Entity* entity ) const
      {
        // std::cout << entity->getProperty( "Position3D" ).type( ) << std::endl;
        return entity->getProperty( "Position3D" ).value< Vector4f >( );
      }

    };
  }
}

#endif
