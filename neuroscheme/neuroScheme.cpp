#include <QApplication>
#include "MainWindow.h"

int main( int argc, char** argv )
{

  QApplication app( argc, argv );

  MainWindow mainWindow( 0 );
  mainWindow.setWindowTitle( "NeuroScheme" );
  mainWindow.show( );

  return app.exec( );

}
