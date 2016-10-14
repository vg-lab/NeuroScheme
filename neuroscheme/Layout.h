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
      std::cout << "Layout::refresh" << std::endl;
      displayItems( scene, _representations, false );
    }
    virtual void displayItems( QGraphicsScene* scene,
                               const shift::Representations& reps,
                               bool animate );

    void updateSelection( QGraphicsScene* scene );

    virtual Layout* clone( void ) const = 0;

  protected:
    void _drawCorners( QGraphicsScene* scene );
    void _clearScene( QGraphicsScene* scene );
    void _addRepresentations( QGraphicsScene* scene,
                              const shift::Representations& reps );
    virtual void _arrangeItems( QGraphicsScene* /* scene */,
                                const shift::Representations& /* reps */,
                                bool /* animate */ ) {}


    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
    shift::Representations _representations;

  };

  class ScatterplotLayout : public Layout
  {
  public:
    ScatterplotLayout( void );
    void displayItems( QGraphicsScene* scene,
                       const shift::Representations& reps,
                       bool animate );
    Layout* clone( void ) const
    {
      return new ScatterplotLayout;
    }
  };

}

#endif
