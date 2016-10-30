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
#include "ScatterPlotLayout.h"
#include "mappers/VariableMapper.h"
#include "reps/QGraphicsItemRepresentation.h"
#include "RepresentationCreatorManager.h"

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
    std::cout << "ScatterPlotLayout::arranging" << std::endl;
    unsigned int margin = 150;

    // QGraphicsView* gv = _scene->views( )[0];
    auto width = _scene->width( );
    auto height = _scene->height( );
    std::cout << "arranging in scene " << width << " x " << height << std::endl;
    auto xMin = -width/2 + margin;
    auto xMax = width/2 - margin;
    auto yMin = -height/2 + margin;
    auto yMax = height/2 - margin;


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
    auto xMaxValue = _properties[ xProp ].rangeMax;
    auto xMinValue = _properties[ xProp ].rangeMin;
    auto yMaxValue = _properties[ yProp ].rangeMax;
    auto yMinValue = _properties[ yProp ].rangeMin;

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
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto graphicsItem = graphicsItemRep->item( _scene );
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
          // std::cout << xMax  << std::endl;
          // std::cout << posX << " " << posY << " scale: " << scale_ << std::endl;

          // if ( useOpacityForFilter &&
          //      filteredOutItems.count( graphicsItem ) == 1 )
          // {
          //   graphicsItem->setOpacity( 0.5f );
          // }
          // else
          //   graphicsItem->setOpacity( 1.0f );

          if ( obj && animate )
          {
            // std::cout << "ScatterPlotLayout::animateItem" << std::endl;
            animateItem( graphicsItem, scale_, QPoint( posX, posY ));
          }
          else
          {
            graphicsItem->setPos( posX, posY );
            graphicsItem->setScale( scale_ );
          }
          // std::cout << posX << " " << posY << " " << scale_ << std::endl;
        }
      }
    } // for all reps
  }


} // namespace neuroscheme
