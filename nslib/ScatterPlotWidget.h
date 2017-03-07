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
#ifndef __NSLIB__SCATTERPLOT_WIDGET__
#define __NSLIB__SCATTERPLOT_WIDGET__

#include <nslib/api.h>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QObject>
#include <QSignalMapper>
#include <QToolButton>
#include <fires/fires.h>

namespace nslib
{
  class Layout;

  class NSLIB_API ScatterPlotWidget : public QFrame
  {
    Q_OBJECT;
  public:
    ScatterPlotWidget( Layout* parentLayout_, QWidget* parent_ = 0 );
    ~ScatterPlotWidget( void );
    void blockChildrenSignals( bool block );
    QComboBox* propertyXSelector( void ) { return _propertyXSelector; }
    QComboBox* propertyYSelector( void ) { return _propertyYSelector; }
    int scale( void ) { return _scaleSlider->value( ); }
public slots:
    void refreshParentLayout( void );

  protected slots:
    void _propertiesChanged( void );
  protected:
    Layout* _parentLayout;
    QLabel* _xLabel;
    QLabel* _yLabel;
    QComboBox* _propertyXSelector;
    QComboBox* _propertyYSelector;
    QLabel* _scaleLabel;
    QSlider* _scaleSlider;
  };

}

#endif
