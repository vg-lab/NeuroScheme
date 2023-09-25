/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include <nslib/Loggers.h>
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

    bool DataLoader::cliLoadData( const ::nslib::NeuroSchemeInputArguments& args )
    {
#ifdef NEUROSCHEME_USE_NSOL

      if ( args.empty( ))
      {
        Loggers::get( )->log( "No arguments provided",
                              LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        return false;
      }

      if ( args.count( "-bc" ) > 0 && args.count( "-xml" ) > 0 )
      {
        Loggers::get( )->log( "-bc and -xml arguments are exclusive",
                              LOG_LEVEL_CRITICAL, NEUROSCHEME_FILE_LINE );
        return false;
      }

      if ( args.count( "-bc" ) == 1 )
      {
        if ( args.at( "-bc" ).size( ) != 1 )
        {
          Loggers::get( )->log( "-bc expect one filename, but " +
                                std::to_string(args.at( "-bc" ).size( )) +
                                " were found.",
                                LOG_LEVEL_CRITICAL, NEUROSCHEME_FILE_LINE );
          return false;
        }

        if ( args.count( "-target" ) != 1 )
        {
          Loggers::get( )->log( "-bc provided but no -target found",
                                LOG_LEVEL_CRITICAL, NEUROSCHEME_FILE_LINE );
          return false;
        }

        if ( args.at( "-target" ).size( ) != 1 )
        {
          Loggers::get( )->log( "-target expect one target name but " +
                                std::to_string(args.at( "-target" ).size( )) +
                                " were found.",
                                LOG_LEVEL_CRITICAL, NEUROSCHEME_FILE_LINE );
          return false;
        }

        if ( args.count( "-cns" ) == 1 )
        {
          if ( args.at( "-cns" ).size( ) != 1 )
          {
            Loggers::get( )->log( "-cns expect one csv file, but " +
                                  std::to_string(args.at( "-cns" ).size( )) +
                                  " were found." );
            return false;
          }
        }

        Loggers::get( )->log( "Loading blue config",
                              nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        nslib::DataManager::loadBlueConfig(
          args.at( "-bc" )[0],
          args.at( "-target" )[0],
          args.count( "-nm" ) == 0 && args.count( "--no-morphologies" ) == 0,
          ( args.count( "-cns" ) == 1 ? args.at( "-cns" )[0] : std::string( )),
          args.count( "-lc" ) == 1 || args.count( "--load-connectivity" ) == 1 );

        createEntitiesFromNsolColumns(
          nslib::DataManager::nsolDataSet( ).columns( ),
          nslib::DataManager::nsolDataSet( ).circuit( ),
          args.count( "-nm" ) == 0,
          ( args.count( "-cns" ) == 1 ? args.at( "-cns" )[0] : std::string( )));
      }

      if ( args.count( "-xml" ) == 1 )
      {
        if ( args.at( "-xml" ).size( ) != 1 )
        {
          Loggers::get( )->log( "-xml expect one filename, but " +
                                std::to_string( args.at( "-xml" ).size( )) +
                                " were found.",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          return false;
        }

        Loggers::get( )->log(
          "Loading nsol xml",
          nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        nslib::DataManager::loadNsolXmlScene( args.at( "-xml" )[0] );

        createEntitiesFromNsolColumns(
          nslib::DataManager::nsolDataSet( ).columns( ),
          nslib::DataManager::nsolDataSet( ).circuit( ));
      }

      nslib::DataManager::nsolDataSet( ).close( );

      return true;
#else
      if ( args.count( "-bc" ) > 0 || args.count( "-xml" ) > 0 )
      {
        nslib::Loggers::get( )->log( "nsol not built in.", LOG_LEVEL_ERROR,
                                     NEUROSCHEME_FILE_LINE );
        return false;
      }

      return true;
#endif
    }

#ifdef NEUROSCHEME_USE_NSOL

    std::string NeuronMorphologyToLabel( nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat )
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
      Loggers::get( )->log( "Creating entities",
                            nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

      auto& _entities = nslib::DataManager::entities( );
      auto& _rootEntities = nslib::DataManager::rootEntities( );

      _entities.clear( );

      auto& relParentOf=
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
        Loggers::get( )->log( "Loading neuron morphology stats from " +
                              csvNeuronStatsFileName, LOG_LEVEL_VERBOSE,
                              NEUROSCHEME_FILE_LINE );

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
              Loggers::get( )->log(
                std::string( "Skipping lineString " ) +
                std::to_string( lineCount ) +
                std::string( ". Expected 26 fields, but found " ) +
                std::to_string( fields ) + ".",
                nslib::LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE);
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

          const auto neuronSomaVolume =
            neuronStats.second.morphologyStats[NNMS::SOMA_VOLUME];
          if ( neuronSomaVolume > maxNeuronSomaVolume )
            maxNeuronSomaVolume = neuronSomaVolume;
          totalSomaVolume += neuronSomaVolume;

          const auto neuronSomaArea =
            neuronStats.second.morphologyStats[NNMS::SOMA_SURFACE];
          if ( neuronSomaArea > maxNeuronSomaArea )
            maxNeuronSomaArea = neuronSomaArea;
          totalSomaArea += neuronSomaArea;

          const auto neuronDendVolume =
            neuronStats.second.morphologyStats[NNMS::DENDRITIC_VOLUME];
          if ( neuronDendVolume > maxNeuronDendVolume )
            maxNeuronDendVolume = neuronDendVolume;
          totalDendsVolume += neuronDendVolume;

          const auto neuronDendArea =
            neuronStats.second.morphologyStats[NNMS::DENDRITIC_SURFACE];
          if ( neuronDendArea > maxNeuronDendArea )
            maxNeuronDendArea = neuronDendArea;
          totalDendsArea += neuronDendArea;

          const auto neuronAxonVolume =
            neuronStats.second.morphologyStats[NNMS::AXON_VOLUME];
          if ( neuronAxonVolume > maxNeuronAxonVolume )
            maxNeuronAxonVolume = neuronAxonVolume;

          const auto neuronAxonArea =
            neuronStats.second.morphologyStats[NNMS::AXON_SURFACE];
          if ( neuronAxonArea > maxNeuronAxonArea )
            maxNeuronAxonArea = neuronAxonArea;
        } // for all neurons

        if ( neuronsStats.size( ) > 0 )
        {
          const float size_1 =  1.0f / float( neuronsStats.size( ));

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

          const float colMaxNeuronSomaVolume =
            col->stats( )->getStat( nsol::ColumnStats::SOMA_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          const float colMaxNeuronSomaArea =
            col->stats( )->getStat( nsol::ColumnStats::SOMA_SURFACE,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          const float colMaxNeuronDendVolume =
            col->stats( )->getStat( nsol::ColumnStats::DENDRITIC_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          const float colMaxNeuronDendArea =
            col->stats( )->getStat( nsol::ColumnStats::DENDRITIC_SURFACE,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          const float colMaxNeuronAxonVolume =
            col->stats( )->getStat( nsol::ColumnStats::AXON_VOLUME,
                                    nsol::TAggregation::MAX,
                                    nsol::TAggregation::MAX );

          const float colMaxNeuronAxonArea =
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

        const auto maxMin = boundingBox.max( ) - boundingBox.min( );
        const double matrix[ 16 ]  = { 1, 0, 0, 0,
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
              const auto gid = neuron->gid( );
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
            const float size_1 =  1.0f / float( numNeurons );
            meanSomaArea = totalSomaArea * size_1;
            meanSomaVolume = totalSomaVolume * size_1;
            meanDendsArea = totalDendsArea * size_1;
            meanDendsVolume = totalDendsVolume * size_1;
            meanBifurcations = totalBifurcations * size_1;
          }
        } // if stats loaded from file
        else if ( withMorphologies )
        {
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


        shift::Entity* colEntity =
          new Column(
            "c" + std::to_string( uint( col->id( ))),
            uint( col->id( )), // Id
            0, // Num Minicolumns
            col->numberOfNeurons( false ),
            0, // Num Neurons Mean
            0, // Num Neurons Max
            0, // Num Neurons Min
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

        shift::Entity* colLayerEntities[ 6 ];
        for ( auto i = 0; i < 6; ++i )
        {
          auto layerEntity = new Layer;
          layerEntity->registerProperty( "Parent gid", uint( colEntity->entityGid( )));
          layerEntity->registerProperty( "Parent Id", uint( col->id( )));
          layerEntity->registerProperty( "Parent Type",
                                         Layer::TLayerParentType::COLUMN);
          layerEntity->registerProperty( "Layer", uint( i+1 ));
          colLayerEntities[ i ] = layerEntity;
          relSuperEntityOf[ colEntity->entityGid( ) ].insert(
            std::make_pair( layerEntity->entityGid( ), nullptr ));
          relSubEntityOf[ layerEntity->entityGid( ) ].entity =
            colEntity->entityGid( );
          _entities.add( layerEntity );

          layerEntity->setProperty( "Name", "c" + std::to_string( uint( col->id( ))) +
            "l" + std::to_string( i ));

        }

        // Pos 0 and 7 will be used for whole column
        shift::Entity* colNeuronTypeAggregationEntities[ 14 ];
        for ( auto i = 0; i < 7; ++i )
        {
          auto neuronTypeAggregationEntity =
            new NeuronTypeAggregation(
              colEntity->entityGid( ), uint( col->id( )),
              Layer::TLayerParentType::COLUMN, uint( i ),
              Neuron::TMorphologicalType::PYRAMIDAL );
          colNeuronTypeAggregationEntities[ i ] = neuronTypeAggregationEntity;
          shift::Relationship::Establish(
            relSuperEntityOf, relSubEntityOf,
            colEntity, neuronTypeAggregationEntity );
          _entities.add( neuronTypeAggregationEntity );

          neuronTypeAggregationEntity->setProperty( "Name", "c" +
            std::to_string( uint( col->id( ))) + "p" + std::to_string( i ));

          neuronTypeAggregationEntity =
            new NeuronTypeAggregation(
              colEntity->entityGid( ), uint( col->id( )),
              Layer::TLayerParentType::COLUMN, uint( i ),
              Neuron::INTERNEURON );
          colNeuronTypeAggregationEntities[ i + 7 ] = neuronTypeAggregationEntity;
          shift::Relationship::Establish(
            relSuperEntityOf, relSubEntityOf,
            colEntity, neuronTypeAggregationEntity );
          _entities.add( neuronTypeAggregationEntity );

          neuronTypeAggregationEntity->setProperty( "Name", "c" +
            std::to_string( uint( col->id( ))) + "i" + std::to_string( i ));
        }

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
              const float size_1 =  1.0f / float( numMiniColNeurons );

              meanSomaArea = totalMiniColSomaArea * size_1;
              meanSomaVolume = totalMiniColSomaVolume * size_1;
              meanDendsArea = totalMiniColDendsArea * size_1;
              meanDendsVolume = totalMiniColDendsVolume * size_1;
              meanBifurcations = totalMiniColBifurcations * size_1;
            }
          }
          else
          {
            if ( withMorphologies )
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
          }

          shift::Entity* mcEntity =
            new MiniColumn(
              "mc" + std::to_string( uint( mc->id( ))),
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

          shift::Entity* mcLayerEntities[ 6 ];
          for ( auto i = 0; i < 6; ++i )
          {
            auto layerEntity = new Layer;
            layerEntity->registerProperty( "Parent gid",
                                           uint( mcEntity->entityGid( )));
            layerEntity->registerProperty( "Parent Id", uint( mc->id( )));
            layerEntity->registerProperty( "Parent Type",
                                           Layer::TLayerParentType::MINICOLUMN );
            layerEntity->registerProperty( "Layer", uint( i+1 ));
            mcLayerEntities[ i ] = layerEntity;
            relSuperEntityOf[ mcEntity->entityGid( ) ].insert(
              std::make_pair( layerEntity->entityGid( ), nullptr ));
            relSubEntityOf[ layerEntity->entityGid( ) ].entity =
              mcEntity->entityGid( );
            _entities.add( layerEntity );

            layerEntity->setProperty( "Name", "mc" +
             std::to_string( uint( mc->id( ))) + "l" + std::to_string( i + 1 ));
          }

          // Pos 0 and 7 will be used for whole minicolumn
          shift::Entity* mcNeuronTypeAggregationEntities[ 14 ];
          for ( auto i = 0; i < 7; ++i )
          {
            auto neuronTypeAggregationEntity =
              new NeuronTypeAggregation(
                mcEntity->entityGid( ), uint( mc->id( )),
                Layer::TLayerParentType::MINICOLUMN, uint( i ),
                Neuron::TMorphologicalType::PYRAMIDAL );
            mcNeuronTypeAggregationEntities[ i ] = neuronTypeAggregationEntity;
            shift::Relationship::Establish(
              relSuperEntityOf, relSubEntityOf,
              mcEntity, neuronTypeAggregationEntity );
            _entities.add( neuronTypeAggregationEntity );

            neuronTypeAggregationEntity->setProperty( "Name", "mc" +
              std::to_string( uint( mc->id( ))) + "p" + std::to_string( i ));

            neuronTypeAggregationEntity =
              new NeuronTypeAggregation(
                mcEntity->entityGid( ), uint( mc->id( )),
                Layer::TLayerParentType::MINICOLUMN, uint( i ),
                Neuron::INTERNEURON );
            mcNeuronTypeAggregationEntities[ i + 7 ] =
              neuronTypeAggregationEntity;
            shift::Relationship::Establish(
              relSuperEntityOf, relSubEntityOf,
              mcEntity, neuronTypeAggregationEntity );
            _entities.add( neuronTypeAggregationEntity );

            neuronTypeAggregationEntity->setProperty( "Name","mc" +
              std::to_string( uint( mc->id( ))) + "i" + std::to_string( i ));
          }

          shift::Relationship::Establish(
            relParentOf, relChildOf, colEntity, mcEntity);

          _entities.add( mcEntity );

          ///////////////////////////////////////////
          // Neurons ////////////////////////////////
          nsol::Neurons& neurons = mc->neurons( );
          for ( const auto& neuron : neurons )
          {
            shift::Entity* neuronEntity;
            if ( !csvNeuronStatsFileName.empty( ))
            {
              const auto neuronGid = neuron->gid( );
#define MORPHO_STATS neuronsStats[neuronGid].morphologyStats
              neuronEntity =
                new shiftgen::Neuron(
                  "n" + std::to_string( neuronGid ), neuronGid,
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
                "n" + std::to_string( uint( neuron->gid( ))), neuron->gid( ),
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
                "n" + std::to_string( uint( neuron->gid( ))),neuron->gid( ),
                nsolToShiftMorphologicalType( neuron->morphologicalType( )),
                nsolToShiftFunctionalType( neuron->functionalType( )),
                .0f, .0f, .0f, .0f,
                neuron->transform( ).col( 3 ).transpose( ));
            }

            if ( neuron->morphology( ) && withMorphologies &&
                csvNeuronStatsFileName.empty( ))
            {
              nsol::NeuronMorphologyStats* nms = neuron->morphology( )->stats( );

              for ( int stat_ = 0; stat_ < NSOL_NEURON_MORPHOLOGY_NUM_STATS; ++stat_ )
              {
                nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat =
                  nsol::NeuronMorphologyStats::TNeuronMorphologyStat( stat_ );

                fires::PropertyManager::registerProperty(
                neuronEntity, NeuronMorphologyToLabel( stat ),
                  nms->getStat( stat ));
              }
            }
            else
            {
              if ( !csvNeuronStatsFileName.empty( ))
              {
                for ( int stat_ = 0; stat_ < NSOL_NEURON_MORPHOLOGY_NUM_STATS; ++stat_ )
                {
                  nsol::NeuronMorphologyStats::TNeuronMorphologyStat stat =
                    nsol::NeuronMorphologyStats::TNeuronMorphologyStat( stat_ );

                  fires::PropertyManager::registerProperty(
                  neuronEntity, NeuronMorphologyToLabel( stat ),
                    neuronsStats[ neuron->gid( ) ].morphologyStats[stat] );
                }
              }
            }

            fires::PropertyManager::registerProperty(
              neuronEntity, "Layer", uint( neuron->layer( )));

            _entities.add( neuronEntity );

            shift::Relationship::Establish( relParentOf, relChildOf,
                                            mcEntity, neuronEntity );

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

            neuronEntitiesByGid[ neuron->gid( )] = neuronEntity;
          } // for all neurons

          ++miniColumnsCounter;

        } // for all minicols

        ++columnsCounter;
      } // for all colums

      const auto& childrenIds = relParentOf[ 0 ];
      _rootEntities.clear( );
      for ( const auto& child : childrenIds )
        _rootEntities.add( nslib::DataManager::entities( ).at( child.first ));

      unsigned int maxConnectionsPerNeuron = 0;
      for ( const auto& preSynapse:
              circuit.synapses( nsol::Circuit::PRESYNAPTICCONNECTIONS ))
      {
        const auto preNeuronGid =
          neuronEntitiesByGid[ preSynapse->preSynapticNeuron( )]->entityGid( );
        const auto postNeuronGid =
          neuronEntitiesByGid[ preSynapse->postSynapticNeuron( )]->entityGid( );

        auto connectsToIt = relConnectsTo.find( preNeuronGid );
        if( connectsToIt != relConnectsTo.end( ))
        {
          auto connectsToMMIt = connectsToIt->second.find( postNeuronGid );
          if( connectsToMMIt != connectsToIt->second.end( ))
          {
            auto& connectsToProperty =
              connectsToMMIt->second->getProperty( "count" );

            const unsigned int value = connectsToProperty.value< unsigned int >( ) + 1;
            if ( value > maxConnectionsPerNeuron )
              maxConnectionsPerNeuron = value;
            connectsToProperty.set< unsigned int >( value );
          }
          else
          {
            const std::string connectionName = neuronEntitiesByGid[ preSynapse->
              preSynapticNeuron( )]->getProperty( "Entity name" ).
              value<std::string>( ) + std::string("-") +
              neuronEntitiesByGid[ preSynapse->
              postSynapticNeuron( )]->getProperty( "Entity name" ).
              value<std::string>( );
            relConnectsTo[ preNeuronGid ].insert(
              std::make_pair( postNeuronGid, new ConnectsWith(
              connectionName, 1 )));
            relConnectedBy[ postNeuronGid ].insert(
              std::make_pair( preNeuronGid, nullptr ));
          }
        }
        else
        {
          const std::string connectionName = neuronEntitiesByGid[ preSynapse->
            preSynapticNeuron( )]->getProperty( "Entity name" ).
            value<std::string>( ) + std::string("-") +
            neuronEntitiesByGid[ preSynapse->
            postSynapticNeuron( )]->getProperty( "Entity name" ).
            value<std::string>( );
          relConnectsTo[ preNeuronGid ].insert(
            std::make_pair( postNeuronGid, new ConnectsWith(
              connectionName, 1 )));
          relConnectedBy[ postNeuronGid ].insert(
            std::make_pair( preNeuronGid, nullptr ));
        }
      }
      neuronEntitiesByGid.clear( );

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
    }
#endif
  } // namespace cortex
} // namespace nslib
