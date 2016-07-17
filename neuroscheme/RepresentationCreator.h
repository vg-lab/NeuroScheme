#ifndef __NEUROSCHEME__REPRESENTATION_CREATOR__
#define __NEUROSCHEME__REPRESENTATION_CREATOR__
#include <shift/shift.h>


namespace neuroscheme
{

  class RepresentationCreator : shift::RepresentationCreator
  {
  public:
    virtual ~RepresentationCreator( void ) {};

    void create( const shift::Objects& objects,
                 shift::Representations& representations );

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
