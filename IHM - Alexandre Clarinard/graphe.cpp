#include "graphe.h"
#include "ui_graphe.h"

graphe::graphe(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphe)
{
    ui->setupUi(this);

   /* QLineSeries * series = new QLineSeries();

    series->append(0,6);
    series->append(2,4);
    series->append(3,8);
    series->append(7,4);
    series->append(10,5);

    *series << QPointF(11,1) << QPointF(13,3) << QPointF(17,6) << QPointF(18,3) << QPointF(20,2);

    Qchart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Line Chart Example");

    QChartView *chartView = new QChartView(chart);*/
}

graphe::~graphe()
{
    delete ui;
}
