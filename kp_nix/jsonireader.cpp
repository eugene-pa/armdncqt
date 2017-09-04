#include <bitset>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "jsonireader.h"

// глобальные переменные, извлекаемые из настроечного файла (пристроить в нужное место!)
QString    krugName;
int        krugId;
bool       debug;
QString    debugPort;
QString    testConfig;
bool       fullPolling;
bool       retrans;
QString    stName;
QString    com3;                                                // описание пассивного модема
QString    com4;                                                // описание активного  модема

std::bitset<48> tsModules;                                      // модули ТС (битовый массив)
std::bitset<48> tuModules;                                      // модули ТУ (битовый массив)

bool       apkdk;
QString    comAPKDK;
int        speedAPKDK;

bool       rpcDialog;


JsoniReader::JsoniReader(const char * file, int addr)
{
    this->file = file;
    this->addr = addr;

    std::string s (file);
    wfilename = std::wstring(s.begin(), s.end());

    try
    {
        Read();
    }
    catch (...)
    {
        Log (L"Исключение при чтении настроечного файла " + wfilename);
    }
}


void JsoniReader::Read()
{
    QFile f (file);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = f.readAll();
    f.close();

    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject doc = d.object();
    krugName   = doc.value("круг"        ).toString();
    krugId     = doc.value("id"          ).toInt();
    debug      = doc.value("отладка"     ).toBool();
    debugPort  = doc.value("dbgport"     ).toString();
    testConfig = doc.value("тест"        ).toString();
    retrans    = doc.value("fullpoll"    ).toBool();
    fullPolling= doc.value("ретрансляция").toBool();

    Log (L"Круг " + krugName.toStdWString());

    QJsonArray stations = doc.value("станции").toArray();
    for(int i=0; i<stations.size(); i++)
    {
        QJsonObject st = stations[i].toObject();
        int addr     = st["адрес"].toInt();
        if (addr != this->addr)
            continue;

        stName = st["имя" ].toString();
        Log (L"Станция " + stName.toStdWString());

        com3   = st["COM3"].toString();
        com4   = st["COM4"].toString();

        QJsonObject item = st["АПКДК"].toObject();
        if (!item.isEmpty())
        {
            apkdk = true;
            comAPKDK   = item["порт" ].toString();
            speedAPKDK = item["speed"].toInt();
        }

        QJsonArray mts = st["МТС"].toArray();
        if (!mts.isEmpty())
        {
            for (int i = 0; i < mts.size(); i++)
            {
                int m = mts[i].toInt();
                if (m > 0 && m <= (int)tsModules.size())
                    tsModules.set(m-1);
                else
                    Log (L"Ошибочный номер модуля МТС: " + std::to_wstring(m)); // ошибка описания
            }
        }

        QJsonArray mtu = st["МТУ"].toArray();
        if (!mtu.isEmpty())
        {
            for (int i = 0; i < mtu.size(); i++)
            {
                int m = mtu[i].toInt();
                if (m > 0 && m <= (int)tuModules.size())
                    tuModules.set(m-1);
                else
                    Log (L"Ошибочный номер модуля МТУ: " + std::to_wstring(m)); // ошибка описания
            }
        }
    }
}
