#include <QFile>
#include <QSettings>
#include <QDir>
#include <QIODevice>
#include <QDebug>
#include <QVector>

#include "meteotable.h"
#include "ui_meteotable.h"

meteoTable::meteoTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::meteoTable)
{
    ui->setupUi(this);

    QSettings settings("RoutageGen","RoutageGen_Config");
    ui->sp_Avant->setValue(settings.value("NbPrevisionsAvant","2").toInt());
    ui->sp_Apres->setValue(settings.value("NbPrevisionsApres","1").toInt());
    m_sLatitude="";
    m_sLongitude="";
    m_sIndVitesse="";
    m_sIndDirection="";
    QObject::connect(this->ui->btn_Valider,SIGNAL(clicked(bool)),this,SLOT(selectionValidee()));
    QObject::connect(this->ui->tableMeteo,SIGNAL(itemSelectionChanged()),this,SLOT(ligneSelectionnee()));
    ui->btn_Valider->setEnabled(false);
}

meteoTable::~meteoTable()
{
    delete ui;
}

void meteoTable::init(QString sChemin)
{
    QString sLigne="";
    QString sX="";
    QString sY="";
    QString sData="";
    QString sDate="";
    QString sHeure="";
    QString sVitesse="";
    QString sDirection="";



    if(sChemin.isEmpty()==false)
    {
        QFile fichierSLK(sChemin);
/********Lecture Fichier et classement des données*******************************************/
        if(fichierSLK.open(QIODevice::ReadOnly| QIODevice::Text))
        {
            QTextStream flux(&fichierSLK);

            while(!flux.atEnd())
            {
                sLigne=flux.readLine();

                sY=sLigne.section(";",1,1);
                sX=sLigne.section(";",2,2);
                sData=sLigne.section(";",3,3);

                if(sY=="Y2"&&sX=="X1")
                {
                    m_sLatitude=sData.section("\"",1,1).section(" ",1,1)+sData.section("\"",2,2).left(1);
                    m_sLongitude=sData.section("\"",2,2).section(" ",2,2)+sData.section("\"",3,3);

                    this->setWindowTitle(sChemin.section("/",-1,-1));
                    this->ui->label_EnTete->setText(tr("Sélectionnez l'heure de passage au point : ")+m_sLatitude+" "+m_sLongitude);
                    this->ui->l_Fichier->setText("Fichier: "+sChemin.section("/",-1,-1));

                }
                else
                {
                    if(sY=="Y2"&&sX!="X2")
                    {
                        sDate=sData.section("\"",1,1);
                        m_vDate.append(sX+","+sDate);
                    }
                    else
                    {
                        if(sY=="Y3")
                        {    sHeure=sData.section("\"",1,1);
                            m_vHeure.append(sX+","+sHeure);
                        }
                        else
                        {
                            if(sData=="K\"Vent (vitesse)\"")
                            {
                                m_sIndVitesse=sY;

                            }
                            else
                            {
                                if(sY==m_sIndVitesse&&sX!="X2")
                                {
                                    sVitesse=sData.section("\"",1,1);
                                    m_vWindSpeed.append(sX+","+sVitesse);

                                }
                                else
                                {
                                    if(sData=="K\"Vent (direction)\"")
                                    {
                                        m_sIndDirection=sY;
                                    }
                                    else
                                    {
                                        if(sY==m_sIndDirection&&sX!="X2"&&sX!="X3")
                                        {
                                            sDirection=sData.section("\"",1,1);
                                            m_vWindDirection.append(sX+","+sDirection);
                                        }
                                    }
                                }
                            }

                        }
                    }


                }


            }

            fichierSLK.close();
        }

/*******************************************************************************************************/

      traitement();

    }


}



void meteoTable::traitement()
{
    QString sDate="";
    QString sHeure="";
    QString sVitesse="";
    QString sDirection="";
    QString sInd="";

    QString sCourant="";

    QVectorIterator<QString> itDate(m_vDate);
    QVectorIterator<QString> itHeure(m_vHeure);
    QVectorIterator<QString> itSpeed(m_vWindSpeed);
    QVectorIterator<QString> itDirection(m_vWindDirection);

    while(itSpeed.hasNext())
    {
        sCourant=itSpeed.next();
        sInd=sCourant.section(",",0,0);
        sVitesse=sCourant.section(",",1,1);
        itDate.toFront();
        while(itDate.hasNext())
        {
            sCourant=itDate.next();

            if(sCourant.section(",",0,0)==sInd)
            {

                sDate=sCourant.section(",",1,1);
            }
        }
        itHeure.toFront();
        while(itHeure.hasNext())
        {
            sCourant=itHeure.next();
            if(sCourant.section(",",0,0)==sInd)
            {
                sHeure=sCourant.section(",",1,1);
            }

        }
        itDirection.toFront();
        while(itDirection.hasNext())
        {
            sCourant=itDirection.next();
            if(sCourant.section(",",0,0)==sInd)
            {
                sDirection=sCourant.section(",",1,1);
            }

        }


        m_vPrevisionTab.append(sDate+","+sHeure+","+sVitesse+","+sDirection);





    }
    RemplitTable(m_vPrevisionTab);




}

void meteoTable::RemplitTable(QVector<QString> vMeteoTable)
{
    QVectorIterator<QString> itMeteo(vMeteoTable);
    QString ligne="";

    int row=0;

    while(itMeteo.hasNext())
    {
        ligne=itMeteo.next();

        if(ligne.section(",",0,0).isEmpty()==false)
        {
            row=ui->tableMeteo->rowCount();
            ui->tableMeteo->insertRow(row);

            for(int column=0;column<ui->tableMeteo->columnCount();column++)
            {

                QTableWidgetItem *NewItem= new QTableWidgetItem(ligne.section(",",column,column));

                ui->tableMeteo->setItem(row,column,NewItem);

            }
        }
    }
    this->show();
}

void meteoTable::ligneSelectionnee()
{
    ui->btn_Valider->setEnabled(true);
}

void meteoTable::selectionValidee()
{
    QSettings settings("RoutageGen","RoutageGen_Config");
    settings.setValue("NbPrevisionsAvant",ui->sp_Avant->value());
    settings.setValue("NbPrevisionsApres",ui->sp_Apres->value());

    QString sDate="";
    QString sHeure="";
    QTableWidgetItem *iDate=new QTableWidgetItem;
    QTableWidgetItem *iHeure=new QTableWidgetItem;

    int nInd=0;
    int nLigne=0;
    nInd=ui->tableMeteo->currentRow();
    iDate=ui->tableMeteo->item(nInd,0);
    sDate=iDate->text();
    iHeure=ui->tableMeteo->item(nInd,1);
    sHeure=iHeure->text();

emit sendSelection(m_sLatitude+","+m_sLongitude+","+sDate+","+sHeure);

    for(nLigne=ui->sp_Avant->value();nLigne>0;nLigne--)
    {
        if(nInd-nLigne>=0)
            ajoutePrevision(nInd-nLigne,false);

    }
    ajoutePrevision(nInd,true);
    for(nLigne=0;nLigne<ui->sp_Apres->value();nLigne++)
    {
        ajoutePrevision(nInd+1+nLigne,false);
    }

    emit sendPrevision(m_vSelectPrevi);

    this->hide();
}

void meteoTable::ajoutePrevision(int nIndice,bool bRef)
{
    QString sDate="";
    QString sHeure="";
    QString sVitesse="";
    QString sDirection="";
    QTableWidgetItem *iDate=new QTableWidgetItem;
    QTableWidgetItem *iHeure=new QTableWidgetItem;
    QTableWidgetItem *iVitesse=new QTableWidgetItem;
    QTableWidgetItem *iDirection=new QTableWidgetItem;

    iDate=ui->tableMeteo->item(nIndice,0);
    iHeure=ui->tableMeteo->item(nIndice,1);
    iVitesse=ui->tableMeteo->item(nIndice,2);
    iDirection=ui->tableMeteo->item(nIndice,3);

    sDate=iDate->text();
    sHeure=iHeure->text();
    sVitesse=iVitesse->text();
    sDirection=iDirection->text();

    m_vSelectPrevi.append(m_sLatitude+","+m_sLongitude+","+sDate+","+sHeure+","+sVitesse+","+sDirection+","+QString::number(bRef));

}
