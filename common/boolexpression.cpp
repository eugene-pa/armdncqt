#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStack>
#include <QDebug>

//#include <../spr/station.h>
#include "boolexpression.h"

/*
 * Класс для вычисления вражений путем предварительного их разбора и приведения к польской инверсной записи
 * Поддерживаемые операции
 *    + - * /   - арифметические
 *    & | ^ !   - логические
 *    < > =     - сравнения
 */

// используем регулярные выражения для проверки синтаксиса и разбора выражений (отдельно для логических и арифметических выражений)
QRegularExpression BoolExpression::LexemRgxMath  ("\\^|\\<|\\>|\\!|\\&|\\(|\\)|\\+|\\-|\\*|\\/|\\||=|([.A-Za-z0-9_~#А-я\\[\\]]*)");
QRegularExpression BoolExpression::LexemRgxLogic ("\\^|\\!|\\&|\\(|\\)|\\||([.A-Za-z0-9_~#\\-\\+\\/\\*А-я\\[\\]]*)");

QRegularExpression BoolExpression::OperatorRgxMath("^[=\\<\\>\\!\\&\\^\\|\\+\\-\\*\\/]$");
QRegularExpression BoolExpression::OperatorRgxLogic("^[!\\&\\^\\|]$");

// конструктор выполняет преобразование выражения в польскую инверсную запись
BoolExpression::BoolExpression(QString& expr, bool fullMathOps)
{
    source      = expr;                                     // выражение
    logicalType = !fullMathOps;
    error       = OK;

    ToRpn();                                                // получить ПОЛИЗ
    GetValue();
}
/*
BoolExpression::BoolExpression(QString expr, bool fullMathOps)
{
    source      = expr;                                     // выражение
    logicalType = !fullMathOps;
    error       = OK;

    ToRpn();                                                // получить ПОЛИЗ
    GetValue();
}
*/
BoolExpression::~BoolExpression()
{
}

// преобразование выражения в польскую инверсную запись
void BoolExpression::ToRpn()
{
    QStack<QString> stack;
    QString expr = source.replace(" ", "");                 // удаляем все пробелы из входной строки

    // эти переменные используются для анализа ошибок
    bool LastTokenIsOperator      = false;                  // последняя лексема была бинарным оператором
    bool LastTokenIsOperatorUnary = false;                  // последняя лексема была унарным оператором

    // отдельно проверяю наличие двойных унарных операторов
    if (expr.indexOf("!!") >= 0)
        error = OperandMissing;

    QRegularExpressionMatchIterator items = logicalType ? LexemRgxLogic.globalMatch(expr) : LexemRgxMath.globalMatch(expr);
    while (items.hasNext())
    {
        QString s = items.next().captured(0);
        if (s.trimmed().length()==0)
            continue;
        if (s == ")")
        {
            // вытаскиваем операции из стека, пока не дойдем до открывающейся скобки
            LastTokenIsOperator = LastTokenIsOperatorUnary = false;

            QString op = "";
            while (stack.count() > 0)
            {
                op = stack.pop();
                if (op == "(")
                    break;
                rpnStr += op + " ";
            }
            if (op != "(")
                error = OpenBracketMissing;
        }

        // если встретили цифру
        else if (IsValuable (s))
        {
            LastTokenIsOperator = LastTokenIsOperatorUnary = false;
            rpnStr += s + " ";                              // сразу добавляем ее к выходной строке
        }

        else if (s == "(")
        {
            LastTokenIsOperator = LastTokenIsOperatorUnary = false;
            stack.push(s);                                  // если открывающую скобку, то помещаем ее в стек
        }

        // если встретили оператор
        else if (IsOperator (s))
        {
            if (LastTokenIsOperator && !IsUnary(s))
            {
                error = OperandMissing;
                continue;
            }

            if (LastTokenIsOperatorUnary && IsUnary(s))
            {
                error = OperandMissing;
                continue;
            }

            LastTokenIsOperator = !IsUnary(s);
            LastTokenIsOperatorUnary = IsUnary(s);

            // если стек - пуст
            if (stack.count() == 0)
            {
                stack.push(s);
            }
            else
            {
                // проверяем приоритеты

                if (GetPriority(s) > GetTopPriority (stack))
                {
                    stack.push(s);
                }
                else
                {

                    while (     stack.count() > 0
                            &&  GetPriority(s) <= GetTopPriority (stack))
                    {
                        rpnStr += stack.pop() + " ";
                    }
                    stack.push(s);
                }
            }
        }
    }

    // добавляем в выходную строку оставшиеся символы
    while (stack.count() > 0)
    {
        rpnStr += stack.pop() + " ";
    }

    if (rpnStr.length() == 0)
        error = EmptyExpr;
    else
    {
        if (rpnStr[rpnStr.length()-2] == '(')
            error = CloseBracketMissing;
    }

}


// Проверка строки на принадлежность к оператору;
// Поддерживаются только однознаковые операторы:    = <  >  !  &  ^  |  +  -  *  /
// (причем опеатор '-' является бинарным; унарным является только оператор !)
bool BoolExpression::IsOperator (QString s)
{
    return logicalType ? OperatorRgxLogic.match(s).hasMatch() : OperatorRgxMath.match(s).hasMatch();
}

// проверка унарности оператора
bool BoolExpression::IsUnary(QString s)
{
    return s == "!";
}

// возможные лексемы: операторы, круглые скобки и переменные или константы
// все, что не оператор и не скобки - переменные
bool BoolExpression::IsValuable (QString s)
{
    return !IsOperator(s) && s != "(" && s != ")";
}

// целое ?
bool BoolExpression::IsInteger (QString s)
{
    QRegularExpression r("^\\d+$");                         // \b\d+\b допускала пробелы до, после и в середине
    return r.match(s).hasMatch();
}

// перегруженная функция проверки сивола на принадлежность к символу операций
bool BoolExpression::IsOperator (char c)
{
    return IsOperator(QString(c));
}

// приоритет операций
int BoolExpression::GetPriority (QString s)
{
    return  s== "(" ? 0 :
            s== ")" ? 1 :
            ((s== "+") | (s== "-") | (s== ">") | (s== "<") | (s== "=") | (s== "&") | (s== "|") | (s== "^")) ? 2 :
            ((s== "*") | (s==  "/")) ? 3 :
            s== "!" ? 4 : 0;
}

int BoolExpression::GetTopPriority (QStack<QString>& stack)
{
    if (stack.empty())
        return 0;
    QString top = stack.pop();
    stack.push(top);
    return GetPriority (top);
}

// получить значение выражения
int BoolExpression::GetValue()
{
    QStringList list = rpnStr.split(' ',QString::SkipEmptyParts);

    QStack<QString> stack;
    int res = 0;
    QString result;

    foreach(QString s, list)
    {
        if (IsOperator(s))
        {
            try
            {
                if (stack.count() == 0)
                {
                    error = OperandMissing;
                    return res;
                }
                int par1 = 0,
                    par2 = (int)(stack.pop().toFloat());    // Convert.ToInt32(stack.Pop()));
                if (!IsUnary(s))
                {
                    if (stack.count() == 0)
                    {
                        error = OperandMissing;
                        return res;
                    }
                    par1 = (int)(stack.pop().toFloat());    // Convert.ToInt32(stack.Pop());
                }
                if (s == "!")
                    res = par2 > 0 ? 0 : 1;
                else
                if (s == "+")                    res = par1 + par2;
                if (s == "-")                    res = par1 - par2;
                if (s == "*")                    res = par1 * par2;
                if (s == "/")                    res = par1 / par2;
                if (s == ">")                    res = par1 > par2 ? 1 : 0;
                if (s == "<")                    res = par1 < par2 ? 1 : 0;
                if (s == "=")                    res = par1 == par2 ? 1 : 0;
                if (s == "&")                    res = (par1 && par2) == true ? 1 : 0;
                if (s == "|")                    res = (par1 || par2) == true ? 1 : 0;
                if (s == "^")                    res = ((par1!=0) ^ (par2!=0)) == true ? 1 : 0;

                stack.push(result.setNum(res));
            }
            catch (...)
            {
                // Ошибка синтаксиса польской инвесной записи; вопрос: что делать.
                // 1. Можно выбросить исключение, возможно - опциональное поведение
                // 2. Можно писать в лог
                qDebug() << "Исключение BoolExpression::GetValue()";
                error = RealtimeError;
            }
        }
        else
        {
            if (IsInteger(s))
                stack.push(s);
            else
            {
                try
                {
                    int var = 0;
                    emit GetVar(s, var);                    // генерируем сигнал вычисления переменной
                    stack.push(QString::number(var));
                }
                catch (...)
                {
                    stack.push("0");
                    UndefinedVarName = s;
                    ErrorUndefinedVarNameText = "BoolExpression::GetValue(). Ошибка при вычислении переменной";
                    error = GetVariableError;
                    qDebug() << ErrorUndefinedVarNameText;
                }
            }
        }
    }

    // на стеке долен быть результат; если его нет - признак ошибки и вернем промежуточный результат
    if (stack.count()==0)
    {
        error = OperandMissing;
        return res;
    }
    return stack.pop().toInt();
}

// получить текст ошибки
QString BoolExpression::ErrorText()
{
    QString s = "?";
    switch (error)
    {
        case OK:
            s = "ОК";                               break;
        case Unknown:
            s = "Ошибка синтаксиса ";               break;
        case OpenBracketMissing:
            s = "Отсутствует открывающая скобка";   break;
        case CloseBracketMissing:
            s = "Отсутствует закрывающая скобка";   break;
        case EmptyExpr:
            s = "Пустое выражение";                 break;
        case OperandMissing:
            s = "Пропущен операнд";                 break;
        case OperatorMissing:
            s = "Пропущен оператор";                break;
        case RealtimeError:
            s = "Ошибка синтаксиса. Корректное вычисление невозможно"; break;
        case GetVariableError:
            s = ErrorUndefinedVarNameText + "'" + UndefinedVarName + "'"; break;
        default:                                    break;

    }
    return s;
}
