#ifndef USHAPE_H
#define USHAPE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>

enum UShapeType
{
    U_Null    = 0,            // - некорректный тип, используется как признак ошибки
    U_Str     = 1,            //
    U_Hor     = 2,            //
    U_Ver     = 3,            //
    U_Box     = 4,            //  BOX
    U_Chr     = 5,            //
    U_Apnd    = 6,            //
    U_Input   = 8,            //
    U_Dyn     = 9,            //
    U_Menu    = 10,           //
    U_Przd    = 15,           //  PRZD
    U_Segment = 16,           //  SEGMENT
    U_Angle   = 17,           //  ANGLE
    U_Svtf    = 18,           //  SVTF
    U_Strl    = 19,           //  STRL
    U_Trnsp   = 20,           //  MODE
    U_Name    = 22,           //  NAME
    U_Text    = 23,           //  TEXT
    U_TrainNo = 24,           //  номер поезда, новый тип примитива
    U_Last,
};

class UShape : public QGraphicsItem
{
public:
    static bool GetTypeName(QString& ret, UShapeType type);     // статическая функция получения имени типа примитива (для XML)
    static UShapeType GetTypeByName (QString typeName);         // статическая функция получения типа примитива по его имени

    UShape();
    ~UShape();
};

#endif // USHAPE_H
