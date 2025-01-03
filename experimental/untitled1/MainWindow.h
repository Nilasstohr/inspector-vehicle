//
// Created by robot1 on 7/5/23.
//

#ifndef UNTITLED1_MAINWINDOW_H
#define UNTITLED1_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QCloseEvent>
#include "KeyEnterReceiver.h"
#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QApplication *app);
    void init(KeyEnterReceiver* key);
    QPushButton *m_button;
private slots:
    void closeEvent (QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;



    void buttonPressEvent() const;



    void refreshTimeout();
private:
    void drawXAxisPoint(int i, int xPosPix, QPainter *painter);
    void drawYAxisPoint(int j, int yPosPix, QPainter *painter);
    int getPixPosX(int i);

    QTextEdit *m_txt;
    KeyEnterReceiver* m_key;
    bool draw=false;
    int m_fieldCmSizeX=250;
    int m_fieldCmSizeY=250;
    int m_pixelPlotOffSetX=250;
    int m_pixelPlotOffSetY=250;
    int m_xScale=5;
    int m_yScale=5;
    int m_cmToPixelX;
    int m_cmToPixelY;
    int m_origoX;
    int m_origoY;


};

#endif //UNTITLED1_MAINWINDOW_H
