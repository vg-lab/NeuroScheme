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
#include <QLabel>
#include <QErrorMessage>

class CreationDialog : public QWidget
{
    Q_OBJECT

public:
    CreationDialog (QWidget *parent = 0);

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
    QCheckBox *native;
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
