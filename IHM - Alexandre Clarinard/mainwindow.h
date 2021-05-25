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
     */
    void addPoint(double x, double y);
    /**
     * @brief addPoint_constructeur
     * @param x
     * @param y
     */
    void addPoint_constructeur(double x, double y);
    /**
     * @brief clearData
     */
    void clearData();
    /**
     * @brief plot
     */
    void plot();

    /*TESTS CONNECTION IHM-SQL*/
    void bdd();

/*SERVEUR TCP*/
public slots:
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();

    void onNewConnection_image();
    void onSocketStateChanged_image(QAbstractSocket::SocketState socketState);
    void onReadyRead_image();

private slots:

    /*HORLOGE IHM*/
    void heure();

    void UpdateTime();


    /*GRAPHE IHM*/

    void on_btn_clear_clicked();

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
