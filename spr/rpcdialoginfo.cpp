#include "rpcdialoginfo.h"

rpcDialogInfo::rpcDialogInfo(UINT addrBmEx, UINT tsGroups, UINT tsOffset)
{
    this->addrBmEx = addrBmEx;                              // адрес БМ, включая ЕСР, имеющие место быть; заполняется при чтении INI файла
    this->tsGroups = tsGroups;                              // число групп ТС
    this->tsOffset = tsOffset;                              // смещение группы ТС
}

rpcDialogInfo::~rpcDialogInfo()
{

}


// РПЦ Диалог может содержать более 1 БМ, описываются БМ перечислением
// Формат:  РПЦДИАЛОГ(БМАААААА:ГГ[,БМАААААА:ГГ])
//          АААААА - адрес БМ
//			ГГ     - число групп
// Кашпир:  РПЦДИАЛОГ(БМ06361101:64,БМ06361102:64)
bool rpcDialogInfo::Parse(std::vector<rpcDialogInfo*>& list, QString& config,Logger& logger)
{
    bool ret = false;

    // выделяем опцию полностью
    QString option = QRegularExpression("[Рр][Пп][Цц][Дд][Ии][Аа][Лл][Оо][Гг][^ ]*").match(config).captured();
    if (option.length() != 0 )
    {
        ret = true;
        // разбор расширенного описания при его наличии
        if (option.length() > QString("РПЦДИАЛОГ").length())
        {
            // проверка синтаксиса
            QRegularExpressionMatch match1 = QRegularExpression ("[Рр][Пп][Цц][Дд][Ии][Аа][Лл][Оо][Гг]\\([БбМм:\\d,]+\\)").match(config);
            if (match1.hasMatch())
            {
                // обработка блоков формата "БМXX:YY"
                int pos = 0;
                QRegularExpressionMatch match2;
                while ((match2 = QRegularExpression ("[Бб][Мм]\\d+:\\d+").match(match1.captured(), pos)).hasMatch())
                {
                    // обработка блока - выделение адреса блока и длины данных
                    QString s = match2.captured();
                    QRegularExpressionMatch match3 = QRegularExpression("\\d+").match(s);
                    bool ret;
                    UINT addr   = match3.captured().toUInt(&ret, 16);
                    UINT length = QRegularExpression("\\d+").match(s,match3.capturedEnd()).captured().toUInt();

                    list.push_back(new rpcDialogInfo(addr, length, sumLength(list)));

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
UINT rpcDialogInfo::sumLength(std::vector<rpcDialogInfo*>& list)
{
    UINT length = 0;
    foreach (rpcDialogInfo* rpc, list)
    {
        length += rpc->tsGroups;
    }
    return length;
}



/*
if ((indx = s.Find("РПЦДИАЛОГ")) >=0)
{
    bRpcDialog	= true;

//		CStringEx sTmp(s);
    CStringEx sTmp(CStringEx(s.Mid(indx)).GetToken());	// 2012.12.26. безопасное извлечение лексемы

    CStringEx sRpc = sTmp.GetQuotedToken('(',')');
    sRpc.Replace (_T(','),_T(' '));						// заменяем запятые на пробелы

    int nOffset=0;										// подсчет групп
    CString sTok;
    while ((sTok = sRpc.GetToken()).GetLength())		// читаем лексему
    {
        DWORD addr=0, nGroups=0;
        if (sscanf(sTok,_T("БМ%x:%d]"),&addr, &nGroups)==2)// разбор лексемы типа:  БМ1:24
        {
            arRpcAddrBmEx.Add(addr);					// полные адреса БМ, включая ЕСР, имеющие место быть; заполняется при чтении INI файла
            arRpcTsGroups.Add((WORD)nGroups);			// массив числа групп ТС в порядке перечисения БМ
            arRpcTsOffset.Add(nOffset);					// массив смещений групп ТС в порядке перечисения БМ

            nOffset += nGroups;
            OffsetDk += nGroups;
        }
        else
            AfxMessageBox(_T("Error RPC config line"));
    }
    nAllGroups  = nOffset;

    // 2008.11.26. Регламентная длина блока диагностики.
    // 4 байта - наши и по 6 на каждый шкаф
    nAllDiagLen = 4 + arRpcAddrBmEx.GetSize() * 6;

    OffsetDk += nAllDiagLen;
}

*/
