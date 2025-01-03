//#include <QApplication>
//#include <QPushButton>

#include "MainWindow.h"
#include "KeyEnterReceiver.h"
#include <QApplication>
#include <iostream>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <thread>

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
    MainWindow mainWindow(&app);
    auto* key = new KeyEnterReceiver(mainWindow.m_button);
    mainWindow.installEventFilter(key);
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