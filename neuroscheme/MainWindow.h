#ifndef __NEUROSCHEME__MAINWINDOW__
#define __NEUROSCHEME__MAINWINDOW__

#include <ui_MainWindow.h>
#include <QMainWindow>
#include "Canvas.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT;

public:

  explicit MainWindow( QWidget *parent = 0 );
  ~MainWindow( void );


private:

  neuroscheme::Canvas* _canvas;
  Ui::MainWindow* _ui;

}; // class MainWindow

#endif // __NEUROSCHEME__MAINWINDOW__
