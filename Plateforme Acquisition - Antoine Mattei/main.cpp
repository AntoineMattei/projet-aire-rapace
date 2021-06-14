#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <raspicam/raspicam.h>
#include <QRandomGenerator>

#include "capteurs.h"
#include "ssd1306.h"
using namespace std;
using namespace raspicam;
QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList liste_trame;
    QByteArray toSend;

    QRandomGenerator aleatoire;

    printf("Heure :\n");

    QDateTime date_time;
    QTime heure_actuelle;

    QString date_heure = date_time.currentDateTime().toString("dddd dd MMMM yyyy");
    QString dateHeureToSend = date_time.currentDateTime().toString("dd.MM.yy,hh:mm:ss");

    cout << "Nous sommes le " << date_time.currentDateTime().toString("dddd dd MMMM yyyy").toStdString()
         << ". Il est " << date_time.currentDateTime().toString("hh").toStdString()
         << "h " << date_time.currentDateTime().toString("mm").toStdString() << "min"
         << date_time.currentDateTime().toString("ss").toStdString() << "secondes"
         << date_time.currentDateTime().toString("zzz").toStdString() << "ms" << endl;

    // Déclaration Camera
    RaspiCam Camera;    // Camera object
    // Ouverture de la caméra

    cout << "Ouverture de la caméra" << endl;
    if(!Camera.open()) {
        cerr << "Erreur lors de l'ouverture de la caméra" << endl;
        return -1;
    }

    // Instanciation du port série
    QSerialPort *serial_USB0 = new QSerialPort("/dev/ttyUSB0");

    QByteArray trame_recue, trame_1, trame_2;
    double vitesseVent, poidsAire, luminosite, temperature, humidite;
    vitesseVent = poidsAire = luminosite = temperature = humidite = 0;

    // Déclaration serveur
    string addr_Serveur_String;
    cout << "Entrez l'adresse du serveur : ";
    cin >> addr_Serveur_String;
    cout << "Adresse séléctionnée : " << addr_Serveur_String << endl;
    QString Qstring_temp;
    QString addr_Serveur = Qstring_temp.fromStdString(addr_Serveur_String);
    QTcpSocket socket;

    // Déclaration écran
    ssd1306 ecran;

    //--------- Début programme Acquisition -------------

    while (1) {
        //------------------ Début Caméra -------------------


        // Attente de la stabilisation de la caméra
        cout << "Attente de 3 secondes pour la stabilisation de la caméra" << endl;
        sleep(3);
        // Capture
        Camera.grab();
        // Allocation de la mémoire
        unsigned char *donnees = new unsigned char[ Camera.getImageTypeSize(RASPICAM_FORMAT_RGB) ];
        // Extraction de l'image en format RGB
        Camera.retrieve(donnees, RASPICAM_FORMAT_RGB);  // Obtenir l'image de la camera
        // Sauvegarde
        ofstream outFile ("/home/pi/raspicam_image.ppm", std::ios::binary);
        outFile << "P6\n" << Camera.getWidth() << " " << Camera.getHeight() << " 255\n"; // en-tête)
        outFile.write(reinterpret_cast<char *>(donnees), static_cast<int>(Camera.getImageTypeSize(RASPICAM_FORMAT_RGB)));
        cout << "Image sauvegardée dans 'raspicam_image.ppm'" << endl;
        // Libération de la ressource
        delete [] donnees;

        // -------------------------------------------
        // Liaison série

        // On recherche les différentes interfaces RS232

        foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            qDebug() << "Available ports :";
            qDebug() << "Name            : " << info.portName();
            qDebug() << "Description     : " << info.description();
            qDebug() << "Manufacturer    : " << info.manufacturer();
        }

        // Configuration
        serial_USB0->setBaudRate(QSerialPort::Baud9600);
        serial_USB0->setDataBits(QSerialPort::Data8);
        serial_USB0->setParity(QSerialPort::NoParity);
        serial_USB0->setStopBits(QSerialPort::OneStop);
        serial_USB0->setFlowControl(QSerialPort::NoFlowControl);

        // On ouvre le port
        if(serial_USB0->open(QIODevice::ReadWrite)){
            serial_USB0->waitForReadyRead(-1);
            trame_recue = serial_USB0->readAll();

            qDebug() << "Trame reçue = " << trame_recue;
            QString trame(trame_recue);
            liste_trame = trame.split('*');
            qDebug() << liste_trame.size();

            trame_1.append(liste_trame.at(0));
            qDebug() << "trame_1 = " << trame_1;
            QString trame_1_String(trame_1);
            QStringList trame_1_list = trame_1_String.split(',');
            if(trame_1_list.at(0) == "$TRACQ1"){
                vitesseVent = trame_1_list.at(1).toDouble();
                toSend.append("$TRACQ," + trame_1_list.at(1).toUtf8() + ",");
                poidsAire   = trame_1_list.at(2).toDouble();
                toSend.append(trame_1_list.at(2).toUtf8() + ",");
            }
            else {
                qDebug("Erreur $TRACQ1");
            }

            trame_2.append(liste_trame.at(1));
            qDebug() << "trame_2 = " << trame_2;
            QString trame_2_String(trame_2);
            QStringList trame_2_list = trame_2_String.split(',');
            if(trame_2_list.at(0) == "$TRACQ2"){
                luminosite  = trame_2_list.at(1).toDouble();
                toSend.append(trame_2_list.at(1).toUtf8() + ",");
                temperature = trame_2_list.at(2).toDouble();
                toSend.append(trame_2_list.at(2).toUtf8() + ",");
                humidite    = trame_2_list.at(3).toDouble();
                toSend.append(trame_2_list.at(3).toUtf8() + ",");
            }
        } else{
            perror("Erreur lors de l'ouverture du port :");
            serial_USB0->errorString();
        }

        if(serial_USB0->isOpen()){
            serial_USB0->close();
        }

        // -----------------------------------------------
        // --------------------SOCKET---------------------
        // -----------------------------------------------

        // ------------------- Image ---------------------
        socket.connectToHost(addr_Serveur,4243);

        socket.waitForConnected(-1);

        if(socket.state() != QAbstractSocket::ConnectedState){
            qDebug() << "Etat lors de l'arrêt du programme : " << socket.state();
            qDebug() << socket.errorString();
        }
        else{
            qDebug() << "Connected ! State : " << socket.state();
            socket.waitForReadyRead();
            if(socket.isReadable()){
                QByteArray donneesEntrees = socket.readAll();
                cout << donneesEntrees.toStdString();
                ifstream image_entree("/home/pi/raspicam_image.ppm", std::ifstream::binary);

                if(image_entree){
                    image_entree.seekg(0, image_entree.end);
                    int length = static_cast<int>(image_entree.tellg());
                    image_entree.seekg(0,image_entree.beg);

                    char *buffer = new char[static_cast<unsigned int>(length)];
                    cout << "Lecture de l'image. Taille : " << length << endl;
                    image_entree.read(buffer, length);
                    socket.write(buffer, length);

                    delete [] buffer;
                }

                image_entree.close();
            }
        }
        sleep(1);
        while (socket.state() != QAbstractSocket::UnconnectedState) {
            socket.disconnectFromHost();
            socket.waitForDisconnected();
        }

        // ------------------- Trame ---------------------

        socket.connectToHost(addr_Serveur,4242);
        socket.waitForConnected(-1);

        if(socket.state() != QAbstractSocket::ConnectedState){
            qDebug() << "Etat lors de l'arrêt du programme : " << socket.state();
            qDebug() << socket.errorString();
        }
        else{
            qDebug() << "Connected ! State : " << socket.state();
            socket.waitForReadyRead();
            if(socket.isReadable()){
                toSend.append(dateHeureToSend.toUtf8() + ",*");
                cout << toSend.toStdString() << endl;

                socket.write(toSend);
            }
        }

        sleep(1);
        while (socket.state() != QAbstractSocket::UnconnectedState) {
            socket.disconnectFromHost();
            socket.waitForDisconnected();
        }

        // -------------------------------------------

        // Ecran :
        ecran.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

        ecran.display(); //Adafruit logo is visible
        sleep(1);

        ecran.clearDisplay();
        usleep(5000);

        ecran.setTextSize(2);

        // ************ Premier écran *************
        Capteurs capteur(vitesseVent,poidsAire,luminosite,temperature,humidite);

        ecran.ecrireValeurEcran(Capteurs::VitesseVent, capteur);

        ecran.ecrireValeurEcran(Capteurs::PoidsAire, capteur);

        ecran.ecrireValeurEcran(Capteurs::Luminosite, capteur);

        ecran.ecrireValeurEcran(Capteurs::Temperature, capteur);

        ecran.display();

        sleep(5);

        // On efface l'écran
        ecran.clearDisplay();
        usleep(5000);

        ecran.setTextSize(2);
        // -************ Second écran **************
        // Humidité

        ecran.ecrireValeurEcran(Capteurs::Humidite, capteur);
        char *heureDisplay = const_cast<char *>(heure_actuelle.currentTime().toString("hh:mm:ss").toStdString().c_str());
        char * H = const_cast<char *>("H:");
        ecran.drawString(H);
        ecran.drawString(heureDisplay);

        ecran.display();
        sleep(5);

        ecran.clearDisplay();
        usleep(5000);

        ecran.setTextSize(2);
        ecran.display();
        //sleep(20);
        qDebug() << "Fin de la boucle";
        trame_1.clear();
        trame_2.clear();
        trame_recue.clear();
        toSend.clear();
    }

    return 0;
}
