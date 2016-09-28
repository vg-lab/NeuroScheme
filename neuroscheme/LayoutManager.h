#ifndef __NEUROSCHEME_LAYOUT_MANAGER__
#define __NEUROSCHEME_LAYOUT_MANAGER__

#include <QGraphicsScene>
#include <shift/shift.h>

namespace neuroscheme
{
  class LayoutManager
  {
  public:

    static void setScene( QGraphicsScene* scene )
    {
      _scene = scene;
    }

    static void update( void )
    {
      std::cout << "items" << std::endl;
      displayItems( _representations );
    }

    static void displayItems( const shift::Representations& reps,
                              bool clearFirst = true );

    protected:
      static QGraphicsScene* _scene;
      static shift::Representations _representations;

  };
} // namespace neuroscheme

#endif // __NEUROSCHEME_LAYOUT_MANAGER__
