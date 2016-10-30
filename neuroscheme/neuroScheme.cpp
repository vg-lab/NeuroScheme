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
#include "Config.h"
#include "MainWindow.h"
#include "ZeroEQManager.h"
#include <neuroscheme/version.h>


bool atLeastTwo( bool a, bool b, bool c )
{
  return a ^ b ? c : a;
}

void usageMessage( void )
{
  std::cerr << std::endl
            << "Usage: "
            << "neuroscheme" << std::endl
            << "\t[ -bc blue_config_path | -swc swc_file_list "
            << " | -xml scene_xml ] "
            << std::endl
            << "\t[ -target target_label ] "
            << std::endl
            << "\t[ --no-morphologies | -nm ] "
            << std::endl
#ifdef NEUROSCHEME_USE_ZEROEQ
            << "\t[ -zeroeq [ session ] ]"
            << std::endl
#endif
#ifdef NEUROSCHEME_USE_DEFLECT
            << "\t[ [ -dh | --deflect-host ] host ]"
            << std::endl
#endif
            << "\t[ -fs | --fullscreen ] "
            << std::endl
            << "\t[ -ws | --window-size ] width height ]"
            << std::endl
            << "\t[ -mw | --maximize-window ]"
            << std::endl
            << "\t[ -naps | --no-auto-publish-selection ]"
            << std::endl
            << "\t[ -cns | --csv-neuron-stats ] csv_file"
            << std::endl
            << "\t[ --version ]"
            << std::endl
            << "\t[ --help ]"
            << std::endl << std::endl;
  exit( -1 );
}

void dumpVersion( void )
{

  std::cerr << std::endl
            << "neuroscheme "
            << neuroscheme::Version::getMajor( ) << "."
            << neuroscheme::Version::getMinor( ) << "."
            << neuroscheme::Version::getPatch( )
            << " (" << neuroscheme::Version::getRevision( ) << ")"
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

int main( int argc, char** argv )
{

  bool swcInput = false, bcInput = false, xmlInput = false;
  bool fullscreen = false;
  bool initWindowSize = false;
  bool initWindowMaximized = false;
  int initWindowWidth;
  int initWindowHeight;

  for ( int i = 1; i < argc; i++ )
  {
    if ( strcmp( argv[i], "--version" ) == 0 )
    {
      dumpVersion( );
      return 0;
    }
    if (strcmp(argv[i],"--help") == 0)
    {
      usageMessage( );
      return 0;
    }
    if (strcmp(argv[i],"-bc") == 0)
    {
      neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_BLUECONFIG;
      if( ++i < argc )
        neuroscheme::Config::cliInputFile = std::string( argv[i] );
      else
        usageMessage( );

      bcInput = true;
    }
    if ( strcmp( argv[i], "-swc" ) == 0 )
    {
      neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_SWCLIST;
      neuroscheme::Config::cliInputFile = std::string(argv[++i]);
      swcInput = true;
    }
    if ( strcmp( argv[i], "-xml" ) == 0 )
    {
      neuroscheme::Config::cliDataSource = neuroscheme::Config::CLI_XMLSCENE;
      neuroscheme::Config::cliInputFile = std::string( argv[ ++i ] );
      xmlInput = true;
    }
    if ( strcmp( argv[i], "-target" ) == 0 )
    {
      neuroscheme::Config::targetLabel = std::string( argv[++i] );
    }
    if ( strcmp( argv[i], "-cns" ) == 0 ||
         strcmp( argv[i], "--csv-neuron-stats" ) == 0 )
    {
      if( ++i < argc )
        neuroscheme::Config::csvNeuronStatsFileName = std::string( argv[i] );
      else
        usageMessage( );
    }

    if ( strcmp( argv[i], "--fullscreen" ) == 0 ||
         strcmp( argv[i],"-fs") == 0 )
    {
      fullscreen = true;
    }
    if ( strcmp( argv[i], "--maximize-window" ) == 0 ||
         strcmp( argv[i],"-mw") == 0 )
    {
      initWindowMaximized = true;
    }
    if ( strcmp( argv[i], "--window-size" ) == 0 ||
         strcmp( argv[i],"-ws") == 0 )
    {
      initWindowSize = true;
      if ( i + 2 >= argc )
        usageMessage( );
      initWindowWidth = atoi( argv[ ++i ] );
      initWindowHeight = atoi( argv[ ++i ] );

    }
    if ( strcmp( argv[i], "--no-morphologies" ) == 0 ||
         strcmp( argv[i],"-nm") == 0 )
    {
      neuroscheme::Config::loadMorphologies = false;
    }


    if (strcmp( argv[i],"-zeroeq") == 0 )
    {
#ifndef NEUROSCHEME_USE_ZEROEQ
      std::cerr << "Error: ZeroEQ support not built-in" << std::endl;
      return -1;
#endif
      if ( i+1 >= argc )
        std::cout << "Using default ZeroEQ session" << std::endl;
      else
        neuroscheme::Config::zeroeqSession = std::string( argv[ ++i ] );
    }

    if ( strcmp( argv[i], "--deflect-host" ) == 0 ||
         strcmp( argv[i],"-dh") == 0 )
    {
#ifndef NEUROSCHEME_USE_DEFLECT
      std::cerr << "Error: Deflect support not built-in" << std::endl;
      return -1;
#endif
      if ( i+1 >= argc )
        usageMessage( );
      neuroscheme::Config::deflectHost = std::string( argv[ ++i ] );
    }

    if ( strcmp( argv[i], "--deflect-exit-on-close" ) == 0 )
    {
#ifndef NEUROSCHEME_USE_DEFLECT
      std::cerr << "Error: Deflect support not built-in" << std::endl;
      return -1;
#endif
      neuroscheme::Config::deflectExitOnClose = true;
    }

    if ( strcmp( argv[i], "--no-auto-publish-selection" ) == 0 ||
         strcmp( argv[i],"-naps") == 0 )
    {
      neuroscheme::Config::autoPublishSelection = false;
    }


  }

  if ( atLeastTwo( swcInput, bcInput, xmlInput ))
  {
    std::cerr << "Error: -swc and -bc options are exclusive" << std::endl;
    return -1;
  }

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

  if ( !neuroscheme::Config::zeroeqSession.empty( ))
    neuroscheme::ZeroEQManager::init( neuroscheme::Config::zeroeqSession );

//   std::cout << "init" << std::endl;
//  mainWindow.init( );
//   std::cout << "init" << std::endl;
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
  argv[0] = _strdup("NeuroScheme");
  int argc = 1;
  return main( argc, argv );
}

#endif
#endif
