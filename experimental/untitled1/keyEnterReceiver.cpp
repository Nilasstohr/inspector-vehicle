//
// Created by robot1 on 7/6/23.
//

#include <QKeyEvent>
#include "keyEnterReceiver.h"
#include <iostream>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), closeSerialPort()

keyEnterReceiver::keyEnterReceiver(QPushButton *button) {
    this->button = button;
    this->m_serial_id = open("/dev/ttyACM0", O_RDWR);
    char read_buf [256];
    //int n = read(m_serial_id, &read_buf, sizeof(read_buf));
    //write(m_serial_id,"5\n",1);
    int n = read(m_serial_id, &read_buf, sizeof(read_buf));
    //writeSerialChar('f');
    //n = read(m_serial_id, &read_buf, sizeof(read_buf));
    //closeSerialPort(m_serial_id);
}

void keyEnterReceiver::writeSerialChar(char c) {
    char write_buf [2];
    write_buf[0]=c;
    write_buf[1]='\n';
    write(m_serial_id, write_buf, 2);
}

void keyEnterReceiver::closeSerialPort() {
        close(m_serial_id);
}

bool keyEnterReceiver::eventFilter(QObject* obj, QEvent* event)
{
    //pEdiyt->setText("event");
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        //button->setText(key->text());
        button->setText("");
        if( key->key()==Qt::Key_Y){
            writeSerialChar('y');
        }else if( key->key()==Qt::Key_H){
            writeSerialChar('h');
        }else if( key->key()==Qt::Key_G){
            writeSerialChar('g');
        }else if( key->key()==Qt::Key_J){
            writeSerialChar('j');
        }else if( key->key()==Qt::Key_S){
             writeSerialChar('s');
        }else if( key->key()==Qt::Key_0){
            writeSerialChar('0');
        }else if( key->key()==Qt::Key_1){
            writeSerialChar('1');
        }else if( key->key()==Qt::Key_5){
            writeSerialChar('1');
        }else if( key->key()==Qt::Key_D){
            writeSerialChar('d');
        }else if( key->key()==Qt::Key_F){
            writeSerialChar('f');
        }
        int n = read(m_serial_id, &read_buf, sizeof(read_buf));
        button->setText(read_buf);
        return true;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
}
