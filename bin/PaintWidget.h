#pragma once

#include <QWidget>

namespace Ui {class RobustSelectGUI;}

class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    PaintWidget(QString filename, Ui::RobustSelectGUI *ui, QWidget *parent = 0);

    Ui::RobustSelectGUI *ui;

    void loadImage(QString imgFilename);

    QString filename;
    QImage curImg;
    std::vector< std::vector<bool> > fg, bg;
    bool isBackground;
    bool isReady;
    bool scribbling;
    QPoint lastPoint;

    void clear(int strokeType);

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void save();
    void run();
signals:

public slots:

};

