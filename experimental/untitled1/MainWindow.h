//
// Created by robot1 on 7/5/23.
//

#ifndef UNTITLED1_MAINWINDOW_H
#define UNTITLED1_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QCloseEvent>
#include "keyEnterReceiver.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void init(keyEnterReceiver* key);
    QPushButton *m_button;
private slots:
    void handleButton();
    void closeEvent (QCloseEvent *event);
private:
    QTextEdit *m_txt;
    keyEnterReceiver* m_key;
};

#endif //UNTITLED1_MAINWINDOW_H
