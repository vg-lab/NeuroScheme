//
// Created by iagocl on 24/01/18.
//

#include "autoConnectionArrowRep.h"
#include "ConnectionArrowItem.h"
#include "../../ShiFT/shift/Representation.h"
#include <iostream>


namespace nslib
{
  namespace congen
  {

    autoConnectionArrowRep::autoConnectionArrowRep(
        shift::Representation* rep_)
        : ConnectionArrowRep( rep_,
                              rep_ )
    {
      //IAGODEBUG
      std::cout << "creada autoconexion" << std::endl;

    }
  }
}