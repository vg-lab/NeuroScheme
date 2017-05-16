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
#ifndef __NSLIB__CANVAS__
#define __NSLIB__CANVAS__

#include <nslib/api.h>
#include "layouts/Layouts.h"
#include "Properties.h"
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <iostream>

namespace nslib
{

  class NSLIB_API GraphicsView : public QGraphicsView
  {

    Q_OBJECT;

  public:
    GraphicsView( QWidget* parent = 0 );
    void mousePressEvent( QMouseEvent* /* event */ );

  protected:
    virtual void wheelEvent( QWheelEvent* event_ );

  }; // class GraphicsView

  class GraphicsScene : public QGraphicsScene
  {
    // void mousePressEvent( QMouseEvent* /* event */ )
    // {
    //   PaneManager::activePane( dynamic_cast< Canvas* >( this->parentWidget( )));
    // }

  }; // class GraphicsScene


  class NSLIB_API Canvas : public QFrame
  {
    Q_OBJECT
  public:
    Canvas( QWidget* parent = 0 );
    ~Canvas( void );

    const GraphicsScene& scene( void ) const;
    GraphicsScene& scene( void );

    const GraphicsView& view( void ) const;
    GraphicsView& view( void );

    void enterEvent( QEvent* ) final;
    void leaveEvent( QEvent* ) final;
    void mousePressEvent( QMouseEvent* ) final;
    void keyPressEvent( QKeyEvent* ) final;
    void resizeEvent( QResizeEvent* ) final;
    void showEvent( QShowEvent* ) final;
    void connectLayoutSelector( void );
    const Layouts& layouts( void ) const;
    Layouts& layouts( void );
    void addLayout( Layout* );

    int activeLayoutIndex( void );
    void activeLayoutIndex( int );

    std::string name;

    void displayEntities( shift::Entities& entities_, bool animate,
                          bool refreshProperties );
//    void displayReps( shift::Representations&, bool animate );
    Canvas* clone( void ) const;
    const shift::Representations& reps( void ) const;
    shift::Representations& reps( void );
    shift::Entities& entities( void ) { return _entities; }
    const TProperties& properties( void ) { return _properties; }
    void refreshProperties( const shift::Entities& entities_ );


protected:
    GraphicsView* _graphicsView;
    GraphicsScene* _graphicsScene;
    Layouts _layouts;
    int _activeLayoutIndex;

    shift::Representations _reps;
    shift::Entities _entities;
    TProperties _properties;

  public slots:
    void layoutChanged( int );
  }; // class Canvas



} // namespace nslib

#endif // __NSLIB__CANVAS__