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
#include <nslib/Loggers.h>
#include <nslib/Config.h>
#include <nslib/ZeroEQManager.h>
#include <nslib/reps/SelectableItem.h>
#include <nslib/version.h>
#include <nsplugins/cortex/Domain.h>
#include <nsplugins/congen/Domain.h>
#include "MainWindow.h"


void usageMessage( const std::string& errorMsg = "" )
{
  if ( !errorMsg.empty( ))
    std::cerr << std::endl << "Error: " << errorMsg
              << std::endl << std::endl;

  std::cerr << std::endl;
  std::cerr << "Usage: "
            << "NeuroScheme" << std::endl;

  std::cerr << "\t[ -d | --domain ] domainName "
            << std::endl
            << "\t[ --version ]"
            << std::endl
            << "\t[ --help ]"
            << std::endl
            << "\t[ [ --scale | -sc ] scaleFactor = 1.0f ]"
            << "\t[ [ --log-file | -l ] log_file_name ]"
            << "\t[ [ --not-colored-log | -ncl ]";
  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "Domains and their options:" << std::endl;
  std::cout << "\t - cortex:" << std::endl;
  nslib::cortex::Domain::usageMessage( );
  std::cout << std::endl;
  std::cout << "\t - congen:" << std::endl;
  nslib::congen::Domain::usageMessage( );

  std::cout << std::endl;
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

  std::cerr << "Nsol support built-in: ";
#ifdef NEUROSCHEME_USE_NSOL
  std::cerr << "yes";
#else
  std::cerr << "no";
#endif
  std::cerr << std::endl;

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

  exit( 0 );

}


std::string checkArg( const std::vector< std::string >& args,
                      unsigned int nbArgParams )
{

  auto& allArgs = nslib::Config::inputArgs( );
  unsigned int numArgs;
  auto foundArg = allArgs.checkIfOnlyOne( args, numArgs );
  if ( numArgs > 1 )
  {
    std::string errorMsg;
    for( const auto& arg : args )
      errorMsg += arg + std::string( " " );
    usageMessage( "Incompatible arguments: " + errorMsg );
  }
  if ( !foundArg.empty( ))
  {
    if ( allArgs[ foundArg ].size( ) != nbArgParams )
      usageMessage( foundArg + " specified with " +
                    std::to_string( allArgs[ foundArg ].size( )) + " value/s, " +
                    std::to_string( nbArgParams ) + " expected" );
    else
      return foundArg;
  }

  return std::string( );
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

  std::string foundArg;
  auto& args = nslib::Config::inputArgs( );
  parseArguments( argc, argv, args );

  std::ostream* logStream = &std::cerr;
  std::ofstream logFileStream;
  foundArg = checkArg( { "--log-file", "-l" }, 1 );
  if ( !foundArg.empty( ))
  {
    logFileStream.open( args[ foundArg ][ 0 ],
                        std::ofstream::out | std::ofstream::app );
    logStream = &logFileStream;
  }

  bool coloredOutput = true;
  foundArg = checkArg( { "--not-colored-log", "-ncl" }, 0 );
  if ( !foundArg.empty( ))
    coloredOutput = false;

  nslib::Loggers::add(
    new nslib::Logger( "nslib", *logStream,
#ifdef DEBUG
                    nslib::LOG_LEVEL_WARNING
#else
                    nslib::LOG_LEVEL_ERROR
#endif
                    ,
                    coloredOutput ));
  nslib::Loggers::get( )->setCurrentThreadName( "main thread" );


  if ( args.count( "--help" ) == 1 )
    usageMessage( );

  if ( args.count( "--version" ) == 1 )
    dumpVersion( );

  bool fullscreen = false;
  bool initWindowSize = false;
  bool initWindowMaximized = false;
  int initWindowWidth = 0;
  int initWindowHeight = 0;

  foundArg = checkArg( { "--window-size", "-ws" }, 2 );
  if ( !foundArg.empty( ))
  {
    initWindowSize = true;
    initWindowWidth = std::stoi( args[ foundArg ][0] );
    initWindowHeight = std::stoi( args[ foundArg ][1] );
  }

  foundArg = checkArg( { "--fullscreen", "-fs" }, 0 );
  if ( !foundArg.empty( ))
    fullscreen = true;

  foundArg = checkArg( { "--scale", "-sc" }, 1 );
  if ( !foundArg.empty( ))
    nslib::Config::scale( std::stof( args[ foundArg ][ 0 ] ));



  QApplication app( argc, argv );
  //QGuiApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

  MainWindow mainWindow( 0 );
  mainWindow.setWindowTitle( "NeuroScheme" );
  mainWindow.show( );
  mainWindow.selectDomain( );

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
      nslib::Loggers::get( )->log( "Error -zeroeq value empty.",
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
