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
#include "MiniColumnItem.h"
#include "NeuronItem.h"
#include <QPen>

namespace nslib
{
  namespace cortex
  {

    MiniColumnItem::MiniColumnItem( const MiniColumnRep& miniColumnRep,
                                    QGraphicsScene* scene_,
                                    unsigned int size )
      : NeuronAggregationItem( )
    {
      // std::cout << "props: " << miniColumnRep.properties( ).size( ) << std::endl;;
      // for ( const auto p : miniColumnRep.properties( ))
      //   std::cout << p.first << std::endl;
      // const auto& shiftNeuronRep =
      //   miniColumnRep.getProperty( "meanNeuron" ).value< shiftgen::NeuronRep >( );
      // NeuronRep meanNeuron( shiftNeuronRep );

      const NeuronRep& meanNeuron =
        miniColumnRep.getProperty( "meanNeuron" ).value< NeuronRep >( );
      const auto& layers =
        miniColumnRep.getProperty( "layers" ).value< MiniColumnRep::Layers >( );
      const auto& neuronAggReps =
        miniColumnRep.getProperty( "neuronTypeAggregations" ).
        value< MiniColumnRep::NeuronTypeAggregations >( );

      // Create the polygon for the basic column icon
      QPainterPath path_;
      QPolygon poly;

      QPoint pUL (-int(size)/3, -int(size)/24);
      QPoint pUM (           0, -int(size)/6);
      QPoint pUR ( int(size)/3, -int(size)/24);

      QPoint pLR ( int(size)/3, +int(size)/24);
      QPoint pLM (           0, +int(size)/6);
      QPoint pLL (-int(size)/3, +int(size)/24);

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
        QColor( 126, 204, 145 ),
        size );

      this->_parentRep = &( const_cast< MiniColumnRep& >( miniColumnRep ));

      //  this->setBrush( QBrush( QColor( 114, 188, 196 )));
    }
  } // namespace cortex
} // namespace nslib
