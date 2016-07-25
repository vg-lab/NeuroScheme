#ifndef __NEUROSCHEME_LAYOUT_MANAGER__
#define __NEUROSCHEME_LAYOUT_MANAGER__

#include <QGraphicsScene>
#include <shift/shift.h>

namespace neuroscheme
{
  class LayoutManager
  {
  public:

    static void displayItems( QGraphicsScene& scene,
                              const shift::Representations& reps,
                              bool clearFirst = true );

  };
} // namespace neuroscheme

#endif // __NEUROSCHEME_LAYOUT_MANAGER__
