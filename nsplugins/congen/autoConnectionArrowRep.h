//
// Created by iagocl on 24/01/18.
//

#ifndef NEUROSCHEME_AUTOCONNECTIONARROWREP_H
#define NEUROSCHEME_AUTOCONNECTIONARROWREP_H

#include "ConnectionArrowRep.h"


namespace nslib
{
  namespace congen
  {
    class autoConnectionArrowRep : public nslib::congen::ConnectionArrowRep
    {
      public :
      autoConnectionArrowRep( shift::Representation* originRep_,
                              shift::Representation* destRep_ );

      autoConnectionArrowRep( const autoConnectionArrowRep& );
      autoConnectionArrowRep( shift::Representation* rep
                              );
    };
  }
}


#endif //NEUROSCHEME_AUTOCONNECTIONARROWREP_H
