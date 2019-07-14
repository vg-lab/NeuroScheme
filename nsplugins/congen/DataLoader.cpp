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
#include <nslib/Loggers.h>
#include <nslib/DataManager.h>
#include <nslib/PaneManager.h>
#include <nslib/RepresentationCreatorManager.h>
#include "RepresentationCreator.h"
#include <shift_ConnectsWith.h>
#include <shift_NeuronPop.h>
#include <nslib/DomainManager.h>

namespace nslib
{
  namespace congen
  {
    using NeuronPop = shiftgen::NeuronPop;

    bool DataLoader::cliLoadData(
      const ::nslib::NeuroSchemeInputArguments& args )
    {
      if ( args.count( "-x" ) == 1 )
      {
        if ( args.at( "-x" ).size( ) != 1 )
        {
          Loggers::get( )->log( "-x expect one filename, but " +
                                std::to_string( args.at( "-x" ).size( )) +
                                " were found.",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          return false;
        }

        Loggers::get( )->log(
          "Loading NeuroML xml",
          LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        congen::DataLoader::loadNeuroML(
          std::string( args.at( "-x" )[0] ));
      } // "-x"

      return true;
    }


    void DataLoader::_loadPopulation(
      QXmlStreamReader& xml,
      std::unordered_map< std::string, unsigned int >& popNameToGid,
      unsigned int& maxNeuronsPerPopulation )
    {
      auto attributes = xml.attributes( );
      std::string popName;
      std::string cellType;
      unsigned int popSize = 0;

      if( attributes.hasAttribute( "name" ))
        popName = attributes.value( "name" ).toString( ).toStdString( );

      if( attributes.hasAttribute( "cell_type" ))
        cellType = attributes.value( "cell_type" ).toString( ).toStdString( );

      xml.readNextStartElement( );
      if ( xml.name( ) == "pop_location" )
      {
        xml.readNextStartElement( );
        if ( xml.name( ) == "random_arrangement" )
        {
          attributes = xml.attributes( );
          if( attributes.hasAttribute( "population_size" ))
          {
            popSize = attributes.value( "population_size" ).toUInt( );
            if( popSize > maxNeuronsPerPopulation )
            {
              maxNeuronsPerPopulation = popSize;
            }
          }
          xml.skipCurrentElement( ); // random_arrangement
        }
        xml.skipCurrentElement( ); // pop_location
      }
      xml.skipCurrentElement( ); // population

      NeuronPop::TNeuronModel neuronModel =
        NeuronPop::TNeuronModel::undefined;

      if ( cellType == "iaf psc alpha" )
        neuronModel = NeuronPop::TNeuronModel::iaf_psc_alpha;

      shift::Entity* neuronPop = new NeuronPop( popName, popSize, neuronModel );
      popNameToGid[ popName ] = neuronPop->entityGid( );

      auto& entities = DataManager::entities( );
      auto& rootEntities = DataManager::rootEntities( );
      entities.add( neuronPop );
      rootEntities.add( neuronPop );
    }

    void DataLoader::_loadProjection(
      QXmlStreamReader& xml,
      const std::unordered_map< std::string, unsigned int >& popNameToGid,
      float& maxAbsoluteWeight )
    {
      std::string projName, targetName, sourceName;
      auto attributes = xml.attributes( );
      if( attributes.hasAttribute( "name" ))
        projName = attributes.value( "name" ).toString( ).toStdString( );
      if( attributes.hasAttribute( "target" ))
        targetName = attributes.value( "target" ).toString( ).toStdString( );
      if( attributes.hasAttribute( "source" ))
        sourceName = attributes.value( "source" ).toString( ).toStdString( );

      float weight = 1.0f, weightGaussMean = 0.0f, weightGaussSigma = 0.0f,
        delay = 0.0f, delayGaussMean = 0.0f, delayGaussSigma = 0.0f,
        threshold = 0.0f;
      shiftgen::ConnectsWith::TConnectivityModel connModel =
        shiftgen::ConnectsWith::TConnectivityModel::All_to_all;
      float randProb = 0.0f, fanOutDegree = 0.0f, fanInDegree = 0.0f,
        spatialGaussProb = 0.0f, spatialGaussSigma = 0.0f;
      shiftgen::ConnectsWith::TFixedOrDistribution
        weightType = shiftgen::ConnectsWith::TFixedOrDistribution::Fixed,
        delayType = shiftgen::ConnectsWith::TFixedOrDistribution::Fixed;

      std::string lastGaussianPossibleElement;
      bool weightTextProcessed = false, delayTextProcessed = false;

      while( !xml.atEnd( ) && !xml.hasError( ) &&
             !( xml.name( ) == "projection" &&
                xml.tokenType( ) == QXmlStreamReader::EndElement ))
      {
        xml.readNext( );
        if ( xml.tokenType( ) == QXmlStreamReader::Characters )
        {
          if ( lastGaussianPossibleElement == "weight" && !weightTextProcessed )
          {
            weight = xml.text( ).toFloat( );
            weightTextProcessed = true;
            if( weight > maxAbsoluteWeight )
            {
              maxAbsoluteWeight = weight;
            }
          }
          if ( lastGaussianPossibleElement == "internal_delay" && !delayTextProcessed )
          {
            delay = xml.text( ).toFloat( );
            delayTextProcessed = true;
          }
          continue;
        }
        if ( xml.tokenType( ) != QXmlStreamReader::StartElement )
          continue;

        const auto tokenName = xml.name( );

        if ( tokenName == "synapse_props" )
        {
          attributes = xml.attributes( );
          if( attributes.hasAttribute( "threshold" ))
            threshold = attributes.value( "name" ).toString( ).toFloat( );
        }
        else
        if ( tokenName == "weight" )
        {
          lastGaussianPossibleElement = "weight";
        }
        else
        if ( tokenName == "internal_delay" )
        {
          lastGaussianPossibleElement = "internal_delay";
        }
        else
        if ( tokenName == "GaussianDistribution" )
        {
          auto gaussianDistAttrs = xml.attributes( );

          if ( lastGaussianPossibleElement == "weight" )
          {
            weightType = shiftgen::ConnectsWith::TFixedOrDistribution::Gaussian;
            if( gaussianDistAttrs.hasAttribute( "center" ))
              weightGaussMean = gaussianDistAttrs.value( "center" ).toFloat( );
            if( gaussianDistAttrs.hasAttribute( "deviation" ))
              weightGaussSigma = gaussianDistAttrs.value( "deviation" ).toFloat( );
          }
          else if ( lastGaussianPossibleElement == "internal_delay" )
          {
            delayType = shiftgen::ConnectsWith::TFixedOrDistribution::Gaussian;
            if( gaussianDistAttrs.hasAttribute( "center" ))
              delayGaussMean = gaussianDistAttrs.value( "center" ).toFloat( );
            if( gaussianDistAttrs.hasAttribute( "deviation" ))
              delayGaussSigma = gaussianDistAttrs.value( "deviation" ).toFloat( );
          }
          lastGaussianPossibleElement = "";
        }
        else if ( tokenName == "connectivity_pattern" )
        {
          lastGaussianPossibleElement = "connectivity_pattern";
        }
        else if ( tokenName == "All-to-all" )
        {
          connModel = shiftgen::ConnectsWith::TConnectivityModel::All_to_all;
          lastGaussianPossibleElement = "";
        }
        else if ( tokenName == "On-to-one" )
        {
          connModel = shiftgen::ConnectsWith::TConnectivityModel::One_to_one;
        }
        else if ( tokenName == "fixed_probability" )
        {
          connModel = shiftgen::ConnectsWith::TConnectivityModel::Random;
          auto fixedProbAttribs = xml.attributes( );
          if( fixedProbAttribs.hasAttribute( "probability" ))
              randProb = fixedProbAttribs.value( "probability" ).toFloat( );
        }
        else if ( tokenName == "spatial_gaussian" )
        {
          connModel = shiftgen::ConnectsWith::TConnectivityModel::Random;
          auto fixedProbAttribs = xml.attributes( );
          if( fixedProbAttribs.hasAttribute( "cutoff" ))
              spatialGaussProb = fixedProbAttribs.value( "cutoff" ).toFloat( );
          if( fixedProbAttribs.hasAttribute( "sigma" ))
              spatialGaussSigma = fixedProbAttribs.value( "sigma" ).toFloat( );
        }
        else if ( tokenName == "per_cell_connection" )
        {
          auto perCellConnAttribs = xml.attributes( );
          if ( perCellConnAttribs.hasAttribute( "direction" ))
          {
            auto perCellDir = perCellConnAttribs.value( "direction" ).toString( );
            if ( perCellDir == "PreToPost" ) // FanIn
            {
              connModel = shiftgen::ConnectsWith::TConnectivityModel::FanIn;
              if ( perCellConnAttribs.hasAttribute( "num_per_source" ))
                fanInDegree = perCellConnAttribs.value( "num_per_source" ).toFloat( );
            }
            else if ( perCellDir == "PostToPre" ) // FanOut
            {
              connModel = shiftgen::ConnectsWith::TConnectivityModel::FanOut;
              if ( perCellConnAttribs.hasAttribute( "num_per_source" ))
                fanOutDegree = perCellConnAttribs.value( "num_per_source" ).toFloat( );
            }
          }
        }
        else
        {
          lastGaussianPossibleElement = "";
        }
      }

      auto connProps = new shiftgen::ConnectsWith(
          projName, connModel, randProb, fanOutDegree, fanInDegree,
        spatialGaussProb, spatialGaussSigma,
        weightType, weight, weightGaussMean, weightGaussSigma,
        delayType, delay, delayGaussMean, delayGaussSigma, threshold );

      const auto sourceIt = popNameToGid.find( sourceName );
      const auto targetIt = popNameToGid.find( targetName );
      unsigned int sourceGid = 0, targetGid = 0;
      if ( sourceIt != popNameToGid.end( ) &&
           targetIt != popNameToGid.end( ))
      {
        sourceGid = sourceIt->second;
        targetGid = targetIt->second;
        auto& dataEntities = DataManager::entities( );
        auto& entities = dataEntities.entities( );
        auto& relAggregatedConnectsTo = *( dataEntities
          .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ) );
        auto& relAggregatedConnectedBy = *( dataEntities
          .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ) );
        shift::Relationship::EstablishAndAggregate( relAggregatedConnectsTo,
          relAggregatedConnectedBy, dataEntities,entities.at( sourceGid ),
          entities.at( targetGid ), connProps, connProps );
      }
    }

    void DataLoader::_loadInput(
        QXmlStreamReader& xml,
        std::unordered_map< std::string, unsigned int >& popNameToGid )
    {
      auto attributes = xml.attributes( );
      std::string popName;
      unsigned int popSize = 0u;
      shiftgen::Input::TInputModel model =
        shiftgen::Input::TInputModel::undefined_generator;
      shiftgen::Input::TInputType type =
        shiftgen::Input::TInputType::Pulse_input;
      float delay = 0.0f;
      float duration = 0.0f;
      float amplitude = 0.0f;
      unsigned int frequency = 0u;
      shiftgen::Input::TSynapticMechanism synaptic_mechanism =
        shiftgen::Input::TSynapticMechanism::undefined;

      if( attributes.hasAttribute( "name" ))
      {
        popName = attributes.value( "name" ).toString( ).toStdString( );
      }

      xml.readNextStartElement( );
      if ( xml.name( ) == "pulse_input" )
      {
        attributes = xml.attributes( );
        if( attributes.hasAttribute( "delay" ))
        {
          delay = attributes.value( "delay" ).toFloat( );
        }
        if( attributes.hasAttribute( "duration" ))
        {
          duration = attributes.value( "duration" ).toFloat( );
        }
        if( attributes.hasAttribute( "amplitude" ))
        {
          amplitude = attributes.value( "amplitude" ).toFloat( );
        }
        xml.skipCurrentElement( ); //pulse_input
      }
      else if ( xml.name( ) == "random_stim" )
      {
        type = shiftgen::Input::TInputType::Random_stim;
        attributes = xml.attributes( );
        if( attributes.hasAttribute( "frequency" ))
        {
          frequency = attributes.value( "frequency" ).toUInt( );
        }
        if( attributes.hasAttribute( "synaptic_mechanism" ))
        {
          std::string synapticMechanism = attributes
              .value( "synaptic_mechanism" ).toString( ).toStdString( );
          if( synapticMechanism == "DoubExpSynA" ){
            synaptic_mechanism =
              shiftgen::Input::TSynapticMechanism::DoubExpSynA;
          }
        }
        xml.skipCurrentElement( ); // random_stim
      }
      auto& dataEntities = DataManager::entities( );
      auto& entities = dataEntities.entities( );
      auto& noHierarchyEntities = DataManager::noHierarchyEntities( );
      auto& relAggregatedConnectsTo = *( dataEntities
        .relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));
      auto& relAggregatedConnectedBy = *( dataEntities
        .relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));

      shift::Entity* stimulator = new shiftgen::Input( popName, popSize, model,
        type, delay, duration, amplitude, frequency, synaptic_mechanism );

      dataEntities.add( stimulator );
      noHierarchyEntities.add( stimulator );
      const auto& relationshipProperties = DomainManager::getActiveDomain( )
        ->relationshipPropertiesTypes( );
      while( xml.readNextStartElement( ))
      {
        if( xml.name( ) == "target" ){
          attributes = xml.attributes( );
          if( attributes.hasAttribute( "population" ))
          {
            std::string targetName =
              attributes.value( "population" ).toString( ).toStdString( );
            auto targetIt = popNameToGid.find( targetName );
            if ( targetIt != popNameToGid.end( ))
            {
              auto connProps = relationshipProperties.getRelationshipProperties(
                "connectsTo" )->create( );
              connProps->setProperty("Name","R:" + popName + "-" + targetName );
              shift::Relationship::EstablishAndAggregate(
                relAggregatedConnectsTo,relAggregatedConnectedBy, dataEntities,
                stimulator, entities.at( targetIt->second ), connProps,
                connProps );
            }
          }
          xml.skipCurrentElement( );//target
        }
        else
        {
          xml.skipCurrentElement( );
        }
      }
    }

    bool DataLoader::loadNeuroML( const std::string& fileName )
    {
      DataManager::reset( );
      float maxAbsoluteWeight = 0.0f;
      unsigned int maxNeuronsPerPopulation = 0;

      QFile qFile ( fileName.c_str( ));
      if ( ! qFile.exists( ))
      {
        Loggers::get( )->log( "NeuroML file not found",
          LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      qFile.open( QIODevice::ReadOnly | QIODevice::Text );

      if ( !qFile.isOpen( ))
      {
        Loggers::get( )->log( "NeuroML file not readable",
          LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      QXmlStreamReader xml( &qFile );

      if ( xml.hasError( ))
      {
        Loggers::get( )->log( "NeuroML file has errors",
          LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      std::unordered_map< std::string, unsigned int > popNameToGid;
      xml.readNextStartElement( ); // to skip neuroml

      while( !xml.atEnd( ) && !xml.hasError( ))
      {
        bool f = xml.readNextStartElement( );

        if ( !f )
          continue;

        if ( xml.name( ) == "population" )
        {
          _loadPopulation( xml, popNameToGid, maxNeuronsPerPopulation );
        }
        else if ( xml.name( ) == "projection" )
        {
          _loadProjection( xml, popNameToGid, maxAbsoluteWeight );
        }
        else if ( xml.name( ) == "input" )
        {
          _loadInput( xml, popNameToGid );
        }
      }

      // Sets new maximum and minimum in the RepresentationCreator
      auto repCreator = ( RepresentationCreator* )
        RepresentationCreatorManager::getCreator( );
      repCreator->maxAbsoluteWeight( maxAbsoluteWeight );
      repCreator->maxNeuronsPerPopulation( maxNeuronsPerPopulation );

      return true;


    }
  } // namespace congen
} // namespace nslib
