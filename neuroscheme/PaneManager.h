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
#ifndef __NEUROSCHEME_PANE_MANAGER__
#define __NEUROSCHEME_PANE_MANAGER__

#include "Canvas.h"
#include <set>
#include <QGridLayout>

namespace neuroscheme
{
  class PaneManager
  {
  public:
    typedef std::set< Canvas* > TPanes;
    static void mainLayout( QGridLayout* );
    static Canvas* activePane( void );
    static void activePane( Canvas* );
    static TPanes& panes( void );
    static QGridLayout* layout( void );
    static void layout( QGridLayout* );
    static void updateSelection( void )
    {
      for ( auto canvas : _panes )
      {
        canvas->layouts( ).getLayout(
          canvas->activeLayoutIndex( ))->updateSelection( );
      }
    }

    typedef enum
    {
      HORIZONTAL,
      VERTICAL
    } TPaneDivision;
    
    static Canvas* newPane( Canvas* orig = nullptr,
                            TPaneDivision division = HORIZONTAL );

  protected:
    static QGridLayout* _mainGridLayout;
    static Canvas* _activePane;
    static TPanes _panes;
    static QGridLayout* _layout;
    static unsigned int _paneNextNumber;
    //TODO this is just a PoC, not final
    static unsigned int _nextRow, _nextColumn;
  };
} // namespace neuroscheme

#endif // __NEUROSCHEME_LAYOUT_MANAGER__
