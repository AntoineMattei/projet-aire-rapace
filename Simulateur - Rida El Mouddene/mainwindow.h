#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "rs232.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QWidget>
#include <QTabWidget>
#include <QDialog>
#include <QDebug>

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

    void on_pushButton_automatique_clicked();

    void on_pushButton_manuel_clicked();

    void on_pushButton_vent_clicked();

    void on_pushButton_poids_clicked();

    void on_pushButton_lux_clicked();

    void on_pushButton_temp_clicked();

    void on_pushButton_humi_clicked();

    void on_pushButton_reset_clicked();



    void on_pushButton_00h_vent_clicked();

    void on_pushButton_03h_vent_clicked();

    void on_pushButton_06h_vent_clicked();

    void on_pushButton_09h_vent_clicked();

    void on_pushButton_12h_vent_clicked();

    void on_pushButton_15h_vent_clicked();

    void on_pushButton_18h_vent_clicked();

    void on_pushButton_21h_vent_clicked();



    void addPoint(int,double,double);

    void addPoint_lancement(int,double,double);

    void plot(char);

    void on_verticalSlider_vent_sliderReleased();

    void clearPoint();



    void on_pushButton_connexion_clicked();

    void on_pushButton_appliquer_clicked();

    void on_verticalSlider_poids_sliderReleased();

    void on_pushButton_00h_poids_clicked();

    void on_pushButton_03h_poids_clicked();

    void on_pushButton_06h_poids_clicked();

    void on_pushButton_09h_poids_clicked();

    void on_pushButton_12h_poids_clicked();

    void on_pushButton_15h_poids_clicked();

    void on_pushButton_18h_poids_clicked();

    void on_pushButton_21h_poids_clicked();

    void on_pushButton_00h_lux_clicked();

    void on_pushButton_03h_lux_clicked();

    void on_pushButton_06h_lux_clicked();

    void on_pushButton_09h_lux_clicked();

    void on_pushButton_12h_lux_clicked();

    void on_pushButton_15h_lux_clicked();

    void on_pushButton_18h_lux_clicked();

    void on_pushButton_21h_lux_clicked();

    void on_verticalSlider_lux_sliderReleased();

    void on_pushButton_00h_temp_clicked();

    void on_pushButton_03h_temp_clicked();

    void on_pushButton_06h_temp_clicked();

    void on_pushButton_09h_temp_clicked();

    void on_pushButton_12h_temp_clicked();

    void on_pushButton_15h_temp_clicked();

    void on_pushButton_18h_temp_clicked();

    void on_pushButton_21h_temp_clicked();

    void on_verticalSlider_temp_sliderReleased();

    void on_pushButton_00h_humi_clicked();

    void on_pushButton_03h_humi_clicked();

    void on_pushButton_06h_humi_clicked();

    void on_pushButton_09h_humi_clicked();

    void on_pushButton_12h_humi_clicked();

    void on_pushButton_15h_humi_clicked();

    void on_pushButton_18h_humi_clicked();

    void on_pushButton_21h_humi_clicked();

    void on_verticalSlider_humi_sliderReleased();

private:
    Ui::MainWindow *ui;
    int fd,fd2,nb_click=0;

    QVector<double> qv_x_vent, qv_y_vent;
    double pv_x_vent, pv_y_vent;

    QVector<double> qv_x_poids, qv_y_poids;
    double pv_x_poids, pv_y_poids;

    QVector<double> qv_x_lux, qv_y_lux;
    double pv_x_lux, pv_y_lux;

    QVector<double> qv_x_temp, qv_y_temp;
    double pv_x_temp, pv_y_temp;

    QVector<double> qv_x_humi, qv_y_humi;
    double pv_x_humi, pv_y_humi;
};

#endif // MAINWINDOW_H





