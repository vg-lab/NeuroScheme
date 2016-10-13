#include <QApplication>
#include "MainWindow.h"
#include "ZeroEQManager.h"

int main( int argc, char** argv )
{

  QApplication app( argc, argv );

  MainWindow mainWindow( 0 );
  mainWindow.setWindowTitle( "NeuroScheme" );
  mainWindow.show( );

  neuroscheme::ZeroEQManager::init( "hbp" );

  return app.exec( );

}
