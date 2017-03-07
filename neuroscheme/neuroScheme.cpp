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
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <QApplication>
#include <nslib/Log.h>
#include <nslib/Config.h>
#include <nslib/ZeroEQManager.h>
#include <nslib/reps/SelectableItem.h>
#include <nslib/version.h>
#include "MainWindow.h"


bool atLeastTwo( bool a, bool b, bool c )
{
  return a ^ b ? c : a;
}

void usageMessage( void )
{
  std::cerr << std::endl;
  std::cerr << "TODO" << std::endl;
  exit( -1 );
}

void dumpVersion( void )
{

  std::cerr << std::endl
            << "neuroscheme "
            << nslib::Version::getMajor( ) << "."
            << nslib::Version::getMinor( ) << "."
            << nslib::Version::getPatch( )
            << " (" << nslib::Version::getRevision( ) << ")"
            << std::endl << std::endl;

  std::cerr << "Brion support built-in: ";
#ifdef NSOL_USE_BRION
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

  std::cerr << "ZeroEQ support built-in: ";
#ifdef NEUROSCHEME_USE_ZEROEQ
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

  std::cerr << "FiReS support built-in: ";
#ifdef NEUROSCHEME_USE_FIRES
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

  std::cerr << "GmrvLex support built-in: ";
#ifdef NEUROSCHEME_USE_GMRVLEX
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

  std::cerr << "Deflect support built-in: ";
#ifdef NEUROSCHEME_USE_DEFLECT
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

  std::cerr << std::endl;


}


void parseArguments( int argc, char** argv,
                     nslib::NeuroSchemeInputArguments& args )
{
  for ( int i = 1; i < argc; i++ )
  {
    if ( argv[i][0] == '-' )
    {
      std::string mainArg( argv[i] );
      // If args already exists, details will be overwritten
      args[ mainArg ].clear( );
      while ( i+1 < argc && argv[i+1][0] != '-' )
      {
        ++i;
        args[ mainArg ].push_back( argv[i] );
      }
    }
    else
    {
      std::cerr << "Skipping " << argv[i] << " argument" << std::endl;
    }
  }
}


int main( int argc, char** argv )
{
  auto& args = nslib::Config::inputArgs;
  parseArguments( argc, argv, args );
  std::cout << args.size( ) << std::endl;
  // for ( const auto& a : args )
  // {
  //   std::cout << a.first << ": ";
  //   for ( const auto &b : a.second )
  //     std::cout << b << ",";
  //   std::cout << std::endl;
  // }

//   bool swcInput = false, bcInput = false, xmlInput = false;
  bool fullscreen = false;
  bool initWindowSize = false;
  bool initWindowMaximized = false;
  int initWindowWidth;
  int initWindowHeight;

  std::string windowSizeArg;

  if ( args.count( "--window-size" ) == 1 )
    windowSizeArg = "--window-size";

  if ( args.count( "--ws" ) == 1 )
  {
    if ( !windowSizeArg.empty( ))
    {
      std::cerr << "--windows-size and -ws are non compatible" << std::endl;
      usageMessage( );
    }
    windowSizeArg = "-ws";
  }
  if ( !windowSizeArg.empty( ))
  {
    initWindowSize = true;
    if ( args[ windowSizeArg ].size( ) != 2 )
    {
      std::cerr << "--windows-size or -ws require two arguments" << std::endl;
      usageMessage( );
      exit( -1 );

    }
    initWindowWidth = atoi( args[ windowSizeArg ][0].c_str( ));
    initWindowHeight = atoi( args[ windowSizeArg ][1].c_str( ) );
  }

  if ( args.count( "--fullscreen" ) == 1 ||
       args.count( "-fs" ) == 1 )
  {
    fullscreen = true;
  }

//     }
//   for ( int i = 1; i < argc; i++ )
//   {
//     if ( strcmp( argv[i], "--version" ) == 0 )
//     {
//       dumpVersion( );
//       return 0;
//     }
//     if (strcmp(argv[i],"--help") == 0)
//     {
//       usageMessage( );
//       return 0;
//     }
//     if (strcmp(argv[i],"-bc") == 0)
//     {
//       neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_BLUECONFIG;
//       if( ++i < argc )
//         neuroscheme::Config::cliInputFile = std::string( argv[i] );
//       else
//         usageMessage( );

//       bcInput = true;
//     }
//     if ( strcmp( argv[i], "-swc" ) == 0 )
//     {
//       neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_SWCLIST;
//       neuroscheme::Config::cliInputFile = std::string(argv[++i]);
//       swcInput = true;
//     }
//     if ( strcmp( argv[i], "-xml" ) == 0 )
//     {
//       neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_XMLSCENE;
//       neuroscheme::Config::cliInputFile = std::string( argv[ ++i ] );
//       xmlInput = true;
//     }
//     if ( strcmp( argv[i], "-target" ) == 0 )
//     {
//       neuroscheme::Config::targetLabel = std::string( argv[++i] );
//     }
//     if ( strcmp( argv[i], "-cns" ) == 0 ||
//          strcmp( argv[i], "--csv-neuron-stats" ) == 0 )
//     {
//       if( ++i < argc )
//         neuroscheme::Config::csvNeuronStatsFileName = std::string( argv[i] );
//       else
//         usageMessage( );
//     }

//     if ( strcmp( argv[i], "--fullscreen" ) == 0 ||
//          strcmp( argv[i],"-fs") == 0 )
//     {
//       fullscreen = true;
//     }
//     if ( strcmp( argv[i], "--maximize-window" ) == 0 ||
//          strcmp( argv[i],"-mw") == 0 )
//     {
//       initWindowMaximized = true;
//     }
//     if ( strcmp( argv[i], "--window-size" ) == 0 ||
//          strcmp( argv[i],"-ws") == 0 )
//     {
//       initWindowSize = true;
//       if ( i + 2 >= argc )
//         usageMessage( );
//       initWindowWidth = atoi( argv[ ++i ] );
//       initWindowHeight = atoi( argv[ ++i ] );

//     }
//     if ( strcmp( argv[i], "--no-morphologies" ) == 0 ||
//          strcmp( argv[i],"-nm") == 0 )
//     {
//       neuroscheme::Config::loadMorphologies = false;
//     }


//     if (strcmp( argv[i],"-zeroeq") == 0 )
//     {
// #ifndef NEUROSCHEME_USE_ZEROEQ
//       std::cerr << "Error: ZeroEQ support not built-in" << std::endl;
//       return -1;
// #endif
//       if ( i+1 >= argc )
//         std::cout << "Using default ZeroEQ session" << std::endl;
//       else
//         neuroscheme::Config::zeroeqSession = std::string( argv[ ++i ] );
//     }

//     if ( strcmp( argv[i], "--deflect-host" ) == 0 ||
//          strcmp( argv[i],"-dh") == 0 )
//     {
// #ifndef NEUROSCHEME_USE_DEFLECT
//       std::cerr << "Error: Deflect support not built-in" << std::endl;
//       return -1;
// #endif
//       if ( i+1 >= argc )
//         usageMessage( );
//       neuroscheme::Config::deflectHost = std::string( argv[ ++i ] );
//     }

//     if ( strcmp( argv[i], "--deflect-exit-on-close" ) == 0 )
//     {
// #ifndef NEUROSCHEME_USE_DEFLECT
//       std::cerr << "Error: Deflect support not built-in" << std::endl;
//       return -1;
// #endif
//       neuroscheme::Config::deflectExitOnClose = true;
//     }

//     if ( strcmp( argv[i], "--no-auto-publish-selection" ) == 0 ||
//          strcmp( argv[i],"-naps") == 0 )
//     {
//       neuroscheme::Config::autoPublishSelection = false;
//     }


//   }

//   if ( atLeastTwo( swcInput, bcInput, xmlInput ))
//   {
//     std::cerr << "Error: -swc and -bc options are exclusive" << std::endl;
//     return -1;
//   }

  QApplication app( argc, argv );

  MainWindow mainWindow( 0 );
  mainWindow.setWindowTitle( "NeuroScheme" );
  mainWindow.show( );
  if ( initWindowSize )
    mainWindow.resize( initWindowWidth, initWindowHeight );
  // else
  //   mainWindow.resize( 800, 600 );
  if ( initWindowMaximized )
    mainWindow.showMaximized( );
  if ( fullscreen )
    mainWindow.showFullScreen( );

  if ( args.count( "-zeroeq" ) == 1 )
  {
    if ( args["-zeroeq"].size( ) == 1 )
      nslib::ZeroEQManager::init( args["-zeroeq"][0] );
    else
    {
      nslib::Log::log( "Error -zeroeq value empty.",
                       nslib::LOG_LEVEL_ERROR );
      usageMessage( );
    }
  }

  nslib::SelectableItem::init( );

  return app.exec( );

}

#ifdef Win32
#ifdef NEUROSCHEME_WITH_NO_CONSOLE

#include "windows.h"

  int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
    )
  {
    char* argv[1];
    argv[0] = _strdup( "NeuroScheme" );
    int argc = 1;
    return main( argc, argv );
  }

#endif
#endif
