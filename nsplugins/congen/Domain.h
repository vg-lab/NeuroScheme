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
#ifndef __NSLIB__CONGEN_DOMAIN_MANAGER__
#define __NSLIB__CONGEN_DOMAIN_MANAGER__

#include <nslibcongen/api.h>
#include <nslib/DomainManager.h>
#include "DataLoader.h"

namespace nslib
{

  namespace congen
  {

    class NSLIBCONGEN_API Domain
      : public ::nslib::Domain
    {

    public:

      Domain( void )
      {
        this->_dataLoader = new DataLoader;
      }

      virtual ~Domain( void )
      {
        delete _dataLoader;
      }

      bool isSelectableEntity( shift::Entity* entity ) const;
      unsigned int selectableEntityId( shift::Entity* entity ) const;
      const Vector4f entity3DPosition ( shift::Entity* entity ) const;
      static void usageMessage( void );
    };
  }
}

#endif
