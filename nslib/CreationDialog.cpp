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
//#include "dialog.h"



#define MESSAGE \
    CreationDialog::tr("<p>Message boxes have a caption, a text, " \
               "and any number of buttons, each with standard or custom texts." \
               "<p>Click a button to close the message box. Pressing the Esc button " \
               "will activate the detected escape button (if any).")

//shift::Entity* entity,
CreationDialog::CreationDialog(shift::Entity* entity, QWidget *parent)
    : QWidget(parent)
{
    errorMessageDialog = new QErrorMessage(this);

    int frameStyle = QFrame::Sunken | QFrame::Panel;

    integerLabel = new QLabel;
    integerLabel->setFrameStyle(frameStyle);
    QPushButton *integerButton =
            new QPushButton(tr("QInputDialog::get&Int()"));

    doubleLabel = new QLabel;
    doubleLabel->setFrameStyle(frameStyle);
    QPushButton *doubleButton =
            new QPushButton( tr("QInputDialog::get&Double()"));

    itemLabel = new QLabel;
    itemLabel->setFrameStyle(frameStyle);
    QPushButton *itemButton = new QPushButton(tr("QInputDialog::getIte&m()"));

    textLabel = new QLabel;
    textLabel->setFrameStyle(frameStyle);
    QPushButton *textButton = new QPushButton(tr("QInputDialog::get&Text()"));

    colorLabel = new QLabel;
    colorLabel->setFrameStyle(frameStyle);
    QPushButton *colorButton = new QPushButton(tr("QColorDialog::get&Color()"));

    fontLabel = new QLabel;
    fontLabel->setFrameStyle(frameStyle);
    QPushButton *fontButton = new QPushButton(tr("QFontDialog::get&Font()"));

    directoryLabel = new QLabel;
    directoryLabel->setFrameStyle(frameStyle);
    QPushButton *directoryButton =
            new QPushButton(tr("QFileDialog::getE&xistingDirectory()"));

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNameButton =
            new QPushButton(tr("QFileDialog::get&OpenFileName()"));

    openFileNamesLabel = new QLabel;
    openFileNamesLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNamesButton =
            new QPushButton(tr("QFileDialog::&getOpenFileNames()"));

    saveFileNameLabel = new QLabel;
    saveFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *saveFileNameButton =
            new QPushButton(tr("QFileDialog::get&SaveFileName()"));

    criticalLabel = new QLabel;
    criticalLabel->setFrameStyle(frameStyle);
    QPushButton *criticalButton =
            new QPushButton(tr("QMessageBox::critica&l()"));

    informationLabel = new QLabel;
    informationLabel->setFrameStyle(frameStyle);
    QPushButton *informationButton =
            new QPushButton(tr("QMessageBox::i&nformation()"));

    questionLabel = new QLabel;
    questionLabel->setFrameStyle(frameStyle);
    QPushButton *questionButton =
            new QPushButton(tr("QMessageBox::&question()"));

    warningLabel = new QLabel;
    warningLabel->setFrameStyle(frameStyle);
    QPushButton *warningButton = new QPushButton(tr("QMessageBox::&warning()"));

    errorLabel = new QLabel;
    errorLabel->setFrameStyle(frameStyle);
    QPushButton *errorButton =
            new QPushButton(tr("QErrorMessage::showM&essage()"));

    connect(integerButton, SIGNAL(clicked()), this, SLOT(setInteger()));
    connect(doubleButton, SIGNAL(clicked()), this, SLOT(setDouble()));
    connect(itemButton, SIGNAL(clicked()), this, SLOT(setItem()));
    connect(textButton, SIGNAL(clicked()), this, SLOT(setText()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
    connect(fontButton, SIGNAL(clicked()), this, SLOT(setFont()));
    connect(directoryButton, SIGNAL(clicked()),
            this, SLOT(setExistingDirectory()));
    connect(openFileNameButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));
    connect(openFileNamesButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileNames()));
    connect(saveFileNameButton, SIGNAL(clicked()),
            this, SLOT(setSaveFileName()));
    connect(criticalButton, SIGNAL(clicked()), this, SLOT(criticalMessage()));
    connect(informationButton, SIGNAL(clicked()),
            this, SLOT(informationMessage()));
    connect(questionButton, SIGNAL(clicked()), this, SLOT(questionMessage()));
    connect(warningButton, SIGNAL(clicked()), this, SLOT(warningMessage()));
    connect(errorButton, SIGNAL(clicked()), this, SLOT(errorMessage()));

    native = new QCheckBox(this);
    native->setText("Use native file dialog.");
    native->setChecked(true);

    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(1, 250);

    /*
    populationType = new QLabel;
    populationType->setFrameStyle(frameStyle);
    populationType->setText("Population type");

    selPopulationType=new QComboBox();
    selPopulationType->addItem("Excitatory");
    selPopulationType->addItem("Inhibitory");
    selPopulationType->addItem("Both");

    //layout->addWidget(populationType, 0, 0);
    //layout->addWidget(selPopulationType, 0, 1);
  */

    unsigned int element=0;
    unsigned int pos    =0;

    //Edit mode
    if (entity!=nullptr)
    {
      for ( const auto& propPair : entity->properties( ))
      {
        const auto prop = propPair.first;
        auto caster     = fires::PropertyManager::getPropertyCaster( prop );
        if ( caster )
        {
          //std::cout << "\t" << fires::PropertyGIDsManager::getPropertyLabel( prop ) << " ";
          // Esta linea da error envezencuando
          // std::cout << caster->toString( propPair.second ) ;

          //fires::ScalarPropertyCaster< int > spci;
          //std::string str = spci.toString(( obj.getProperty( "p2" )));

          entityParamLabelCont.push_back( new QLabel(
              QString::fromStdString(fires::PropertyGIDsManager::getPropertyLabel( prop ) ) ) );

          layout->addWidget( entityParamLabelCont.back(), element, pos%2 );
          ++element;
          ++pos;

          const auto& categories = caster->categories( );

          //If is there any category, fill the combo
          if ( categories.size( ) > 0 )
          {
            entityParamCategoricalCont.push_back( new QComboBox );
            for ( const auto value : categories )
            {
              entityParamCategoricalCont.back( )->addItem( QString::fromStdString( value ) );
            }
            layout->addWidget( entityParamCategoricalCont.back(), element, pos%2 );
            ++element;
            ++pos;
          }
          //Simple QEdit
          else
          {
            entityParamEditStringCont.push_back(new QLineEdit);
            layout->addWidget( entityParamEditStringCont.back(), element, pos%2 );
            ++element;
            ++pos;
          }
        }
      }
    }
    //Creation mode
    else
    {
      entityParamLabelCont.push_back( new QLabel(
                    QString::fromStdString( "Population type" ) ) );
      //*entityParamLabelCont.end()->setFrameStyle(frameStyle);

      entityParamCategoricalCont.push_back( new QComboBox );
      entityParamCategoricalCont.back( )->addItem( "Excitatory" );
      entityParamCategoricalCont.back( )->addItem( "Inhibitory" );
      entityParamCategoricalCont.back( )->addItem( "Both" );
    }


    layout->addWidget(doubleButton, 1, 0);
    layout->addWidget(doubleLabel, 1, 1);
    layout->addWidget(itemButton, 2, 0);
    layout->addWidget(itemLabel, 2, 1);
    layout->addWidget(textButton, 3, 0);
    layout->addWidget(textLabel, 3, 1);
    layout->addWidget(colorButton, 4, 0);
    layout->addWidget(colorLabel, 4, 1);
    layout->addWidget(fontButton, 5, 0);
    layout->addWidget(fontLabel, 5, 1);
    layout->addWidget(directoryButton, 6, 0);
    layout->addWidget(directoryLabel, 6, 1);
    layout->addWidget(openFileNameButton, 7, 0);
    layout->addWidget(openFileNameLabel, 7, 1);
    layout->addWidget(openFileNamesButton, 8, 0);
    layout->addWidget(openFileNamesLabel, 8, 1);
    layout->addWidget(saveFileNameButton, 9, 0);
    layout->addWidget(saveFileNameLabel, 9, 1);
    layout->addWidget(criticalButton, 10, 0);
    layout->addWidget(criticalLabel, 10, 1);
    layout->addWidget(informationButton, 11, 0);
    layout->addWidget(informationLabel, 11, 1);
    layout->addWidget(questionButton, 12, 0);
    layout->addWidget(questionLabel, 12, 1);
    layout->addWidget(warningButton, 13, 0);
    layout->addWidget(warningLabel, 13, 1);
    layout->addWidget(errorButton, 14, 0);
    layout->addWidget(errorLabel, 14, 1);
    layout->addWidget(native, 15, 0);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(widget);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
#else
    setLayout(layout);
#endif

    setWindowTitle(tr("Standard Dialogs"));
}

void CreationDialog::setInteger()
{
    bool ok;
    int i = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                 tr("Percentage:"), 25, 0, 100, 1, &ok);
    if (ok)
        integerLabel->setText(tr("%1%").arg(i));
}

void CreationDialog::setDouble()
{
    bool ok;
    double d = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("Amount:"), 37.56, -10000, 10000, 2, &ok);
    if (ok)
        doubleLabel->setText(QString("$%1").arg(d));
}

void CreationDialog::setItem()
{
    QStringList items;
    items << tr("Spring") << tr("Summer") << tr("Fall") << tr("Winter");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                         tr("Season:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        itemLabel->setText(item);
}

void CreationDialog::setText()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        textLabel->setText(text);
}


void CreationDialog::setColor()
{
    QColor color;
    if (native->isChecked())
        color = QColorDialog::getColor(Qt::green, this);
    else
        color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

    if (color.isValid()) {
        colorLabel->setText(color.name());
        colorLabel->setPalette(QPalette(color));
        colorLabel->setAutoFillBackground(true);
    }
}

void CreationDialog::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(fontLabel->text()), this);
    if (ok) {
        fontLabel->setText(font.key());
        fontLabel->setFont(font);
    }
}

void CreationDialog::setExistingDirectory()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    if (!native->isChecked())
        options |= QFileDialog::DontUseNativeDialog;
    QString directory = QFileDialog::getExistingDirectory(this,
                                tr("QFileDialog::getExistingDirectory()"),
                                directoryLabel->text(),
                                options);
    if (!directory.isEmpty())
        directoryLabel->setText(directory);
}

void CreationDialog::setOpenFileName()
{
    QFileDialog::Options options;
    if (!native->isChecked())
        options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                openFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        openFileNameLabel->setText(fileName);
}

void CreationDialog::setOpenFileNames()
{
    QFileDialog::Options options;
    if (!native->isChecked())
        options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QStringList files = QFileDialog::getOpenFileNames(
                                this, tr("QFileDialog::getOpenFileNames()"),
                                openFilesPath,
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (files.count()) {
        openFilesPath = files[0];
        openFileNamesLabel->setText(QString("[%1]").arg(files.join(", ")));
    }
}

void CreationDialog::setSaveFileName()
{
    QFileDialog::Options options;
    if (!native->isChecked())
        options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("QFileDialog::getSaveFileName()"),
                                saveFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        saveFileNameLabel->setText(fileName);
}

void CreationDialog::criticalMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
                                    MESSAGE,
                                    QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
    if (reply == QMessageBox::Abort)
        criticalLabel->setText(tr("Abort"));
    else if (reply == QMessageBox::Retry)
        criticalLabel->setText(tr("Retry"));
    else
        criticalLabel->setText(tr("Ignore"));
}

void CreationDialog::informationMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("QMessageBox::information()"), MESSAGE);
    if (reply == QMessageBox::Ok)
        informationLabel->setText(tr("OK"));
    else
        informationLabel->setText(tr("Escape"));
}

void CreationDialog::questionMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("QMessageBox::question()"),
                                    MESSAGE,
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
        questionLabel->setText(tr("Yes"));
    else if (reply == QMessageBox::No)
        questionLabel->setText(tr("No"));
    else
        questionLabel->setText(tr("Cancel"));
}

void CreationDialog::warningMessage()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("QMessageBox::warning()"),
                       MESSAGE, 0, this);
    msgBox.addButton(tr("Save &Again"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("&Continue"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        warningLabel->setText(tr("Save Again"));
    else
        warningLabel->setText(tr("Continue"));

}

void CreationDialog::errorMessage()
{
    errorMessageDialog->showMessage(
            tr("This dialog shows and remembers error messages. "
               "If the checkbox is checked (as it is by default), "
               "the shown message will be shown again, "
               "but if the user unchecks the box the message "
               "will not appear again if QErrorMessage::showMessage() "
               "is called with the same message."));
    errorLabel->setText(tr("If the box is unchecked, the message "
                           "won't appear again."));
}
