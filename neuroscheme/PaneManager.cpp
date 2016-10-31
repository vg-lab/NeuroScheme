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
#include "DataManager.h"
#include "PaneManager.h"
#include <assert.h>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

namespace neuroscheme
{
  QSplitter* PaneManager::_splitter = nullptr;
  Canvas* PaneManager::_activePane = nullptr;
  PaneManager::TPanes PaneManager::_panes = PaneManager::TPanes( );
  QGridLayout* PaneManager::_layout = nullptr;
  unsigned int PaneManager::_paneNextNumber = 0;
  unsigned int PaneManager::_nextRow = 0;
  unsigned int PaneManager::_nextColumn = 0;
  Eigen::Matrix4f PaneManager::_modelViewMatrix =
    Eigen::Matrix4f::Identity( );
  std::chrono::time_point< std::chrono::system_clock >
  PaneManager::lastMatrixClock = std::chrono::system_clock::now( );

  void PaneManager::splitter( QSplitter* splitter_ )
  {
    _splitter = splitter_;
  }

  Canvas* PaneManager::activePane( void )
  {
    return _activePane;
  }

  void PaneManager::activePane( Canvas* pane )
  {
    _activePane = pane;
    assert( _activePane->layouts( ).layoutSelector( ));

    pane->setStyleSheet("#pane { border: 3px dotted rgba( 0,0,0,15%); }");
    for ( auto& otherPane : _panes )
      if ( pane != otherPane )
        otherPane->setStyleSheet(
          "#pane { border: 3px dotted rgba( 0,0,0,0%); }" );

    if ( _layout )
    {

      // Replace pane name
      auto item = _layout->itemAtPosition( 0, 0 );
      if ( item )
      {
        auto widget = item->widget( );
        if ( widget )
        {
          auto index = _layout->indexOf( widget );
          if ( index != -1 )
          {
            _layout->takeAt( index );
            widget->hide( );
          }
        }
      }
      _layout->addWidget( new QLabel( _activePane->name.c_str( )), 0, 0 );


      // Replace layout selector
      item = _layout->itemAtPosition( 1, 0 );
      if ( item )
      {
        auto widget = item->widget( );
        if ( widget )
        {
          auto index = _layout->indexOf( widget );
          if ( index != -1 )
          {
            _layout->takeAt( index );
            widget->hide( );
          }
        }
      }
      _layout->addWidget( _activePane->layouts( ).layoutSelector( ), 1, 0 );
      _activePane->layouts( ).layoutSelector( )->show( );


      // Replace layout options
      item = _layout->itemAtPosition( 2, 0 );
      if ( item )
      {
        auto widget = item->widget( );
        if ( widget )
        {
          auto index = _layout->indexOf( widget );
          if ( index != -1 )
          {
            _layout->takeAt( index );
            widget->hide( );
          }
        }
      }
      auto layoutIdx = _activePane->activeLayoutIndex( );
      auto activeLayout = _activePane->layouts( ).getLayout( layoutIdx );
      _layout->addWidget( activeLayout->optionsWidget( ) , 2, 0 );
      activeLayout->optionsWidget( )->show( );

    }
  }

  PaneManager::TPanes& PaneManager::panes( void )
  {
    return _panes;
  }

  QGridLayout* PaneManager::layout( void )
  {
    return _layout;
  }

  void PaneManager::layout( QGridLayout* layout_ )
  {
    _layout = layout_;
  }

  Canvas* PaneManager::newPane( Canvas* orig, TPaneDivision division )
  {
    ( void ) division;
    assert( _splitter );
    Canvas* canvas;
    if ( !orig )
    {
      canvas = new Canvas( _splitter->parentWidget( ));
      _splitter->addWidget( canvas ); //, _nextRow, _nextColumn );
      canvas->connectLayoutSelector( );
    }
    else
    {
      canvas = orig->clone( );
      _splitter->addWidget( canvas );
      auto sizes = _splitter->sizes( );
      for ( auto& size_ : sizes )
        size_ = 100;
      _splitter->setSizes( sizes );
    }

    canvas->name = std::string( "Pane ") + std::to_string( _paneNextNumber++ );
    _panes.insert( canvas );
    canvas->show( );

    return canvas;
  }

  void PaneManager::setViewMatrix( const double* values )
  {
    _modelViewMatrix.row( 0 ) =
      Vector4f( values[0], values[4], values[8], values[12] );
    _modelViewMatrix.row( 1 ) =
      Vector4f( values[1], values[5], values[9], values[13] );
    _modelViewMatrix.row( 2 ) =
      Vector4f( values[2], values[6], values[10], values[14] );
    _modelViewMatrix.row( 3 ) =
      Vector4f( values[3], values[7], values[11], values[15] );

    int elapsedMs = std::chrono::duration_cast< std::chrono::milliseconds >
      ( std::chrono::system_clock::now( ) - lastMatrixClock ).count( );

    // Only update if 50ms passed
    if ( elapsedMs > 50 )
    {
      for ( auto canvas : _panes )
      {
        auto layout
          = canvas->layouts( ).getLayout( canvas->activeLayoutIndex( ));
        if ( layout->flags( ) & Layout::CAMERA_ENABLED )
          layout->refresh( false, false );
      }
      lastMatrixClock = std::chrono::system_clock::now( );
    }
  }


} // namespace neuroscheme
