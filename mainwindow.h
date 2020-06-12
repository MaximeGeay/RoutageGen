#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/*
class QFileDialog;
class meteoTable;
class QSettings;*/

namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void importMeteoTable(void);
    void getSelection(QString sSelection);
    void getPrevision(QVector<QString> vPrevisions);
    void genererRoutage();
    void textChanged(void);
    void selectionChanged(void);



private:
    Ui::MainWindow *ui;


QVector<QString> m_vPrevisions;




};

#endif // MAINWINDOW_H
