#ifndef __NEUROSCHEME__CANVAS__
#define __NEUROSCHEME__CANVAS__

#include "layouts/Layouts.h"
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
    void mousePressEvent( QMouseEvent* /* event */ );

  protected:
    virtual void wheelEvent( QWheelEvent* event_ );

  }; // class GraphicsView

  class GraphicsScene : public QGraphicsScene
  {
    // void mousePressEvent( QMouseEvent* /* event */ )
    // {
    //   PaneManager::activePane( dynamic_cast< Canvas* >( this->parentWidget( )));
    // }

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

    void enterEvent( QEvent* ) final;
    void leaveEvent( QEvent* ) final;
    void mousePressEvent( QMouseEvent* ) final;
    void keyPressEvent( QKeyEvent* ) final;
    void resizeEvent( QResizeEvent* ) final;
    void showEvent( QShowEvent* ) final;
    void connectLayoutSelector( void );
    const Layouts& layouts( void ) const;
    Layouts& layouts( void );
    void addLayout( Layout* );

    int activeLayoutIndex( void );
    void activeLayoutIndex( int );

    std::string name;

    void displayEntities( const shift::Entities& entities_, bool animate,
                          bool refreshProperties );
//    void displayReps( shift::Representations&, bool animate );
    Canvas* clone( void ) const;
    const shift::Representations& reps( void ) const;
    shift::Representations& reps( void );
    shift::Entities& entities( void ) { return _entities; }
protected:
    GraphicsView* _graphicsView;
    GraphicsScene* _graphicsScene;
    Layouts _layouts;
    int _activeLayoutIndex;
    shift::Representations _reps;
    shift::Entities _entities;
  public slots:
    void layoutChanged( int );
  }; // class Canvas



} // namespace neuroscheme

#endif // __NEUROSCHEME__CANVAS__
