//
// Created by robot1 on 7/6/23.
//

#ifndef UNTITLED1_KEYENTERRECEIVER_H
#define UNTITLED1_KEYENTERRECEIVER_H


#include <QObject>
#include <QTextEdit>
#include <QPushButton>

class KeyEnterReceiver : public QObject{

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    KeyEnterReceiver(QPushButton *button);
    void closeSerialPort();
private:
    void writeSerialChar(char c);
    char read_buf [256];
    int m_serial_id;
    QPushButton *button;

};



#endif //UNTITLED1_KEYENTERRECEIVER_H
