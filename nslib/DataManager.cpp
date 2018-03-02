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
#include "Loggers.h"
#include "DataManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
//#include "domains/domains.h"
#include "error.h"
#include <QMessageBox>


namespace nslib
{
  shift::Entities DataManager::_rootEntities =
    shift::Entities( );
#ifdef NEUROSCHEME_USE_NSOL
  nsol::DataSet DataManager::_nsolDataSet = nsol::DataSet( );
#endif

  shift::EntitiesWithRelationships& DataManager::entities( void )
  {
    return shift::EntitiesWithRelationships::entities( );
    // return _entities;
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
#endif

  void DataManager::loadBlueConfig( const std::string& blueConfig,
                                    const std::string& targetLabel,
                                    const bool loadMorphologies,
                                    const std::string& csvNeuronStatsFileName,
                                    const bool loadConnectivity )
  {
#ifndef NSOL_USE_BRION
    ( void ) blueConfig;
    ( void ) targetLabel;
    ( void ) loadMorphologies;
    ( void ) csvNeuronStatsFileName;
    ( void ) loadConnectivity;

    Loggers::get( )->log("Error loading BlueConfig: Brion support not built-in",
                         LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
    QMessageBox::critical(0, "Error loading BlueConfig",
                         "Brion support not built-in");
    return;
#else
    ( void ) csvNeuronStatsFileName;
    try
    {

      //this->closeData( );
      _nsolDataSet.loadBlueConfigHierarchy<
        nsol::Node,
        nsol::NeuronMorphologySectionStats,
        nsol::DendriteStats,
        nsol::AxonStats,
        nsol::SomaStats,
        nsol::NeuronMorphologyCachedStats,
        nsol::Neuron,
        nsol::MiniColumnStats,
        nsol::ColumnStats >( blueConfig,
                             targetLabel );

      if ( loadMorphologies )
      {
        _nsolDataSet.loadAllMorphologies<
          nsol::Node,
          nsol::NeuronMorphologySectionStats,
          nsol::DendriteStats,
          nsol::AxonStats,
          nsol::SomaStats,
          nsol::NeuronMorphologyCachedStats,
          nsol::Neuron,
          nsol::MiniColumnStats,
          nsol::ColumnStats >( );
      }
      if ( loadConnectivity )
      {
        _nsolDataSet.loadBlueConfigConnectivity<
          nsol::Node,
          nsol::NeuronMorphologySectionStats,
          nsol::DendriteStats,
          nsol::AxonStats,
          nsol::SomaStats,
          nsol::NeuronMorphologyCachedStats,
          nsol::Neuron,
          nsol::MiniColumnStats,
          nsol::ColumnStats >( );
      }
    } catch ( std::exception& ex )
    {
      std::string msg("Error loading BlueConfig: " + std::string( ex.what( )));
      Loggers::get( )->log(msg, LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

      QMessageBox::critical(0, "Error loading BlueConfig",
                            QString::fromStdString(msg));
      return;
    }
    // createEntitiesFromNsolColumns( _nsolDataSet.columns( ), loadMorphologies,
    //                                csvNeuronStatsFileName );

#endif
  }


  void DataManager::loadNsolXmlScene( const std::string& xmlSceneFile )
  {
#ifdef NSOL_USE_QT5CORE
    try
    {
      // this->CloseData( );
      _nsolDataSet.loadXmlScene<
        nsol::NodeCached,
        nsol::NeuronMorphologySectionCachedStats,
        nsol::DendriteCachedStats,
        nsol::AxonCachedStats,
        nsol::SomaStats,
        nsol::NeuronMorphologyCachedStats,
        nsol::Neuron,
        nsol::MiniColumnStats,
        nsol::ColumnStats >( xmlSceneFile );
    } catch ( std::exception& ex )
    {
      Loggers::get( )->log( "Error loading XML scene: " +
                           std::string( ex.what( )), LOG_LEVEL_ERROR,
                           NEUROSCHEME_FILE_LINE );
      QMessageBox::critical(0, "Error loading XML Scene",
                            QString::fromStdString(xmlSceneFile + " : " + ex.what( )));
      return;
    }

#else
    (void) xmlSceneFile;
    Loggers::get( )->log( "nsol not built or built without QtCore",
                         LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

#endif
  }


} // namespace nslib
