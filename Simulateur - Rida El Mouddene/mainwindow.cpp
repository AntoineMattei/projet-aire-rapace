#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    QObject::connect(ui->pushButton_quitter,SIGNAL(clicked()),qApp,SLOT(quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_connexion_clicked()
{
    RS232 serial;
    fd = serial.OpenSerial();
    fd2 = serial.OpenSerial2();

    if( (fd > 0) | (fd2 > 0) )
    {
        ui->pushButton_connexion->setText("Connecté");
        ui->pushButton_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #27ae60 ; font: bold;");
    }
    else
    {
        ui->pushButton_connexion->setText("Connexion échoué");
        ui->pushButton_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #f1230a ; font: bold;");
    }

}

void MainWindow::on_pushButton_automatique_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");
    ui->stackedWidget->setCurrentIndex(0);
    // IL FAUT CLEAR ICI !!!
}

void MainWindow::on_pushButton_manuel_clicked()
{
    ui->pushButton_manuel->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_automatique->setStyleSheet("");
    ui->stackedWidget->setCurrentIndex(6);

    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("");

    ui->spinBox_vent->setRange(0,100);
    ui->spinBox_aire->setRange(10,100);
    ui->spinBox_lux->setRange(0,150000);
    ui->spinBox_temp->setRange(0,45);
    ui->spinBox_humi->setRange(0,100);

}


void MainWindow::on_pushButton_appliquer_clicked()
{
    if((ui->pushButton_connexion->text() != "Connecté") | (ui->pushButton_connexion->text() == "Connexion échoué") )
    {
        ui->label_warning_connexion->setText("Connectez-vous !");
        ui->label_warning_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #f1230a ; font: bold;");
    }
    else
    {
        RS232 serial;

        if(ui->pushButton_manuel->styleSheet() == "border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;")
        {
            QByteArray trame_manuel1;
            QByteArray trame_manuel2;

            trame_manuel1 = "$TRACQ1,";

            trame_manuel1 += QByteArray::number(ui->spinBox_vent->value()) += ",";
            trame_manuel1 += QByteArray::number(ui->spinBox_aire->value()) += ",";
            trame_manuel1 += QByteArray::number(ui->spinBox_lux->value()) += ",*";

            trame_manuel2 = "$TRACQ2,";

            trame_manuel2 += QByteArray::number(ui->spinBox_temp->value()) += ",";
            trame_manuel2 += QByteArray::number(ui->spinBox_humi->value()) += ",*";

            ui->label_warning_connexion->setText("Enregistré");
            ui->label_warning_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #27ae60 ; font: bold;");


            if(serial.SendSerial(fd,trame_manuel1) == -1)
            {
                ui->label_warning_connexion->setText("Erreur d'envoi trame 1 !");
                ui->label_warning_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #f1230a ; font: bold;");
            }
            sleep(1);

            if(serial.SendSerial2(fd2,trame_manuel2) == -1)
            {
                ui->label_warning_connexion->setText("Erreur d'envoi trame 2 !");
                ui->label_warning_connexion->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #f1230a ; font: bold;");
            }
        }
        else if (ui->pushButton_automatique->styleSheet() == "border-style: inset; border-width: 1px; border-radius: 5px; border-color:   #27ae60 ; font: bold;")
        {


        }

    }

}

void MainWindow::on_pushButton_reset_clicked()
{
    RS232 serial;
    serial.close_fd(fd);
    serial.close_fd2(fd2);

    //clearPoint();

    ui->pushButton_connexion->setText("Connexion RS232");
    ui->pushButton_connexion->setStyleSheet("");

    ui->verticalSlider_vent->setValue(0);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton_manuel->setStyleSheet("");
    ui->pushButton_automatique->setStyleSheet("");
    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("");
    ui->label_warning_connexion->setStyleSheet("");
    ui->label_warning_connexion->setText("");

}


//******************************************************** ONGLETS DES GRANDEURS PHYSIQUES *****************************************************************//

void MainWindow::on_pushButton_vent_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");

    //configuration du graphe de l'onglet "Vitesse du vent"
    if(nb_click < 1)
    {
        ui->customPlot_vent->addGraph();
        ui->customPlot_vent->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot_vent->xAxis->setRange(0, 21);
        ui->customPlot_vent->yAxis->setRange(0, 100);
        ui->customPlot_vent->xAxis->setLabel("Temps en heure");
        ui->customPlot_vent->yAxis->setLabel("Vitesse du vent en Km/h");

        for(double i = 0.0 ; i <= 21.0 ; i += 3.0)
        {
            addPoint_lancement(1,i,0.0);
        }
        nb_click += 1;
    }

    ui->stackedWidget->setCurrentIndex(1);
    ui->pushButton_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_humi->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("");


}

void MainWindow::on_pushButton_poids_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");

    //configuration du graphe de l'onglet "Masse de l'air"
    if(nb_click < 2)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->customPlot_poids->addGraph();
        ui->customPlot_poids->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot_poids->xAxis->setRange(0, 21);
        ui->customPlot_poids->yAxis->setRange(10, 100);
        ui->customPlot_poids->xAxis->setLabel("Temps en heure");
        ui->customPlot_poids->yAxis->setLabel("Masse en Kg");
        for(double i = 0.0 ; i <= 21.0 ; i += 3.0)
        {
            addPoint_lancement(2,i,0.0);
        }
        nb_click += 1;
    }

    //changement de couleur des bouttons

    ui->pushButton_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("");


}

void MainWindow::on_pushButton_lux_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");

    //configuration du graphe de l'onglet "Luminosité"
    if(nb_click < 3)
    {
        ui->customPlot_lux->addGraph();
        ui->customPlot_lux->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot_lux->xAxis->setRange(0, 21);
        ui->customPlot_lux->yAxis->setRange(0, 150000);
        ui->customPlot_lux->xAxis->setLabel("Temps en heure");
        ui->customPlot_lux->yAxis->setLabel("Intensité lumineuse en lux");
        for(double i = 0.0 ; i <= 21.0 ; i += 3.0)
        {
            addPoint_lancement(3,i,0.0);
        }
        nb_click += 1;
    }

    //changement de couleur des bouttons
    ui->stackedWidget->setCurrentIndex(3);
    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_temp->setStyleSheet("");
}

void MainWindow::on_pushButton_temp_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");

    //configuration du graphe de l'onglet "Temperature"
    if(nb_click < 4)
    {
        ui->customPlot_temp->addGraph();
        ui->customPlot_temp->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot_temp->xAxis->setRange(0, 21);
        ui->customPlot_temp->yAxis->setRange(0, 45);
        ui->customPlot_temp->xAxis->setLabel("Temps en heure");
        ui->customPlot_temp->yAxis->setLabel("Température en °C ");
        for(double i = 0.0 ; i <= 21.0 ; i += 3.0)
        {
            addPoint_lancement(4,i,0.0);
        }
        nb_click += 1;
    }


    //changement de couleur des bouttons
    ui->stackedWidget->setCurrentIndex(4);
    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
}

void MainWindow::on_pushButton_humi_clicked()
{
    ui->pushButton_automatique->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_manuel->setStyleSheet("");

    //configuration du graphe de l'onglet "Humidité"
    if(nb_click < 5)
    {
        ui->customPlot_humi->addGraph();
        ui->customPlot_humi->graph()->setLineStyle(QCPGraph::lsLine);
        ui->customPlot_humi->xAxis->setRange(0, 21);
        ui->customPlot_humi->yAxis->setRange(0, 100);
        ui->customPlot_humi->xAxis->setLabel("Temps en heure");
        ui->customPlot_humi->yAxis->setLabel("Humidité en %");
        for(double i = 0.0 ; i <= 21.0 ; i += 3.0)
        {
            addPoint_lancement(5,i,0.0);
        }
        nb_click += 1;
    }
    ui->stackedWidget->setCurrentIndex(5);
    ui->pushButton_poids->setStyleSheet("");
    ui->pushButton_vent->setStyleSheet("");
    ui->pushButton_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_lux->setStyleSheet("");
    ui->pushButton_temp->setStyleSheet("");
}


//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//

//******************************************************** PARAMETRAGE DES GRAPHES ****************************************************************//



void MainWindow::addPoint_lancement(int who, double x, double y)
{
    switch (who)
    {
    case 1 :
        qv_x_vent.append(x);
        qv_y_vent.append(y);
        ui->customPlot_vent->graph()->setData(qv_x_vent,qv_y_vent);
        ui->customPlot_vent->replot();
        ui->customPlot_vent->update();
        break;

    case 2 :
        qv_x_poids.append(x);
        qv_y_poids.append(y);
        ui->customPlot_poids->graph()->setData(qv_x_poids,qv_y_poids);
        ui->customPlot_poids->replot();
        ui->customPlot_poids->update();
        break;

    case 3 :
        qv_x_lux.append(x);
        qv_y_lux.append(y);
        ui->customPlot_lux->graph()->setData(qv_x_lux,qv_y_lux);
        ui->customPlot_lux->replot();
        ui->customPlot_lux->update();
        break;

    case 4 :
        qv_x_temp.append(x);
        qv_y_temp.append(y);
        ui->customPlot_temp->graph()->setData(qv_x_temp,qv_y_temp);
        ui->customPlot_temp->replot();
        ui->customPlot_temp->update();
        break;

    case 5 :
        qv_x_humi.append(x);
        qv_y_humi.append(y);
        ui->customPlot_humi->graph()->setData(qv_x_humi,qv_y_humi);
        ui->customPlot_humi->replot();
        ui->customPlot_humi->update();
        break;

    }
}

void MainWindow::addPoint(int who, double x, double y)
{
    switch (who)
    {
    case 1 :
        qv_x_vent.append(x);
        qv_y_vent.replace(x,y);
        ui->customPlot_vent->graph()->setData(qv_x_vent,qv_y_vent);
        break;
    case 2 :
        qv_x_poids.append(x);
        qv_y_poids.replace(x,y);
        ui->customPlot_poids->graph()->setData(qv_x_poids,qv_y_poids);
        break;
    case 3 :
        qv_x_lux.append(x);
        qv_y_lux.replace(x,y);
        ui->customPlot_lux->graph()->setData(qv_x_lux,qv_y_lux);
        break;
    case 4 :
        qv_x_temp.append(x);
        qv_y_temp.replace(x,y);
        ui->customPlot_temp->graph()->setData(qv_x_temp,qv_y_temp);
        break;
    case 5 :
        qv_x_humi.append(x);
        qv_y_humi.replace(x,y);
        ui->customPlot_humi->graph()->setData(qv_x_humi,qv_y_humi);
        break;
    }

}

void MainWindow::clearPoint()
{
    qv_y_vent.clear();
    qv_x_vent.clear();

    qv_y_poids.clear();
    qv_x_poids.clear();

    qv_y_lux.clear();
    qv_x_lux.clear();

    qv_y_temp.clear();
    qv_x_temp.clear();

    qv_y_humi.clear();
    qv_x_humi.clear();

    pv_x_vent = 0.0;
    pv_y_vent = 0.0;

    pv_x_poids = 0.0;
    pv_y_poids = 0.0;

    pv_x_lux = 0.0;
    pv_y_lux = 0.0;

    pv_x_temp = 0.0;
    pv_y_temp = 0.0;

    pv_x_humi = 0.0;
    pv_y_humi = 0.0;

    plot('a');
}

void MainWindow::plot(char who)
{
    switch(who)
    {
    case 'v' :
        addPoint(1,pv_x_vent,pv_y_vent);
        ui->customPlot_vent->replot();
        ui->customPlot_vent->update();
        break;

    case 'p' :
        addPoint(2,pv_x_poids,pv_y_poids);
        ui->customPlot_poids->replot();
        ui->customPlot_poids->update();
        break;

    case 'l' :
        addPoint(3,pv_x_lux,pv_y_lux);
        ui->customPlot_lux->replot();
        ui->customPlot_lux->update();
        break;

    case 't' :
        addPoint(4,pv_x_temp,pv_y_temp);
        ui->customPlot_temp->replot();
        ui->customPlot_temp->update();
        break;

    case 'h' :
        addPoint(5,pv_x_humi,pv_y_humi);
        ui->customPlot_humi->replot();
        ui->customPlot_humi->update();
        break;

    case 'a' :
        addPoint(1,pv_x_vent,pv_y_vent);
        ui->customPlot_vent->replot();
        ui->customPlot_vent->update();
        addPoint(2,pv_x_poids,pv_y_poids);
        ui->customPlot_poids->replot();
        ui->customPlot_poids->update();
        addPoint(3,pv_x_lux,pv_y_lux);
        ui->customPlot_lux->replot();
        ui->customPlot_lux->update();
        addPoint(4,pv_x_temp,pv_y_temp);
        ui->customPlot_temp->replot();
        ui->customPlot_temp->update();
        addPoint(5,pv_x_humi,pv_y_humi);
        ui->customPlot_humi->replot();
        ui->customPlot_humi->update();
        break;

    }
}

//**************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//

//******************************************************** ONGLET VITESSE DU VENT *****************************************************************//


void MainWindow::on_pushButton_00h_vent_clicked()
{
    pv_x_vent = 0.0;
    ui->pushButton_00h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');
}

void MainWindow::on_pushButton_03h_vent_clicked()
{
    pv_x_vent = 1.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');
}

void MainWindow::on_pushButton_06h_vent_clicked()
{
    pv_x_vent = 2.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');

}

void MainWindow::on_pushButton_09h_vent_clicked()
{
    pv_x_vent = 3.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');

}

void MainWindow::on_pushButton_12h_vent_clicked()
{
    pv_x_vent = 4.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');
}

void MainWindow::on_pushButton_15h_vent_clicked()
{
    pv_x_vent = 5.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');
}

void MainWindow::on_pushButton_18h_vent_clicked()
{
    pv_x_vent = 6.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_21h_vent->setStyleSheet("");
    plot('v');
}

void MainWindow::on_pushButton_21h_vent_clicked()
{
    pv_x_vent = 7.0;
    ui->pushButton_00h_vent->setStyleSheet("");
    ui->pushButton_03h_vent->setStyleSheet("");
    ui->pushButton_06h_vent->setStyleSheet("");
    ui->pushButton_09h_vent->setStyleSheet("");
    ui->pushButton_12h_vent->setStyleSheet("");
    ui->pushButton_15h_vent->setStyleSheet("");
    ui->pushButton_18h_vent->setStyleSheet("");
    ui->pushButton_21h_vent->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    plot('v');
}

void MainWindow::on_verticalSlider_vent_sliderReleased()
{
    pv_y_vent = ui->verticalSlider_vent->value() + 0.0;
}


//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//

//******************************************************** ONGLET MASSE DE L'AIR *****************************************************************//

void MainWindow::on_pushButton_00h_poids_clicked()
{
    pv_x_poids = 0.0;
    ui->pushButton_00h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');

}

void MainWindow::on_pushButton_03h_poids_clicked()
{
    pv_x_poids = 1.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_06h_poids_clicked()
{
    pv_x_poids = 2.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_09h_poids_clicked()
{
    pv_x_poids = 3.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_12h_poids_clicked()
{
    pv_x_poids = 4.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_15h_poids_clicked()
{
    pv_x_poids = 5.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_18h_poids_clicked()
{
    pv_x_poids = 6.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_21h_poids->setStyleSheet("");
    plot('p');
}

void MainWindow::on_pushButton_21h_poids_clicked()
{
    pv_x_poids = 7.0;
    ui->pushButton_00h_poids->setStyleSheet("");
    ui->pushButton_06h_poids->setStyleSheet("");
    ui->pushButton_03h_poids->setStyleSheet("");
    ui->pushButton_09h_poids->setStyleSheet("");
    ui->pushButton_12h_poids->setStyleSheet("");
    ui->pushButton_15h_poids->setStyleSheet("");
    ui->pushButton_18h_poids->setStyleSheet("");
    ui->pushButton_21h_poids->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    plot('p');
}

void MainWindow::on_verticalSlider_poids_sliderReleased()
{
    pv_y_poids = ui->verticalSlider_poids->value() + 0.0;
}
//************************************************************************************************************************************************//
//************************************************************************************************************************************************//
//************************************************************************************************************************************************//

//******************************************************** ONGLET LUMINOSITE ************************************************************//


void MainWindow::on_pushButton_00h_lux_clicked()
{
    pv_x_lux = 0.0;
    ui->pushButton_00h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_03h_lux_clicked()
{
    pv_x_lux = 1.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');
}

void MainWindow::on_pushButton_06h_lux_clicked()
{
    pv_x_lux = 2.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_09h_lux_clicked()
{
    pv_x_lux = 3.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_12h_lux_clicked()
{
    pv_x_lux = 4.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_15h_lux_clicked()
{
    pv_x_lux = 5.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_18h_lux_clicked()
{
    pv_x_lux = 6.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_21h_lux->setStyleSheet("");
    plot('l');

}

void MainWindow::on_pushButton_21h_lux_clicked()
{
    pv_x_lux = 7.0;
    ui->pushButton_00h_lux->setStyleSheet("");
    ui->pushButton_06h_lux->setStyleSheet("");
    ui->pushButton_03h_lux->setStyleSheet("");
    ui->pushButton_09h_lux->setStyleSheet("");
    ui->pushButton_12h_lux->setStyleSheet("");
    ui->pushButton_15h_lux->setStyleSheet("");
    ui->pushButton_18h_lux->setStyleSheet("");
    ui->pushButton_21h_lux->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    plot('l');

}

void MainWindow::on_verticalSlider_lux_sliderReleased()
{
    pv_y_lux = ui->verticalSlider_lux->value() + 0.0;
}

//************************************************************************************************************************************************//
//************************************************************************************************************************************************//
//************************************************************************************************************************************************//

//******************************************************** ONGLET TEMPERATURE ************************************************************//


void MainWindow::on_pushButton_00h_temp_clicked()
{
    pv_x_temp = 0.0;
    ui->pushButton_00h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');
}

void MainWindow::on_pushButton_03h_temp_clicked()
{
    pv_x_temp = 1.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');

}

void MainWindow::on_pushButton_06h_temp_clicked()
{
    pv_x_temp = 2.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');
}

void MainWindow::on_pushButton_09h_temp_clicked()
{
    pv_x_temp = 3.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');

}

void MainWindow::on_pushButton_12h_temp_clicked()
{
    pv_x_temp = 4.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');

}

void MainWindow::on_pushButton_15h_temp_clicked()
{
    pv_x_temp = 5.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');

}

void MainWindow::on_pushButton_18h_temp_clicked()
{
    pv_x_temp = 6.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_21h_temp->setStyleSheet("");
    plot('t');

}

void MainWindow::on_pushButton_21h_temp_clicked()
{
    pv_x_temp = 7.0;
    ui->pushButton_00h_temp->setStyleSheet("");
    ui->pushButton_06h_temp->setStyleSheet("");
    ui->pushButton_03h_temp->setStyleSheet("");
    ui->pushButton_09h_temp->setStyleSheet("");
    ui->pushButton_12h_temp->setStyleSheet("");
    ui->pushButton_15h_temp->setStyleSheet("");
    ui->pushButton_18h_temp->setStyleSheet("");
    ui->pushButton_21h_temp->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    plot('t');

}

void MainWindow::on_verticalSlider_temp_sliderReleased()
{
    pv_y_temp = ui->verticalSlider_temp->value() + 0.0;
}

//************************************************************************************************************************************************//
//************************************************************************************************************************************************//
//************************************************************************************************************************************************//

//******************************************************** ONGLET HUMIDITE ************************************************************//

void MainWindow::on_pushButton_00h_humi_clicked()
{
    pv_x_humi = 0.0;
    ui->pushButton_00h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_03h_humi_clicked()
{
    pv_x_humi = 1.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_06h_humi_clicked()
{
    pv_x_humi = 2.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_09h_humi_clicked()
{
    pv_x_humi = 3.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_12h_humi_clicked()
{
    pv_x_humi = 4.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_15h_humi_clicked()
{
    pv_x_humi = 5.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_18h_humi_clicked()
{
    pv_x_humi = 6.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    ui->pushButton_21h_humi->setStyleSheet("");
    plot('h');

}

void MainWindow::on_pushButton_21h_humi_clicked()
{
    pv_x_humi = 7.0;
    ui->pushButton_00h_humi->setStyleSheet("");
    ui->pushButton_06h_humi->setStyleSheet("");
    ui->pushButton_03h_humi->setStyleSheet("");
    ui->pushButton_09h_humi->setStyleSheet("");
    ui->pushButton_12h_humi->setStyleSheet("");
    ui->pushButton_15h_humi->setStyleSheet("");
    ui->pushButton_18h_humi->setStyleSheet("");
    ui->pushButton_21h_humi->setStyleSheet("border-style: inset; border-width: 1px; border-radius: 5px; border-color: #34495E; font: bold;");
    plot('h');

}

void MainWindow::on_verticalSlider_humi_sliderReleased()
{
   pv_y_humi = ui->verticalSlider_humi->value() + 0.0;
}
