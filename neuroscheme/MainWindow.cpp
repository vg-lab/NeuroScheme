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

// OJO PRUEBAS
#include "reps/QGraphicsItemRepresentation.h"
#include "entities/Neuron.h"
#include "RepresentationCreator.h"
#include "RepresentationCreatorManager.h"
#include "LayoutManager.h"
#include "DataManager.h"


MainWindow::MainWindow( QWidget* parent_ )
  : QMainWindow( parent_ )
  , _ui( new Ui::MainWindow )
{

  _canvas = new neuroscheme::Canvas( this );
  this->setCentralWidget( _canvas );


  // shift::EntitiesWithRelationships entities;
  shift::Representations representations;

  neuroscheme::DataManager::loadData( );
  shift::Entities rootEntities;

  auto& relParentOf = *( neuroscheme::DataManager::entities( ).
                         relationships( )[ "isParentOf" ]->asOneToN( ));

  const auto& children = relParentOf[ 0 ];
  std::cout << "-- Root entities " << children.size( ) << std::endl;
  for ( const auto& child : children )
    rootEntities[child] =
      neuroscheme::DataManager::entities( )[child];

  //const auto& entities = neuroscheme::DataManager::entities( );
  // const auto& rootEntity = neuroscheme::DataManager::entities( )[1];
  // rootEntities[1] = rootEntity;
  neuroscheme::RepresentationCreatorManager::addCreator(
    new neuroscheme::RepresentationCreator );
  // neuroscheme::TEntitiesToReps objsToReps;
  // neuroscheme::TRepsToEntities repsToObjs;
  neuroscheme::RepresentationCreatorManager::create(
    rootEntities, representations,
    // objsToReps, repsToObjs
    true, true );

  // neuroscheme::LayoutManager::displayItems(
  //   _canvas->scene( ), representations );
  neuroscheme::LayoutManager::setScene( &_canvas->scene( ));
  neuroscheme::LayoutManager::displayItems( representations, true );

  _representations = representations;

}

void MainWindow::resizeEvent( QResizeEvent* /* event_ */ )
{
  std::cout << "MainWindow::resizeEvent" << std::endl;
  const QSize viewerSize = _canvas->view( ).size( );
  const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                               - viewerSize.height( ) / 2,
                               viewerSize.width( ) -2,
                               viewerSize.height( ) -2);

  QTransform transf = _canvas->view( ).transform( );
  _canvas->view( ).fitInView( rectf, Qt::KeepAspectRatio );
  _canvas->view( ).setSceneRect( rectf );
  _canvas->view( ).setTransform( transf );

  std::cout << _canvas->view( ).width( ) << " x "
            << _canvas->view( ).height( ) << std::endl;

  neuroscheme::LayoutManager::update( );

}



MainWindow::~MainWindow( void )
{
  delete _ui;
  delete _canvas;
}
