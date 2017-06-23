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

private slots:
    void setInteger();
    void setDouble();
    void setItem();
    void setText();
    void setColor();
    void setFont();
    void setExistingDirectory();
    void setOpenFileName();
    void setOpenFileNames();
    void setSaveFileName();
    void criticalMessage();
    void informationMessage();
    void questionMessage();
    void warningMessage();
    void errorMessage();

private:
    QCheckBox*  native;

    std::vector<QLabel*>          entityParamLabelCont;
    std::vector<QLineEdit*>       entityParamEditStringCont;
    std::vector<QDoubleSpinBox*>  entityParamEditDoubleCont;
    std::vector<QSpinBox*>        entityParamEditIntCont;
    std::vector<QComboBox*>       entityParamCategoricalCont;

    //Delete this
    QLabel *integerLabel;
    QLabel *doubleLabel;
    QLabel *itemLabel;
    QLabel *textLabel;
    QLabel *colorLabel;
    QLabel *fontLabel;
    QLabel *directoryLabel;
    QLabel *openFileNameLabel;
    QLabel *openFileNamesLabel;
    QLabel *saveFileNameLabel;
    QLabel *criticalLabel;
    QLabel *informationLabel;
    QLabel *questionLabel;
    QLabel *warningLabel;
    QLabel *errorLabel;
    QErrorMessage *errorMessageDialog;

    QString openFilesPath;
};

#endif /* CREATIONDIALOG_H_ */
