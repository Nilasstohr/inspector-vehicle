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
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    void init(KeyEnterReceiver* key);
    QPushButton *m_button;
private slots:
    void closeEvent (QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void buttonPressEvent() const;
    void refreshTimeout();
private:
    QTextEdit *m_txt;
    KeyEnterReceiver* m_key;
    bool draw=false;
};

#endif //UNTITLED1_MAINWINDOW_H
