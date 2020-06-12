#ifndef METEOTABLE_H
#define METEOTABLE_H

#include <QWidget>

namespace Ui {
class meteoTable;
}


class meteoTable : public QWidget
{
    Q_OBJECT

public:
    explicit meteoTable(QWidget *parent = 0);
    ~meteoTable();

signals:
    void sendSelection(QString PointDateHeure);
    void sendPrevision(QVector<QString>);

public slots:
    void init(QString sChemin);

private slots:
    void selectionValidee(void);
    void ligneSelectionnee(void);



private:
    Ui::meteoTable *ui;

    QString m_sLatitude;
    QString m_sLongitude;
    QString m_sIndVitesse;
    QString m_sIndDirection;
    QVector <QString> m_vDate;
    QVector <QString> m_vHeure;
    QVector <QString> m_vWindSpeed;
    QVector <QString> m_vWindDirection;

    QVector <QString> m_vPrevisionTab;
    QVector <QString> m_vSelectPrevi;

    void traitement(void);
    void RemplitTable(QVector<QString> vMeteoTable);
    void ajoutePrevision(int nIndice, bool bRef);


};

#endif // METEOTABLE_H
