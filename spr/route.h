#ifndef ROUTE_H
#define ROUTE_H

#include "station.h"

class Route : public SprBase
{
    friend class DStDataFromMonitor;                        // для формирования и извлечения информации в потоке ТС
    friend class DDataFromMonitor;

public:

    enum ROTE_TYPE						// Типы маршрутов (поле Type) - СМ.ТАБЛИЦУ TypeOfRoute БД
    {
        ROUT_I_SINGLE	= 0,			// П	прием
        ROUT_O_SINGLE	= 1,			// О	отправление
        ROUT_I_COMPLEX	= 2,			// СП	составной прием
        ROUT_O_COMPLEX	= 3,			// СО	составное отправление
        ROUT_IO_THROUGH	= 4,			// С	составной сквозной (прием,отправление)
        ROUT_MANEVR    	= 5,			// М	маневровый
        ROUT_I_PRD		= 6,			// ПРДП	передача (при этом прием на осн.путь)
        ROUT_O_PRD		= 7,			// ПРДО	передача (при этом отпрвл.с осн.пути)
        ROUT_PRD		= 8,			// ПРД	передача с осн.на осн.путь
        ROUT_M_COMLEX	= 9,			// маневровый составной
        ROUT_INOUTRETIME= 10,           // сквозной Ретаймовский, выдается отдельной ТУ, а не по одиночке
    };

    enum ROUTE_STS
    {
        RQSET		=1,					// переводим стрелки "ж"
        RQOPEN		=2,					// стрелки переведены, открываем сигнал "ж"
        WAIT_CLOSE	=3,					// все ОК, контролируем изменение состояния "з"
        WAIT_RZMK	=4,					// ждем размыкания ИЛИ АВТОДЕЙСТВИЯ
        WAIT_OTHER	=5,					// ожидание установки другого маршрута (освобождения наборной группы)
//		DEPOSIT		=6,					// ожидание выполнения условий для сборки (накопление)
    };


    // открытые статические функции
    static Route * GetById(int id);                         // получить справочник по уникальному ключу(номеру) маршрута
    static Route * GetByNo(int no, Station* st);            // получить справочник по уникальному ключу(номеру) маршрута
    static void DoRoutes(Station* st);                      // обработка всех маршрутов
    static bool ReadBd (QString& dbpath, Logger& logger);   // чтение БД

    Route(QSqlQuery& query, Logger& logger);
    ~Route();

    // открытые функции
    QVector<LinkedStrl*> GetStrlList() { return listStrl; } // получить массив стрелок с указанием заданного положения
    bool StsRqSet() { return sts == RQSET; }                // проверка состояния Установка маршрута
    bool StsOn   () { return sts == WAIT_CLOSE || sts == WAIT_RZMK; }   // проверка замкнутости маршрута
    ROUTE_STS Sts() { return sts; }                         // состояние маршрута

    bool IsManevr() { return type == ROUT_MANEVR; }
    bool IsComplex()
    {
        return     type == ROUT_I_COMPLEX
                || type == ROUT_O_COMPLEX
                || type == ROUT_IO_THROUGH
                || type == ROUT_M_COMLEX
                || listRoutes.length() > 0;
    }

private:
    static QHash <int, Route *> routes;                     // маршруты, индексированные по индексу ТС

    // Константные данные
    int     id;            		                            // номеp (код) маршрута, совпадающий с индексом
    CString Text;				                            // Описание (из БД, пишется "вручную", более осмысленное, чем HelpText)
    bool	dir;				                            // напpавление маpшpута		0-чет, 1-нечет

//    WORD	IndRc;				                            // 1-ый индекс маpш.(РЦ)
//    WORD	IndStrl;			                            // 2-ой инд. маpш.(cтpелки)
//    WORD	IndCross;			                            // 3-й индекс - враждебные
    QString	path;                                           // Имя пути в маpшpуте
    QString	pathFrom;                                       // Имя пути в маpшpуте ОТКУДА для передачи
//  int     park;				                            // номеp парка,если есть, иначе 0
    ROTE_TYPE type;				                            // тип маршрута (см.выше)
                                                            // 0 - прием									ROUT_I_SINGLE
                                                            // 1 - отправление								ROUT_O_SINGLE
                                                            // 2 - составной прием							ROUT_I_COMPLEX
                                                            // 3 - составное отправление					ROUT_O_COMPLEX
                                                            // 4 - составной сквозной (прием,отправление)	ROUT_IO_THROUGH
                                                            // 5 - маневровый								ROUT_MANEVR
                                                            // 6 - передача (при этом прием на осн.путь)	ROUT_I_PRD
                                                            // 7 - передача (при этом отпрвл.с осн.пути)	WORD ROUT_O_PRD
                                                            // 8 - передача с осн.на осн.путь				WORD ROUT_PRD
                                                            // "П","О","М","СП","СО","С","ПРД","СМ"

    bool	bInOutRetime;		                            // составной ПРИЕМ+ОТПР, задаваемый отдельной командой ТУ
    int     noSvtf;				                            // номеp светофоpа из БД, ограждающего начало маршрута (0 для составных)
    Svtf *  svtfBeg;                                        // указатель на класс, описывающий ограничивающий начало маршрута светофор; если не описан - NULL
    Svtf *  svtfEnd;                                        // указатель на класс, описывающий ограничивающий конец маршрута светофор; если не описан - NULL
    Rc   *  nextRc;                                         // указатель на класс, описывающий следующую РЦ за маршрутом

    bool	fBegEnd;                                        // Признак маршрутного набора - задание НАЧАЛО КОНЕЦ
    bool	bSetStrlBefore;		                            // Признак необходимости перевести стрелки перед дачей команды

    bool	bAutoPeregonPending;                            // Идет разворот перегона при установке маршрута
    QDateTime tPeregonOk;			                        // Засечка времени, когда перегон развернулся; после этого надо выдержать N сек. перед установкой маршрута

    CString tuSet;                                          // ЗАДАНИЕ МАРШРУТА (Список ТУ через пробел; если он пустой - переводим стрелки и открываем сигнал)
    QVector <Tu*> tuSetList;                                // список ТУ установки маршрута

    CString	tuCancelName;			                        // ОТМЕНА МАРШРУТА (Список ТУ через пробел; если он пустой - закрываем сигнал)
    QVector <Tu*> tuCancelList;                             // список ТУ установки маршрута

    CString cancelModeTs;		                            // ТС для контроля зависания режима набора маршрута
    CString cancelModeTu;		                            // ТУ для отмены режима набора маршрута в случае зависания

    QString	zmkName;			                            // код имени обобщенного сигнала замыкания
    Ts *    zmkTs;                                          // справочник обобщенного сигнала замыкания

    QVector <Route *     > listRoutes;                      // список составных маршрутов (пустой для элементарных)
    QVector <Rc *        > listRc;                          // список РЦ  в маpшpуте
    QVector <LinkedStrl* > listStrl;                        // список стpелок в маpшpуте
    QVector <Route *     > listCrossRoutes;                 // список враждебных маршрутов

    int     idType;				                            // код вида маршрута (ключ для RouteType)
    CString	tuText;				                            // имя ТУ (для ручного ввода)
    CString	helpText;			                            // расшифровка (генерим автоматически по типу)

    bool	wrongWay;			                            // 2015.02.03. Неправильный путь (в имени встечается текст "неправ")
                                                            // =0 при посекционном замыкании
//	WORD	NoPrompt;			                            // код текст. подсказки

    // Изменяемые данные ===========================================================
    bool	setSignal;                                      // пpизнак С СИГНАЛОМ/БЕЗ
    ROUTE_STS sts;				                            // состояние маршрута
    ROUTE_STS stsPrv;				                        // состояние маршрута на предыдущем шаге
//  int		Repeat;				                            // счетчик накоплений маршрута
//  bool	bForDeposit;		                            // признак строки меню: Накопление маршрута:
//  short	NoRcByBy1;			                            // номер РЦ удаления для контроля перед выдачей накопленных маршрутов отправления

    QDateTime timeSet;			                            // вpемя выдачи команды на сбоpку/отм
//  bool	bSecondAttempt;		                            // вторая попытка

    // === Контроль соответствия зависимостей ===
    enum { TIME_TO_CLOSE_SVTF = 15, };
    bool    alarmNonClosed;	                                // признак выдачи сообщения о неперекрытии сигнала
    bool    alarmSerialError;	                            // признак выдачи сообщения об ошибке логики проследования РЦ
    bool	alarmOverflow;		                            // признак инициализации контроля проезда запрещающего сигнала светофора
    QDateTime timeBusyRc;			                        // время занятия первой РЦ в маршруте
    int		i1,i2;				                            // индекс (1...N) первой и последней занятых РЦ в маршрутах
    int     timeCloseSvtf;		                            // максим.время перекрытия сигнала
    QDateTime timeSvtfEndClosed;	                        // время перекрытия светофора, описывающего ограничивающий конец маршрута для выполнения функции контроля проезда запрещающего
    bool	svtfEndStatus;		                            // состояние светофора, описывающего ограничивающий конец маршрута для выполнения функции контроля проезда запрещающего
    CString	prgTsExpr;			                            // выражение вычисления направления и свободности перегона для определения необходимости его разворота
    CString	PrgTu;				                            // команды разворота перегона
    QVector <Tu*> tuPeregon;                                // список ТУ разворота перегона
    bool	informedPrg;                                    // флаг выдачи пpедупpеждения о пpосpочке контроля разворота перегона

//	bool	bCheckForArmDsp;                                // контролируем в АРМ ДСП ЦИТТРАНС

    // переменные для анализа непрохождения маршрута по враждебностям ввожу новые
    bool problemCross;
    bool problemStrl;
    bool problemSvtf;
    bool problemRc;
    bool problemPrg;
};

#endif // ROUTE_H
