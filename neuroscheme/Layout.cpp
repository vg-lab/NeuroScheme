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
#include "Layout.h"
#include "reps/Item.h"
namespace neuroscheme
{

  LayoutOptionsWidget::LayoutOptionsWidget( void )
    : _layout( new QGridLayout )
  {
    this->setLayout( _layout );
  }

  QGridLayout* LayoutOptionsWidget::layout( void )
  {
    return _layout;
  }

  Layout::Layout( const std::string& name_ )
    : _optionsWidget( new LayoutOptionsWidget )
    , _name( name_ )
  {
  }

  Layout::~Layout( void )
  {
    delete _optionsWidget;
  }

  const std::string& Layout::name( void )
  {
    return _name;
  }

  LayoutOptionsWidget* Layout::optionsWidget( void )
  {
    return _optionsWidget;
  }

  CameraBasedLayout::CameraBasedLayout( void )
    : Layout( "3D" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D1" ), 0, 0 );
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D2" ), 0, 1 );

  }

  void CameraBasedLayout::displayItems( QGraphicsScene* scene_,
                                        const shift::Representations& reps )
  {
    ( void ) reps;
    ( void ) scene_;
    if ( !scene_ ) return;
    auto& scene = *scene_;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }

  }

  ScatterplotLayout::ScatterplotLayout( void )
    : Layout( "Scatterplot" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton(
                                            "hola scatterplot" ));
  }


  void ScatterplotLayout::displayItems( QGraphicsScene* scene_,
                                        const shift::Representations& reps )
  {
    ( void ) reps;
    ( void ) scene_;
    if ( scene_ ) return;
    auto& scene = *scene_;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }

  }

}
