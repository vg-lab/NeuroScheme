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
#ifndef __NSLIB_LAYOUT__
#define __NSLIB_LAYOUT__

#include <nslib/api.h>
#include <QFrame>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QObject>
#include <QPoint>
#include <QPushButton>
#include <QToolBox>
#include <map>
#include <iostream>
#include <shift/shift.h>
#include "../FilterWidget.h"
#include "../ScatterPlotWidget.h"
#include "../SortWidget.h"
#include "../Properties.h"

#define ANIM_DURATION 500

namespace nslib
{

  class GraphicsScene;
  class OpConfig : public shift::OpConfig
  {
  public:
    OpConfig( GraphicsScene* scene_ = nullptr, bool animating_=false,
      bool grid_ = false )
      : _scene( scene_ ), _animating(animating_), _grid(grid_)
    {}
    virtual ~OpConfig( void ) {};
    GraphicsScene* scene( void ) { return _scene; };
    bool isAnimating(){ return _animating; };
    bool isGrid(){ return _grid; };


    protected:
    GraphicsScene*  _scene;
    bool            _animating;
    bool            _grid;
  };

  class Canvas;

  class NSLIB_API LayoutOptionsWidget : public QFrame
  {
  public:
    LayoutOptionsWidget( void );
    QGridLayout* layout( void );

  protected:
    QGridLayout* _layout;
  };

  class NSLIB_API Layout : public QObject
  {
    Q_OBJECT;
  public:

    friend class FilterWidget;

    enum
    {
      SORT_ENABLED = 0x01,
      FILTER_ENABLED = 0x02,
      CAMERA_ENABLED = 0x04,
      SCATTERPLOT_ENABLED = 0x08
    };

    enum
    {
      UNDEFINED_LAYOUT = 0,
      CAMERA_BASED_LAYOUT = 1,
      CIRCULAR_LAYOUT = 2,
      GRID_LAYAOUT = 3,
      SCATTERPLOT_LAYOUT = 4
    };


    Layout( const std::string& name_ = "unnamed",
            unsigned int flags_ = 0 );

    virtual ~Layout( void );

    const std::string& name( void );

    const unsigned int& flags( void ) { return _flags; }

    LayoutOptionsWidget* optionsWidget( void );

    void refresh( bool animate = true );

    virtual void display( shift::Entities& entities,
                          shift::Representations& representations,
                          bool animate = true );

    void updateSelection( );

    void canvas( Canvas* canvas_ )
    {
      _canvas = canvas_;
    }
    virtual Layout* clone( void ) const = 0;

    void animateItem( QGraphicsItem* graphicsItem,
                      float toScale, const QPoint& toPos );

    void refreshWidgetsProperties( const TProperties& properties );


  protected:
    void _drawCorners( );
    void _clearScene( );
    void _addRepresentations( const shift::Representations& reps );
    virtual void _arrangeItems( const shift::Representations& /* reps */,
                                bool /* animate */,
                                const shift::Representations&
                                preFilterReps =
                                shift::Representations( ))
    { ( void ) preFilterReps; }
    virtual void _updateOptionsWidget( void );

    Canvas* _canvas;
    unsigned int _flags;
    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
    QToolBox* _toolbox;
    SortWidget* _sortWidget;
    FilterWidget* _filterWidget;
    ScatterPlotWidget* _scatterPlotWidget;
    unsigned int _layoutType;
  };

}

#endif
