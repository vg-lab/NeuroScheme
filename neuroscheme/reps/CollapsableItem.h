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
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NEUROSCHEME__COLLAPSABLE_ITEM__
#define __NEUROSCHEME__COLLAPSABLE_ITEM__

namespace neuroscheme
{
  class CollapsableItem
  {
  public:

    CollapsableItem( bool collapsed_ = true )
      : _collapsed( collapsed_ ) { }
    virtual ~CollapsableItem( void ) { }
    virtual void toggleCollapse( void )
    {
      if ( _collapsed )
        this->uncollapse( );
      else
        this->collapse( );
    }
    virtual void collapse(bool anim = true) = 0;
    virtual void uncollapse(bool anim = true) = 0;
  protected:
    bool _collapsed;
  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__COLLAPSABLE_ITEM__
