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

    QCOMPARE(BoolExpression("100+50" , nullptr, true).GetValue(), 150);
    QCOMPARE(BoolExpression("100-50" , nullptr, true).GetValue(),  50);
    QCOMPARE(BoolExpression("100*50" , nullptr, true).GetValue(),5000);
    QCOMPARE(BoolExpression("100/50" , nullptr, true).GetValue(),   2);
    QCOMPARE(BoolExpression("100>50" , nullptr, true).GetValue(),   1);
    QCOMPARE(BoolExpression("100>101", nullptr, true).GetValue(),   0);
    QCOMPARE(BoolExpression("100<50" , nullptr, true).GetValue(),   0);
    QCOMPARE(BoolExpression("100<101", nullptr, true).GetValue(),   1);
    QCOMPARE(BoolExpression("100=50" , nullptr, true).GetValue(),   0);
    QCOMPARE(BoolExpression("50=50"  , nullptr, true).GetValue(),   1);
    QCOMPARE(BoolExpression("(1+5)*5-6/2" , nullptr, true).GetValue(), 27);

    QCOMPARE(BoolExpression(" ( 1 + 5 ) * 5 - 6 / 2 " , nullptr, true).GetValue(), 27);


    QCOMPARE(BoolExpression("1&1"  , nullptr, false).GetValue(),   1);
    QCOMPARE(BoolExpression("0&1"  , nullptr, false).GetValue(),   0);
    QCOMPARE(BoolExpression("1&0"  , nullptr, false).GetValue(),   0);
    QCOMPARE(BoolExpression("!0&1" , nullptr, false).GetValue(),   1);
    QCOMPARE(BoolExpression("0|1"  , nullptr, false).GetValue(),    1);
    QCOMPARE(BoolExpression("1|0"  , nullptr, false).GetValue(),    1);
    QCOMPARE(BoolExpression("0|0"  , nullptr, false).GetValue(),    0);
    QCOMPARE(BoolExpression("1^0"  , nullptr, false).GetValue(),    1);
    QCOMPARE(BoolExpression("0^1"  , nullptr, false).GetValue(),    1);
    QCOMPARE(BoolExpression("0^0"  , nullptr, false).GetValue(),    0);
    QCOMPARE(BoolExpression("0^0"  , nullptr, false).GetValue(),    0);
    QCOMPARE(BoolExpression("(0|1)&1", nullptr, false).GetValue(),  1);

    QCOMPARE(BoolExpression("1&2АП"   , nullptr, false).GetValue(),   0);
    QCOMPARE(BoolExpression("1|2АП"   , nullptr, false).GetValue(),   1);
}
/*
void TestExpression::GetValue_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("+") << "100+50" << "150";
}
*/
