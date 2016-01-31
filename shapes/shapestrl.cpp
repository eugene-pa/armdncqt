#include "shapestrl.h"
#include "shapeset.h"
#include "colorscheme.h"
#include "../spr/station.h"

ShapeStrl::ShapeStrl(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    plusNormal = false;
    idrc = 0;
    b45 = false;
    sprStrl = nullptr;                                      // стрелка

    Parse(src);
    setDimensions ();
}

ShapeStrl::~ShapeStrl()
{

}

// Разбор строки текстового описания примитива STRL
// 0  1  2   3    4   5 6   7     8  9  10  11  12    13   14
//    Ст Тип X1  Y1  X2 Y2  No    S1 S2 S3 отв  РЦ   Имя  Градус
// 19 2  2   1233 522 0 0   15    0  0  0   0   45    $3
// 19 17 3   494  217 0 0   3109  0  0  0   0   3081  $    45
// 2 - тип-форма стрелки
// 11- нормальное полложение (0 - по основному ходу, 1 - ответвление)
// 13- $[имя стрелки] - имя может быть пустым, остается только символ $
// 14- [45] - угол наклона 45 градусов, в случае отсутствия по умолчанию 60
void ShapeStrl::Parse(QString& src)
{
    bool ok  = true,
         ret = false;
    QStringList lexems = src.split(' ',QString::SkipEmptyParts);
    if (lexems.length() >= 14)
    {
        // 0 - примитв
        type    = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
        // 1 - станция
        idst    = lexems[1].toInt(&ret);               ok &= ret;
        // 2 - тип стрелки
        subtype = lexems[2].toInt(&ret);               ok &= ret;

        // 3-6 - координаты
        x1    = lexems[3].toFloat(&ret);               ok &= ret;
        y1    = lexems[4].toFloat(&ret);               ok &= ret;
        x2    = lexems[5].toFloat(&ret);               ok &= ret;
        y2    = lexems[6].toFloat(&ret);               ok &= ret;

        // вторая пара координат нужна для универсальных вычислений
        if (x2 == 0 && y2 == 0)
        {
            x2 = x1 + 16;
            y2 = y1 + 16;
        }
        setDimensions ();

        // #стрелка
        idObj = lexems[7].toInt(&ret);                 ok &= ret;

        // 8-10 - направляюшие стрелки
        for (int i=8; i<11; i++)
        {
            int nostrl = nostrl = lexems[i].toInt(&ret);
            if (nostrl && ret)
                strl.append(new LinkedStrl(nostrl));
            ok &= ret;
        }
        if (!LinkedStrl::checkList(strl, set->logger()))
            log (QString("Ошибка описания определяющих стрелок примитва: %1").arg(src));

        // 11- нормальное полложение (0 - по основному ходу, 1 - ответвление)
        int normal = lexems[11].toInt(&ret);            ok &= ret;
        plusNormal = normal ==0 ? true : false;             // TRUE - плюс по основному ходу, FALSE - ответвление

        // 12 - #РЦ
        idrc    = lexems[12].toInt(&ret);               ok &= ret;

        // 13 - символьное имя стрелки (надпись)
        name    = lexems[13];                               // имя (номер стрелки)
        name.replace("$","");

        // 14 - наклон
        if (lexems.length() > 14)                           // наклон
            b45 = lexems[12] == "45";

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        sprStrl = Strl::GetById(idObj);                     // стрелка
        sprRc   = Rc::GetById(idrc);                        // РЦ
        if (sprRc != nullptr && sprStrl != nullptr)
            sprStrl->SetRc(sprRc);                          // Неочевидное поведение: ссылки SprRc и SprRc2 класса StrlInfo устанавливаются ТОЛЬКО ПРИ ЧТЕНИИ ФОРМ

        //Area   = new Rect(X1, Y1 - 8, 16, 16);                   // корректируем прямоугольник  стрелки с учетои ее разброса
        //Center = new Point(Area.X + Width / 2, Area.Y + Height / 2);

        if (!ok)
        {
            log (QString("%1: %2").arg("Ошибка синтаксиса примитива").arg(src));
        }
    }
    else
    {
        log (QString("%1: %2").arg("Ошибка синтаксиса примитива (ош.числа лексем)").arg(src));
    }
}

// проверка нахождения определяющих стрелок в требуемом положении
bool ShapeStrl::isStrlOk()
{
    return LinkedStrl::checkRqSts(strl);
}


// функция рисования
void ShapeStrl::Draw(QPainter* painter)
{
    Q_UNUSED(painter)
}

// вычисление замещаемого прямоугольника
//void ShapeStrl::GetArea(short*,short*,short*,short*)
//{

//}

bool ShapeStrl::CheckIt()
{
    return false;
}

void ShapeStrl::FixUpUnsafe()
{

}

void ShapeStrl::Prepare()
{

}

QString ShapeStrl::Dump()
{
    return "СТРЛ";
}

QString  ShapeStrl::ObjectInfo()
{
    return "СТРЛ";
}
