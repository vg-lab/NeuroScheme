#include "Canvas.h"
#include <QHBoxLayout>

namespace neuroscheme
{

  GraphicsView::GraphicsView( QWidget* parent_ )
    : QGraphicsView( parent_ )
  {
  }

  Canvas::Canvas( QWidget * parent_ )
    : _graphicsView( new GraphicsView( parent_ )),
      _graphicsScene( new GraphicsScene )
  {
    _graphicsView->setScene(_graphicsScene);
    _graphicsView->setInteractive( true );
    _graphicsView->setMouseTracking( true );
    _graphicsView->viewport( )->setMouseTracking( true );
    _graphicsView->setRenderHints( QPainter::Antialiasing |
                                   QPainter::HighQualityAntialiasing );
    _graphicsView->setDragMode( QGraphicsView::ScrollHandDrag );

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget( _graphicsView );
    this->setLayout( layout );

  }

  Canvas::~Canvas( void )
  {
    if ( _graphicsView ) delete _graphicsView;
    if ( _graphicsScene ) delete _graphicsScene;
  }

  const GraphicsScene& Canvas::scene( void ) const
  {
    return *_graphicsScene;
  }

  GraphicsScene& Canvas::scene( void )
  {
    return *_graphicsScene;
  }

} // namespace neuroscheme
