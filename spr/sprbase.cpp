#include "sprbase.h"
#include "station.h"
#include "ts.h"

QString SprBase::buf;                                       // статический буфер для формирования сообщений
std::vector<std::string> SprBase::BaseNames;
/* =                                                        // не везде проходит инициализация!
{
    "?", "ТС", "ТУ", "РЦ", "СВТФ", "СТРЛ",
};*/

SprBase::SprBase()
{
    krugno = 0;                                             // номер круга
    no   = 0;                                               // числовой номер/идентификаторр/ключ объекта
    nost = 0;                                               // номер станции
    norc = 0;
    nosvtf = 0;
    nostrl = 0;
    disabled= false;
    st      = nullptr;                                      // указатель на класс станции
    krug    = nullptr;                                      // указатель на класс круга
    enabled = true;                                         // включен
    tag     = nullptr;
    basetype = BaseUnknown;
    unitype = Unknown;

    BaseNames.push_back("?"    );
    BaseNames.push_back("ТС"   );                           // 1 - BaseTs
    BaseNames.push_back("ТУ"   );                           // 2 - BaseTu
    BaseNames.push_back("ОТУ"  );                           // 3 - BaseOtu
    BaseNames.push_back("РЦ"   );                           // 4 - BaseRc
    BaseNames.push_back("СВТФ" );                           // 5 - BaseSvtf
    BaseNames.push_back("СТРЛ" );                           // 6 - BaseStrl
}

SprBase::~SprBase()
{

}


// получить имя станции если есть справочник, или строку "#номер_станции"
QString& SprBase::StationName()
{
    if (st != nullptr )
        return st->Name();
    return buf = (QString("#%1").arg(nost));
}

QString& SprBase::NameEx()
{
    return buf = no ? QString("%1 %2='%3'(#%4)").arg(StMessage()).arg(GetBaseName()).arg(name).arg(no) :
                      QString("%1 %2='%3'").arg(StMessage()).arg(GetBaseName()).arg(name);
}
QString SprBase::GetBaseName()
{
    return QString::fromStdString(basetype > BaseUnknown && basetype < BaseMax ? BaseNames[basetype] : BaseNames[BaseUnknown]);
}

//--------------------------------------------------------------------------------------------------------------
// глобальная функция получения объекта БД по имени БД
// если БД уже зарегистрирована в пуле БД, возвращаем ее из пула
// иначе - добавляем новую именованную БД в пул
QSqlDatabase GetSqliteBd(QString& dbpath)
{
    if (QSqlDatabase::contains(dbpath))
        return QSqlDatabase::database(dbpath);
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", dbpath);
    dbSql.setDatabaseName(dbpath);
    return dbSql;
}


// безопасные преобразования QByteArray - QBitArray
// можно сделать перегруженные функции:
// - с предачей массива назначения по ссылке/указателю
// - с предачей массива byte[]

// Convert from QBitArray to QByteArray
QByteArray bitsToBytes(QBitArray bits)
{
    QByteArray bytes;
    bytes.resize(bits.count()/8+1);
    bytes.fill(0);
    for(int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
    return bytes;
}

// Convert from QByteArray to QBitArray
QBitArray bytesToBits(QByteArray bytes)
{
    QBitArray bits(bytes.count()*8);
    for(int i=0; i<bytes.count(); ++i)
        for(int b=0; b<8; ++b)
            bits.setBit(i*8+b, bytes.at(i)&(1<<b));
    return bits;
}
