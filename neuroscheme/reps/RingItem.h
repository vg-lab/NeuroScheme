#ifndef __NEUROSCHEME__RING_ITEM__
#define __NEUROSCHEME__RING_ITEM__

#include "../Color.h"
#include <QGraphicsPathItem>


namespace neuroscheme
{


    class RingItem : public QGraphicsPathItem
    {

    public:

      RingItem( unsigned int initAngle, unsigned int xRadius,
                unsigned int yRadius, unsigned int width,
                int angle, Color color );

    protected:


    }; // class RingItem

} // namespace neuroscheme


#endif // __NEUROSCHEME__RING_ITEM__
