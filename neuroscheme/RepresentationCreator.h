#ifndef __NEUROSCHEME__REPRESENTATION_CREATOR__
#define __NEUROSCHEME__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include "mappers/VariableMapper.h"
#include "mappers/ColorMapper.h"

namespace neuroscheme
{

  typedef std::unordered_map< shift::Object*, shift::Representation* >
  TObjectsToReps;
  typedef std::unordered_map< shift::Representation*, shift::Object* >
  TRepsToObjects;

  class RepresentationCreator //: shift::RepresentationCreator
  {
  public:
    virtual ~RepresentationCreator( void ) {};

    void create(
      const shift::Objects& objects,
      shift::Representations& representations,
      TObjectsToReps& objectsToReps,
      TRepsToObjects& repsToObjects,
      bool linkObjectsToReps = false,
      bool linkRepsToObjs = false );


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
                        TObjectsToReps& objectsToReps,
                        TRepsToObjects& repsToObjects,
                        bool linkObjectsToReps = false,
                        bool linkRepsToObjs = false,
                        unsigned int repCreatorId = 0 )
    {
      //TODO check if exists
      _repCreators[ repCreatorId ]->create( objects, representations,
                                            objectsToReps,
                                            repsToObjects,
                                            linkObjectsToReps,
                                            linkRepsToObjs );
    }

  protected:
    static std::map< unsigned int, RepresentationCreator* > _repCreators;
  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
