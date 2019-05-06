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
#ifndef __NSLIB_LAYOUTS__
#define __NSLIB_LAYOUTS__

#include "Layout.h"
#include <QComboBox>
#include <map>
#include <QtGui/QStandardItemModel>

namespace nslib
{

  class Layouts
  {
  public:
    Layouts( )
      : _layoutSelector( new QComboBox )
    {
    }
    void setLayout(
      unsigned int layoutIndex, Layout* layout, bool disabled = false )
    {
      _layouts[ layoutIndex ] = layout;
      _layoutSelector->insertItem( layoutIndex,
        QString( layout->name( ).c_str( )));
      QStandardItemModel* selectorModel =
        qobject_cast< QStandardItemModel* >( _layoutSelector->model( ));
      QStandardItem *item = selectorModel->item( layoutIndex );
      item->setFlags( disabled ? item->flags() & ~Qt::ItemIsEnabled
       : item->flags( ) | Qt::ItemIsEnabled );
    }

    QComboBox* layoutSelector( void )
    {
      return _layoutSelector;
    }

    Layout* getLayout( unsigned int index )
    {
      if ( _layouts.find( index ) == _layouts.end( ))
      {
        return nullptr;
      }
      else
      {
        return _layouts[ index ];
      }
    }

    std::map< unsigned int, Layout* >& map( void )
    {
      return _layouts;
    };

    const std::map< unsigned int, Layout* >& map( void ) const
    {
      return _layouts;
    };

  protected:
    std::map< unsigned int, Layout* > _layouts;
    QComboBox* _layoutSelector;
  };


}

#endif
