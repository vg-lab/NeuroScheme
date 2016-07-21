#ifndef __NEUROSCHEME__REPRESENTATION_CREATOR__
#define __NEUROSCHEME__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include "mappers/VariableMapper.h"
#include "mappers/ColorMapper.h"

namespace neuroscheme
{

  class RepresentationCreator : shift::RepresentationCreator
  {
  public:
    virtual ~RepresentationCreator( void ) {};

    void create( const shift::Objects& objects,
                 shift::Representations& representations );


  protected:
    void _CreateColumnOrMiniColumn(
      shift::Object *obj,
      shift::Representation* rep,
      MapperFloatToFloat& somaAreaToAngle,
      MapperFloatToFloat& dendAreaToAngle,
      ColorMapper& somaVolumeToColor,
      ColorMapper& dendVolumeToColor,
      MapperFloatToFloat& neuronsToPercentage );
  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
