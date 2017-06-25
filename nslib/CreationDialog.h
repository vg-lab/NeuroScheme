/*
 * creationDialog.h
 *
 *  Created on: 22/6/2017
 *      Author: juanpebm
 */

#ifndef CREATIONDIALOG_H_
#define CREATIONDIALOG_H_

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QErrorMessage>

#include <shift/shift.h>


class CreationDialog : public QWidget
{
    Q_OBJECT

public:
    CreationDialog (shift::Entity* entity, QWidget *parent = 0);

public slots:
    //Connect this signal with the slot to create/modify entities (must be in the creation/modifying point)
    void validateDialog();
    void cancelDialog();

private:
    std::vector<QLabel*>          entityParamLabelCont;
    std::vector<QLineEdit*>       entityParamEditStringCont;
    std::vector<QDoubleSpinBox*>  entityParamEditDoubleCont;
    std::vector<QSpinBox*>        entityParamEditIntCont;
    std::vector<QComboBox*>       entityParamCategoricalCont;
};

#endif /* CREATIONDIALOG_H_ */
