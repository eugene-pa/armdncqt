#include <QCoreApplication>
#include <QDebug>
#include <../common/rsproxy.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Example use QSerialPortInfo
//  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
//        qDebug() << "Name : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();

        // Example use QSerialPort
        QSerialPort serial;
        //serial.setPort(info);
        serial.setPortName("COM3");
        serial.setBaudRate(QSerialPort::Baud9600);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity  (QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);
        if (serial.open(QIODevice::ReadWrite))
        {
//            if (serial.waitForReadyRead(1000))
//            {
//                QByteArray data;
//                data = serial.readAll();                            // прием первой пачки данных
//            }
            QByteArray bt("12345");
            serial.write(bt,5);
            serial.waitForBytesWritten(5000);

            while (true)
            {
                if (serial.waitForReadyRead(100))
                {
                    bt = serial.readAll();
                    qDebug() << "Прочитано " << bt.length() << " байт";
                }
            }
            serial.close();
        }
        else
            qDebug() << "Error open";
    }

    //RsProxy rs ("COM3,9600,N,8,1");
    //getchar();

    return a.exec();
}
