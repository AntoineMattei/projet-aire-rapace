#ifndef GRAPHE_H
#define GRAPHE_H

#include <QWidget>

#include <QChartView>
#include <QLineSeries>

namespace Ui {

class graphe;
}

class graphe : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief graphe
     * @param parent
     */
    explicit graphe(QWidget *parent = nullptr);
    ~graphe();

private:
    Ui::graphe *ui;
};

#endif // GRAPHE_H
