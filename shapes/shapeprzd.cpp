#include "shapeset.h"
#include "shapeprzd.h"

// статические ресурсы
QBrush ShapePrzd::BrushOpen;                                // открыт
QBrush ShapePrzd::BrushClose;                               // закрыт
QBrush ShapePrzd::BrushUndef;                               // неизвестно
QBrush ShapePrzd::BrushExpired;                             // устарели ТС
QPen   ShapePrzd::PenOpen;                                  // открыт окантовка
QPen   ShapePrzd::PenClose;                                 // закрыт окантовка
QPen   ShapePrzd::PenUndef;                                 // неизвестно окантовка
QPen   ShapePrzd::PenExpired;                               // устарели ТС
QBrush ShapePrzd::BackgroundAlarm;                          // фон транспаранта аларма
QBrush ShapePrzd::ForeAlarm;                                // цвет транспаранта аларма

QFont  ShapePrzd::font;                                     // шрифт отрисовки А, ЗГ
#ifdef Q_OS_WIN
int ShapePrzd::fontsize = 11;                               // размер шрифта
QString ShapePrzd::fontname = "Segoe UI";                   // шрифт
#endif
#ifdef Q_OS_MAC
int ShapePrzd::fontsize = 15;                               // размер шрифта
QString ShapePrzd::fontname = "Segoe UI";                   // шрифт
#endif
#ifdef Q_OS_LINUX
int ShapePrzd::fontsize = 12;                               // размер шрифта
QString ShapePrzd::fontname = "Segoe UI";                   // шрифт
#endif

QTextOption ShapePrzd::option;                              // опции отрисовки номера (выравнивание)

ShapePrzd::ShapePrzd(QString& src, class ShapeSet* parent) : DShape (src, parent)
{
    for (int i=0; i<exprAll; i++)
        formulas[i] = nullptr;
    pereerd = nullptr;

    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeRc. Исключение при разборе строки: %1").arg(src));
    }
}

ShapePrzd::~ShapePrzd()
{

}

// инициализация статических инструментов отрисовки
void ShapePrzd::InitInstruments()
{
    BrushOpen    = QBrush(colorScheme->GetColor("PrzdOpen"));
    BrushClose   = QBrush(colorScheme->GetColor("PrzdClose"));
    BrushUndef   = QBrush(colorScheme->GetColor("PrzdUnknown"));
    BrushExpired = QBrush(colorScheme->GetColor("Expired"));

    BackgroundAlarm = QBrush(colorScheme->GetColor("PrzdAlarmBack"));
    ForeAlarm       =QBrush(colorScheme->GetColor("PrzdAlarmFore"));

    PenOpen    = QPen(BrushOpen   .color().darker(150),2);
    PenClose   = QPen(BrushClose  .color().darker(150),2);
    PenUndef   = QPen(BrushUndef  .color().darker(150),2);
    PenExpired = QPen(BrushExpired.color().darker(150),2);

    font    =  QFont(fontname,fontsize,65);
    option  = QTextOption(Qt::AlignCenter);
}

// разбор строки описания
// Все имена начинаются с $. Это дает возможность вводить пустые имена, если нет ТС отдельных типов
//  0  1    2    3     4    5
//     Ст   X1   Y1    X2   Y2
// 15  1    57  512    57  612  $П1.З2|!П1.З1 $П1.А|П1.НС $П1.ЗГ $ $
// 15  2  1207  513  1207  613  $!П.З&~П.ИН&~П.ИЧ $П.А|~П.А $ $!~П.ИН $!~П.ИЧ
void ShapePrzd::Parse(QString& src)
{
    bool ok  = true,
         ret = false;
    QStringList lexems = src.split(' ',QString::SkipEmptyParts);        // лексемы
    QStringList names  = src.split('$',QString::SkipEmptyParts);        // имена
    // 0- примитив
    type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
    // 1- номер станции
    idst = lexems[1].toInt(&ret);               ok &= ret;

    // 2-5- координаты
    x1    = lexems[2].toFloat(&ret);            ok &= ret;
    y1    = lexems[3].toFloat(&ret);            ok &= ret;
    x2    = lexems[4].toFloat(&ret);            ok &= ret;
    y2    = lexems[5].toFloat(&ret);            ok &= ret;

    rectText = QRectF(x1 - 50, y1- 30, 100, 24);

    x1 -=3;
    x2 +=3;
    setDimensions ();

    st      = Station::GetById(idst);                   // станция

    // читаем формулы в индексированный массив и связываем
    for (int i=exprOpen; i<exprAll; i++)
    {
        if (st && names.length() > i+1)
        {
            QString name = names[i+1].trimmed();
            if (name.length())
            {
                formulas[i] = new BoolExpression(names[i+1]);
                if (formulas[i]->Valid())
                    QObject::connect(formulas[i], SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
                else
                    log (QString("Ошибка формулы '%1' в описании переезда: '%1'").arg(names[i+1]).arg(src));
            }
        }
    }
    Pereezd::findByShape(this);
}



// вычисление состояния примитива
void ShapePrzd::accept()
{
    if (st == nullptr)
        state->set(Status::StsUndefined, true);             // неопред.состояние - нет справочников
    else
    {
        state->set(Status::StsUndefined, false);
        state->set(Status::StsExpire, st->IsTsExpire());

        state->set(StsOpen, formulas[exprOpen]==nullptr ? false : formulas[exprOpen]->ValueBool());
        state->set(StsClose, !(*state)[StsOpen]);
        state->set(StsAlarm, formulas[exprAlarm]==nullptr ? false : formulas[exprAlarm]->ValueBool());
        state->set(StsBlock, formulas[exprBlock]==nullptr ? false : formulas[exprBlock]->ValueBool());
        state->set(StsNotifying, (formulas[exprIzv1]==nullptr ? false : formulas[exprIzv1]->ValueBool()) |
                                 (formulas[exprIzv2]==nullptr ? false : formulas[exprIzv2]->ValueBool()));
    }

}


void ShapePrzd::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw (painter);
}

// функция рисования
static int dx = 3,  // смещение усика по Х
           dy = 6;  // смещение усика по Y
//           dxt = 8; // отступ текста в траспаранте

void ShapePrzd::Draw (QPainter* painter)
{
    accept();

    QPen& pen = state->isExpire()      ? PenExpired    :
                state->isUndefined()   ? PenUndef      :
                (*state)[StsOpen     ] ? PenOpen       :
                (*state)[StsClose    ] ? PenClose      :
                (*state)[StsNotifying] ? PenClose      :
                                         PenUndef;

    QBrush& brush = state->isExpire()                                       ? BrushExpired  :
                    state->isUndefined()                                    ? BrushUndef    :
                    (*state)[StsOpen     ]                                  ? BrushOpen     :
                    (*state)[StsClose    ] && formulas[exprOpen] != nullptr ? BrushClose    :
                                                                              BrushUndef    ;
    painter->setPen(pen);
    painter->setBrush(brush);

    //painter->drawRect(rect);                              // вариант отрисовки полностью окантованным прямоугольником
    painter->fillRect(rect, brush);                         // вариант отрисовки заливкой и вертикальными линиями окантовки
    painter->drawLine(rect.topLeft   (), rect.bottomLeft());
    painter->drawLine(rect.topRight   (), rect.bottomRight());

    // "усики"
    painter->drawLine(rect.topRight   (), rect.topRight   () + QPoint( dx,-dy));
    painter->drawLine(rect.topLeft    (), rect.topLeft    () + QPoint(-dx,-dy));
    painter->drawLine(rect.bottomRight(), rect.bottomRight() + QPoint( dx, dy));
    painter->drawLine(rect.bottomLeft (), rect.bottomLeft () + QPoint(-dx, dy));

    // транспаранты А, ЗГ
    if (!state->isExpire() && !state->isUndefined() && ((*state)[StsAlarm] || (*state)[StsBlock]))
    {
        QString s = (*state)[StsAlarm] && (*state)[StsBlock] ? "А  ЗГ" :
                    (*state)[StsAlarm]                       ? "А" :
                    (*state)[StsBlock]                       ? "ЗГ" : "";

        if (DShape::globalPulse)
        {
            backRect = painter->boundingRect(rectText, s, option) + QMargins(8,2,8,2);
            painter->fillRect(backRect, BackgroundAlarm);
            painter->setFont(font);
            painter->setPen(QPen(ForeAlarm.color()));
            painter->drawText(rectText, s, option);
        }
    }
}
