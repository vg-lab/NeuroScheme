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
          nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        nslib::congen::DataLoader::loadNeuroML(
          std::string( args.at( "-x" )[0] ));
      } // "-x"

      // ::nslib::Logger::get( )->log(
      //   "Loading data for congen", LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

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


    bool DataLoader::_loadPopulation(
      QXmlStreamReader& xml,
      std::unordered_map< std::string, unsigned int >& popNameToGid )
    {
      std::cout << "--- Load population" << std::endl;

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
            popSize = attributes.value( "population_size" ).toUInt( );
        }
        xml.skipCurrentElement( ); // pop_location
      }
      xml.skipCurrentElement( ); // random_arrangement
      xml.skipCurrentElement( ); // population
      // xml.skipCurrentElement( );
      // xml.readNextStartElement( );
      // std::cout << "** Next elememt " << xml.name( ).toString( ).toStdString( ) << std::endl;
      NeuronPop::TNeuronModel neuronModel =
        NeuronPop::TNeuronModel::undefined;

      std::cout << "\t\t" << popName << std::endl;

      if ( cellType == "iaf psc alpha" )
        neuronModel = NeuronPop::TNeuronModel::iaf_psc_alpha;

      shift::Entity* neuronPop = new NeuronPop( popSize, neuronModel );
      neuronPop->label( ) = popName;
      popNameToGid[ popName ] = neuronPop->entityGid( );

      auto& entities = nslib::DataManager::entities( );
      auto& rootEntities = nslib::DataManager::rootEntities( );
      entities.add( neuronPop );
      rootEntities.add( neuronPop );

      // xml.skipCurrentElement( );

      return true;
    }

    bool DataLoader::_loadProjection(
      QXmlStreamReader& xml,
      const std::unordered_map< std::string, unsigned int >& popNameToGid )
    {
      std::cout << std::endl << "--- Load projection" << std::endl;

      std::string projName, targetName, sourceName;
      auto attributes = xml.attributes( );
      if( attributes.hasAttribute( "name" ))
        projName = attributes.value( "name" ).toString( ).toStdString( );
      if( attributes.hasAttribute( "target" ))
        targetName = attributes.value( "target" ).toString( ).toStdString( );
      if( attributes.hasAttribute( "source" ))
        sourceName = attributes.value( "source" ).toString( ).toStdString( );

      std::cout << "--- " << projName << std::endl;
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

      auto sourceIt = popNameToGid.find( sourceName );
      auto targetIt = popNameToGid.find( targetName );
      unsigned int sourceGid = 0, targetGid = 0;
      if ( sourceIt != popNameToGid.end( ) &&
           targetIt != popNameToGid.end( ))
      {
        sourceGid = sourceIt->second;
        targetGid = targetIt->second;
      }

      std::string lastGaussianPossibleElement;
      while( !xml.atEnd( ) && !xml.hasError( ) &&
             !( xml.name( ) == "projection" &&
                xml.tokenType( ) == QXmlStreamReader::EndElement ))
      {

        std::cout << "pos: " << xml.device( )->pos( ) << std::endl;

        auto hasChildren = xml.readNextStartElement( );
        if ( xml.atEnd( ) ||
             xml.tokenType( ) !=
             QXmlStreamReader::StartElement )
          continue;

        if ( !xml.atEnd( ) && xml.name( ) == "synapse_props" )
        {
          std::cout << "---- Synapse props " << int( hasChildren ) << std::endl;
          attributes = xml.attributes( );
          if( attributes.hasAttribute( "threshold" ))
            threshold = attributes.value( "name" ).toString( ).toFloat( );

        }

        if ( !xml.atEnd( ) && xml.name( ) == "weight" )
        {
          std::cout << "---- Weight " << "isCharacters: " << xml.isCharacters( )
                    << " " << "isStartElement: " << xml.isStartElement( )
                    << " " << int( hasChildren ) << std::endl;
          lastGaussianPossibleElement = "weight";

          // auto file = xml.device( );
          // auto pos = file->pos( );
          // auto et = xml.readElementText(
          //    QXmlStreamReader::IncludeChildElements );
          // weight = et.toFloat( );
          // file->seek( pos );
          // xml.setDevice( file );

          std::cout << "---- Weight float " << weight << std::endl;
          // std::cout << xml.tokenString( ).toStdString( ) << std::endl;
          // std::cout << "---- Weight " << et.size( ) << " "
          //           << qPrintable( et ) << std::endl;

          // std::cout << xml.errorString( ) << std::endl;
        }

        if ( !xml.atEnd( ) && xml.name( ) == "internal_delay" )
        {
          std::cout << "---- Delay " << "isCharacters: " << xml.isCharacters( ) << " " << "isStartElement: " << xml.isStartElement( ) << " " << int( hasChildren ) << std::endl;
          lastGaussianPossibleElement = "internal_delay";
        }

        if ( !xml.atEnd( ) && xml.name( ) == "GaussianDistribution" )
        {
          auto gaussianDistAttrs = xml.attributes( );

          if ( lastGaussianPossibleElement == "weight" )
          {
            if( gaussianDistAttrs.hasAttribute( "center" ))
              weightGaussMean = gaussianDistAttrs.value( "center" ).toFloat( );
            if( gaussianDistAttrs.hasAttribute( "deviation" ))
              weightGaussSigma = gaussianDistAttrs.value( "deviation" ).toFloat( );
            std::cout << "--- Gaussian weight " << weightGaussMean << " " << weightGaussSigma << std::endl;
          }
          else if ( lastGaussianPossibleElement == "internal_delay" )
          {
            if( gaussianDistAttrs.hasAttribute( "center" ))
              delayGaussMean = gaussianDistAttrs.value( "center" ).toFloat( );
            if( gaussianDistAttrs.hasAttribute( "deviation" ))
              delayGaussSigma = gaussianDistAttrs.value( "deviation" ).toFloat( );
            std::cout << "--- Gaussian delay " << delayGaussMean << " " << delayGaussSigma << std::endl;
          }

        }

      }

      auto connProps = new shiftgen::ConnectsWith(
        connModel, randProb, fanOutDegree, fanInDegree,
        spatialGaussProb, spatialGaussSigma,
        weightType, weight, weightGaussMean, weightGaussSigma,
        delayType, delay, delayGaussMean, delayGaussSigma, threshold );

      auto& entities = nslib::DataManager::entities( );
      auto& relConnectsTo =
        *( entities.relationships( )[ "connectsTo" ]->asOneToN( ));
      auto& relConnectedBy =
        *( entities.relationships( )[ "connectedBy" ]->asOneToN( ));
      relConnectsTo[ targetGid ].insert(
        std::make_pair( sourceGid, connProps ));
      relConnectedBy[ sourceGid ].insert(
        std::make_pair( targetGid, nullptr ));
      connProps->label( ) = projName;

      return !xml.hasError( );
    }

    bool DataLoader::loadNeuroML( const std::string& fileName )
    {
      //TODO: compute maximums

      auto& entities = nslib::DataManager::entities( );
      auto& rootEntities = nslib::DataManager::rootEntities( );
      fires::PropertyManager::clear( );
      entities.clear( );
      rootEntities.clear( );

      QFile qFile ( fileName.c_str( ));
      if ( ! qFile.exists( ))
      {
        Loggers::get( )->log( "NeuroML file not found",
                              nslib::LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      qFile.open( QIODevice::ReadOnly | QIODevice::Text );

      if ( !qFile.isOpen( ))
      {
        Loggers::get( )->log( "NeuroML file not readable",
                              nslib::LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      QXmlStreamReader xml( &qFile );

      if ( xml.hasError( ))
      {
        Loggers::get( )->log( "NeuroML file has errors",
                              nslib::LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      std::unordered_map< std::string, unsigned int > popNameToGid;
      xml.readNextStartElement( ); // to skip neuroml

      bool retVal = true;

      while( !xml.atEnd( ) && !xml.hasError( ))
      {
        bool f = xml.readNextStartElement( );

        if ( !f )
          continue;

        std::cout << "pos: " << xml.device( )->pos( ) << std::endl;
        if ( xml.name( ) == "population" )
          retVal |= _loadPopulation( xml, popNameToGid );
        else if ( xml.name( ) == "projection" )
          retVal |= _loadProjection( xml, popNameToGid );
      }

      return retVal;

    }
  } // namespace congen
} // namespace nslib
