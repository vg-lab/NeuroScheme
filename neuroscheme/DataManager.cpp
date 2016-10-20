#include "DataManager.h"
#include "RepresentationCreatorManager.h"


namespace neuroscheme
{
  shift::EntitiesWithRelationships DataManager::_entities =
    shift::EntitiesWithRelationships( );

  shift::Representations DataManager::_representations =
    shift::Representations( );;

  nsol::DataSet DataManager::_nsolDataSet = nsol::DataSet( );

  shift::EntitiesWithRelationships& DataManager::entities( void )
  {
    return _entities;
  }

  shift::Representations& DataManager::representations( void )
  {
    return _representations;
  }

  void DataManager::loadData( void )
    {
      //Temp function for testing
      // auto relHierarchyChild = ;
      _entities.relationships( )[ "isParentOf" ] =
        new shift::RelationshipOneToN;

      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne;

      shift::Entity* entity =
        new neuroscheme::Column(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added column gid " << entity->entityGid( ) << std::endl;
      auto colId1 = entity->entityGid( );

      entity =
        new neuroscheme::Column(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added column gid " << entity->entityGid( ) << std::endl;
      auto colId2 = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol0Id = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol1Id = entity->entityGid( );

            entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol2Id = entity->entityGid( );

      entity =
        new neuroscheme::MiniColumn(
          100,
          75, 25,
          5, 0,
          10, 5,
          15, 10,
          20, 15,
          25, 20,
          30, 25,
          10.0f, 30.0f,
          30.0f, 50.0f );
      _entities.add( entity );
      std::cout << "Added minicolumn gid " << entity->entityGid( ) << std::endl;
      auto miniCol3Id = entity->entityGid( );

      auto& relParentOf =
        *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      auto& relChildOf =
        *( _entities.relationships( )[ "isChildOf" ]->asOneToOne( ));
      relParentOf[ 0 ].insert( colId1 );
      relParentOf[ 0 ].insert( colId2 );
      relChildOf[ colId1 ] = 0;
      relChildOf[ colId2 ] = 0;

      relParentOf[ colId1 ].insert( miniCol0Id );
      relParentOf[ colId1 ].insert( miniCol1Id );
      relChildOf[ miniCol0Id ] = colId1;
      relChildOf[ miniCol1Id ] = colId1;

      relParentOf[ colId2 ].insert( miniCol2Id );
      relParentOf[ colId2 ].insert( miniCol3Id );
      relChildOf[ miniCol2Id ] = colId2;
      relChildOf[ miniCol3Id ] = colId2;

      unsigned int neuronGid = 0;

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol0Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol0Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol0Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol0Id;

      }

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol1Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol1Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol1Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol1Id;

      }
      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol2Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol2Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol2Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol2Id;

      }

      for ( unsigned int i = 0; i < 5; i++ )
      {
        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::INTERNEURON,
          neuroscheme::Neuron::EXCITATORY,
          10.0f, 30.0f, 30.0f, 50.0f );
        _entities.add( entity );
        auto entityGid = entity->entityGid( );
        relParentOf[ miniCol3Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol3Id;

        entity = new neuroscheme::Neuron(
          neuronGid++,
          neuroscheme::Neuron::PYRAMIDAL,
          neuroscheme::Neuron::INHIBITORY,
          70.0f, 60.0f, 20.0f, 30.0f );
        _entities.add( entity );
        entityGid = entity->entityGid( );
        relParentOf[ miniCol3Id ].insert( entityGid );
        relChildOf[ entityGid ] = miniCol3Id;

      }

      // Display root reps
      shift::Entities rootEntities;
      // auto& relParentOf =
      //   *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
      // auto& relParentOf = *( neuroscheme::DataManager::entities( ).
      //                        relationships( )[ "isParentOf" ]->asOneToN( ));

      const auto& childrenIds = relParentOf[ 0 ];
      std::cout << "-- Root entities " << childrenIds.size( ) << std::endl;
      for ( const auto& child : childrenIds )
        rootEntities[child] =
          neuroscheme::DataManager::entities( )[child];

      neuroscheme::RepresentationCreatorManager::addCreator(
        new neuroscheme::cortex::RepresentationCreator );
      neuroscheme::RepresentationCreatorManager::create(
        rootEntities, _representations,
        true, true );

    } // loadData

  void DataManager::loadBlueConfig( const std::string& blueConfig,
                                    const std::string& targetLabel,
                                    const bool loadMorphologies,
                                    const std::string& csvNeuronStatsFileName )
  {
    QErrorMessage errorMessage;
#ifndef NSOL_USE_BRION
    ( void ) blueConfig;
    ( void ) targetLabel;
    ( void ) loadMorphologies;
    ( void ) csvNeuronStatsFileName;
    errorMessage.showMessage( "Brion support not built-in" );
    return;
#else
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
        _dataSet.loadAllMorphologies<
          nsol::Node,
          nsol::SectionStats,
          nsol::DendriteStats,
          nsol::AxonStats,
          nsol::SomaStats,
          nsol::NeuronMorphologyCachedStats,
          nsol::Neuron,
          nsol::MiniColumnStats,
          nsol::ColumnStats >( );

    } catch ( ... )
    {
      std::cerr << "Error loading BlueConfig" << std::endl;
      _errorMessage.showMessage( "Error loading BlueConfig" );
      return;
    }
#endif
  }

#ifdef NEUROSCHEME_USE_NSOL
  void DataManager::CreateEntitiesFromNsolColumn(
    const nsol::Columns& columns,
    bool withMorphologies,
    const std::string& csvNeuronStatsFileName )
  {
    fires::PropertyManager::clear( );

    auto& relParentOf =
      *( _entities.relationships( )[ "isParentOf" ]->asOneToN( ));
    auto& relChildOf =
      *( _entities.relationships( )[ "isChildOf" ]->asOneToOne( ));

    std::set< unsigned int > gids;
    for ( const auto& col : columns )
      for ( const auto mc& : col->miniColumns( ))
        for ( const auto neuron& : mc->neurons( ))
          gids.insert( neuron->gid( ));

    float maxNeuronSomaVolume = 0.0f;
    float maxNeuronSomaArea = 0.0f;
    float maxNeuronDendVolume = 0.0f;
    float maxNeuronDendArea = 0.0f;
    float maxNeuronAxonVolume = 0.0f;
    float maxNeuronAxonArea = 0.0f;

    unsigned int  meanBifurcations = 0;
    float meanSomaArea = .0f;
    float meanSomaVolume = .0f;
    float meanDendsArea = .0f;
    float meanDendsVolume = .0f;

#define NSOL_NEURON_MORPHOLOGY_NUM_STATS                      \
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
            neuroscheme::Log::log( std::string( "Skipping lineString " ) +
                                   std::to_string( lineCount ) +
                                   std::string( ". Not enough fields found" ),
                                   neuroscheme::LOG_LEVEL_WARNING );
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

      for ( const auto col : columns )
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
      auto greenMapper = new DiscreteColorMapper( );
      auto redMapper = new DiscreteColorMapper( );

#define ColorF( r, g, b )                       \
      Color( uint8_t( roundf( r )),             \
             uint8_t( roundf( g )),             \
             uint8_t( roundf( b )))
      greenMapper->push_back( ColorF( 0.62f, 1.00f, 0.75f ));
      greenMapper->push_back( ColorF( 0.55f, 0.88f, 0.65f ));
      greenMapper->push_back( ColorF( 0.46f, 0.75f, 0.56f ));
      greenMapper->push_back( ColorF( 0.40f, 0.63f, 0.47f ));
      greenMapper->push_back( ColorF( 0.31f, 0.51f, 0.37f ));
      greenMapper->push_back( ColorF( 0.22f, 0.38f, 0.27f ));

      redMapper->push_back( ColorF( 1.00f, 0.62f, 0.75f ));
      redMapper->push_back( ColorF( 0.88f, 0.55f, 0.65f ));
      redMapper->push_back( ColorF( 0.75f, 0.46f, 0.56f ));
      redMapper->push_back( ColorF( 0.63f, 0.40f, 0.47f ));
      redMapper->push_back( ColorF( 0.51f, 0.31f, 0.37f ));
      redMapper->push_back( ColorF( 0.38f, 0.22f, 0.27f ));

      greenMapper->max( ) = maxNeuronSomaVolume;
      redMapper->max( ) = maxNeuronDendVolume;

      auto somaAreaToAngle =
        new MapperFloatToFloat( 0, maxNeuronSomaArea, 0, -360 );
      auto dendAreaToAngle =
        new MapperFloatToFloat( 0, maxNeuronDendArea, 0, -360 );
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

    _entities.clear( );

    for ( const auto& col : columns )
    {

      unsigned long totalBifurcations = 0;
      double totalSomaArea = .0f;
      double totalSomaVolume = .0f;
      double totalDendsArea = .0f;
      double totalDendsVolume = .0f;
      unsigned int numNeurons = 0;
      Eigen::Vector4f meanCenter( 0.f, 0.f, 0.f, 0.f );

      unsigned int counter = 0;
      for ( const auto& mc : col->miniColumns( ))
        for ( const auto& neuron : mc->neurons( ))
        {
          meanCenter += neuron->transform( ).col( 3 ).transpose( );
          ++counter;
        }
      if ( counter > 0 )
        meanCenter /= counter;

      if ( !csvNeuronStatsFileName.empty( ))
      {
        for ( const auto& mc : col->miniColumns( ))
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
        if ( neuronsStats.size( ) > 0 )
        {
          float size_1 =  1.0f / float( numNeurons );
          meanSomaArea = totalSomaArea * size_1;
          meanSomaVolume = totalSomaVolume * size_1;
          meanDendsArea = totalDendsArea * size_1;
          meanDendsVolume = totalDendsVolume * size_1;
          meanBifurcations = totalBifurcations * size_1;
        }
      }

      shift::Entity* colEntity =
        new neuroscheme::Column(
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

      _entities.add( colEntity );
      relChildOf[ colEntity->entityGid( ) ] = 0;

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
            float size_1 =  1.0f / float( numMiniColNeurons );

            meanSomaArea = totalMiniColSomaArea * size_1;
            meanSomaVolume = totalMiniColSomaVolume * size_1;
            meanDendsArea = totalMiniColDendsArea * size_1;
            meanDendsVolume = totalMiniColDendsVolume * size_1;
            meanBifurcations = totalMiniColBifurcations * size_1;
          }
        }

        shift::Entity* mcEntity =
          new neuroscheme::MiniColumn(
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
            meanCenter );

        _entities.add( mcEntity );

        relChildOf[ mcEntity->entityGid( ) ] = colEntity->entityGid( );
        relParentOf[ colEntity->entityGid( ) ].insert( mcEntity->entityGid( ));
      }
    }
  }
#endif


} // namespace neuroscheme
