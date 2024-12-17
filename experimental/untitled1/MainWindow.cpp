#include <QMessageBox>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    // Create the button, make "this" the parent
    m_button = new QPushButton("My Button", this);
    m_txt= new QTextEdit();
    m_txt->setText("hello world");
    // set size and location of the button
    m_button->setGeometry(QRect(QPoint(100, 100), QSize(500, 500)));


    // Connect button signal to appropriate slot
    connect(m_button, &QPushButton::released, this, &MainWindow::handleButton);
}

void MainWindow::init(keyEnterReceiver* key){
    this->m_key = key;
}

void MainWindow::handleButton()
{
    // change the text
    m_button->setText("Example");
    // resize button
    m_button->resize(100,100);
}

void MainWindow::closeEvent (QCloseEvent *event)
{
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
}