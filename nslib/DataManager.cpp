#include "DataManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
//#include "domains/domains.h"
#include "error.h"
#include <Eigen/Geometry>


namespace nslib
{
  shift::EntitiesWithRelationships DataManager::_entities =
    shift::EntitiesWithRelationships( );
  shift::Entities DataManager::_rootEntities =
    shift::Entities( );
#ifdef NEUROSCHEME_USE_NSOL
  nsol::DataSet DataManager::_nsolDataSet = nsol::DataSet( );
#endif

  shift::EntitiesWithRelationships& DataManager::entities( void )
  {
    return _entities;
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
    auto  errorMessage = new QErrorMessage;
#ifndef NSOL_USE_BRION
    ( void ) blueConfig;
    ( void ) targetLabel;
    ( void ) loadMorphologies;
    ( void ) csvNeuronStatsFileName;
    ( void ) loadConnectivity;
    errorMessage->showMessage( "Brion support not built-in" );
    return;
#else
    ( void ) csvNeuronStatsFileName;
    try
    {

      //this->closeData( );
      _nsolDataSet.loadBlueConfigHierarchy<
        nsol::Node,
        nsol::SectionStats,
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
          nsol::SectionStats,
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
          nsol::SectionStats,
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
      Log::log( std::string( "Error loading BlueConfig: " ) +
                std::string( ex.what( )), LOG_LEVEL_ERROR );
      errorMessage->showMessage( "Error loading BlueConfig" );
      return;
    }
    // createEntitiesFromNsolColumns( _nsolDataSet.columns( ), loadMorphologies,
    //                                csvNeuronStatsFileName );

#endif
  }


  void DataManager::loadNsolXmlScene( const std::string& xmlSceneFile )
  {
    #ifdef NSOL_USE_QT5CORE
    auto errorMessage = new QErrorMessage;
    try
    {
      // this->CloseData( );
      _nsolDataSet.loadXmlScene<
        nsol::NodeCached,
        nsol::SectionCachedStats,
        nsol::DendriteCachedStats,
        nsol::AxonCachedStats,
        nsol::SomaStats,
        nsol::NeuronMorphologyCachedStats,
        nsol::Neuron,
        nsol::MiniColumnStats,
        nsol::ColumnStats >( xmlSceneFile );
    } catch ( std::exception& ex )
    {
      Log::log( std::string( "Error loading scene: " ) +
                std::string( ex.what( )), LOG_LEVEL_ERROR );
      errorMessage->showMessage( QString("Error loading scene") +
                                 QString(ex.what( )));
      return;
    }

    #else
    (void) xmlSceneFile;
    Log::log( "nsol not built or built without QtCore",
              LOG_LEVEL_ERROR );

    #endif
  }


} // namespace nslib
