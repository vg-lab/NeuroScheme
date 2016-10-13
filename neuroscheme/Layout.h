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
#ifndef __NEUROSCHEME_LAYOUT__
#define __NEUROSCHEME_LAYOUT__

#include <QComboBox>
#include <map>

namespace neuroscheme
{

  class LayoutOptionsWidget : public QWidget
  {
  };

  class Layout
  {
  public:
    Layout( const std::string& name_ = "unnamed" )
      : _optionsWidget( nullptr )
      , _name( name_ )
    {
    }

    const std::string& name( void )
    {
      return _name;
    }

    LayoutOptionsWidget* optionsWidget( void )
    {
      return _optionsWidget;
    }

  protected:
    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
  };

  class Layouts
  {
  public:
    Layouts( )
      : _nextLayoutIdx( 0 )
      , _layoutSelector( new QComboBox )
    {
    }
    void addLayout( Layout* layout )
    {
      _layouts[ _nextLayoutIdx ] = layout;
      _layoutSelector->insertItem( _nextLayoutIdx,
                                   QString( layout->name( ).c_str( )));
    }

    QComboBox* layoutSelector( void )
    {
      return _layoutSelector;
    }
  protected:
    unsigned int _nextLayoutIdx;
    std::map< unsigned int, Layout* > _layouts;
    QComboBox* _layoutSelector;
  };

  class GridLayout : public Layout
  {
  public:
    GridLayout( void )
      : Layout( "Grid" )
    {
    }
  };

  class CameraBasedLayout : public Layout
  {
  public:
    CameraBasedLayout( void )
      : Layout( "3D" )
    {
    }
  };

  class ScatterplotLayout : public Layout
  {
  public:
    ScatterplotLayout( void )
      : Layout( "Scatterplot" )
    {
    }
  };

}

#endif
