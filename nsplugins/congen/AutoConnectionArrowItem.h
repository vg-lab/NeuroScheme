//
// Created by iagocl on 25/01/18.
//

#ifndef NEUROSCHEME_AUTOCONNECTIONAROWITEM_H
#define NEUROSCHEME_AUTOCONNECTIONAROWITEM_H

#include "AutoConnectionArrowRep.h"
#include "ConnectionArrowItem.h"

namespace nslib
{
  namespace congen
  {
    class AutoConnectionArrowItem : public nslib::congen::ConnectionArrowItem
    {
      public:
      AutoConnectionArrowItem( const AutoConnectionArrowRep& connectionArrowRep );
      void setLine( const QLineF& line_ );

      void createArrow( const QPointF& origin, const QPointF& dest );
    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONAROWITEM_H


