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
#include "PaneManager.h"
#include <assert.h>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

namespace neuroscheme
{
  Canvas* PaneManager::_activePane = nullptr;
  PaneManager::TPanes PaneManager::_panes = PaneManager::TPanes( );
  QGridLayout* PaneManager::_layout = nullptr;

  Canvas* PaneManager::activePane( void )
  {
    return _activePane;
  }

  void PaneManager::activePane( Canvas* pane )
  {
    _activePane = pane;
    assert( _activePane->layouts( ).layoutSelector( ));

    if ( _layout )
    {

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


      _activePane->layoutChanged( _activePane->activeLayoutIndex( ));

      //std::cout << _activePane->layouts( ).layoutSelector( ) << " " << _activePane << std::endl;
    }
    // if ( _layout )
    // {
    //   auto item = _layout->itemAtPosition( 0, 0 );
    //   if ( item )
    //   {
    //     std::cout << "Replacing and deleting" << item->widget( ) << std::endl;
    //     //_layout->removeWidget( item->widget( ));
    //     _layout->replaceWidget( item->widget( ),
    //                             new QLabel( _activePane->name.c_str( )));
    //     assert( dynamic_cast< QPushButton* >( item->widget( )));
    //     delete item->widget( );
    //   }
    //   else
    //     _layout->addWidget( new QLabel( _activePane->name.c_str( )), 1, 0 );

    //   std::cout << "Adding selector" << std::endl;
    //   item = _layout->itemAtPosition( 1, 0 );
    //   if ( item )
    //   {
    //     //_layout->removeWidget( item->widget( ));
    //     assert( dynamic_cast< QComboBox* >( item->widget( )));
    //     _layout->replaceWidget( item->widget( ),
    //                             _activePane->layouts( ).layoutSelector( ));
    //     std::cout << "Replacing" << item->widget( ) << " with"
    //               << _activePane->layouts( ).layoutSelector( ) << std::endl;
    //   }
    //   else
    //     _layout->addWidget( _activePane->layouts( ).layoutSelector( ), 1, 0 );

    // }
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

}
