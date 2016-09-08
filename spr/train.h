#ifndef TRAIN_H
#define TRAIN_H

#include <stack>
#include "../common/logger.h"
#include "sprbase.h"


class Train : public SprBase
{
    friend class DStDataFromMonitor;                        // для формирования и извлечения информации в потоке ТС
    friend class DDataFromMonitor;
    friend class DPrgDataFromMonitor;
    friend class DlgTrains;
    friend class ShapeTrain;
public:

    // открытые статические функции
    static Train * GetBySysNo(int sno, class KrugInfo * krug = nullptr);// получить справочник по номеру поезда
    static Train * AddTrain(int sno, int no=0, class KrugInfo * krug = nullptr);   // добавить поезд
    static Train * restore(int sno, int no, class KrugInfo * krug);
    static void ClearAllRc();
    static void AcceptTS (class Station *);                 // обработка объектов по станции
    static void ClearMark();                                // очистить пометки

    Train();
    Train(int sno, int no=0, class KrugInfo * krug = nullptr);
    ~Train();

    void update(int sno, int no, class KrugInfo * krug);

    void AddRc(class Rc * rc);                              // добавить занятую РЦ
    void ClearRc();                                         // очистить список РЦ
    bool IsEvn() { return ((no > 0 ? no : sno) % 2) == 0; } // четный ?

private:
    // учитывая динамический характер объекта "поезд" можно было бы организовать пул справочников поездов таким образом,
    // чтобы не возвращать однажды выделеннную память, а повторно использовать ее после удаления поезда
    // ВАРИАНТ: сохранять удаленные справочники в отдельном списке и брать по возможности их оттуда.
    //          Можно было бы просто помечать справочник как "пустой", но тогда нельзя организовать хэш-таблицу по ключу sno
    static std::unordered_map <int, Train *> Trains;        // поезда , индексированные по ключу: круг + системный номер
    static std::stack <Train *> FreeTrains;                 // пул удаленных справочников для повторного использования

    int     sno;                                            // ключ: krug + системный номер
    int     no;                                             // номер
    short   Ind1;                                           // индекс
    short   Ind2;
    short   Ind3;

    float	XMin;										    // миним.абсцисса
    float	YMin; 										    // максим.абсцисса
    float	XMax;										    // миним.абсцисса
    float	YMax; 										    // максим.абсцисса

    int	    RcMin;										    // РЦ с миним.абсциссой
    int	    RcMax;										    // РЦ с максим.абсциссой

    // нужно иметь список РЦ под поездом;
    // дилемма: использовать динамические массивы или статический под максимальное число

    class Station * st; 								    // вычисленная станция нахождения поезда
    QDateTime tmdt;                                         // Время последней операции

    int     nrc;                                            // число занятых поездом РЦ
    std::vector <class Rc *> Rc;                            // массив указателей на справочники занятых РЦ (не используемые обнуляются)

    time_t  tBlindPrgnOn;                                   // время вступления поезда на слепой перегон
    bool    marked;                                         // пометка при отображении

// хочу сделать более осмысленной модель
//	short	NoRc;										    // номер последней занятой РЦ
//	BOOL	EraseMark;									    // Пометка для удаления
//	int		x1,x2,y1,y2;								    // координаты прорисованного номера
//	short	SvtfWasOpened;								    // Признак-Номер выходного светофора,который открывался
//	bool	stsSvtf;									    // состояние светофора (истина - ОТКРЫТ)
//	float	HeadPlace;									    // Положение головы в КЛМ

// 2010.04.18. Направление слепого перегона -------------------------------------------------------
//	bool	bBlindPrg;                                      // аоезд на слепом перегоне
//	bool	bPeregonUp;                                     // для того чтобы корректно рисовать поезда на слепых перегонах
// ------------------------------------------------------------------------------------------------
//	int		NoKrug;                                         // 2012.04.04. Номкр круга при многопоточном подключении, иначе - 0
// -----------------------------------------------------------------------------------------------

// 2009.07.10. время получения последнего сообщения от СКСИ-МП по поезду
// Одновременно служит признаком наличия информации СКСИ-МП
//	time_t  tSksi;										// время получения последнего сообщения по поезду
//------------------------------------------------------------------------------------------------
//	2009.07.10. Информация с борта, полученная от СКСИ-МП
//    UINT idDriver;										// 13-16 номер машиниста
//    WORD axies;											// 17-18 осей
//    BYTE cars;											// 19    вагонов
//    WORD mass;											// 20-21 масса
//    UINT noLoc;											// 22-25 локомотив
//    WORD typeLoc;										// 26-27 тип локомотива
//    BYTE category;										// 28    категория
//    WORD speed;											// 29-30 скорость
//    BYTE status;										// статус
//														// D8    - невалидность
//														// D7    - keyAlarm - нажатие кнопки тревоги
//														// D5-D6 - уровень бодрствования машиниста по ТСКБМ
//														// D3-D4 - режим работы локомотива
//														// D1-D2 - режим работы поездной блокировки
// -----------------------------------------------------------------------------------------------

// 2011.12.23. информация, полученная от ГИД УРАЛ (номер и индекс пишем в штатные поля)
    QTime  tGidUral;									// время получения последнего сообщения по поезду
    BYTE  guColor;										// цвет нитки от ГИД
                                                        // 0 -	черный;		    8 -		темно-серый;
                                                        // 1 -	синий;			9 - 	ярко-синий;
                                                        // 2 -	зеленый;		10 - 	ярко-зеленый;
                                                        // 3 -	голубой;		11 -	ярко-голубой;
                                                        // 4 -	красный;		12 - 	ярко-красный;
                                                        // 5 -	фиолетовый;		13 -	ярко-фиолетовый;
                                                        // 6 -	коричневый;		14 -	желтый;
                                                        // 7 -	светло-серый	15 -	белый
    BYTE  guType;										// тип поезда от ГИД
                                                        // 0 - разрядный;
                                                        // 1 - негабаритный;
                                                        // 2 - тяжеловесный;
                                                        // 3 - длинносоставный;
                                                        // 4 - соединенный;
                                                        // 5 - с ограничением скорости;
                                                        // 6 - с толкачем;
                                                        // 7 - неисправные АЛСН или РС
    BYTE  guCars;										// условная длина в вагонах
    WORD  guBrutto;										// вес брутто
    WORD  guLokser;										// серия локомотива
    UINT  guLokno;										// номер локомотива
    QChar suffix[9];									// суффикс номера поезда
// -----------------------------------------------------------------------------------------------
    short	saveNo;										// номер поезда до наложения информации ГИД-УРАЛ
//	bool	bKeyAlarmInformed;
// -----------------------------------------------------------------------------------------------
};

#endif // TRAIN_H
