#include <QPainter>
#include <QMouseEvent>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include "PaintWidget.h"
#include "ui_RobustSelectGUI.h"

PaintWidget::PaintWidget(QString filename, Ui::RobustSelectGUI *ui, QWidget *parent)
    : QWidget(parent), ui(ui), filename(filename), isReady(false), scribbling(false), isBackground(true)
{
    loadImage( filename );

    this->setMouseTracking(true);
}

void PaintWidget::loadImage(QString imgFilename)
{
    filename = imgFilename;
    curImg = QImage(filename);

    if( curImg.isNull() )
    {
        isReady = false;
        filename = "";
        bg.clear();
        fg.clear();
        return;
    }

    int w = curImg.width();
    int h = curImg.height();

    bg.resize(h, std::vector<bool>(w, false));
    fg.resize(h, std::vector<bool>(w, false));

    isReady = true;
}

void PaintWidget::clear(int strokeType)
{
    strokeType ? fg.clear() : bg.clear();
    update();
}

void PaintWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.drawImage(0, 0, curImg);

    // Draw strokes
    int w = curImg.width();
    int h = curImg.height();

    // Prepare overlays for background and foreground strokes
    QImage fgOverlay(w,h, QImage::Format_ARGB32);
    fgOverlay.fill( QColor(0,0,0,0) );
    QImage bgOverlay = fgOverlay;

    // Draw the strokes
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            if(bg.size() && bg[y][x]) bgOverlay.setPixel(x,y, qRgb(255,0,0));
            if(fg.size() && fg[y][x]) fgOverlay.setPixel(x,y, qRgb(0,0,225));
        }
    }

    painter.drawImage(0, 0, bgOverlay);
    painter.drawImage(0, 0, fgOverlay);
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}
void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        scribbling = false;
    }
}

void PaintWidget::save()
{
    if(!bg.size() || !fg.size()) {qDebug() << "empty strokes.."; return;}

    QString path = QFileInfo( QCoreApplication::applicationFilePath() ).absolutePath() + "/";
    QString f = QFileInfo(filename).baseName();
    QStringList filenames;
    filenames << (path + QString("%1.bg.dat").arg(f)) << (path + QString("%1.fg.dat").arg(f));

    // Copy image
    QString copyFile = path + f + filename.right(4);
    qDebug() << copyFile;
    QFile::copy(filename, copyFile);

    qDebug() << filenames;

    std::vector<QFile*> file;
    std::vector<QTextStream*> out;

    for(int i = 0; i < 2; i++)
    {
        file.push_back(new QFile(filenames[i]));
        if (!file.back()->open(QIODevice::WriteOnly | QIODevice::Text)) return;
        out.push_back(new QTextStream(file.back()) );
    }

    int w = curImg.width();
    int h = curImg.height();

    (*out.front()) << QString("%1 %2\n").arg(h).arg(w);
    (*out.back()) << QString("%1 %2\n").arg(h).arg(w);

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            if(bg[y][x]) (*out.front()) << QString("%1 %2\n").arg(y).arg(x);
            if(fg[y][x]) (*out.back()) << QString("%1 %2\n").arg(y).arg(x);
        }
    }

    qDeleteAll(out);
    qDeleteAll(file);

    QMessageBox::about(0, "Save", "Stroke data files and original image saved.");
}

void PaintWidget::run()
{
    QString path = QFileInfo( QCoreApplication::applicationFilePath() ).absolutePath() + "/";
    QString f = QFileInfo(filename).baseName();
    QStringList filenames;
    filenames << QString("%1.bg.dat").arg(f) << QString("%1.fg.dat").arg(f);

    QStringList options;
    options << (path + "RobustSelect");
    options << QString("-v %1").arg( 0 ); // UseLUV
    options << QString("-x %1").arg( 1 ); // Downsample
    options << QString("-d %1").arg( 5 ); // Dimensions
    options << QString("-p %1").arg( path ); // Path
    options << QString("-f %1").arg( f + filename.right(4) ); // File
    options << QString("-D %1").arg( "EUC" ); // Dist metric
    options << QString("-n %1").arg( 0.01 ); // Nystrom sampling rate
    options << QString("--patch_size %1").arg( 1 );
    options << QString("-r %1 -s %2").arg( 0.5 ).arg( 5 ); // parameters for dissimilarity
    options << QString("-C %1").arg( filenames.back() ); // Foreground
    options << QString("-C %1").arg( filenames.front() ); // Background
    options << QString("-C %1").arg( "output" ); // Ouput

    options << QString("--crf_sigma %1").arg( 0.1 );
    options << QString("--crf_weight %1").arg( 20 );

    qDebug() << "Running : " << options.join(" ").toLatin1();
    system( options.join(" ").toLatin1() );
}

void horizontalLine(std::vector< std::vector<bool> > & m, bool color,  int xpos, int ypos, int x1){
    for(int x = xpos; x <= x1; ++x){
        if(x < 0 || x > m.front().size() - 1 || ypos < 0 || ypos > m.size() - 1) continue;
        m[ypos][x] = color;
    }
}

void plot4points(std::vector< std::vector<bool> > & buffer, bool color, int cx, int cy, int x, int y){
    horizontalLine(buffer, color, cx - x, cy + y, cx + x);
    if (x != 0 && y != 0) horizontalLine(buffer, color, cx - x, cy - y, cx + x);
}

void circle(std::vector< std::vector<bool> > & buffer, bool color, int cx, int cy, int radius){
    int error = -radius;
    int x = radius;
    int y = 0;

    while (x >= y){
        int lastY = y;
        error += y;
        ++y;
        error += y;
        plot4points(buffer, color, cx, cy, x, lastY);
        if (error >= 0){
            if (x != lastY)
                plot4points(buffer, color, cx, cy, lastY, x);
            error -= x;
            --x;
            error -= x;
        }
    }
}

void DrawLine( float x1, float y1, float x2, float y2, std::vector< std::vector<bool> > & img, int radius = 10, bool color = true )
{
    // Bresenham's line algorithm
    const bool steep = (std::abs(y2 - y1) > std::abs(x2 - x1));
    if(steep){
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    if(x1 > x2){
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    if(!img.size()) return;

    for(int x=(int)x1; x<maxX; x++){
        if(steep)
            circle(img, color, y, x, radius);
        else
            circle(img, color, x, y, radius);
        error -= dy;
        if(error < 0){
            y += ystep;
            error += dx;
        }
    }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
    {
        int prev_y = lastPoint.y();
        int prev_x = lastPoint.x();

        int y = event->pos().y();
        int x = event->pos().x();

        int w = curImg.width();
        int h = curImg.height();

        if(!bg.size()) bg.resize(h, std::vector<bool>(w, false));
        if(!fg.size()) fg.resize(h, std::vector<bool>(w, false));

        if(isBackground) DrawLine(prev_x,prev_y, x, y, bg, ui->brushRadius->value(), true);
        else DrawLine(prev_x,prev_y, x, y, fg, ui->brushRadius->value(), true);
    }

    lastPoint = event->pos();

    update();
}

