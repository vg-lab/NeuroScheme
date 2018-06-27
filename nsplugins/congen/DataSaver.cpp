/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#include <iostream>
#include <QFileDialog>

#include <nslib/DataManager.h>

#include "NeuronPopItem.h"
#include "DataSaver.h"
#include <shift_NeuronPop.h>

namespace nslib
{
  namespace congen
  {
    void DataSaver::saveXmlScene( QWidget* ptr )
    {
      QString fileName = QFileDialog::getSaveFileName( ptr,
         "Save Scene to NeuroML format","",
         QObject::tr("NeuroML (*.xml);;All Files (*)"));
      if ( fileName != "" )
      {
        XMLExporter* exporter_ = new XMLExporter;
        std::vector< unsigned int > entitiesGids;

        auto caster = fires::PropertyManager::getPropertyCaster( "Neuron model" );
        for ( const auto& entity : DataManager::entities( ).vector( ))
        {
           if ( dynamic_cast< shiftgen::NeuronPop* >( entity ))
           {
             entitiesGids.push_back( entity->entityGid( ));
             exporter_->addPopulation( "http://morphml.org/networkml/schema",
               QString::fromStdString( entity->getProperty(
               "Entity name" ).value<std::string>( )),
               QString::fromStdString( caster->toString(
               entity->getProperty( "Neuron model" ))),
               QString::fromStdString( std::to_string( entity->getProperty(
               "Nb of neurons" ).value<unsigned int>( ))),
               "0", "0", "0", "0", "0", "0" );
           }
        }
        const auto& relConnectsTo = *( DataManager::entities( ).
          relationships( )[ "connectsTo" ]->asOneToN( ));
        std::unordered_map< std::string, std::string > params;
        for ( auto relIt = relConnectsTo.begin( ); relIt != relConnectsTo.end( ); ++relIt )
        {
          const auto& relParameters = relIt->second;
          for ( auto relPropIt = relParameters.begin( ); relPropIt != relParameters.end( ); ++relPropIt )
          {
            params.clear( );
            params[ "units" ] = "Physiological Units";
            params[ "xmlns" ] = "http://morphml.org/networkml/schema";
            params[ "source" ] = DataManager::entities( ).map( ).find(
              relIt->first)->second->getProperty(
              "Entity name" ).value< std::string >( );
            params[ "target" ] = DataManager::entities( ).map( ).find(
              relPropIt->first)->second->getProperty(
              "Entity name" ).value< std::string >( );
            params[ "name" ] = relPropIt->second->getProperty(
              "Name" ).value< std::string >( );
            params[ "synapse_type" ] = "StaticSynapse";
            params[ "Threshold" ] = std::to_string( relPropIt->second->
              getProperty( "Threshold" ).value< float >( ));

            //Dependent parameters
            auto casterCM = fires::PropertyManager::getPropertyCaster( "Connectivity Model" );
            params["Connectivity Model"] = casterCM->toString( relPropIt->second->
                getProperty( "Connectivity Model" ));

            if ( params["Connectivity Model"] == "Random" )
            {
              params[ "Random probability" ] = std::to_string( relPropIt->second->
                  getProperty( "Random probability" ).value< float >( ));

            } else if ( params[ "Connectivity Model" ] == "FanOut" )
            {
              params[ "FanOut Outdegree" ] = std::to_string( relPropIt->second->
                  getProperty( "FanOut Outdegree" ).value< float >( ));

            } else if ( params["Connectivity Model" ] == "FanIn" )
            {
              params[ "FanIn Indegree" ] = std::to_string( relPropIt->second->
                  getProperty( "FanIn Indegree" ).value< float >( ));
            } else if ( params[ "Connectivity Model" ] == "Spatial Gaussian" )
            {
              params[ "Spatial Gaussian Probability" ] = std::to_string( relPropIt->second->
                  getProperty( "Spatial Gaussian Probability" ).value< float >( ));
              params[ "Spatial Gaussian Sigma" ] = std::to_string( relPropIt->second->
                  getProperty( "Spatial Gaussian Sigma" ).value< float >( ));
            }

            auto casterWT = fires::PropertyManager::getPropertyCaster( "Weight Type" );
            params[ "Weight Type" ] = casterWT->toString( relPropIt->second->
                getProperty( "Weight Type" ));
            if ( params[ "Weight Type" ] == "Fixed" )
            {
              params[ "Weight" ] = std::to_string( relPropIt->second->
                  getProperty( "Weight" ).value< float >( ));
            } else if ( params[ "Weight Type" ] == "Gaussian" )
            {
              params[ "Weight Gaussian Mean" ] = std::to_string( relPropIt->second->
                  getProperty( "Weight Gaussian Mean" ).value< float >( ));
              params[ "Weight Gaussian Sigma" ] = std::to_string( relPropIt->second->
                  getProperty( "Weight Gaussian Sigma" ).value< float >( ));
            }

            auto casterDT = fires::PropertyManager::getPropertyCaster( "Delay Type" );
            params[ "Delay Type" ] = casterDT->toString( relPropIt->second->
                getProperty( "Delay Type" ));
            if ( params[ "Delay Type" ] == "Fixed" )
            {
              params[ "Delay" ] = std::to_string( relPropIt->second->
                  getProperty( "Delay" ).value< float >( ));
            } else if ( params[ "Delay Type" ] == "Gaussian" )
            {
              params[ "Delay Gaussian Mean" ] = std::to_string( relPropIt->second->
                  getProperty( "Delay Gaussian Mean" ).value< float >( ));
              params[ "Delay Gaussian Sigma" ] = std::to_string( relPropIt->second->
                  getProperty( "Delay Gaussian Sigma" ).value< float >( ));
            }
            exporter_->addProjection( params );
          }
        }
        //Add inputs here!

        entitiesGids.clear( );
        exporter_->exportConGenXML( fileName.toStdString( ));
        delete exporter_;
      }
    }
  } /* namespace congen */
} /* namespace nslib */
