#include "rasrs.h"

// 0. Добавляем в проект поддежку QSerialPort: QT += serialport
// 1. Создаем класс, производный от BlockingRs: class RasRs : public BlockingRs
// 2. Переопределяем виртуальную функцию mainLoop() в соответствии с шаблоном: while (!quit) { передача/прием }
// 3. Добавляем функции формирования и разбора пакетов
// 4. Встраиваем класс в родителя в соответствии с шаблоном:
//
//      4.1. создаем класс
//      rasRs = new RasRs(this, '1', 600);
//
//      4.2. привязываем слоты и сигналы
//      connect(rasRs, SIGNAL(dataready(QByteArray)), this, SLOT(dataready(QByteArray)));   // данные приняты
//      connect(rasRs, SIGNAL(timeout()), this, SLOT(timeout()));                           // нет данных
//      connect(rasRs, SIGNAL(error(int)), this, SLOT(error(int)));                         // ошибка
//      connect(rasRs, SIGNAL(started() ), this, SLOT(rsStarted()));                        // поток запущен
//      connect(this, SIGNAL(exit()), rasRs, SLOT(exit()));                                 // завершение работы потока
//
//      4.3. определяем задержки и стартуем поток
//      COMMTIMEOUTS tm = { 10, 1, 3000, 1, 250 };
//      rasRs->startRs("COM3,57600,N,8,1", tm);
// 5. В момент завершения программу генеим сигнал exit() и удаляем объект
//
// ВАЖНО:
// - ПРИНЦИП: - таймаут больше допустимого мебайтового разрыва - окончание приема пакета
// Этот подход имеет определенные недостатки:
// - хорошо работает в полудуплексном режиме: запрос - ответ, или передача - прием
// - если таймауты между пакетами ненамного отличаются от тайм-аутов между байтами может возникнуть рассинхронизация
// - в случае буыерирования входных данных нескольких пакетов они скорее всего будут потеряны
// В сложных случаях может быть переопределена функция readData(*pSerial), где можно реализовать более сложные элементы форматного контроля
// Можно сделать реализацию readData, умеющую вычленять длину пакета и ограничивающую прием данных этой длиной

RasRs::RasRs(QObject *parent, BYTE marker, int maxlength) : BlockingRs(parent, marker, maxlength)
{
    seans = 0;
}

RasRs::~RasRs()
{

}

// основной цикл
void RasRs::mainLoop()
{
    while (!quit)
    {
        pSerial->write(prepareData());                      // запрос

        QByteArray data = readData(*pSerial);               // прием отклика
        //  ....                                            // обработка

        emit dataready(data);                               // уведомление
    }
}

//bool RasRs::open(QString config)
//{
//}

// формирование пакета для передачи
QByteArray RasRs::prepareData()
{
    QByteArray data;
    data.append(01);                                        // маркер
    data.append(03);                                        // длина
    data.append((char)0);
    data.append(01);                                        // приемник
    data.append((char)0);                                   // источник
    data.append(seans++);                                   // сеанс
    addCRC(data);                                           // CRC
    data.append(04);                                        // маркер конца пакета
    return data;
}
