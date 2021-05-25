#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QTimer>
#include <QThread>

#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QtSql>
#include <QFile>

#include <QPixmap>

#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      _server(this)
{
    ui->setupUi(this);
    QPixmap pix("C:/Users/alexa/Pictures/BTS/Revue projet 1/toit immeuble.jpg");
    ui->label_image->setPixmap(pix.scaled(int(ui->label_image->width()),int(ui->label_image->height()),Qt::KeepAspectRatio));

    /*HORLOGE IHM*/
    timer_1s = new QTimer(this);
    QObject::connect(timer_1s, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    timer_1s->start(1000);

    /*WIDGET GRAPHE*/
    ui->widgetGraphe->addGraph();
    ui->widgetGraphe->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->widgetGraphe->graph(0)->setLineStyle(QCPGraph::lsNone);
    // give the axes some labels:
    ui->widgetGraphe->xAxis->setLabel("Heure");
    ui->widgetGraphe->yAxis->setLabel("Poids total de l'aire en kg");
    // set axes ranges, so we see all data:
    ui->widgetGraphe->xAxis->setRange(0, 24);
    ui->widgetGraphe->yAxis->setRange(0, 50);
    ui->widgetGraphe->replot();

    for(int i=0; i < 24 ; i++)
        addPoint_constructeur(i,-1);    //reset du graphe
    chrono = 0;

    /*SERVER TCP*/
    _server.listen(QHostAddress::Any, 4242);
    connect(&_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    qDebug() << "Serveur TCP données : " << _server.serverAddress().toString() << " au port : " << _server.serverPort();


    /*SERVEUR TCP IMAGE*/
    _server_image.listen(QHostAddress::Any, 4243);
    connect(&_server_image, SIGNAL(newConnection()), this, SLOT(onNewConnection_image()));
    qDebug() << "Serveur TCP image   : " << _server_image.serverAddress().toString() << " au port : " << _server_image.serverPort();

    /*TESTS CONNECT BDD*/
    bdd();

    /*LABEL IP*/
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            this->ui->label_IP->setText("Adresse pour se connecter : " + address.toString());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*HORLOGE IHM*/
void MainWindow::heure()
{

    QTime time;
    QString heure_string;
    //while(1){
        heure_string = time.currentTime().toString("hh:mm:ss");
        ui->heure->setText(heure_string);
    //}
}

void MainWindow::UpdateTime()
{
    ui->heure->setText(QTime::currentTime().toString("hh:mm:ss"));
}

/*GRAPHE IHM*/

void MainWindow::on_btn_clear_clicked()
{
    clearData();
    plot();
}

void MainWindow::addPoint_constructeur(double x, double y){
    qv_x.append(x);
    qv_y.append(y);
}

void MainWindow::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.replace(x,y);
}

void MainWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

void MainWindow::plot()
{
    ui->widgetGraphe->graph(0)->setData(qv_x, qv_y);
    ui->widgetGraphe->replot();
    ui->widgetGraphe->update();
}


/*BASE DE DONNEES*/

void MainWindow::bdd()
{
    qDebug() << "ouverture fonction bdd";
    // Instanciation de la variable membre dbMeteo
    dbMeteo = QSqlDatabase::addDatabase("QMYSQL");

    // Définition des paramètres de connexion à la base de données
    dbMeteo.setHostName("127.0.0.1"); // @ip serveur MySQL
    dbMeteo.setDatabaseName("air_rapace"); // Nom de la base
    dbMeteo.setUserName("root"); // Nom utilisateur
    dbMeteo.setPassword(""); // Mot de passe

    if(dbMeteo.open()) {
        qDebug() << "Ok - ouverture de la base de donnée";

        // Exécution d'une requête
        QSqlQuery requete;

        /*TEST ULTIME DE LA MORT MOUHAHAHA*/
        /*requete.prepare("INSERT INTO mesures (date,temperature,vitesse_vent,humidite,luminosite,poids_aire)"
                        "VALUES (:newDate,:newTemp,:newVit,:newHum,:newLum,newPoids)");
        requete.bindValue(":newDate",date);
        requete.bindValue(":newTemp",temp);
        requete.bindValue(":newVit",vit);
        requete.bindValue(":newHum",hum);
        requete.bindValue(":newLum",lum);
        requete.bindValue(":newPoids",poids);*/
        if(requete.exec("SELECT * FROM mesures")) {
            qDebug() << "Ok - requete";

            // Boucle qui permet de parcourir les enregistrements renvoyés par la requête
            while(requete.next()) {
            // On accède ici aux différents champs par leurs noms, il est également possible
            // d'y accéder par leur index : requete.value(0)
            qDebug() << requete.value("date") << " " << requete.value("temperature") << " "
                     << requete.value("vitesse_vent") << " " << requete.value("humidite")
                     << requete.value("luminosite") << requete.value("poids_aire");
            }
        }
        else {
            qDebug() << "Echec de la requête";
            // La méthode lastError permet d'afficher un message
            // plus explicite sur les causes de l'erreur
            qDebug() << requete.lastError();
        }
        //dbMeteo.close(); // Fermeture de la base de données
    }
    else {
        qDebug() << "Echec d'ouverture de la base de donnée";
        qDebug() << dbMeteo.lastError();
    }
}


/*SERVEUR TCP*/
void MainWindow::onNewConnection()
{
   QTcpSocket *clientSocket = _server.nextPendingConnection();
   connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
   connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
    _sockets.push_back(clientSocket);
    for (QTcpSocket* socket : _sockets) {
        socket->write(QByteArray::fromStdString(clientSocket->peerAddress().toString().toStdString() + " connected to server !\n"));
    }
    bdd();
}

void MainWindow::onNewConnection_image()
{
    QTcpSocket *clientSocket = _server_image.nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead_image()));
    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged_image(QAbstractSocket::SocketState)));
     _sockets_image.push_back(clientSocket);
     for (QTcpSocket* socket : _sockets_image) {
         socket->write(QByteArray::fromStdString(clientSocket->peerAddress().toString().toStdString() + " connected to server !\n"));
     }
}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        _sockets.removeOne(sender);
    }
}

void MainWindow::onSocketStateChanged_image(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        _sockets_image.removeOne(sender);
    }
}

void MainWindow::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();
    cout << datas.size() << endl;

    check_image = 1;
    QPixmap pix("C:/Users/alexa/Pictures/image_recue.ppm");
    ui->label_image->setPixmap(pix.scaled(int(ui->label_image->width()),int(ui->label_image->height()),Qt::KeepAspectRatio));

    //données air

    qDebug() << datas;
    QString string(datas);
    QStringList liste;
    liste << string.split(',');
    if((liste.at(0) == "$TRACQ") && (liste.at(8) == "*")){
        ui->labelTempInfo->setText(liste.at(1) + " °C");
        ui->labelVentInfo->setText(liste.at(2) + " km/h");
        ui->labelHumideInfo->setText(liste.at(3) + " %");
        ui->labelLumInfo->setText(liste.at(4) + " lux");
        ui->labelAireInfo->setText(liste.at(5) + " kg");

        this->temp  = liste.at(1).toFloat();
        this->vit   = liste.at(2).toFloat();
        this->hum   = liste.at(3).toFloat();
        this->lum   = liste.at(4).toFloat();
        this->poids = liste.at(5).toFloat();
        qDebug() << temp << vit << hum << lum << poids;

        /// Gestion de la date + affichage (debug)
        QString stringDate = liste.at(6);
        QStringList stringListDate = stringDate.split('.');
        date.setDate(stringListDate.at(2).toInt(), stringListDate.at(1).toInt(), stringListDate.at(0).toInt()); // QDate::setDate(int year, int month, int day)
        qDebug() << "<debug>" << date.toString("dddd d MMMM yy");

        /*INSERTION DONNEES DANS LA BDD*/
        QSqlQuery requete;
        //qDebug() <<
        requete.prepare("INSERT INTO mesures set temperature=:temp, vitesse_vent=:vit, humidite=:hum, luminosite=:lum, poids_aire=:poids");
        requete.bindValue(":temp",this->temp);
        requete.bindValue(":vit",this->vit);
        requete.bindValue(":hum",this->hum);
        requete.bindValue(":lum",this->lum);
        requete.bindValue(":poids",this->poids);
        requete.exec();
        qDebug() << requete.lastError().text();

        /*datas.value(2).toFloat()->liste.at(1);
        datas.value(3).toFloat()->liste.at(2);
        datas.value(4).toFloat()->liste.at(3);
        datas.value(5).toFloat()->liste.at(4);
        datas.value(6).toFloat()->liste.at(5);*/
        addPoint(chrono, liste.at(5).toDouble());

        chrono = chrono + 1;
        qDebug() << "+1 heure";
        qDebug() << chrono;
        if (chrono >= 23)
        {
            chrono = 0;
            qDebug() << "Minuit, nouvelle journée";
        }
        plot();
    } else{
        qDebug("Trame invalide : Commencez par '$TRACQ', et terminez par '*'");
    }

    for (QTcpSocket* socket : qAsConst(_sockets)) {
        if (socket != sender)
            socket->write(QByteArray::fromStdString(sender->peerAddress().toString().toStdString() + ": " + datas.toStdString()));
    }
}

void MainWindow::onReadyRead_image()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();
    cout << datas.size() << endl;

    if(check_image){
        QFile::remove("C:/Users/alexa/Pictures/image_recue.ppm");
        check_image=0;
    }
    //image caméra
    QFile image("C:/Users/alexa/Pictures/image_recue.ppm");
    image.open(QIODevice::ReadWrite | QIODevice::Append);
    image.write(datas);
    //image.close();


    for (QTcpSocket* socket : qAsConst(_sockets_image)) {
        if (socket != sender)
            socket->write(QByteArray::fromStdString(sender->peerAddress().toString().toStdString() + ": " + datas.toStdString()));
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    QFile::remove("C:/Users/alexa/Pictures/image.png");
    close();
}
