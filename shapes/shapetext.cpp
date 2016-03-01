#include "shapeset.h"
#include "shapetext.h"

QStringList ShapeText::FontIds;


ShapeText::ShapeText(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    if (FontIds.length() < 1)
        FontIds << "Arial" << "Courier New" << "Consolas" << "Times New Roman" << "MS Sans Serif" << "Consolas" << "Calibri" << "Tahoma" << "Verdana";
    exprCondition = nullptr;
    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeText. Исключение при разборе строки: %1").arg(src));
    }
}

ShapeText::~ShapeText()
{

}


// разбор строки описания примитивов ИМЯ СТАНЦИИ(22) и ТЕКСТ (23)
//  0   1    2    3    4   5   6     7     8     9     10             11        12
//      Ст  X1    Y1   H   W  font  bold  ital  clr    Текст         Условие  Коммент
// 22   2   128  288  $Суворовская                                                              - имя станции
// 23   2   128  288  16   6   5     1     1    98559 $ПРИМЕР ТЕКСТА $УСЛОВИЕ;;РЦ=22            - текст
// 23   6   2364 1911 20   8   0     1     0    4210752 $1П;;РЦ=924                             - текст
void ShapeText::Parse(QString& src)
{
    bool ok  = true,
         ret = false;

    QStringList parts  = src.split(";;",QString::SkipEmptyParts);       // отделяем от комментария
    QStringList lexems = parts[0].split(' ',QString::SkipEmptyParts);   // лексемы
    QStringList texts  = parts[0].split('$' ,QString::SkipEmptyParts);  // текстовые строки
    text = texts.length() > 1 ? texts[1] : "";
    condition = texts.length() > 2 ? texts[2] : "";
    // 0- примитив
    type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
    // 1- номер станции
    idst = lexems[1].toInt(&ret);               ok &= ret;

    // 2-3- координаты
    x1    = lexems[2].toFloat(&ret);            ok &= ret;
    y1    = lexems[3].toFloat(&ret);            ok &= ret;

    st      = Station::GetById(idst);                       // станция

    if (type == NAME_COD)
    {
        x1 -= 150;
        height = 20;
        width = 9;
        familyId = Tahoma/*Arial*/;
        bold = true;
        italian = false;
        color = Qt::black;
        option = QTextOption(Qt::AlignCenter);
    }
    else
    {
        height   = lexems[4].toInt(&ret);         ok &= ret;
        width    = lexems[5].toInt(&ret);         ok &= ret;
        familyId = (FamilyIds)lexems[6].toInt(&ret);    ok &= ret;
        bold     = lexems[7].toInt(&ret) > 0;     ok &= ret;
        italian  = lexems[8].toInt(&ret) > 0;     ok &= ret;
        int rgb  = lexems[9].toInt(&ret);         ok &= ret;
        color    = QColor::fromRgb(rgb&0xff, (rgb >> 8)&0xff, (rgb>>16)&0xff);
        option = QTextOption(Qt::AlignLeft);

        // 2012.08.21. Танцы с бубном - для системных шрифтов делаем подстановки размера
        // Так как размер и вид шрифтов не соответствуют
        if (   familyId == System           // "System"   = Consolas
            || familyId == Fixedsys         // "Fixedsys" = Consolas
            )
        {
            height = 14;                    // фиксированный размер
        }
        else
        {
            height--;
            if (familyId == Arial)
            {
                height--;
//                if (!bold)
//                    height--;
//                if (height < 15)
                    //y1 += 3;
            }
        }


        // условие отображения
        if (st != nullptr && condition.length())
        {
            exprCondition = new BoolExpression(condition);
            QObject::connect(exprCondition, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        }
    }
    x2    = x1 + 300;
    y2    = y1 + 24;
    setDimensions ();

    font = QFont(FontIds[familyId],height/1.3, bold ? 60 : 50, italian);
    pen = QPen(color);
}


// при отрисовке вызывается вирт.функция paint, которая вызывает функцию непосредственной отрисовки Draw(QPainter* painter)
void ShapeText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}

// вычисление состояния примитива
void ShapeText::accept()
{
}

// функция рисования
void ShapeText::Draw(QPainter* painter)
{
    accept();

    if (exprCondition && !exprCondition->ValueBool())
        return;
    painter->setFont(font);
    painter->setPen (pen);
    painter->drawText(rect, text, option);
}
