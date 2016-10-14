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
#include <QGraphicsScene>
#include <map>
#include <iostream>
#include <shift/shift.h>

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
    virtual ~Layout( void );
    const std::string& name( void );
    LayoutOptionsWidget* optionsWidget( void );

    void refresh( QGraphicsScene* scene )
    {
      displayItems( scene, _representations );
    }
    virtual void displayItems( QGraphicsScene* scene,
                               const shift::Representations& reps ) = 0;

  protected:
    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
    shift::Representations _representations;

  };


  class CameraBasedLayout : public Layout
  {
  public:
    CameraBasedLayout( void );
    void displayItems( QGraphicsScene* scene,
                       const shift::Representations& reps );

  };

  class ScatterplotLayout : public Layout
  {
  public:
    ScatterplotLayout( void );
    void displayItems( QGraphicsScene* scene,
                       const shift::Representations& reps );
  };

}

#endif
