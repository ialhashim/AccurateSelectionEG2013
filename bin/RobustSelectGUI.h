#pragma once

#include <QWidget>

namespace Ui {
class RobustSelectGUI;
}

class RobustSelectGUI : public QWidget
{
    Q_OBJECT

public:
    explicit RobustSelectGUI(QWidget *parent = 0);
    ~RobustSelectGUI();

private:
    Ui::RobustSelectGUI *ui;
};

