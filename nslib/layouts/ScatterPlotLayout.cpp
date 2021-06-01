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
#include "../Loggers.h"
#include "ScatterPlotLayout.h"
#include "../mappers/VariableMapper.h"
#include "../reps/QGraphicsItemRepresentation.h"
#include "../reps/Item.h"
#include "../RepresentationCreatorManager.h"

namespace nslib
{
  ScatterPlotLayout::ScatterPlotLayout( void )
    : Layout( "ScatterPlot", Layout::SCATTERPLOT_ENABLED )
  {}

  void ScatterPlotLayout::_arrangeItems(
    const shift::Representations& reps ,
    bool animate,
    const shift::Representations& )
  {
    if ( reps.size( ) == 0 )
    {
      Loggers::get( )->log( " empty set of reps to arrange.",
                           LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE );
      return;
    }
    constexpr unsigned int margin = 150;

    QGraphicsView* gv = _canvas->scene( ).views( )[0];
    const auto width = gv->width( );
    const auto height = gv->height( );
    const float xMin = -float( width ) / 2.0f + margin;
    const float xMax = width/2 - margin;
    const float yMin = -float( height ) / 2.0f + margin;
    const float yMax = height / 2.0f - margin;

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

    const auto xProp =
      _scatterPlotWidget->propertyXSelector( )->currentText( ).toStdString( );
    const auto yProp =
      _scatterPlotWidget->propertyYSelector( )->currentText( ).toStdString( );

    if ( xProp.empty( ) || yProp.empty( ))
      return;

    const float xMaxValue = _canvas->properties( ).at( xProp ).rangeMax;
    const float xMinValue = _canvas->properties( ).at( xProp ).rangeMin;
    const float yMaxValue = _canvas->properties( ).at( yProp ).rangeMax;
    const float yMinValue = _canvas->properties( ).at( yProp ).rangeMin;

    MapperFloatToFloat xMapper( xMinValue, xMaxValue, xMin, xMax );
    MapperFloatToFloat yMapper( yMinValue, yMaxValue, yMin, yMax );

    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        Loggers::get( )->log( "Item null", LOG_LEVEL_WARNING );
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
          auto xVal = entity->hasProperty( xProp ) ?
            fires::PropertyManager::getPropertyCaster( xProp )->toInt(
            entity->getProperty( xProp )) : 0;
          auto yVal = entity->hasProperty( yProp ) ?
            fires::PropertyManager::getPropertyCaster( yProp )->toInt(
            entity->getProperty( yProp )) : 0;

          qreal posX = xMapper.map( xVal );
          qreal posY = - yMapper.map( yVal );
          if ( posX != posX ) posX = 0;
          if ( posY != posY ) posY = 0;
          const qreal scale_ = _scatterPlotWidget->scale( ) / 100.0f;

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

} // namespace nslib
