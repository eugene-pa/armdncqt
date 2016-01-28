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
        type    = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
        idst    = lexems[1].toInt(&ret);               ok &= ret;
        subtype = lexems[2].toInt(&ret);               ok &= ret;

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

        idObj = lexems[7].toInt(&ret);                 ok &= ret;

        for (int i=8; i<11; i++)
        {
            if (int nostrl = lexems[7].toInt(&ret) && ret)
                strl.append(new LinkedStrl(nostrl));
            ok &= ret;
        }

        // 11- нормальное полложение (0 - по основному ходу, 1 - ответвление)
        int normal = lexems[11].toInt(&ret);            ok &= ret;
        plusNormal = normal ==0 ? true : false;             // TRUE - плюс по основному ходу, FALSE - ответвление

        // 12 - РЦ
        idrc    = lexems[12].toInt(&ret);               ok &= ret;

        // 13 - имя
        name    = lexems[13];                               // имя (номер стрелки)
        name.replace("$","");

        // 14 - наклон
        if (lexems.length() > 14)                           // наклон
            b45 = lexems[12] == "45";

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        sprStrl = Strl::GetById(idObj);                     // стрелка
        sprRc   = Rc::GetById(idrc);                        // РЦ

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

/*
    protected override void CheckData(string src)               // вирт. функция подготовки справочников
    {
        if (X2 == 0 && Y2 == 0)
        {
            X2 = X1 + 16;
            Y2 = Y1 + 16;
        }

        SetDimensions();
        Area   = new Rect(X1, Y1 - 8, 16, 16);                   // корректируем прямоугольник  стрелки с учетои ее разброса
        Center = new Point(Area.X + Width / 2, Area.Y + Height / 2);

        SprSt   = CheckSt(NoSt, src);
        SprRc   = CheckRc(NoRc, src);
        SprStrl = CheckStrl(NoStrl, src);
        if (SprRc != null && SprStrl != null)
        {
            // Неочевидное поведение: ссылки SprRc и SprRc2 класса StrlInfo устанавливаются ТОЛЬКО ПРИ ЧТЕНИИ ФОРМ
            if (SprStrl.SprRc == null)
                SprStrl.SprRc = SprRc;
            else
            if (SprStrl.SprRc != SprRc)
                SprStrl.SprRc2 = SprRc;
        }
        foreach (LinkedStrl linkedStrl in StrlList)
            CheckStrl(Math.Abs(linkedStrl.SignNoStrl), src);

    }



*/
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
