//
// Created by iagocl on 25/01/18.
//

#ifndef NEUROSCHEME_LINECONNECTIONARROWITEM_H
#define NEUROSCHEME_LINECONNECTIONARROWITEM_H


#include "ConnectionArrowItem.h"
#include "LineConnectionArrowRep.h"

namespace nslib
{
  namespace congen
  {
    class LineConnectionArrowItem : public nslib::congen::ConnectionArrowItem
    {
      public:

      LineConnectionArrowItem( const LineConnectionArrowRep& connectionArrowRep );

      void setLine( const QLineF& line_ );

      void createArrow( const QPointF& origin, const QPointF& dest );
    };
  } // namespace congen
} // namespace nslib



#endif //NEUROSCHEME_LINECONNECTIONARROWITEM_H
