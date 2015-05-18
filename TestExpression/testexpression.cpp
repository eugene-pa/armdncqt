#include <QString>
#include <QtTest/QtTest>

#include "../common/boolexpression.h"
#include "testexpression.h"

TestExpression::TestExpression()
{

}

TestExpression::~TestExpression()
{

}

void TestExpression::GetValue()
{
    QString s1("100+50");
    BoolExpression ex1(s1, nullptr, true);
    QCOMPARE(ex1.GetValue(), 150);

    QString s2("100-50");
    BoolExpression ex2(s2, nullptr, true);
    QCOMPARE(ex2.GetValue(), 50);

    QString s3("10*50");
    BoolExpression ex3(s3, nullptr, true);
    QCOMPARE(ex3.GetValue(), 500);

    QString s4("50/10");
    BoolExpression ex4(s4, nullptr, true);
    QCOMPARE(ex4.GetValue(), 5);

}
