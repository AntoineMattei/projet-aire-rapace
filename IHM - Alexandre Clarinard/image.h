#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>

namespace Ui {
class Image;
}

class Image : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Image
     * @param parent
     */
    explicit Image(QWidget *parent = nullptr);
    ~Image();

private:
    Ui::Image *ui;
};

#endif // IMAGE_H
