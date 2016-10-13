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

#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <map>
#include <iostream>

namespace neuroscheme
{

  class LayoutOptionsWidget : public QFrame
  {
  public:
    LayoutOptionsWidget( void );
    QGridLayout* layout( void );

  protected:
    QGridLayout* _layout;
  };

  class Layout
  {
  public:
    Layout( const std::string& name_ = "unnamed" );
    ~Layout( void );
    const std::string& name( void );
    LayoutOptionsWidget* optionsWidget( void );

  protected:
    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
  };


  class CameraBasedLayout : public Layout
  {
  public:
    CameraBasedLayout( void );
  };

  class ScatterplotLayout : public Layout
  {
  public:
    ScatterplotLayout( void );
  };

}

#endif
