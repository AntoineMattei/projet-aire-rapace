/**
  * @file mainwindow.h
  * @details Fichier Mainwindow.h
  * @details Toutes les déclarations liées à la classe MainWindow sont écrites ici.
  *
  * @author CLARINARD Alexandre
  */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>

#include <QTcpServer>
#include <QTcpSocket>
#include <QValueAxis>

/*TESTS CONNECTION QT-SQL*/
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow
     * @param parent
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*GRAPHE IHM*/
    /**
     * @brief addPoint
     * @param x
     * @param y
     * @details Permet de rajouter, ou de mettre à jour, les points dans le graphe.
     */
    void addPoint(double x, double y);
    /**
     * @brief addPoint_constructeur
     * @param x
     * @param y
     * @details Permet de créer les points par défauts sur le graphe, en ordonnée négative, afin de pouvoir les mettre à jour.
     */
    void addPoint_constructeur(double x, double y);
    /**
     * @brief plot
     * @details Permet de mettre à jour un point dans le graphe.
     */
    void plot();

    /*TESTS CONNECTION IHM-SQL*/
    /**
     * @brief bdd
     * @details Permet de se connecter à la Base De Données.
     */
    void bdd();

/*SERVEUR TCP*/
public slots:
    /**
     * @brief onNewConnection
     * @details S'exécute lorsqu'un client se connecte à la liaison TCP/IP, permet de recevoir les trames de données via liaison TCP/IP.
     */
    void onNewConnection();
    /**
     * @brief onSocketStateChanged
     * @param socketState
     * @details S'exécute lorsqu'un client se déconnecte de la liaison TCP/IP, permet au programme de se supprimer la socket qui lui a été allouée.
     */
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    /**
     * @brief onReadyRead
     * @details Permet d'extraire puis d'afficher sur l'IHM les données reçues via liaison TCP/IP. Envoie également les données extraites à la Base De Données.
     */
    void onReadyRead();

    /**
     * @brief onNewConnection_image
     * @details S'exécute lorsqu'un client se connecte à la liaison TCP/IP, permet de recevoir les images via liaison TCP/IP.
     */
    void onNewConnection_image();
    /**
     * @brief onSocketStateChanged_image
     * @param socketState
     * @details S'exécute lorsqu'un client se déconnecte de la liaison TCP/IP, permet au programme de se supprimer la socket qui lui a été allouée.
     */
    void onSocketStateChanged_image(QAbstractSocket::SocketState socketState);
    /**
     * @brief onReadyRead_image
     * @details Permet d'extraire puis d'afficher sur l'IHM l'image reçue via liaison TCP/IP.
     */
    void onReadyRead_image();

private slots:

    /*HORLOGE IHM*/

    /**
     * @brief UpdateTime
     * @details Affiche l'heure en temps réel.
     */
    void UpdateTime();


    /*GRAPHE IHM*/

    /**void on_btn_clear_clicked(); //inutilisé**/

    /**
     * @brief on_pushButton_3_clicked
     * @details Permet de quitter l'application
     */
    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;

    /*HORLOGE IHM*/
    QTimer *timer_1s;

    /*GRAPHE IHM*/
    QVector<double> qv_x, qv_y;

    /*SERVEUR MYSQL*/
    QSqlDatabase dbMeteo;

    /*SERVEUR TCP*/
    QTcpServer  _server;
    QTcpServer  _server_image;
    QList<QTcpSocket*>  _sockets;
    QList<QTcpSocket*>  _sockets_image;

    /*TEST DE LA MORT*/
    QDate date;
    float temp,vit,hum,lum,poids;
    int chrono;
    int check_image;

};

#endif // MAINWINDOW_H
