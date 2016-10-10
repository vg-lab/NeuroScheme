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
#ifndef __NEUROSCHEME_LAYOUT_MANAGER__
#define __NEUROSCHEME_LAYOUT_MANAGER__

#include <QGraphicsScene>
#include <shift/shift.h>
#include "reps/SelectableItem.h"

namespace neuroscheme
{
  class LayoutManager
  {
  public:

    static void setScene( QGraphicsScene* scene )
    {
      _scene = scene;
    }

    static void update( void )
    {
      std::cout << "----> update items in scene " << _scene << std::endl;
      displayItems( _representations[ _scene ] );
    }

    static void updateAllScenes( void )
    {
      std::cout << "----> update items in scene " << _scene << std::endl;
      for ( const auto& scene : _scenes )
      {
        _scene = scene;
        displayItems( _representations[ _scene ] );
      }
    }

    static std::set< QGraphicsScene* >& scenes( void )
    {
      return _scenes;
    }

    static void updateSelection( void );

    static void updateAllScenesSelection( void )
    {
      std::cout << "----> update items in scene " << _scene << std::endl;
      for ( const auto& scene : _scenes )
      {
        _scene = scene;
        updateSelection( );
      }
    }

    static void displayItems( const shift::Representations& reps,
                              bool clearFirst = true );

  protected:
    static QGraphicsScene* _scene;
    static std::set< QGraphicsScene* > _scenes;
    static std::map< QGraphicsScene*, shift::Representations > _representations;

  };
} // namespace neuroscheme

#endif // __NEUROSCHEME_LAYOUT_MANAGER__
