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
#include "Neuron.h"
#include "RepresentationCreator.h"
#include <shift_ConnectsWith.h>

namespace nslib
{
  namespace cortex
  {

    using ConnectsWith = shiftgen::ConnectsWith;

    bool DataLoader::loadData( const ::nslib::NeuroSchemeInputArguments& args )
    {
#ifdef NEUROSCHEME_USE_NSOL
      // std::cout << "Loading data" << std::endl;
      if ( args.empty( ))
        Log::log( "No arguments provided", LOG_LEVEL_ERROR );

      if ( args.count( "-bc" ) > 0 && args.count( "-xml" ) > 0 )
      {
        Log::log( "-bc and -xml arguments are exclusive",
                  LOG_LEVEL_ERROR );
        return false;
      }

      if ( args.count( "-bc" ) == 1 )
      {
        if ( args.at( "-bc" ).size( ) != 1 )
        {
          Log::log( "-bc provided with more or less that one filename",
                    LOG_LEVEL_ERROR );
          return false;
        }
        if ( args.count( "-target" ) != 1 )
        {
          Log::log( "-bc provided but no -target found", LOG_LEVEL_ERROR );
          return false;
        }
        if ( args.at( "-target" ).size( ) != 1 )
        {
          Log::log( "-target provided with more or less that one target name",
                    LOG_LEVEL_ERROR );
          return false;
        }
        if ( args.count( "-cns" ) == 1 )
        {
          if ( args.at( "-cns" ).size( ) != 1 )
          {
            Log::log( "-cns provided with more or less that one csv files",
                      LOG_LEVEL_ERROR );
            return false;
          }
        }
        nslib::Log::log( NS_LOG_HEADER + "Loading blue config",
                               nslib::LOG_LEVEL_VERBOSE );
        nslib::DataManager::loadBlueConfig(
          args.at( "-bc" )[0],
          args.at( "-target" )[0],
          args.count( "-nm" ) == 0 || args.count( "--no-morphologies" ) == 0,
          ( args.count( "-cns" ) == 1 ? args.at( "cns" )[0] : std::string( )),
          args.count( "-lc" ) == 1 || args.count( "--load-connectivity" ) == 1 );

        createEntitiesFromNsolColumns(
          nslib::DataManager::nsolDataSet( ).columns( ),
          nslib::DataManager::nsolDataSet( ).circuit( ),
          args.count( "-nm" ) == 0,
          ( args.count( "-cns" ) == 1 ? args.at( "cns" )[0] : std::string( )));

      }

      if ( args.count( "-xml" ) == 1 )
      {
        if ( args.at( "-xml" ).size( ) != 1 )
        {
          Log::log( "-xml provided with more or less that one filename",
                    LOG_LEVEL_ERROR );
          return false;
        }

        nslib::Log::log( NS_LOG_HEADER + "Loading nsol xml",
                               nslib::LOG_LEVEL_VERBOSE );

        nslib::DataManager::loadNsolXmlScene( args.at( "-xml" )[0] );

        createEntitiesFromNsolColumns(
          nslib::DataManager::nsolDataSet( ).columns( ),
          nslib::DataManager::nsolDataSet( ).circuit( ));

      }

      nslib::DataManager::nsolDataSet( ).close( );
      return true;
#else
      ( void ) args;
      Log::log( "nsol not built in.", LOG_LEVEL_ERROR );
      return false;
#endif
    }

#ifdef NEUROSCHEME_USE_NSOL

    std::string NeuronMorphologyToLabel(
      nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat )
    {
      switch( stat )
      {
        // Volume
      case nsol::NeuronMorphologyStats::DENDRITIC_VOLUME:
        return std::string("Dendritic Volume");
        break;
      case nsol::NeuronMorphologyStats::AXON_VOLUME:
        return std::string("Axon Volume");
        break;
      case nsol::NeuronMorphologyStats::NEURITIC_VOLUME:
        return std::string("Neuritic Volume");
        break;
      case nsol::NeuronMorphologyStats::SOMA_VOLUME:
        return std::string("Soma Volume");
        break;
      case nsol::NeuronMorphologyStats::VOLUME:
        return std::string("Volume");
        break;

        // Surface
      case nsol::NeuronMorphologyStats::DENDRITIC_SURFACE:
        return std::string("Dendritic Surface");
        break;
      case nsol::NeuronMorphologyStats::AXON_SURFACE:
        return std::string("Axon Surface");
        break;
      case nsol::NeuronMorphologyStats::NEURITIC_SURFACE:
        return std::string("Neuritic Surface");
        break;
      case nsol::NeuronMorphologyStats::SOMA_SURFACE:
        return std::string("Soma Surface");
        break;
      case nsol::NeuronMorphologyStats::SURFACE:
        return std::string("Surface");
        break;

        // Length
      case nsol::NeuronMorphologyStats::DENDRITIC_LENGTH:
        return std::string("Dendritic Length");
        break;
      case nsol::NeuronMorphologyStats::AXON_LENGTH:
        return std::string("Axon Length");
        break;
      case nsol::NeuronMorphologyStats::NEURITIC_LENGTH:
        return std::string("Neuritic Length");
        break;

        // Bifurcations
      case nsol::NeuronMorphologyStats::DENDRITIC_BIFURCATIONS:
        return std::string("Dendritic Bifurcations");
        break;
      case nsol::NeuronMorphologyStats::AXON_BIFURCATIONS:
        return std::string("Axon Bifurcations");
        break;
      case nsol::NeuronMorphologyStats::NEURITIC_BIFURCATIONS:
        return std::string("Neuritic Bifurcations");
        break;

      default:
        break;
      }
      return std::string("");
    }
    shiftgen::Neuron::TMorphologicalType
    nsolToShiftMorphologicalType( nsol::Neuron::TMorphologicalType nsolType )
    {
      switch ( nsolType )
      {
      case nsol::Neuron::TMorphologicalType::UNDEFINED:
        return shiftgen::Neuron::UNDEFINED_MORPHOLOGICAL_TYPE;
        break;
      case nsol::Neuron::TMorphologicalType::PYRAMIDAL:
        return shiftgen::Neuron::PYRAMIDAL;
        break;
      case nsol::Neuron::TMorphologicalType::INTERNEURON:
        return shiftgen::Neuron::INTERNEURON;
        break;
      default:
        break;
      }
      return shiftgen::Neuron::UNDEFINED_MORPHOLOGICAL_TYPE;
    }

    shiftgen::Neuron::TFunctionalType
    nsolToShiftFunctionalType( nsol::Neuron::TFunctionalType nsolType )
    {
      switch ( nsolType )
      {
      case nsol::Neuron::TFunctionalType::UNDEFINED_FUNCTIONAL_TYPE:
        return shiftgen::Neuron::UNDEFINED_FUNCTIONAL_TYPE;
        break;
      case nsol::Neuron::TFunctionalType::INHIBITORY:
        return shiftgen::Neuron::INHIBITORY;
        break;
      case nsol::Neuron::TFunctionalType::EXCITATORY:
        return shiftgen::Neuron::EXCITATORY;
        break;
      default:
        break;
      }
      return shiftgen::Neuron::UNDEFINED_FUNCTIONAL_TYPE;
    }

    void DataLoader::createEntitiesFromNsolColumns(
      const nsol::Columns& columns,
      const nsol::Circuit& circuit,
      bool withMorphologies,
      const std::string& csvNeuronStatsFileName )
    {
      auto& _entities = nslib::DataManager::entities( );
      auto& _rootEntities = nslib::DataManager::rootEntities( );
      //fires::PropertyManager::clear( );
      _entities.clear( );

      _entities.relationships( )[ "isParentOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "isAGroupOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isPartOf" ] =
        new shift::RelationshipOneToN;

      _entities.relationships( )[ "isSuperEntityOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isSubEntityOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "connectsTo" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "connectedBy" ] =
        new shift::RelationshipOneToN;

      auto& relParentOf =
        *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      auto& relChildOf =
        *( _entities.relationships( )[ "isChildOf" ]->asOneToOne( ));

      auto& relGroupOf =
        *( _entities.relationships( )[ "isAGroupOf" ]->asOneToN( ));
      auto& relPartOf =
        *( _entities.relationships( )[ "isPartOf" ]->asOneToN( ));

      auto& relSuperEntityOf =
        *( _entities.relationships( )[ "isSuperEntityOf" ]->asOneToN( ));
      auto& relSubEntityOf =
        *( _entities.relationships( )[ "isSubEntityOf" ]->asOneToOne( ));

      auto& relConnectsTo =
        *( _entities.relationships( )[ "connectsTo" ]->asOneToN( ));
      auto& relConnectedBy =
        *( _entities.relationships( )[ "connectedBy" ]->asOneToN( ));

      assert( DataManager::entities( ).
              relationships( ).count( "isSubEntityOf" ) == 1 );
      std::set< unsigned int > gids;
      std::unordered_map< unsigned int, shift::Entity* > neuronEntitiesByGid;
      for ( const auto& col : columns )
        for ( const auto& mc : col->miniColumns( ))
          for ( const auto& neuron : mc->neurons( ))
            gids.insert( neuron->gid( ));

      float maxNeuronSomaVolume = 0.0f;
      float maxNeuronSomaArea = 0.0f;
      float maxNeuronDendVolume = 0.0f;
      float maxNeuronDendArea = 0.0f;
      float maxNeuronAxonVolume = 0.0f;
      float maxNeuronAxonArea = 0.0f;

      unsigned int  meanBifurcations = 0;
      ( void ) meanBifurcations; // remove
      float meanSomaArea = .0f;
      float meanSomaVolume = .0f;
      float meanDendsArea = .0f;
      float meanDendsVolume = .0f;

#define NSOL_NEURON_MORPHOLOGY_NUM_STATS                        \
      nsol::NeuronMorphologyStats::NEURON_MORPHOLOGY_NUM_STATS
#define NNMS nsol::NeuronMorphologyStats

      typedef struct
      {
        float x, y, z;
        uint8_t layer, column;
        unsigned short miniColumn;
        uint8_t mophoType, functType;
        float somaMaxRadius;
        float morphologyStats[NSOL_NEURON_MORPHOLOGY_NUM_STATS];
      } TNeuronStats;

      typedef std::unordered_map< unsigned int, TNeuronStats > TNeuronsStats;
      TNeuronsStats neuronsStats;

      if ( !csvNeuronStatsFileName.empty( ))
      {
        Log::log( NS_LOG_HEADER + "Loading neuron morphology stats from " +
                  csvNeuronStatsFileName, LOG_LEVEL_VERBOSE );
        std::ifstream csvNeuronStatsFile( csvNeuronStatsFileName );
        if ( !csvNeuronStatsFile.is_open( ))
        {
          throw std::runtime_error( std::string( "File" ) +
                                    csvNeuronStatsFileName +
                                    std::string( " could not be opened"));
        }

        std::string line;
        int lineCount = 0;
        // skip first line
        std::getline( csvNeuronStatsFile, line );
        while ( std::getline( csvNeuronStatsFile, line ))
        {
          lineCount++;

          if ( line[0] != '#' )
          {
            // Trim spaces
            line.erase( line.find_last_not_of( " \r\t") +1 );
            unsigned int fields =
              1 + std::count_if( line.begin( ), line.end( ),
                                 []( unsigned char c )
                                 {
                                   return ( c == ',' );
                                 });

            if ( fields != 26 )
            {
              nslib::Log::log( std::string( "Skipping lineString " ) +
                                     std::to_string( lineCount ) +
                                     std::string( ". Not enough fields found" ),
                                     nslib::LOG_LEVEL_WARNING );
              continue;
            }

            std::istringstream iss( line );
            unsigned int gid;
            TNeuronStats stats;
            char c;
            iss >> gid >> c; assert( c == ',' );
            iss >> stats.x >> c; assert( c == ',' );
            iss >> stats.y >> c; assert( c == ',' );
            iss >> stats.z >> c; assert( c == ',' );
            iss >> stats.layer >> c; assert( c == ',' );;
            iss >> stats.column >> c; assert( c == ',' );;
            iss >> stats.miniColumn >> c; assert( c == ',' );
            iss >> stats.mophoType >> c; assert( c == ',' );
            iss >> stats.functType >> c; assert( c == ',' );
            iss >> stats.somaMaxRadius;
            for ( unsigned int statIdx = 0;
                  statIdx < NSOL_NEURON_MORPHOLOGY_NUM_STATS;
                  ++statIdx )
            {
              iss >> c; assert( c == ',' );
              iss >> stats.morphologyStats[statIdx];
            }

            if ( gids.find( gid ) != gids.end( ))
              neuronsStats[gid] = stats;

          } // if not comment
        } // while getline

        unsigned long totalBifurcations = 0;
        double totalSomaArea = .0f;
        double totalSomaVolume = .0f;
        double totalDendsArea = .0f;
        double totalDendsVolume = .0f;

        for ( const auto& neuronStats : neuronsStats )
        {
          totalBifurcations += ( unsigned long )
            neuronStats.second.morphologyStats[NNMS::DENDRITIC_BIFURCATIONS];

          auto neuronSomaVolume =
            neuronStats.second.morphologyStats[NNMS::SOMA_VOLUME];
          if ( neuronSomaVolume > maxNeuronSomaVolume )
            maxNeuronSomaVolume = neuronSomaVolume;
          totalSomaVolume += neuronSomaVolume;

          auto neuronSomaArea =
            neuronStats.second.morphologyStats[NNMS::SOMA_SURFACE];
          if ( neuronSomaArea > maxNeuronSomaArea )
            maxNeuronSomaArea = neuronSomaArea;
          totalSomaArea += neuronSomaArea;

          auto neuronDendVolume =
            neuronStats.second.morphologyStats[NNMS::DENDRITIC_VOLUME];
          if ( neuronDendVolume > maxNeuronDendVolume )
            maxNeuronDendVolume = neuronDendVolume;
          totalDendsVolume += neuronDendVolume;

          auto neuronDendArea =
            neuronStats.second.morphologyStats[NNMS::DENDRITIC_SURFACE];
          if ( neuronDendArea > maxNeuronDendArea )
            maxNeuronDendArea = neuronDendArea;
          totalDendsArea += neuronDendArea;

          auto neuronAxonVolume =
            neuronStats.second.morphologyStats[NNMS::AXON_VOLUME];
          if ( neuronAxonVolume > maxNeuronAxonVolume )
            maxNeuronAxonVolume = neuronAxonVolume;

          auto neuronAxonArea =
            neuronStats.second.morphologyStats[NNMS::AXON_SURFACE];
          if ( neuronAxonArea > maxNeuronAxonArea )
            maxNeuronAxonArea = neuronAxonArea;
        } // for all neurons

        if ( neuronsStats.size( ) > 0 )
        {
          float size_1 =  1.0f / float( neuronsStats.size( ));

          meanSomaArea = totalSomaArea * size_1;
          meanSomaVolume = totalSomaVolume * size_1;
          meanDendsArea = totalDendsArea * size_1;
          meanDendsVolume = totalDendsVolume * size_1;
          meanBifurcations = totalBifurcations * size_1;
        }

      } // if ( csvNeuronStatsFileName != "" )

      if ( withMorphologies && csvNeuronStatsFileName.empty( ))
      {

        for ( const auto& col : columns )
        {
          NSOL_DEBUG_CHECK( col->stats( ), "no stats in column" );

          float colMaxNeuronSomaVolume =
            col->stats( )->getStat( nsol::ColumnStats::SOMA_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          float colMaxNeuronSomaArea =
            col->stats( )->getStat( nsol::ColumnStats::SOMA_SURFACE,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          float colMaxNeuronDendVolume =
            col->stats( )->getStat( nsol::ColumnStats::DENDRITIC_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          float colMaxNeuronDendArea =
            col->stats( )->getStat( nsol::ColumnStats::DENDRITIC_SURFACE,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          float colMaxNeuronAxonVolume =
            col->stats( )->getStat( nsol::ColumnStats::AXON_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          float colMaxNeuronAxonArea =
            col->stats( )->getStat( nsol::ColumnStats::AXON_SURFACE,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          if ( colMaxNeuronSomaVolume > maxNeuronSomaVolume )
            maxNeuronSomaVolume = colMaxNeuronSomaVolume;
          if ( colMaxNeuronSomaArea > maxNeuronSomaArea )
            maxNeuronSomaArea = colMaxNeuronSomaArea;
          if ( colMaxNeuronDendVolume > maxNeuronDendVolume )
            maxNeuronDendVolume = colMaxNeuronDendVolume;
          if ( colMaxNeuronDendArea > maxNeuronDendArea )
            maxNeuronDendArea = colMaxNeuronDendArea;
          if ( colMaxNeuronAxonVolume > maxNeuronAxonVolume )
            maxNeuronAxonVolume = colMaxNeuronAxonVolume;
          if ( colMaxNeuronAxonArea > maxNeuronAxonArea )
            maxNeuronAxonArea = colMaxNeuronAxonArea;

        } // for all columns
        // col->maxDendriteSurface( );
      } // if withMorphologies && csvNeuronStatsFileName.empty( )

      //
      // Create mappers
      //
      if ( withMorphologies || !csvNeuronStatsFileName.empty( ))
      {

// #ifdef fdfdf
// auto greenMapper = new DiscreteColorMapper( );
//       auto redMapper = new DiscreteColorMapper( );

#define ColorF( r, g, b )                       \
        Color( uint8_t( roundf( r )),           \
               uint8_t( roundf( g )),           \
               uint8_t( roundf( b )))
//       greenMapper->push_back( ColorF( 0.62f, 1.00f, 0.75f ));
//       greenMapper->push_back( ColorF( 0.55f, 0.88f, 0.65f ));
//       greenMapper->push_back( ColorF( 0.46f, 0.75f, 0.56f ));
//       greenMapper->push_back( ColorF( 0.40f, 0.63f, 0.47f ));
//       greenMapper->push_back( ColorF( 0.31f, 0.51f, 0.37f ));
//       greenMapper->push_back( ColorF( 0.22f, 0.38f, 0.27f ));

//       redMapper->push_back( ColorF( 1.00f, 0.62f, 0.75f ));
//       redMapper->push_back( ColorF( 0.88f, 0.55f, 0.65f ));
//       redMapper->push_back( ColorF( 0.75f, 0.46f, 0.56f ));
//       redMapper->push_back( ColorF( 0.63f, 0.40f, 0.47f ));
//       redMapper->push_back( ColorF( 0.51f, 0.31f, 0.37f ));
//       redMapper->push_back( ColorF( 0.38f, 0.22f, 0.27f ));

//       greenMapper->max( ) = maxNeuronSomaVolume;
//       redMapper->max( ) = maxNeuronDendVolume;

//       auto somaAreaToAngle =
//         new MapperFloatToFloat( 0, maxNeuronSomaArea, 0, -360 );
//       auto dendAreaToAngle =
//         new MapperFloatToFloat( 0, maxNeuronDendArea, 0, -360 );
// #endif
      } // if morphologies || !csvNeuronStatsFileName.empty( )

      // Compute maximums per layer for minicol and col reps
      unsigned int maxNeuronsPerColumnLayer =
        std::numeric_limits< unsigned int >::min( );
      unsigned int maxNeuronsPerMiniColumnLayer =
        std::numeric_limits< unsigned int >::min( );
      for ( const auto& col : columns )
      {
        for ( unsigned int layer = 1; layer < 7; layer++ )
        {
          unsigned int numNeuronsPerColumnLayer = col->numberOfNeurons(
            false, nsol::Neuron::PYRAMIDAL, layer );

          if ( numNeuronsPerColumnLayer > maxNeuronsPerColumnLayer )
            maxNeuronsPerColumnLayer = numNeuronsPerColumnLayer;

          numNeuronsPerColumnLayer = col->numberOfNeurons(
            false, nsol::Neuron::INTERNEURON, layer );

          if ( numNeuronsPerColumnLayer > maxNeuronsPerColumnLayer )
            maxNeuronsPerColumnLayer = numNeuronsPerColumnLayer;
        }

        for ( const auto miniCol : col->miniColumns( ))
        {
          for ( unsigned int layer = 1; layer < 7; layer++ )
          {
            unsigned int numNeuronsPerMiniColumnLayer = miniCol->numberOfNeurons(
              false, nsol::Neuron::PYRAMIDAL, layer );

            if ( numNeuronsPerMiniColumnLayer > maxNeuronsPerMiniColumnLayer )
              maxNeuronsPerMiniColumnLayer = numNeuronsPerMiniColumnLayer;

            numNeuronsPerMiniColumnLayer = miniCol->numberOfNeurons(
              false, nsol::Neuron::INTERNEURON, layer );

            if ( numNeuronsPerMiniColumnLayer > maxNeuronsPerMiniColumnLayer )
              maxNeuronsPerMiniColumnLayer = numNeuronsPerMiniColumnLayer;
          }
        }
      }

      int columnsCounter = 0;
      for ( const auto& col : columns )
      {

        unsigned long totalBifurcations = 0;
        double totalSomaArea = .0f;
        double totalSomaVolume = .0f;
        double totalDendsArea = .0f;
        double totalDendsVolume = .0f;
        unsigned int numNeurons = 0;
        Eigen::Vector4f meanCenter( 0.f, 0.f, 0.f, 0.f );


        Eigen::AlignedBox4f boundingBox;
        unsigned int counter = 0;
        for ( const auto& mc : col->miniColumns( ))
          for ( const auto& neuron : mc->neurons( ))
          {
            meanCenter += neuron->transform( ).col( 3 ).transpose( );
            ++counter;
            boundingBox.extend( neuron->transform( ).col( 3 ));
          }
        if ( counter > 0 )
          meanCenter /= counter;

        // std::cout << boundingBox.max( ) << std::endl;
        // std::cout << boundingBox.min( ) << std::endl;
        auto maxMin = boundingBox.max( ) - boundingBox.min( );
        double matrix[ 16 ]  = { 1, 0, 0, 0,
                                 0, 1, 0, 0,
                                 0, 0, 1, 0,
                                 - maxMin.x( ) / 2,
                                 - maxMin.y( ) / 2,
                                 - maxMin.y( ) * 1.5, 1 };
        PaneManager::setViewMatrix( matrix );

        if ( !csvNeuronStatsFileName.empty( ))
        {
          for ( const auto& mc : col->miniColumns( ))
          {
            for ( const auto& neuron : mc->neurons( ))
            {
              auto gid = neuron->gid( );
              numNeurons++;
              totalBifurcations += ( unsigned long )
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_BIFURCATIONS];
              totalSomaVolume +=
                neuronsStats[gid].morphologyStats[NNMS::SOMA_VOLUME];
              totalSomaArea +=
                neuronsStats[gid].morphologyStats[NNMS::SOMA_SURFACE];
              totalDendsVolume +=
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_VOLUME];
              totalDendsArea +=
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_SURFACE];
            }
          }
          if ( neuronsStats.size( ) > 0 )
          {
            float size_1 =  1.0f / float( numNeurons );
            meanSomaArea = totalSomaArea * size_1;
            meanSomaVolume = totalSomaVolume * size_1;
            meanDendsArea = totalDendsArea * size_1;
            meanDendsVolume = totalDendsVolume * size_1;
            meanBifurcations = totalBifurcations * size_1;
          }
        } // if stats loaded from file
        else if ( withMorphologies )
        {
          // column_->stats( )->getStat(
          // nsol::ColumnStats::DENDRITIC_BIFURCATIONS,
          // nsol::TAggregation::MEAN,
          // nsol::TAggregation::MEAN ),
          meanSomaArea = col->stats( )->getStat(
            nsol::ColumnStats::SOMA_SURFACE,
            nsol::TAggregation::MEAN,
            nsol::TAggregation::MEAN );
          meanSomaVolume = col->stats( )->getStat(
            nsol::ColumnStats::SOMA_VOLUME,
            nsol::TAggregation::MEAN,
            nsol::TAggregation::MEAN );
          meanDendsArea = col->stats( )->getStat(
            nsol::ColumnStats::DENDRITIC_SURFACE,
            nsol::TAggregation::MEAN,
            nsol::TAggregation::MEAN );
          meanDendsVolume = col->stats( )->getStat(
            nsol::ColumnStats::DENDRITIC_VOLUME,
            nsol::TAggregation::MEAN,
            nsol::TAggregation::MEAN );
        }
        // std::cout << "Creating col with"
        //           << meanSomaVolume << " "
        //           << meanSomaArea << " "
        //           << meanDendsVolume << " "
        //           << meanDendsArea <<  " "
        //           << meanCenter << std::endl;

        shift::Entity* colEntity =
          new Column(
            uint( col->id( )),
            col->numberOfNeurons( false ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 1 ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 2 ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 3 ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 4 ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 5 ),
            col->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 6 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 1 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 2 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 3 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 4 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 5 ),
            col->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 6 ),
            meanSomaVolume,
            meanSomaArea,
            meanDendsVolume,
            meanDendsArea,
            meanCenter );

        colEntity->label( ) = "c" + std::to_string( uint( col->id( )));

        shift::Entity* colLayerEntities[ 6 ];
        for ( auto i = 0; i < 6; ++i )
        {
          auto layerEntity = new Layer;
          layerEntity->registerProperty( "Parent Id", uint( col->id( )));
          layerEntity->registerProperty( "Parent Type", uint( 0 ));
          layerEntity->registerProperty( "Layer", uint( i+1 ));
          colLayerEntities[ i ] = layerEntity;
          relSuperEntityOf[ colEntity->entityGid( ) ].insert(
            std::make_pair( layerEntity->entityGid( ), nullptr ));
          relSubEntityOf[ layerEntity->entityGid( ) ].entity =
            colEntity->entityGid( );
          _entities.add( layerEntity );

          layerEntity->label( ) = "c" + std::to_string( uint( col->id( ))) +
            "l" + std::to_string( i );

        }

        // Pos 0 and 7 will be used for whole column
        shift::Entity* colNeuronTypeAggregationEntities[ 14 ];
        for ( auto i = 0; i < 7; ++i )
        {
          auto neuronTypeAggregationEntity =
            new NeuronTypeAggregation(
              uint( col->id( )), uint( 0 ), uint( i ),
              Neuron::PYRAMIDAL );
          colNeuronTypeAggregationEntities[ i ] = neuronTypeAggregationEntity;
          shift::Relationship::Establish(
            relSuperEntityOf, relSubEntityOf,
            colEntity, neuronTypeAggregationEntity );
          _entities.add( neuronTypeAggregationEntity );

          neuronTypeAggregationEntity->label( ) = "c" +
            std::to_string( uint( col->id( ))) + "p" + std::to_string( i );

          neuronTypeAggregationEntity =
            new NeuronTypeAggregation(
              uint( col->id( )), uint( 0 ), uint( i ),
              Neuron::INTERNEURON );
          colNeuronTypeAggregationEntities[ i + 7 ] = neuronTypeAggregationEntity;
          shift::Relationship::Establish(
            relSuperEntityOf, relSubEntityOf,
            colEntity, neuronTypeAggregationEntity );
          _entities.add( neuronTypeAggregationEntity );

          neuronTypeAggregationEntity->label( ) = "c" +
            std::to_string( uint( col->id( ))) + "i" + std::to_string( i );
        }

        // fires::PropertyManager::registerProperty(
        //     colEntity, "Id", col->id( ));

        _entities.add( colEntity );
        relParentOf[ 0 ].insert( std::make_pair( colEntity->entityGid( ),
                                                 nullptr));
        relChildOf[ colEntity->entityGid( ) ].entity = 0 ;

        nsol::MiniColumns& miniCols = col->miniColumns( );
        int miniColumnsCounter = 0;
        for ( const auto& mc : miniCols )
        {

          Eigen::Vector4f mcMeanCenter( 0.f, 0.f, 0.f, 0.f );

          unsigned int mcNeuronCounter = 0;
          for ( const auto& neuron : mc->neurons( ))
          {
            mcMeanCenter += neuron->transform( ).col( 3 ).transpose( );
            ++mcNeuronCounter;
          }
          if ( counter > 0 )
            mcMeanCenter /= mcNeuronCounter;

          // std::cout << "mc " << miniColumnsCounter << " center: " << mcMeanCenter << std::endl;
          unsigned long totalMiniColBifurcations = 0;
          double totalMiniColSomaArea = .0f;
          double totalMiniColSomaVolume = .0f;
          double totalMiniColDendsArea = .0f;
          double totalMiniColDendsVolume = .0f;
          unsigned int numMiniColNeurons = 0;
          if ( !csvNeuronStatsFileName.empty( ))
          {
            for ( auto neu : mc->neurons( ))
            {
              auto gid = neu->gid( );
              numMiniColNeurons++;
              totalMiniColBifurcations += ( unsigned long )
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_BIFURCATIONS];
              totalMiniColSomaVolume +=
                neuronsStats[gid].morphologyStats[NNMS::SOMA_VOLUME];
              totalMiniColSomaArea +=
                neuronsStats[gid].morphologyStats[NNMS::SOMA_SURFACE];
              totalMiniColDendsVolume +=
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_VOLUME];
              totalMiniColDendsArea +=
                neuronsStats[gid].morphologyStats[NNMS::DENDRITIC_SURFACE];
            }
            if ( neuronsStats.size( ) > 0 )
            {
              float size_1 =  1.0f / float( numMiniColNeurons );

              meanSomaArea = totalMiniColSomaArea * size_1;
              meanSomaVolume = totalMiniColSomaVolume * size_1;
              meanDendsArea = totalMiniColDendsArea * size_1;
              meanDendsVolume = totalMiniColDendsVolume * size_1;
              meanBifurcations = totalMiniColBifurcations * size_1;
            }
          } else if ( withMorphologies )
          {
            meanSomaArea = mc->stats( )->getStat(
              nsol::MiniColumnStats::SOMA_SURFACE,
              nsol::TAggregation::MEAN );
            meanSomaVolume = mc->stats( )->getStat(
              nsol::MiniColumnStats::SOMA_VOLUME,
              nsol::TAggregation::MEAN );
            meanDendsArea = mc->stats( )->getStat(
              nsol::MiniColumnStats::DENDRITIC_SURFACE,
              nsol::TAggregation::MEAN );
            meanDendsVolume = mc->stats( )->getStat(
              nsol::MiniColumnStats::DENDRITIC_VOLUME,
              nsol::TAggregation::MEAN );
          }

          shift::Entity* mcEntity =
            new MiniColumn(
              mc->id( ),
              mc->numberOfNeurons( false ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 1 ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 2 ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 3 ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 4 ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 5 ),
              mc->numberOfNeurons( false, nsol::Neuron::PYRAMIDAL, 6 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 1 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 2 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 3 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 4 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 5 ),
              mc->numberOfNeurons( false, nsol::Neuron::INTERNEURON, 6 ),
              meanSomaVolume,
              meanSomaArea,
              meanDendsVolume,
              meanDendsArea,
              mcMeanCenter );

          mcEntity->label( ) = "mc" + std::to_string( uint( mc->id( )));

          shift::Entity* mcLayerEntities[ 6 ];
          for ( auto i = 0; i < 6; ++i )
          {
            auto layerEntity = new Layer;
            layerEntity->registerProperty( "Parent Id", uint( mc->id( )));
            layerEntity->registerProperty( "Parent Type", uint( 1 ));
            layerEntity->registerProperty( "Layer", uint( i+1 ));
            mcLayerEntities[ i ] = layerEntity;
            relSuperEntityOf[ mcEntity->entityGid( ) ].insert(
              std::make_pair( layerEntity->entityGid( ), nullptr ));
            relSubEntityOf[ layerEntity->entityGid( ) ].entity =
              mcEntity->entityGid( );
            _entities.add( layerEntity );

            layerEntity->label( ) = "mc" + std::to_string( uint( mc->id( ))) +
              "l" + std::to_string( i + 1 );
          }

          // Pos 0 and 7 will be used for whole minicolumn
          shift::Entity* mcNeuronTypeAggregationEntities[ 14 ];
          for ( auto i = 0; i < 7; ++i )
          {
            auto neuronTypeAggregationEntity =
              new NeuronTypeAggregation(
                uint( mc->id( )), uint( 1 ), uint( i ),
                Neuron::PYRAMIDAL );
            mcNeuronTypeAggregationEntities[ i ] = neuronTypeAggregationEntity;
            shift::Relationship::Establish(
              relSuperEntityOf, relSubEntityOf,
              mcEntity, neuronTypeAggregationEntity );
            _entities.add( neuronTypeAggregationEntity );

            neuronTypeAggregationEntity->label( ) = "mc" +
              std::to_string( uint( mc->id( ))) + "p" + std::to_string( i );

            neuronTypeAggregationEntity =
              new NeuronTypeAggregation(
                uint( mc->id( )), uint( 1 ), uint( i ),
                Neuron::INTERNEURON );
            mcNeuronTypeAggregationEntities[ i + 7 ] =
              neuronTypeAggregationEntity;
            shift::Relationship::Establish(
              relSuperEntityOf, relSubEntityOf,
              mcEntity, neuronTypeAggregationEntity );
            _entities.add( neuronTypeAggregationEntity );

            neuronTypeAggregationEntity->label( ) = "mc" +
              std::to_string( uint( mc->id( ))) + "i" + std::to_string( i );
          }

          // fires::PropertyManager::registerProperty(
          //   mcEntity, "Id", mc->id( ));

          _entities.add( mcEntity );
          relChildOf[ mcEntity->entityGid( ) ].entity = colEntity->entityGid( );
          relParentOf[ colEntity->entityGid( ) ].insert(
            std::make_pair( mcEntity->entityGid( ), nullptr ));

          ///////////////////////////////////////////
          // Neurons ////////////////////////////////
          nsol::Neurons& neurons = mc->neurons( );
          for ( const auto& neuron : neurons )
          {
            shift::Entity* neuronEntity;
            if ( !csvNeuronStatsFileName.empty( ))
            {
              auto neuronGid = neuron->gid( );
#define MORPHO_STATS neuronsStats[neuronGid].morphologyStats
              neuronEntity =
                new shiftgen::Neuron(
                  neuronGid,
                  nsolToShiftMorphologicalType( neuron->morphologicalType( )),
                  nsolToShiftFunctionalType( neuron->functionalType( )),
                  MORPHO_STATS[NNMS::SOMA_VOLUME],
                  MORPHO_STATS[NNMS::SOMA_SURFACE],
                  MORPHO_STATS[NNMS::DENDRITIC_VOLUME],
                  MORPHO_STATS[NNMS::DENDRITIC_SURFACE],
                  neuron->transform( ).col( 3 ).transpose( ));
            }
            else if ( withMorphologies )
            {
              assert( neuron->morphology( ) && neuron->morphology( )->stats( ) );
              nsol::NeuronMorphologyStats* nms = neuron->morphology( )->stats( );
              neuronEntity =
                new shiftgen::Neuron(
              neuron->gid( ),
                nsolToShiftMorphologicalType( neuron->morphologicalType( )),
                nsolToShiftFunctionalType( neuron->functionalType( )),
                nms->getStat( nsol::NeuronMorphologyStats::SOMA_VOLUME ),
                nms->getStat( nsol::NeuronMorphologyStats::SOMA_SURFACE ),
                nms->getStat( nsol::NeuronMorphologyStats::DENDRITIC_VOLUME ),
                nms->getStat( nsol::NeuronMorphologyStats::DENDRITIC_SURFACE ),
                neuron->transform( ).col( 3 ).transpose( ));
            }
            else
            {
              neuronEntity =
                new shiftgen::Neuron(
              neuron->gid( ),
                nsolToShiftMorphologicalType( neuron->morphologicalType( )),
                nsolToShiftFunctionalType( neuron->functionalType( )),
                .0f, .0f, .0f, .0f,
                neuron->transform( ).col( 3 ).transpose( ));
            }

            neuronEntity->label( ) = "n" + std::to_string( uint( neuron->gid( )));

            if ( neuron->morphology( ) && withMorphologies &&
                csvNeuronStatsFileName.empty( ))
            {

              nsol::NeuronMorphologyStats* nms = neuron->morphology( )->stats( );

              for ( int stat_ = 0;
              stat_ < NSOL_NEURON_MORPHOLOGY_NUM_STATS;
              ++stat_ )
              {
              nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat =
                nsol::NeuronMorphologyStats::TNeuronMorphologyStat( stat_ );

              fires::PropertyManager::registerProperty(
              neuronEntity, NeuronMorphologyToLabel( stat ),
                nms->getStat( stat ));
            }
            }
            else if ( !csvNeuronStatsFileName.empty( ))
            {
              for ( int stat_ = 0;
              stat_ < NSOL_NEURON_MORPHOLOGY_NUM_STATS;
              ++stat_ )
              {
              nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat =
                nsol::NeuronMorphologyStats::TNeuronMorphologyStat( stat_ );

              fires::PropertyManager::registerProperty(
              neuronEntity, NeuronMorphologyToLabel( stat ),
                neuronsStats[ neuron->gid( ) ].morphologyStats[stat] );
            }
            }
            fires::PropertyManager::registerProperty(
              neuronEntity, "Layer", uint( neuron->layer( )));


            _entities.add( neuronEntity );
            relChildOf[ neuronEntity->entityGid( ) ].entity =
              mcEntity->entityGid( );
            relParentOf[ mcEntity->entityGid( ) ].insert(
              std::make_pair( neuronEntity->entityGid( ), nullptr ));

            relGroupOf[ mcEntity->entityGid( ) ].insert(
              std::make_pair( neuronEntity->entityGid( ), nullptr ));
            relPartOf[ neuronEntity->entityGid( ) ].insert(
              std::make_pair( mcEntity->entityGid( ), nullptr ));

            relGroupOf[ colEntity->entityGid( ) ].insert(
              std::make_pair( neuronEntity->entityGid( ), nullptr ));
            relPartOf[ neuronEntity->entityGid( ) ].insert(
              std::make_pair( colEntity->entityGid( ), nullptr ));

            for ( auto layer = 0; layer < 6; ++layer )
            {
              if ( neuron->layer( ) == layer+1 )
              {
              shift::Relationship::Establish(
              relGroupOf, relPartOf,
                mcLayerEntities[ layer ], neuronEntity );
              shift::Relationship::Establish(
              relGroupOf, relPartOf,
                colLayerEntities[ layer ], neuronEntity );
              // relGroupOf[ mcLayerEntities[ layer ]->entityGid( ) ].insert(
              //   neuronEntity->entityGid( ));
              // relPartOf[ neuronEntity->entityGid( ) ].insert(
              //   mcLayerEntities[ layer ]->entityGid( ));

              // relGroupOf[ colLayerEntities[ layer ]->entityGid( ) ].insert(
              //   neuronEntity->entityGid( ));
              // relPartOf[ neuronEntity->entityGid( ) ].insert(
              //   colLayerEntities[ layer ]->entityGid( ));
            }
            }


            if ( neuron->morphologicalType( ) == nsol::Neuron::PYRAMIDAL )
            {
              shift::Relationship::Establish(
              relGroupOf, relPartOf,
              mcNeuronTypeAggregationEntities[ 0 ], neuronEntity );
              shift::Relationship::Establish(
                relGroupOf, relPartOf,
                colNeuronTypeAggregationEntities[ 0 ], neuronEntity );
            }
            else if ( neuron->morphologicalType( ) == nsol::Neuron::INTERNEURON )
            {
              shift::Relationship::Establish(
                relGroupOf, relPartOf,
                mcNeuronTypeAggregationEntities[ 7 ], neuronEntity );
              shift::Relationship::Establish(
                relGroupOf, relPartOf,
                colNeuronTypeAggregationEntities[ 7 ], neuronEntity );
            }

            for ( auto layer = 1; layer < 7; ++layer )
            {
              if ( neuron->layer( ) == layer &&
                   neuron->morphologicalType( ) == nsol::Neuron::PYRAMIDAL )
              {
                shift::Relationship::Establish(
                  relGroupOf, relPartOf,
                  mcNeuronTypeAggregationEntities[ layer ], neuronEntity );
                shift::Relationship::Establish(
                  relGroupOf, relPartOf,
                  colNeuronTypeAggregationEntities[ layer ], neuronEntity );
              }
              else if ( neuron->layer( ) == layer &&
                        neuron->morphologicalType( ) ==
                        nsol::Neuron::INTERNEURON )
              {
                shift::Relationship::Establish(
                  relGroupOf, relPartOf,
                  mcNeuronTypeAggregationEntities[ layer+7 ], neuronEntity );
                shift::Relationship::Establish(
                  relGroupOf, relPartOf,
                  colNeuronTypeAggregationEntities[ layer+7 ], neuronEntity );
              }
            }

            // ( void ) mcLayerEntities;
            // ( void ) colLayerEntities;
            // ( void ) relPartOf;
            // ( void ) relGroupOf;

            neuronEntitiesByGid[ neuron->gid( )] = neuronEntity;
          } // for all neurons

          std::cout << "\r("
                    << 100 * ( columnsCounter + 1 ) / columns.size( )
                    << "%) \tColumn:\t " << columnsCounter
                    << "\tMiniColumn:\t " << miniColumnsCounter;

          ++miniColumnsCounter;

        } // for all minicols

        ++columnsCounter;
      } // for all colums

      std::cout << "\n";

      const auto& childrenIds = relParentOf[ 0 ];
      _rootEntities.clear( );
      for ( const auto& child : childrenIds )
        _rootEntities.add( nslib::DataManager::entities( ).at( child.first ));

      unsigned int maxConnectionsPerNeuron = 0;
      for ( const auto& preSynapse:
              circuit.synapses( nsol::Circuit::PRESYNAPTICCONNECTIONS ))
      {
        shift::Entity::EntityGid preNeuronGid =
          neuronEntitiesByGid[ preSynapse->preSynapticNeuron( )]->entityGid( );
        shift::Entity::EntityGid postNeuronGid =
          neuronEntitiesByGid[ preSynapse->postSynapticNeuron( )]->entityGid( );

        auto connectsToIt = relConnectsTo.find( preNeuronGid );
        if( connectsToIt != relConnectsTo.end( ))
        {
          auto connectsToMMIt = connectsToIt->second.find( postNeuronGid );
          if( connectsToMMIt != connectsToIt->second.end( ))
          {
            auto& connectsToProperty =
              connectsToMMIt->second->getProperty( "count" );

            unsigned int value = connectsToProperty.value< unsigned int >( ) + 1;
            if ( value > maxConnectionsPerNeuron )
              maxConnectionsPerNeuron = value;
            connectsToProperty.set< unsigned int >( value );
          }
          else
          {
            relConnectsTo[ preNeuronGid ].insert(
              std::make_pair( postNeuronGid, new ConnectsWith( 1 )));
            relConnectedBy[ postNeuronGid ].insert(
              std::make_pair( preNeuronGid, nullptr ));
          }
        }
        else
        {
          relConnectsTo[ preNeuronGid ].insert(
            std::make_pair( postNeuronGid, new ConnectsWith( 1 )));
          relConnectedBy[ postNeuronGid ].insert(
            std::make_pair( preNeuronGid, nullptr ));
        }
      }
      neuronEntitiesByGid.clear( );

      // auto repCretor = new RepresentationCreator( );
      auto repCreator = RepresentationCreatorManager::getCreator( );
      assert( repCreator );
      auto cortexRepCreator = dynamic_cast< RepresentationCreator* >( repCreator );
      assert( cortexRepCreator );
      cortexRepCreator->setMaximums( maxNeuronSomaVolume, maxNeuronSomaArea,
                                     maxNeuronDendVolume, maxNeuronDendArea,
                                     gids.size( ),
                                     maxNeuronsPerColumnLayer,
                                     maxNeuronsPerMiniColumnLayer,
                                     maxConnectionsPerNeuron );

      // nslib::RepresentationCreatorManager::addCreator( repCretor );

    }
#endif
  } // namespace cortex
} // namespace nslib
