#ifndef __NEUROSCHEME_LAYOUT_MANAGER__
#define __NEUROSCHEME_LAYOUT_MANAGER__

#include <QGraphicsScene>
#include <shift/shift.h>
#include "reps/SelectableItem.h"

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
      std::cout << "----> update items in scene " << _scene << std::endl;
      displayItems( _representations[ _scene ] );
    }

    static void updateAllScenes( void )
    {
      std::cout << "----> update items in scene " << _scene << std::endl;
      for ( const auto& scene : _scenes )
      {
        _scene = scene;
        displayItems( _representations[ _scene ] );
      }
    }

    static std::set< QGraphicsScene* >& scenes( void )
    {
      return _scenes;
    }

    static void updateSelection( void );

    static void updateAllScenesSelection( void )
    {
      std::cout << "----> update items in scene " << _scene << std::endl;
      for ( const auto& scene : _scenes )
      {
        _scene = scene;
        updateSelection( );
      }
    }

    static void displayItems( const shift::Representations& reps,
                              bool clearFirst = true );

  protected:
    static QGraphicsScene* _scene;
    static std::set< QGraphicsScene* > _scenes;
    static std::map< QGraphicsScene*, shift::Representations > _representations;

  };
} // namespace neuroscheme

#endif // __NEUROSCHEME_LAYOUT_MANAGER__
