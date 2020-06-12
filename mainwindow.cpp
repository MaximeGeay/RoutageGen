#include <QFileDialog>
#include <QSettings>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "meteotable.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("RoutageGen","RoutageGen_Config");
    ui->sp_Ratio->setValue(settings.value("RatioGrib",15).toInt());
    QObject::connect(ui->btn_SelMeteoTable,SIGNAL(clicked(bool)),this,SLOT(importMeteoTable()));
    QObject::connect(ui->btn_Generer,SIGNAL(clicked(bool)),this,SLOT(genererRoutage()));
    QObject::connect(ui->TextRoutage,SIGNAL(textChanged()),this,SLOT(textChanged()));
    QObject::connect(ui->TextRoutage,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
    ui->btn_Generer->setEnabled(false);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importMeteoTable()
{

    QSettings settings("RoutageGen","RoutageGen_Config");

    QString sChemin=settings.value("RepMeteoTable",QDir::homePath()).toString();

    sChemin=QFileDialog::getOpenFileName(this,QString::fromUtf8("Sélectionner le fichier mététable à importer"),sChemin,tr("Fichiers MeteoTable(*.slk)"));

    if(sChemin.isEmpty()==false)
    {

        settings.setValue("RepMeteoTable",sChemin);
        meteoTable *meteo=new meteoTable();
        QObject::connect(meteo,SIGNAL(sendSelection(QString)),this,SLOT(getSelection(QString)));
        QObject::connect(meteo,SIGNAL(sendPrevision(QVector<QString>)),this,SLOT(getPrevision(QVector<QString>)));

        meteo->init(sChemin);



    }

}

void MainWindow::getSelection(QString sSelection)
{
    int row=0;
    row=ui->tablePoints->rowCount();
    ui->tablePoints->insertRow(row);
    for(int column=0;column<ui->tablePoints->columnCount();column++)
    {
        QTableWidgetItem *NewItem= new QTableWidgetItem(sSelection.section(",",column,column));
        ui->tablePoints->setItem(row,column,NewItem);
    }

    if(ui->tablePoints->rowCount()>0)
        ui->btn_Generer->setEnabled(true);

    /**********************************/



}
void MainWindow::getPrevision(QVector<QString> vPrevisions)
{
    QVectorIterator<QString> it(vPrevisions);
    QString sLigne="";
    while (it.hasNext())
    {
        sLigne=QString::number(ui->tablePoints->rowCount())+","+it.next();

        m_vPrevisions.append(sLigne);

    }

}

void MainWindow::genererRoutage()
{
    QSettings settings("RoutageGen","RoutageGen_Config");
    QVectorIterator<QString> it(m_vPrevisions);
    QString sRoutage;
    QString sIndicePrevi="";
    QString sIndiceCourant="";
    QString sLigne="";
    QString sLat="";
    QString sLong="";
    QString sHeure="";
    QString sDate="";
    QString sDateHeure="";
    QString sDirection="";
    double dDirection=0;
    int nDirectionAr=0;
    QString sVitesse="";
    double dVitesse=0;
    int nRatioGrib=ui->sp_Ratio->value();
    settings.setValue("RatioGrib",nRatioGrib);



    while(it.hasNext())
    {
        sLigne=it.next();
        //sLigne :"1,42°45'00N,009°49'59W,06-04-2017,21:00 UTC,6.5 ,36,0"
        //sLigne : indice,Latitude,Longitude,Date,Heure,vitesse,Direction,Heure de réference
        sIndiceCourant=sLigne.section(",",0,0);
        if(sIndiceCourant!=sIndicePrevi)
        {
            sLat=sLigne.section(",",1,1);
            sLat=sLat.section("°",0,0)+sLat.section("°",1,1).left(2)+sLat.section("°",1,1).right(1);

            sLong=sLigne.section(",",2,2);
            sLong=sLong.section("°",0,0)+sLong.section("°",1,1).left(2)+sLong.section("°",1,1).right(1);


            sRoutage=sRoutage+sIndiceCourant+"- "+sLat+" "+sLong+"\r\n";


            sIndicePrevi=sIndiceCourant;
        }

        sDate=sLigne.section(",",3,3).section("-",0,0);
        sHeure=sLigne.section(",",4,4).section(":",0,0);
        sDateHeure=sDate+sHeure+"z";
        if(sLigne.section(",",-1,-1)=="1")
            sDateHeure=sDateHeure+"*";

        sDirection=sLigne.section(",",6,6);
        dDirection=sDirection.toDouble();
        dDirection=dDirection/10;
        nDirectionAr=qRound(dDirection)*10;
        sDirection=QString::number(nDirectionAr);

        sVitesse=sLigne.section(",",5,5);
        dVitesse=sVitesse.toDouble();
        dVitesse=dVitesse+dVitesse*nRatioGrib/100;

        //0/5 5/10 10/15 15/20 20/25 25/30 30/40 40/50 >60
        if(dVitesse>=0&&dVitesse<5)
            sVitesse="0/5";
        if(dVitesse>=5&&dVitesse<10)
            sVitesse="5/10";
        if(dVitesse>=10&&dVitesse<15)
            sVitesse="10/15";
        if(dVitesse>=15&&dVitesse<20)
            sVitesse="15/20";
        if(dVitesse>=20&&dVitesse<25)
            sVitesse="20/25";
        if(dVitesse>=25&&dVitesse<30)
            sVitesse="25/30";
        if(dVitesse>=30&&dVitesse<40)
            sVitesse="30/40";
        if(dVitesse>=40&&dVitesse<50)
            sVitesse="40/50";
        if(dVitesse>=60)
            sVitesse=">60";


        sRoutage=sRoutage+sDateHeure+"-"+" "+sVitesse+" "+sDirection+QChar(8233);

        ui->TextRoutage->setText(sRoutage);

    }
}
void MainWindow::textChanged()
{
    QString sRoutage=ui->TextRoutage->toPlainText();
    int nNbLF=sRoutage.count("\n");

    this->statusBar()->showMessage("Taille du routage : "+QString::number(sRoutage.size()+nNbLF)+" caractères");

}

void MainWindow::selectionChanged()
{
    QString sRoutage=ui->TextRoutage->toPlainText();
    QString sSelection;
    QTextCursor t=ui->TextRoutage->textCursor();
    int nNbLF=sRoutage.count("\n");
    if(t.hasSelection())
    {
        sSelection=t.selectedText();
        nNbLF=sSelection.count(QChar(8233));

        this->statusBar()->showMessage("Taille de la sélection : "+ QString::number(t.selectedText().size()+nNbLF)+" caractères");
    }
    else
    {
        this->statusBar()->showMessage("Taille du routage : "+QString::number(sRoutage.size()+nNbLF)+" caractères");
    }
}


