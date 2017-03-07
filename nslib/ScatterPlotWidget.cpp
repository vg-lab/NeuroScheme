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
#include "layouts/Layout.h"
#include "ScatterPlotWidget.h"

namespace nslib
{
  ScatterPlotWidget::ScatterPlotWidget( Layout* parentLayout_,
                                        QWidget* parent_ )
    : QFrame( parent_ )
    , _parentLayout( parentLayout_ )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    this->setLayout( layout_ );

    _xLabel = new QLabel( "X: " );
    _yLabel = new QLabel( "Y: " );

    _propertyXSelector = new QComboBox( );
    _propertyXSelector->setSizePolicy( QSizePolicy::Expanding,
                                        QSizePolicy::Expanding );
    // _propertyXSelector->setMinimumSize( 190, 20 );

    _propertyYSelector = new QComboBox( );
    _propertyYSelector->setSizePolicy( QSizePolicy::Expanding,
                                        QSizePolicy::Expanding );
    // _propertyYSelector->setMinimumSize( 190, 20 );

    _scaleLabel = new QLabel( "Scale size: " );
    _scaleSlider = new QSlider( Qt::Horizontal );
    _scaleSlider->setSizePolicy( QSizePolicy::Expanding,
                                 QSizePolicy::Expanding );
    // _scaleSlider->setMinimumSize( 150, 20 );
    _scaleSlider->setMinimum( 0);
    _scaleSlider->setMaximum( 100 );
    _scaleSlider->setValue( 20 );

    layout_->addWidget( _scaleLabel, 0, 0, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _scaleSlider, 0, 1, 1, 2,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _xLabel, 1, 0, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _propertyXSelector, 1, 1, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _yLabel, 2, 0, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _propertyYSelector, 2, 1, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );

    connect( _propertyXSelector, SIGNAL( currentIndexChanged( int )),
             this, SLOT( _propertiesChanged( )));
    connect( _propertyYSelector, SIGNAL( currentIndexChanged( int )),
             this, SLOT( _propertiesChanged( )));
    connect( _scaleSlider, SIGNAL( valueChanged( int )),
             this, SLOT( refreshParentLayout( )));

  }

  void ScatterPlotWidget::blockChildrenSignals( bool block )
  {
    _propertyXSelector->blockSignals( block );
    _propertyYSelector->blockSignals( block );
    _scaleSlider->blockSignals( block );
  }

  void ScatterPlotWidget::refreshParentLayout( void )
  {
    // refresh witout animation
    _parentLayout->refresh( false );
  }


  void ScatterPlotWidget::_propertiesChanged( void )
  {
    // refresh witout animation
    _parentLayout->refresh( true );
  }

  ScatterPlotWidget::~ScatterPlotWidget( void )
  {
    delete _propertyXSelector;
    delete _propertyYSelector;
    delete _xLabel;
    delete _yLabel;
    delete _scaleLabel;
    delete _scaleSlider;
  }

}
