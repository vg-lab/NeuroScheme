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
#include "DataLoader.h"
#include <nslib/DataManager.h>
#include <nslib/PaneManager.h>
#include <nslib/RepresentationCreatorManager.h>
#include "RepresentationCreator.h"
#include <shift_NeuronPop.h>
#include <random>

namespace nslib
{
  namespace congen
  {
    using NeuronPop = shiftgen::NeuronPop;

    bool DataLoader::loadData( const ::nslib::NeuroSchemeInputArguments& /* args */ )
    {
      nslib::Log::log( "Loading data for congen", LOG_LEVEL_VERBOSE );

      // auto& _entities = nslib::DataManager::entities( );
      // auto& _rootEntities = nslib::DataManager::rootEntities( );
      // fires::PropertyManager::clear( );
      // _entities.clear( );
      // _rootEntities.clear( );

      // _entities.relationships( )[ "isParentOf" ] =
      //   new shift::RelationshipOneToN;
      // _entities.relationships( )[ "isChildOf" ] =
      //   new shift::RelationshipOneToOne;

      // _entities.relationships( )[ "isAGroupOf" ] =
      //   new shift::RelationshipOneToN;
      // _entities.relationships( )[ "isPartOf" ] =
      //   new shift::RelationshipOneToN;

      // _entities.relationships( )[ "isSuperEntityOf" ] =
      //   new shift::RelationshipOneToN;
      // _entities.relationships( )[ "isSubEntityOf" ] =
      //   new shift::RelationshipOneToOne;

      // auto& relParentOf =
      //   *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      // auto& relChildOf =
      //   *( _entities.relationships( )[ "isChildOf" ]->asOneToOne( ));

      // shift::Entity* neuronPop = new NeuronPop( 50 );
      // _rootEntities.add( neuronPop );
      // _entities.add( neuronPop );

      // std::default_random_engine generator;
      // std::uniform_int_distribution< int > distribution( 0,100 );
      // // int dice_roll = distribution( generator );
      // for ( unsigned int i = 0; i < 10; ++i )
      // {
      //   shift::Entity* neuronPop2 = new NeuronPop( distribution( generator ));
      //   shift::Relationship::Establish( relParentOf, relChildOf,
      //                                   neuronPop, neuronPop2 );
      //   _entities.add( neuronPop2 );
      // }

      // auto repCretor = new RepresentationCreator( );
      // nslib::RepresentationCreatorManager::addCreator( repCretor );

      return true;
    }
  } // namespace congen
} // namespace nslib
