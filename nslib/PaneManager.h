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
#ifndef __NSLIB_PANE_MANAGER__
#define __NSLIB_PANE_MANAGER__

#include <nslib/api.h>
#include "Canvas.h"
#include <set>
#include <QGridLayout>
#include <QSplitter>
#include <Eigen/Dense>
#include <chrono>

namespace nslib
{
  class NSLIB_API PaneManager
  {
  public:
    typedef std::set< Canvas* > TPanes;
    static void splitter( QSplitter* );
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

    static Canvas* newPane( Canvas* orig = nullptr );
    static Canvas* newPaneFromActivePane( void );
    static void killActivePane( void );
    static void killPane( Canvas* orig );

    static void setViewMatrix( const double* values );

    using Matrix4f = Eigen::Matrix4f;
    static const Matrix4f& viewMatrix( void ) { return _modelViewMatrix; }

    static void paneDivision( TPaneDivision paneDivision_ )
    {
      _paneDivision = paneDivision_;
    }
  protected:
    static QSplitter* _splitter;
    static Canvas* _activePane;
    static TPanes _panes;
    static QGridLayout* _layout;
    static unsigned int _paneNextNumber;
    //TODO this is just a PoC, not final
    static unsigned int _nextRow, _nextColumn;

    static Matrix4f _modelViewMatrix;
    static std::chrono::time_point< std::chrono::system_clock > lastMatrixClock;
    static TPaneDivision _paneDivision;
  };
} // namespace nslib

#endif // __NSLIB_PANE_MANAGER__
