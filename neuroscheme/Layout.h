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
#include <QGraphicsScene>
#include <QGridLayout>
#include <QObject>
#include <QPoint>
#include <QPushButton>
#include <QToolBox>
#include <map>
#include <iostream>
#include <shift/shift.h>
#include "SortWidget.h"
#include "FilterWidget.h"

namespace neuroscheme
{
  typedef struct
  {
    int rangeMin;
    int rangeMax;
  } TPropertyData;
  typedef std::map< std::string, TPropertyData > TProperties;

  class LayoutOptionsWidget : public QFrame
  {
  public:
    LayoutOptionsWidget( void );
    QGridLayout* layout( void );

  protected:
    QGridLayout* _layout;
  };

  class Layout : public QObject
  {
    Q_OBJECT;
  public:

    friend class FilterWidget;

    enum
    {
      SORT_ENABLED = 0x01,
      FILTER_ENABLED = 0x02,
    };


    Layout( const std::string& name_ = "unnamed",
            unsigned int flags_ = 0 );
    virtual ~Layout( void );
    const std::string& name( void );
    LayoutOptionsWidget* optionsWidget( void );

    void refresh( bool animate = true,
                  bool refreshProperties = true )
    {
      std::cout << "Layout::refresh" << std::endl;
      // displayItems( _representations, false );
      displayEntities( _entities, animate, refreshProperties );
    }

    virtual void displayEntities( const shift::Entities& entities,
                                  bool animate = true,
                                  bool refreshProperties = true );

    // virtual void displayItems( const shift::Representations& reps,
    //                            bool animate );

    void updateSelection( );
    void scene( QGraphicsScene* scene_ )
    {
      _scene = scene_;
    }
    virtual Layout* clone( void ) const = 0;

    void animateItem( QGraphicsItem* graphicsItem,
                      float toScale, const QPoint& toPos );

  public slots:
    void addedSortProperty( void )
    {
      std::cout << "<><><><> Added sorted propery" << std::endl;
    }
  protected:
    void _refreshProperties( void );
    void _drawCorners( );
    void _clearScene( );
    void _addRepresentations( const shift::Representations& reps );
    virtual void _arrangeItems( const shift::Representations& /* reps */,
                                bool /* animate */ ) {}
    virtual void _updateOptionsWidget( void );

    QGraphicsScene* _scene;
    unsigned int _flags;
    LayoutOptionsWidget* _optionsWidget;
    std::string _name;
    shift::Representations _representations;
    shift::Entities _entities;
    TProperties _properties;
    QToolBox* _toolbox;
    SortWidget* _sortWidget;
    FilterWidget* _filterWidget;
  };

  class ScatterplotLayout : public Layout
  {
  public:
    ScatterplotLayout( void );
    void displayItems( const shift::Representations& reps,
                       bool animate );
    Layout* clone( void ) const
    {
      return new ScatterplotLayout;
    }
  };

}

#endif
