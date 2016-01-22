#ifndef BOOLEXPRESSION_H
#define BOOLEXPRESSION_H

#include <QObject>
#include <QString>

class BoolExpression : public QObject
{
    Q_OBJECT

public:
    enum Error
    {
        OK                  = 0,
        OpenBracketMissing  = 1,
        CloseBracketMissing = 2,
        EmptyExpr           = 3,
        OperandMissing      = 4,
        OperatorMissing     = 5,
        RealtimeError       = 6,                            // ошибка во время вычисления выражения
        GetVariableError    = 7,                            // ошибка вычисления переменной
        Unknown,
    };

    // Для разбора выражений используются регулярные выражения в двух версиях: арифметической и логической
    // РЕГУЛЯРНОЕ ВЫРАЖЕНИЕ ОПИСЫВЕТ ЛЕКСЕМЫ СЛЕДУЮЩЕГО ВИДА
    // ! & ( ) + - * / | < > =
    // ([A-z0-9_А-я#\[\]]*)
    // (\d+\.?\d*)
    static QRegularExpression LexemRgxMath; // = new Regex(@"\^|\<|\>|\!|\&|\(|\)|\+|\-|\*|\/|\||=|([.A-Za-z0-9_~#А-я\[\]]*)");
    static QRegularExpression LexemRgxLogic;// = new Regex(@"\^|\!|\&|\(|\)|\||([.A-Za-z0-9_~#\-\+\/\*А-я\[\]]*)");

    static QRegularExpression OperatorRgxMath; //  = new Regex(@"^[=\<\>\!\&\^\|\+\-\*\/]$");
    static QRegularExpression OperatorRgxLogic;// = new Regex(@"^[!\&\^\|]$");

    //BoolExpression(QString expr, class Station* pst, bool fullMathOps = false);
    BoolExpression(QString& expr, bool fullMathOps = false);
    //BoolExpression(QString  expr, bool fullMathOps = false);


    ~BoolExpression();

    int GetValue();                                         // получить значение
    QString Source()  { return source; }
    bool    Valid()   { return error == OK; }
    bool    ValueBool(){ return GetValue() == 0 ? false : true; } // булево значение
    bool    LogicalType() { return logicalType; }
    QString ErrorText();                                    // получить текст ошибки

    // вычисление переменной - через обработку сигнала в слоте
signals:
    void GetVar(QString& name, int& ret);

private:
    QString source;                                         // исходная строка
    QString rpnStr;                                         // строка в польской обратной записи
    bool    logicalType;
    class Logger * logger;

//  public delegate int GetVarDelegate (string xpr);        // тип делегата
//  GetVarDelegate GetVar;                                  // функция делегата

    QString UndefinedVarName;                               // имя неопределенной переменной
    QString ErrorUndefinedVarNameText;                      // текст исключения
    Error error;                                            // тип ошибки

    void ToRpn();
    bool IsOperator (QString s);                            // лексема - оператор?
    bool IsUnary(QString s);                                // проверка унарности оператора
    bool IsValuable (QString s);                            // возможные лексемы: операторы, круглые скобки и переменные или константы; все, что не оператор и не скобки - переменные
    bool IsInteger (QString s);                             // лексема - целое ?
    bool IsOperator (char c);                               // перегруженная функция проверки сивола на принадлежность к символу операций
    int  GetPriority (QString s);                           // приоритет операций
    int  GetTopPriority (QStack<QString>& stack);           // приоритет операции на вершине стека
};

#endif // BOOLEXPRESSION_H
