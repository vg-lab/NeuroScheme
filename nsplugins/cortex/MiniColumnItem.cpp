/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include "MiniColumnItem.h"
#include "NeuronItem.h"
#include <QPen>
#include <nslib/Config.h>

namespace nslib
{
  namespace cortex
  {
    const QColor BASE_COLOR = QColor( 126, 204, 145 );

    MiniColumnItem::MiniColumnItem( const MiniColumnRep& miniColumnRep,
                                    QGraphicsScene* scene_,
                                    const unsigned int size )
      : NeuronAggregationItem( )
      , _itemText( nullptr )
    {
      const NeuronRep* meanNeuron = new NeuronRep(
        miniColumnRep.getPropertyValue< NeuronRep >( "meanNeuron" ));
      const auto& layers =
        miniColumnRep.getPropertyValue< MiniColumnRep::Layers >( "layers" );
      const auto& neuronAggReps = miniColumnRep.getPropertyValue
        < MiniColumnRep::NeuronTypeAggregations >( "neuronTypeAggregations" );

      // Create the polygon for the basic column icon
      QPainterPath path_;
      QPolygon poly;

      const int iSize = static_cast<int>(size);
      QPoint pUL (-iSize/3, -iSize/24);
      QPoint pUM (       0, -iSize/6);
      QPoint pUR ( iSize/3, -iSize/24);

      QPoint pLR ( iSize/3,  iSize/24);
      QPoint pLM (       0,  iSize/6);
      QPoint pLL (-iSize/3,  iSize/24);

      poly << pLR << pLM << pLL
           << pUL << pUM << pUR;

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

      this->_parentRep = &( const_cast< MiniColumnRep& >( miniColumnRep ));

      if ( Config::showEntitiesName( ))
      {
        _itemText = new ItemText( QString::fromStdString( miniColumnRep
          .getPropertyValue< std::string >( "Entity name", "" )), this,
          0.4f, 1.0f );
      }
    }

    MiniColumnItem::~MiniColumnItem( void )
    {
      if(_itemText) delete _itemText;
    }
  } // namespace cortex
} // namespace nslib
