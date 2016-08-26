#ifndef ROUTE_H
#define ROUTE_H

#include "station.h"

class Route : public SprBase
{
    friend class DStDataFromMonitor;                        // для формирования и извлечения информации в потоке ТС
    friend class DDataFromMonitor;
    friend class DlgRoutes;
    friend class Station;

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
        PASSIVE     =0,                 // неактивное состояние
        RQSET		=1,					// переводим стрелки "ж"
        RQOPEN		=2,					// стрелки переведены, открываем сигнал "ж"
        WAIT_CLOSE	=3,					// все ОК, контролируем изменение состояния "з"
        WAIT_RZMK	=4,					// ждем размыкания ИЛИ АВТОДЕЙСТВИЯ
        WAIT_OTHER	=5,					// ожидание установки другого маршрута (освобождения наборной группы)
//		DEPOSIT		=6,					// ожидание выполнения условий для сборки (накопление)
    };


    // открытые статические функции
    static Route * GetById(int id, class KrugInfo * krug = nullptr);  // получить справочник по уникальному ключу(номеру) маршрута
    static Route * GetByNo(int no, Station* st);            // получить справочник по уникальному ключу(номеру) маршрута
    static void DoRoutes(Station* st);                      // обработка всех маршрутов
    static bool ReadBd (QString&, class KrugInfo*, Logger&);// чтение БД

    Route(QSqlQuery&, class KrugInfo*, Logger&);            // конструктор
    ~Route();

    // открытые функции
    bool IsOpen();                                          // Проверка открытого состояния ограждающего светофора
    std::vector<LinkedStrl*> GetStrlList() { return listStrl; } // получить массив стрелок с указанием заданного положения
    bool StsRqSet() { return sts == RQSET; }                // проверка состояния Установка маршрута
    bool StsOn   () { return sts == WAIT_CLOSE || sts == WAIT_RZMK; }   // проверка замкнутости маршрута
    ROUTE_STS Sts() { return sts; }                         // состояние маршрута

    int RelNo   () { return relNo; }                        // номер на станции
    QString Text() { return text;  }                        // описание
    QString TypeName() { return typeName; }                 // тип
    class Svtf * SvtfBeg() { return svtfBeg; }              // светофор, ограждающий начало маршрута

    bool IsManevr() { return type == ROUT_MANEVR; }
    bool IsComplex()
    {
        return     type == ROUT_I_COMPLEX
                || type == ROUT_O_COMPLEX
                || type == ROUT_IO_THROUGH
                || type == ROUT_M_COMLEX
                || listRoutes.size() > 0;
    }

    QString GetRcEnum();                                    // получить перечисление идентифицированных РЦ маршрута
    QString GetStrlEnum();                                  // получить перечисление идентифицированных стрелок маршрута
    QString GetTuSetEnum();                                 // получить перечисление ТУ установки
    QString GetTuCancelEnum();                              // получить перечисление ТУ отмены

    bool CheckRqState (std::vector<LinkedStrl*>&);          // проверка полного соответствия заданного списка направляюших стрелок положению стрелок маршрута

private:
    static std::unordered_map <int, Route *> routes;        // маршруты, индексированные по коду маршрута

    // Константные данные
    int     relNo;        		                            // [No]         номеp маршрута на станции
    QString text;				                            // [Text]       описание (из БД, пишется "вручную", более осмысленное, чем HelpText)
    bool	dir;				                            // [Dir]        напpавление маpшpута		false = Ч, true = Н
    QString	path;                                           // [Path]       oсновной путь приема/отправления; в маршрутах передачи путь на который отправляется поезд
    QString	pathFrom;                                       // [PathFrom]   путь отправления в маршрутах передачи с пути на путь
//  int     park;				                            // номеp парка,если есть, иначе 0

    QString typeName;                                       // [Type]       имя типа
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

    // объекты
    Svtf *  svtfBeg;                                        // указатель на класс, описывающий ограничивающий начало маршрута светофор; если не описан - NULL
    Svtf *  svtfEnd;                                        // указатель на класс, описывающий ограничивающий конец маршрута светофор; если не описан - NULL
    Rc   *  nextRc;                                         // указатель на класс, описывающий следующую РЦ за маршрутом

    // исходные описания
    QString srcStrl;                                        // [STRL        ] исходное описание положения стрелок в маршруте
    QString srcRc;                                          // [Rc          ] исходное описание РЦ в маршруте
    QString srcTuSet;                                       // [TuBegEnd    ] ЗАДАНИЕ МАРШРУТА (Список ТУ через пробел; если он пустой - переводим стрелки и открываем сигнал)
    QString	srcTuCancel;                                    // [TuCancel    ] ОТМЕНА МАРШРУТА (Список ТУ через пробел; если он пустой - закрываем сигнал)
    QString srcTuCancelPending;                             // [CancelModeTu] ТУ для отмены режима набора маршрута в случае зависания
    QString	srcHangSetMode;                                 // [CancelModeTs] выражение для контроля заисания маршрута
    QString	srcPrgTsExpr;			                        // [PrgTsExpr   ] выражение вычисления направления и свободности перегона для определения необходимости его разворота
    QString	srcPrgTu;				                        // [PrgTu       ] команды разворота перегона
    QString	srcZzmk;			                            // [CodZmk      ] обобщенный сигнала замыкания
    QString	srcComplex;                                     // [Complex     ] перечисление состаных маршрутов через '+' или ' '
    QString	srcOpponentRoutes;                              // ["Враждебные маршруты"] перечисление враждебных через '+' или ' '
    QString	srcComment;                                     // [Question    ] комментарий
    QString	srcpark;                                        // [Park        ] парк

    int     timeCloseSvtf;		                            // [TimeCloseSvtf] максим.время перекрытия сигнала

    // Ts *    zmkTs;                                          // справочник обобщенного сигнала замыкания

    // признаки ошибки описаний
    bool    srcStrlError;                                   // ошибка в описании стрелок
    bool    srcRcError;                                     // ошибка в описании РЦ
    bool    tuSetError;                                     // ошибка в описании ТУ установки маршрута
    bool    tuCancelError;                                  // ошибка в описании ТУ отмены маршрута
    bool    tuCancelPendingError;                           // ошибка в описании ТУ для отмены режима набора маршрута в случае зависания
    bool    hangSetModeError;                               // ошибка в выражении контроля зависания маршрута
    bool    srcPrgTsError;                                  // ошибка в описании выражения контроля направления перегона
    bool    srcPrgTuError;                                  // ошибка в описании ТУ разворота перегона
    bool    srcZzmkError;                                   // ошибка в описании обобщенного выражения замыкания маршрута
    bool    srcComplexError;
    bool    srcOpponentError;

    bool	fBegEnd;                                        // Признак маршрутного набора - задание НАЧАЛО КОНЕЦ
    bool	bSetStrlBefore;		                            // Признак необходимости перевести стрелки перед дачей команды (лексема СТРЕЛКИ)
    bool	bAutoPeregonPending;                            // Идет разворот перегона при установке маршрута
    QDateTime tPeregonOk;			                        // Засечка времени, когда перегон развернулся; после этого надо выдержать N сек. перед установкой маршрута

    // списки
    std::vector <Rc *        > listRc;                      // список РЦ  в маpшpуте
    std::vector <LinkedStrl* > listStrl;                    // список стpелок в маpшpуте
    std::vector <Tu*         > tuSetList;                   // последовательность ТУ установки маршрута
    std::vector <Tu*         > tuCancelList;                // последовательность ТУ отмены маршрута
    std::vector <Tu*         > tuCancelPendingList;         // последовательность ТУ отмены набора
    std::vector <Tu*         > tuPeregon;                   // последовательность ТУ разворота перегона

    std::vector <Route *     > listCrossRoutes;             // список враждебных маршрутов
    std::vector <Route *     > listRoutes;                  // список составных маршрутов (пустой для элементарных)

    int     idType;				                            // код вида маршрута (ключ для RouteType)
    QString	tuText;				                            // имя ТУ (для ручного ввода)
    QString	helpText;			                            // расшифровка (генерим автоматически по типу)

    class BoolExpression * hangSetModeExpr;                 // вычислитель выражения контроля "зависания" набора маршрута
    class BoolExpression * prgTsExpr;                       // вычислитель выражения контроля требуемого направления на перегоне
    class BoolExpression * zmkExpr;                         // вычислитель выражения контроля замыкания

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
    QDateTime timeSvtfEndClosed;	                        // время перекрытия светофора, описывающего ограничивающий конец маршрута для выполнения функции контроля проезда запрещающего
    bool	svtfEndStatus;		                            // состояние светофора, описывающего ограничивающий конец маршрута для выполнения функции контроля проезда запрещающего
    bool	informedPrg;                                    // флаг выдачи пpедупpеждения о пpосpочке контроля разворота перегона

//	bool	bCheckForArmDsp;                                // контролируем в АРМ ДСП ЦИТТРАНС

    // переменные для анализа непрохождения маршрута по враждебностям ввожу новые
    bool problemCross;
    bool problemStrl;
    bool problemSvtf;
    bool problemRc;
    bool problemPrg;

    // закрытые функции
    QString nameLog();                                      // обозначение маршрута для лога в формате: Ст.
    bool parseTuList (QSqlQuery& query, QString field, QString& src, std::vector <Tu*> list, Logger& logger);
    bool parseExpression(QSqlQuery& query, QString field, QString& src, BoolExpression *& expr, Logger& logger);
    bool checkComplex   (Logger&);                          // обработать списки составных  маршрута
    bool checkOpponents (Logger&);                          // обработать списки враждебных маршрута
    static void checkComplex  (class KrugInfo*, Logger& );  // обработать списки составных  всех маршрутов, входящих в круг
    static void checkOpponents(class KrugInfo*, Logger& );  // обработать списки враждебных всех маршрутов, входящих в круг
};

#endif // ROUTE_H
