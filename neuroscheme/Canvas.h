#ifndef __NEUROSCHEME__CANVAS__
#define __NEUROSCHEME__CANVAS__

#include "Layout.h"
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <iostream>

namespace neuroscheme
{

  class GraphicsView : public QGraphicsView
  {

    Q_OBJECT;

  public:
    GraphicsView( QWidget* parent = 0 );

  protected:
    virtual void wheelEvent( QWheelEvent* event_ );

  }; // class GraphicsView

  class GraphicsScene : public QGraphicsScene
  {
  }; // class GraphicsScene


  class Canvas : public QFrame
  {
    Q_OBJECT
  public:
    Canvas( QWidget* parent = 0 );
    ~Canvas( void );

    const GraphicsScene& scene( void ) const;
    GraphicsScene& scene( void );

    const GraphicsView& view( void ) const;
    GraphicsView& view( void );

    void enterEvent( QEvent* /* event */ );
    void leaveEvent( QEvent* /* event */ );

    const Layouts& layouts( void ) const;
    Layouts& layouts( void );

    std::string name;
protected:
    GraphicsView* _graphicsView;
    GraphicsScene* _graphicsScene;

    Layouts _layouts;

  public slots:
    void layoutChanged( void )
    {
      std::cout << "layout changed in " << this << std::endl;
    }
  }; // class Canvas



} // namespace neuroscheme

#endif // __NEUROSCHEME__CANVAS__
