#ifndef __NEUROSCHEME__CANVAS__
#define __NEUROSCHEME__CANVAS__

#include <QWidget>
#include <QGraphicsView>

namespace neuroscheme
{

  class GraphicsView : public QGraphicsView
  {

    Q_OBJECT
  public:

    GraphicsView( QWidget* parent = 0 );

  }; // class GraphicsView

  class GraphicsScene : public QGraphicsScene
  {
  }; // class GraphicsScene


  class Canvas : public QWidget
  {
  public:
    Canvas( QWidget* parent = 0 );
    ~Canvas( void );

    const GraphicsScene& scene( void ) const;
    GraphicsScene& scene( void );

  protected:
    GraphicsView* _graphicsView;
    GraphicsScene* _graphicsScene;

  }; // class Canvas



} // namespace neuroscheme

#endif // __NEUROSCHEME__CANVAS__
