#include "ecmpkinfo.h"

ecMpkInfo::ecMpkInfo(UINT length, UINT offset)
{
    tsGroups = length;                                      // число групп ТС
    tsOffset = offset;                                      // смещение группы ТС
}

ecMpkInfo::~ecMpkInfo()
{

}

// ЭЦ-МПК может содержать больше 1 блока на станции на 1 КП
// В этом случае ТС передаются от КП одним блоком, кодируются сквозной нумерацией с учетом
// группировки по 8 импульсов в группе. ТУ кодируются сквозным образом. ТУ для второго блока
// имеет смещени, опионируемое в КП, например 1000.
// При описании ТС номер блока, группы и импульса записываются в поля модуль, колодка, контакт
// Чтобы контролировать корректность нумерации при числе блоков > 1, надо знать сколько групп в модуле
// Для этого спецификация ЭЦ-МПК имеет возможность перечисления числа групп
// Читаем спецификацию блоков ЭЦ МПК.
// Формат: ЭЦ-МПК[(ГГ[,ГГ])]   (без пробелов, разделитель групп разных блоков - запятая)
//			ГГ     - число групп
// ЭЦ-МПК(186,188) - Кинель-1
bool ecMpkInfo::Parse(std::vector<ecMpkInfo*>& list, QString& config,Logger& logger)
{
    bool ret = false;

    // выделяем опцию полностью
    QString option = QRegularExpression("[Ээ][Цц]-[Мм][Пп][Кк][^ ]*").match(config).captured();
    if (option.length() != 0 )
    {
        ret = true;
        // разбор расширенного описания при его наличии
        if (option.length() > QString("ЭЦ-МПК").length())
        {
            // проверка синтаксиса
            QRegularExpressionMatch match1 = QRegularExpression ("[Ээ][Цц]-[Мм][Пп][Кк]\\([\\d,]+\\)").match(config);
            if (match1.hasMatch())
            {
                // обработка блоков формата "XX"
                int pos = 0;
                QRegularExpressionMatch match2;
                while ((match2 = QRegularExpression ("\\d+").match(match1.captured(), pos)).hasMatch())
                {
                    UINT length = match2.captured().toUInt();           // обработка блока - выжеление длины данных
                    list.push_back(new ecMpkInfo(length, sumLength(list)));
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
UINT ecMpkInfo::sumLength(std::vector<ecMpkInfo*>& list)
{
    UINT length = 0;
    foreach (ecMpkInfo* rpc, list)
    {
        length += rpc->tsGroups;
    }
    return length;
}
