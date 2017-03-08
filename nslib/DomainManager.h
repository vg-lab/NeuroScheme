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
#include "DataLoader.h"

namespace nslib
{
  using Matrix4f = ::Eigen::Matrix4f;
  using Vector4f = ::Eigen::Vector4f;

  class NSLIB_API Domain
  {

  public:
    Domain( void )
      : _dataLoader( nullptr )
    {
    }
    virtual ~Domain( void ) {}
    virtual bool isSelectableEntity( shift::Entity* entity ) const = 0;
    virtual unsigned int selectableEntityId( shift::Entity* entity ) const = 0;
    virtual const Vector4f entity3DPosition ( shift::Entity* entity ) const = 0;
    DataLoader* dataLoader( void ) { return _dataLoader; }

  protected:
    DataLoader* _dataLoader;
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
