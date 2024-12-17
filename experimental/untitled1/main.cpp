//#include <QApplication>
//#include <QPushButton>

#include "MainWindow.h"
#include "keyEnterReceiver.h"
#include <QApplication>
#include <iostream>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDesktopWidget>

/*
QPushButton *button;
QTextEdit *textEdit;
void handleButton();


int main(int argv, char **args)
{
  QApplication app(argv, args);
  textEdit = new QTextEdit;
  QPushButton *quitButton = new QPushButton("&Quit");
  button= new QPushButton("My Button");

  keyEnterReceiver* key = new keyEnterReceiver(textEdit);

  QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
  QObject::connect(button, &QPushButton::released, handleButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(textEdit);
  layout->addWidget(quitButton);
  layout->addWidget(button);

  QWidget window;
  window.installEventFilter(key);
  window.setLayout(layout);

  window.show();

 return app.exec();
}

void handleButton()
{
    // change the text
    button->setText("Example");
    textEdit->setText("yes sir");
    // resize button
    button->resize(100,100);
}
*/



int main(int argc, char *argv[])
{


    QApplication app(argc, argv);
    MainWindow mainWindow;
    keyEnterReceiver* key = new keyEnterReceiver(mainWindow.m_button);
    mainWindow.installEventFilter(key);
    mainWindow.adjustSize();
    QDesktopWidget dw;
    float screenPercent = 0.5;
    int x=dw.width()*screenPercent;
    int y=dw.height()*screenPercent;
    mainWindow.setFixedSize(x,y);
    mainWindow.move(app.desktop()->screen()->rect().center() - mainWindow.rect().center());
    mainWindow.show();
    mainWindow.showNormal();
    return app.exec();
}


/*
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QPushButton button("Hello world!", nullptr);
    button.resize(200, 100);
    button.setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));
    button.show();
    return QApplication::exec();
}
*/