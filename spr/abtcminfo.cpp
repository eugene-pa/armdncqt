#include "abtcminfo.h"

AbtcmInfo::AbtcmInfo(UINT addr, UINT length, UINT offset)
{
    this->addr   = addr  ;
    this->length = length;
    this->offset = offset;
    count = 0;
}

AbtcmInfo::~AbtcmInfo()
{

}

// разбор строки конфигурации АБТЦМ с учетом многоканальности
// АБТЦМ(БУ12:27,БУ13:26)
// Для контроля передачи по каждому блоку АБТЦМ используются байтовые циклические счетчики, дописываемые после объединенного блока данных
// Счетчики располагаются в порядке следования блоков следом за объединенными данными.
// Наличие счетчиков определяется длиной общего блока данных: она должна быть больше сумм длин блоков ровно на число счетчиков
// Если счетчик застыл - считаем, что по блоку данные не передаются, данные обнуляются и
// устанавливается вртуальный сигнал АБТЦМ1.ОШБ
bool AbtcmInfo::Parse(std::vector<AbtcmInfo*>& list, QString& config, Logger& logger)
{
    bool ret = false;
    // выделяем опцию полностью
    QString option = QRegularExpression("[Аа][Бб][Тт][Цц][Мм][^ ]*").match(config).captured();
    if (option.length() != 0 )
    {
        ret = true;
        // разбор расширенного описания при его наличии
        if (option.length() > QString("АБТЦМ").length())
        {
            QRegularExpression regexAbtcm1("[Аа][Бб][Тт][Цц][Мм]\\([БбУу:\\d,]+\\)");  // проверка синтаксиса
            QRegularExpressionMatch match1 = regexAbtcm1.match(config);
            if (match1.hasMatch())
            {
                // обработка блоков формата "БУXX:YY"
                int pos = 0;
                QRegularExpression regexAbtcm2("[Бб][Уу]\\d+:\\d+");
                QRegularExpressionMatch match2;
                while ((match2 = regexAbtcm2.match(match1.captured(), pos)).hasMatch())
                {
                    // обработка блока - выделение адреса блока и длины данных
                    QString s = match2.captured();
                    QRegularExpressionMatch match3 = QRegularExpression("\\d+").match(s);
                    UINT addr   = match3.captured().toUInt();
                    UINT length = QRegularExpression("\\d+").match(s,match3.capturedEnd()).captured().toUInt();
                    list.push_back(new AbtcmInfo(addr, length, sumLength(list)));

                    pos = match2.capturedEnd();
                }
            }
            else
                logger.log(QString("Ошибка синтакиса опции: '%1'").arg(option));
        }
    }
    return ret;
}

// подсчет общей длины данных по всем блокам
UINT AbtcmInfo::sumLength(std::vector<AbtcmInfo*>& list)
{
    UINT length = 0;
    foreach (AbtcmInfo* abtcm, list)
    {
        length += abtcm->length;
    }
    return length;
}

