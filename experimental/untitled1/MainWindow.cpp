#include <QMessageBox>
#include "MainWindow.h"

#include <cmath>
#include <iostream>
#include <math.h>
#include <QDesktopWidget>


MainWindow::MainWindow(QApplication *app)
        : QMainWindow()
{
    // Create the button, make "this" the parent
    m_button = new QPushButton("My Button", this);
    m_txt= new QTextEdit();
    m_txt->setText("hello world");
    // set size and location of the button
    m_button->setGeometry(QRect(QPoint(200, 200), QSize(50, 50)));
    auto * myTimer= new QTimer(this);
    myTimer->setInterval(std::chrono::milliseconds(500)); // ms
    connect(myTimer, SIGNAL(timeout()), this, SLOT(refreshTimeout()));
    // Connect button signal to appropriate slot
    connect(m_button, &QPushButton::released, this, &MainWindow::buttonPressEvent);
    myTimer->start();

    adjustSize();
    const QDesktopWidget dw;
    float screenPercent = 0.5;
    int width = 2000;//dw.width()*screenPercent;
    int height= 1500;//dw.height()*screenPercent;
    int plotWidthX = width-m_pixelPlotOffSetX;
    int plotWidthY = height-m_pixelPlotOffSetY;
    std::cout << width << height << std::endl;
    setFixedSize(width,height);
    move(app->desktop()->screen()->rect().center() - rect().center());
    show();
    showNormal();
    m_cmToPixelX = plotWidthX/m_fieldCmSizeY;
    m_cmToPixelY = plotWidthY/m_fieldCmSizeY;
    std::cout << m_cmToPixelX << " " << m_cmToPixelY << std::endl;
}

static int i=0;
void MainWindow::refreshTimeout()  {
    //m_button->setText(std::to_string(++i).data());
    if(draw==false) {
        //std::cout << "timeout: " << std::endl;
        draw=true;
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect  rect;
    bool hasYScaleBeenDrawn= false;
    painter.setFont(QFont("Arial", 6, QFont::Bold));
    if(draw==true) {
        painter.drawText(QPoint(100, 100), std::to_string(++i).c_str());
        //painter.drawLine(QPoint(100+i, 100+i), QPoint(120+i, 120+i));
        bool c=false;
        for(int i=0;i<m_fieldCmSizeX;i++) {
            int xPosPix =getPixPosX(i);
            for(int j=0;j<m_fieldCmSizeY;j++) {
                int yPosPix = j*m_cmToPixelY+m_pixelPlotOffSetY/2;
                rect =QRect(xPosPix,yPosPix, m_cmToPixelX, m_cmToPixelY);
                if (c==false) {
                    painter.fillRect(rect, QBrush(Qt::green, Qt::SolidPattern));
                    c=true;
                }else {
                    painter.fillRect(rect, QBrush(Qt::yellow, Qt::SolidPattern));
                    c=false;
                }
                painter.drawRect(rect);
                if(hasYScaleBeenDrawn==false) {
                    drawYAxisPoint(m_fieldCmSizeY-j, yPosPix,&painter);
                }
            }
            drawXAxisPoint(i,xPosPix,&painter);
            hasYScaleBeenDrawn=true;
        }
        drawXAxisPoint(m_fieldCmSizeX,getPixPosX(m_fieldCmSizeX),&painter);
        draw=false;
    }
}

void MainWindow::drawXAxisPoint(int i,int xPosPix,QPainter * painter) {
    if(i%m_xScale==0 && i!=0) {
        int yPosPix1 = (m_fieldCmSizeY-1)*m_cmToPixelY+m_pixelPlotOffSetY/2;
        int yPosPix2 = yPosPix1+10;
        painter->drawLine(xPosPix, yPosPix1, xPosPix, yPosPix2);
        painter->drawText(QPoint(xPosPix,yPosPix2+10),std::to_string(i).c_str());
    }
}

void MainWindow::drawYAxisPoint(int j,int yPosPix,QPainter * painter) {
    if(j%m_yScale==0) {
        int xPosPix1 = m_cmToPixelX+m_pixelPlotOffSetX/2;
        int xPosPix2 = xPosPix1-10;
        painter->drawLine(xPosPix1, yPosPix, xPosPix2-5, yPosPix);
        painter->drawText(QPoint(xPosPix2-25,yPosPix),std::to_string(j).c_str());
    }
}

int MainWindow::getPixPosX(int i) {
    return i*m_cmToPixelX+m_pixelPlotOffSetX/2;
}

void MainWindow::init(KeyEnterReceiver* key){
    this->m_key = key;
}

void MainWindow::buttonPressEvent() const {
    std::to_string(m_button->winId()).append("\n");
    // change the text
    m_button->setText( std::to_string(m_button->winId()).append("\n").c_str());
    // resize button
    m_button->resize(100,100);
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    /*
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "app",
                                                               tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        this->m_key->closeSerialPort();
        event->accept();
    }
    */

    this->m_key->closeSerialPort();
    event->accept();
}