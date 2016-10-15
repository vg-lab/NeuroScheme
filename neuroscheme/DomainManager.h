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
#ifndef __NEUROSCHEME__DOMAIN_MANAGER__
#define __NEUROSCHEME__DOMAIN_MANAGER__

#include <shift/shift.h>
#include <Eigen/Dense>

namespace neuroscheme
{
  using Matrix4f = ::Eigen::Matrix4f;
  using Vector4f = ::Eigen::Vector4f;

  class Domain
  {

  public:
    virtual ~Domain( void ) {}
    virtual bool isSelectableEntity( shift::Entity* entity ) const = 0;
    virtual unsigned int selectableEntityId( shift::Entity* entity ) const = 0;
    virtual const Vector4f entity3DPosition ( shift::Entity* entity ) const = 0;
  };

  class DomainManager
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
