#include <QFileDialog>
#include "RobustSelectGUI.h"
#include "ui_RobustSelectGUI.h"

#include "PaintWidget.h"
PaintWidget * pw = NULL;

RobustSelectGUI::RobustSelectGUI(QWidget *parent) : QWidget(parent), ui(new Ui::RobustSelectGUI)
{
    ui->setupUi(this);

    ui->viewerLayout->addWidget( pw = new PaintWidget(QFileDialog::getOpenFileName(0, "Open Image", "", "Image Files (*.png *.jpg *.bmp)"), ui) );

    // Connections
    connect(ui->loadImg, &QPushButton::released, [=](){ pw->loadImage(QFileDialog::getOpenFileName(0, "Open Image", "", "Image Files (*.png *.jpg *.bmp)")); });
    connect(ui->beginBackground, &QPushButton::released, [=](){ pw->isBackground = true; });
    connect(ui->beginForeground, &QPushButton::released, [=](){ pw->isBackground = false; });
    connect(ui->clearBG, &QPushButton::released, [=](){ pw->clear(0); });
    connect(ui->clearFG, &QPushButton::released, [=](){ pw->clear(1); });
    connect(ui->saveFiles, &QPushButton::released, [=](){ pw->save(); });
    connect(ui->run, &QPushButton::released, [=](){ pw->run(); });
}

RobustSelectGUI::~RobustSelectGUI()
{
    delete ui;
}
