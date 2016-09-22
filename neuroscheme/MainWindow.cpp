#include "MainWindow.h"

// OJO PRUEBAS
#include "reps/QGraphicsItemRepresentation.h"
#include "objs/Neuron.h"
#include "RepresentationCreator.h"
#include "LayoutManager.h"


MainWindow::MainWindow( QWidget* parent_ )
  : QMainWindow( parent_ )
  , _ui( new Ui::MainWindow )
{

  _canvas = new neuroscheme::Canvas( this );
  this->setCentralWidget( _canvas );


  shift::Objects objects;
  shift::Representations representations;

  
  // for ( unsigned int i = 0; i < 5; i++ )
  // {
  //   objects.push_back( new neuroscheme::Neuron(
  //                        neuroscheme::Neuron::INTERNEURON,
  //                        neuroscheme::Neuron::EXCITATORY,
  //                        10.0f, 30.0f, 30.0f, 50.0f ));

  //   objects.push_back( new neuroscheme::Neuron(
  //                        neuroscheme::Neuron::PYRAMIDAL,
  //                        neuroscheme::Neuron::INHIBITORY,
  //                        70.0f, 60.0f, 20.0f, 30.0f ));
  // }

  objects.push_back( new neuroscheme::Column(
                       100,
                       75, 25,
                       5, 0,
                       10, 5,
                       15, 10,
                       20, 15,
                       25, 20,
                       30, 25,
                       10.0f, 30.0f,
                       30.0f, 50.0f ));

  objects.push_back( new neuroscheme::MiniColumn(
                       100,
                       75, 25,
                       5, 0,
                       10, 5,
                       15, 10,
                       20, 15,
                       25, 20,
                       30, 25,
                       10.0f, 30.0f,
                       30.0f, 50.0f ));

  neuroscheme::RepresentationCreatorManager::addCreator(
    new neuroscheme::RepresentationCreator );
  neuroscheme::TObjectsToReps objsToReps;
  neuroscheme::TRepsToObjects repsToObjs;
  neuroscheme::RepresentationCreatorManager::create( objects, representations,
                                                     objsToReps, repsToObjs );

  neuroscheme::LayoutManager::displayItems(
    _canvas->scene( ), representations );
  neuroscheme::LayoutManager::displayItems(
    _canvas->scene( ), representations, true );

  _representations = representations;

  // unsigned int x = 0;
  // for ( const auto representation : representations )
  // {
  //   auto graphicsItemRep =
  //     dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
  //     representation );
  //   if ( !graphicsItemRep )
  //   {
  //     std::cerr << "Item null" << std::endl;
  //   } else
  //   {

  //     _canvas->scene( ).addItem( graphicsItemRep->item( ));
  //     if ( graphicsItemRep->item( ))
  //     {
  //       graphicsItemRep->item( )->setPos( x, 0 ); x+= 120;
  //       // graphicsItemRep->item( )->setScale( 10 );
  //     }
  //   }
  // }
}

void MainWindow::resizeEvent( QResizeEvent* /* event_ */ )
{
  std::cout << "MainWindow::resizeEvent" << std::endl;
  const QSize viewerSize = _canvas->view( ).size( );
  const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                               - viewerSize.height( ) / 2,
                               viewerSize.width( ) -2,
                               viewerSize.height( ) -2);

  QTransform transf = _canvas->view( ).transform( );
  _canvas->view( ).fitInView( rectf, Qt::KeepAspectRatio );
  _canvas->view( ).setSceneRect( rectf );
  _canvas->view( ).setTransform( transf );

  std::cout << _canvas->view( ).width( ) << " x "
            << _canvas->view( ).height( ) << std::endl;

  neuroscheme::LayoutManager::displayItems(
    _canvas->scene( ), _representations );

}



MainWindow::~MainWindow( void )
{
  delete _ui;
  delete _canvas;
}
