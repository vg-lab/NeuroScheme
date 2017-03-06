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
#include "../Log.h"
#include "ScatterPlotLayout.h"
#include "../mappers/VariableMapper.h"
#include "../reps/QGraphicsItemRepresentation.h"
#include "../reps/Item.h"
#include "../RepresentationCreatorManager.h"

namespace neuroscheme
{

  ScatterPlotLayout::ScatterPlotLayout( void )
    : Layout( "ScatterPlot", Layout::SCATTERPLOT_ENABLED )
  {
  }


  void ScatterPlotLayout::_arrangeItems(
    const shift::Representations& reps ,
    bool animate,
    const shift::Representations&
    // preFilterReps =
    // shift::Representations( )
   )
  {
    if ( reps.size( ) == 0 )
    {
      Log::log( NS_LOG_HEADER + " empty set of reps to arrange.",
                LOG_LEVEL_WARNING );
      return;
    }
    unsigned int margin = 150;

    QGraphicsView* gv = _canvas->scene( ).views( )[0];
    auto width = gv->width( );
    auto height = gv->height( );
    float xMin = -float( width ) / 2.0f + margin;
    float xMax = width/2 - margin;
    float yMin = -float( height ) / 2.0f + margin;
    float yMax = height / 2.0f - margin;

    // auto xAxis = new QGraphicsLineItem( -_scene->width( )/2 + margin,
    //                                     +_scene->height( )/2 - margin,
    //                                     +_scene->width( )/2 - margin,
    //                                     +_scene->height( )/2 - margin );
    // auto yAxis = new QGraphicsLineItem( -_scene->width( )/2 + margin,
    //                                     +_scene->height( )/2 - margin,
    //                                     -_scene->width( )/2 + margin,
    //                                     -_scene->height( )/2 + margin );
    // _scene->addItem( xAxis );
    // _scene->addItem( yAxis );

    auto xProp =
      _scatterPlotWidget->propertyXSelector( )->currentText( ).toStdString( );
    auto yProp =
      _scatterPlotWidget->propertyYSelector( )->currentText( ).toStdString( );

    if ( xProp.empty( ) || yProp.empty( ))
      return;
    float xMaxValue = _canvas->properties( ).at( xProp ).rangeMax;
    float xMinValue = _canvas->properties( ).at( xProp ).rangeMin;
    float yMaxValue = _canvas->properties( ).at( yProp ).rangeMax;
    float yMinValue = _canvas->properties( ).at( yProp ).rangeMin;

    MapperFloatToFloat xMapper( xMinValue, xMaxValue, xMin, xMax );
    MapperFloatToFloat yMapper( yMinValue, yMaxValue, yMin, yMax );

    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        Log::log( "Item null", LOG_LEVEL_WARNING );
      }
      else
      {
        auto graphicsItem = graphicsItemRep->item( &_canvas->scene( ));
        if ( graphicsItem->parentItem( ))
          continue;
        auto item = dynamic_cast< Item* >( graphicsItem );
        auto obj = dynamic_cast< QObject* >( graphicsItem );
        if ( item )
        {
          auto& entity = *( repsToEntities.at( representation ).begin( ));
          auto xVal = fires::PropertyManager::getPropertyCaster( xProp )->toInt(
            entity->getProperty( xProp ));
          auto yVal = fires::PropertyManager::getPropertyCaster( yProp )->toInt(
            entity->getProperty( yProp ));

          qreal posX = xMapper.map( xVal );
          qreal posY = - yMapper.map( yVal );
          if ( posX != posX ) posX = 0;
          if ( posY != posY ) posY = 0;
          qreal scale_ = _scatterPlotWidget->scale( ) / 100.0f;

          if ( obj && animate )
          {
            animateItem( graphicsItem, scale_, QPoint( posX, posY ));
          }
          else
          {
            graphicsItem->setPos( posX, posY );
            graphicsItem->setScale( scale_ );
          }
        }
      }
    } // for all reps
  }


} // namespace neuroscheme
