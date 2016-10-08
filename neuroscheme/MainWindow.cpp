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
#include "MainWindow.h"
#include "LayoutManager.h"
#include "DataManager.h"

#include <QGridLayout>

MainWindow::MainWindow( QWidget* parent_ )
  : QMainWindow( parent_ )
  , _ui( new Ui::MainWindow )
{

  auto gl = new QGridLayout;
  gl->setContentsMargins( 0, 0, 0, 0 );
  gl->setSpacing( 0 );
  QWidget* widget = new QWidget( this );
  widget->setLayout( gl );
  this->setCentralWidget( widget );

  neuroscheme::DataManager::loadData( );


  _canvasses.push_back( new neuroscheme::Canvas( this ));
  gl->addWidget( _canvasses[0], 1, 1 );
  neuroscheme::LayoutManager::setScene( &_canvasses[0]->scene( ));
  neuroscheme::LayoutManager::displayItems(
    neuroscheme::DataManager::representations( ), true );
  neuroscheme::LayoutManager::scenes( ).insert( &_canvasses[0]->scene( ));

  _canvasses.push_back( new neuroscheme::Canvas( this ));
  gl->addWidget( _canvasses[1], 1, 2 );
  neuroscheme::LayoutManager::setScene( &_canvasses[1]->scene( ));
  neuroscheme::LayoutManager::displayItems(
    neuroscheme::DataManager::representations( ), true );
  neuroscheme::LayoutManager::scenes( ).insert( &_canvasses[1]->scene( ));
}

void MainWindow::resizeEvent( QResizeEvent* /* event_ */ )
{
  std::cout << "MainWindow::resizeEvent" << std::endl;
  for ( const auto& _canvas : _canvasses )
  {
    const QSize viewerSize = _canvas->view( ).size( );
    const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                                 - viewerSize.height( ) / 2,
                                 viewerSize.width( ) -2,
                                 viewerSize.height( ) -2);

    QTransform transf = _canvas->view( ).transform( );
    _canvas->view( ).fitInView( rectf, Qt::KeepAspectRatio );
    _canvas->view( ).setSceneRect( rectf );
    _canvas->view( ).setTransform( transf );

    // std::cout << _canvas->view( ).width( ) << " x "
    //           << _canvas->view( ).height( ) << std::endl;

    neuroscheme::LayoutManager::setScene( &_canvas->scene( ));
    neuroscheme::LayoutManager::update( );
  }

}



MainWindow::~MainWindow( void )
{
  delete _ui;
  for ( const auto& _canvas : _canvasses )
    delete _canvas;
}
