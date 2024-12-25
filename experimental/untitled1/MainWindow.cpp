#include <QMessageBox>
#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow()
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
}

static int i=0;
void MainWindow::refreshTimeout()  {
    //m_button->setText(std::to_string(++i).data());
    if(draw==false) {
        std::cout << "timeout: " << std::endl;
        draw=true;
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    auto type = event->type();
    std::cout << "paintEvent: " << type<< std::endl;
    //pixmap.toImage().setPixel()
    QPainter painter(this);
    QRect  rect;
    if(draw==true) {
        painter.drawText(QPoint(100, 100), std::to_string(++i).c_str());
        //painter.drawLine(QPoint(100+i, 100+i), QPoint(120+i, 120+i));
        bool c=false;
        int l = 10;
        for(int i=0; i< 250; i++) {
            for(int j=0; j< 250; j++) {
                rect =QRect(i, j, l, l);
                if (c==false) {
                    painter.fillRect(rect, QBrush(Qt::green, Qt::SolidPattern));
                    c=true;
                }else {
                    painter.fillRect(rect, QBrush(Qt::yellow, Qt::SolidPattern));
                    c=false;
                }
                painter.drawRect(rect);
                j=j+l;
            }
            i=i+l;
        }
        painter.drawRect( rect =QRect(i+10, i+10, l, l));
        painter.fillRect(rect, QBrush(Qt::red, Qt::SolidPattern));
        painter.drawRect( rect =QRect(i+10, i, l, l));
        painter.fillRect(rect, QBrush(Qt::blue, Qt::SolidPattern));
        painter.drawRect(rect);
        //painter.drawRect(50,50,10,10);
        draw=false;
    }
    //m_painter->setPen(QPen(Qt::black, 12, Qt::DashDotLine, Qt::RoundCap));
    //m_painter->drawLine(0, 0, 200, 200);
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