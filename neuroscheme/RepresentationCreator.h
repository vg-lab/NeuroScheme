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

  class RepresentationCreatorManager
  {
  public:
    static void addCreator( RepresentationCreator* repCreator,
                            unsigned int repCreatorId = 0 )
    {
      //TODO check if exists
      _repCreators[ repCreatorId ] = repCreator;
    }
    static void create( const shift::Objects& objects,
                        shift::Representations& representations,
                        unsigned int repCreatorId = 0 )
    {
      //TODO check if exists
      _repCreators[ repCreatorId ]->create( objects, representations );
    }

  protected:
    static std::map< unsigned int, RepresentationCreator* > _repCreators;
  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
