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
#include <nslib/error.h>
#include <nslib/reps/CollapseButtonItem.h>
#include "ColumnItem.h"
#include "NeuronItem.h"
#include <QPen>
#include <nslib/Config.h>

namespace nslib
{
  namespace cortex
  {
    const QColor BASE_COLOR = QColor( 114, 188, 196 );

    ColumnItem::ColumnItem( const ColumnRep& columnRep,
                            QGraphicsScene* scene_,
                            const unsigned int size )
      : NeuronAggregationItem( )
      , _itemText( nullptr )
    {

      const NeuronRep* meanNeuron = new NeuronRep(
        columnRep.getPropertyValue< NeuronRep >( "meanNeuron" ));
      const auto& layers =
        columnRep.getPropertyValue< ColumnRep::Layers >( "layers" );
      const auto& neuronAggReps = columnRep.getPropertyValue
        < ColumnRep::NeuronTypeAggregations >( "neuronTypeAggregations" );
      // Create the polygon for the basic column icon
      QPainterPath path_;
      QPolygon poly;

      const int iSize = static_cast<int>(size);
      QPoint pUL ( -iSize/3, -iSize/6 );
      QPoint pUR (  iSize/3, -iSize/6 );
      QPoint pLR (  iSize/3,  iSize/24 );
      QPoint pLM (        0,  iSize/6 );
      QPoint pLL ( -iSize/3,  iSize/24 );

      poly << pUL << pUR << pLR << pLM << pLL;

      path_.addPolygon( poly );
      path_.closeSubpath(  );

      _createNeuronAggregationItem(
        scene_,
        meanNeuron,
        layers,
        neuronAggReps,
        path_,
        pLL, pLM, pLR,
        BASE_COLOR,
        size );

      if ( Config::showEntitiesName( ))
      {
        _itemText = new ItemText( QString::fromStdString( columnRep
          .getPropertyValue< std::string >( "Entity name", "" )), this,
          0.4f, 1.0f );
      }

      this->_parentRep = &( const_cast< ColumnRep& >( columnRep ));
    }

    ColumnItem::~ColumnItem( void )
    {
      if(_itemText) delete _itemText;
    }

  } // namespace cortex
} // namespace nslib
