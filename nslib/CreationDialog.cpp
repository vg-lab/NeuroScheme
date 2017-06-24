/*
 * creationDialog.cpp
 *
 *  Created on: 22/6/2017
 *      Author: juanpebm
 */

#include "CreationDialog.h"

#include <QtGui>
#include <QPushButton>
#include <QGridLayout>
#include <QInputDialog>

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>

CreationDialog::CreationDialog(shift::Entity* entity, QWidget *parent)
    : QWidget(parent)
{
    QGridLayout* layout = new QGridLayout;
    layout->setColumnStretch( 1, 1 );
    layout->setColumnMinimumWidth( 1, 150 );

    unsigned int element=0;
    unsigned int pos    =0;

    if ( entity!=nullptr )
    {
      for ( const auto& propPair : entity->properties( ) )
      {
        const auto prop = propPair.first;
        auto caster     = fires::PropertyManager::getPropertyCaster( prop );
        if ( caster )
        {
          // Esta linea da error envezencuando
          // std::cout << caster->toString( propPair.second ) ;

          //fires::ScalarPropertyCaster< int > spci;
          //std::string str = spci.toString(( obj.getProperty( "p2" )));

          entityParamLabelCont.push_back( new QLabel(
              QString::fromStdString(
                  fires::PropertyGIDsManager::getPropertyLabel( prop ) ) ) );

          layout->addWidget( entityParamLabelCont.back(), element, pos%2 );
          ++pos;

          const auto& categories = caster->categories( );

          if ( categories.size( ) > 0 )
          {
            entityParamCategoricalCont.push_back( new QComboBox );
            for ( const auto value : categories )
            {
              entityParamCategoricalCont.back( )->addItem(
                  QString::fromStdString( value ) );
            }
            layout->addWidget( entityParamCategoricalCont.back( ), element, pos%2 );
            ++pos;
          }
          else
          {
            //TODO: Recover the values from the entities
            std::string valueType =
                //caster-> toString
                (
                "42.42"
                //int( 42 )
                //propPair.second
                //propPair.first
                //entity->getProperty ( prop )
                //entity->getProperty( fires::PropertyGIDsManager::getPropertyLabel( prop ). )//.value< int >( )
                );

            entityParamEditStringCont.push_back( new QLineEdit );
            entityParamEditStringCont.back()->setText(
                QString::fromStdString( valueType ) );
            layout->addWidget( entityParamEditStringCont.back(), element, pos%2 );
            ++pos;
          }
          ++element;
        }
      }
    }
    else
    {
      //Only for testing purposes
      entityParamLabelCont.push_back( new QLabel(
                    QString::fromStdString( "<--..:)..--->" ) ) );
    }

    QPushButton* validationButton = new QPushButton(tr( "Validate" ) );
    layout->addWidget( validationButton, element, 1 );

    connect(validationButton, SIGNAL( clicked( ) ), this, SLOT( validateDialog( ) ) );

    setLayout( layout );
    setWindowTitle( tr( "Entity management dialog" ) );
}

//void CreationDialog::validateDialog()
//{
//  //do something here;
//}
