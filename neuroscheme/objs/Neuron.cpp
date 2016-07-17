#include "Neuron.h"

namespace neuroscheme
{

  Neuron::Neuron( TMorphologicalType morphoType,
                  TFunctionalType funcType,
                  float somaVolume,
                  float somaArea,
                  float dendVolume,
                  float dendArea )
  {
    this->registerProperty( "morphoType", morphoType );
    this->registerProperty( "funcType", funcType );
    this->registerProperty( "somaVolume", somaVolume );
    this->registerProperty( "somaArea", somaArea );
    this->registerProperty( "dendVolume", dendVolume );
    this->registerProperty( "dendArea", dendArea );
  }

  Neuron::Neuron( const Neuron& other )
  {
    this->registerProperty( "morphoType",
      other.getProperty( "morphoType" ).value< TMorphologicalType >( ));
    this->registerProperty( "funcType",
      other.getProperty( "funcType" ).value< TFunctionalType >( ));
    this->registerProperty( "somaVolume",
      other.getProperty( "somaVolume" ).value< float >( ));
    this->registerProperty( "somaArea",
      other.getProperty( "somaArea" ).value< float >( ));
    this->registerProperty( "somaVolume",
      other.getProperty( "dendVolume" ).value< float >( ));
    this->registerProperty( "somaArea",
      other.getProperty( "dendArea" ).value< float >( ));
  }

} // namespace neuroscheme
