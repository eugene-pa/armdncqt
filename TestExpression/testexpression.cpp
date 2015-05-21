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

    QCOMPARE(BoolExpression("100+50" , true).GetValue(), 150);
    QCOMPARE(BoolExpression("100-50" , true).GetValue(),  50);
    QCOMPARE(BoolExpression("100*50" , true).GetValue(),5000);
    QCOMPARE(BoolExpression("100/50" , true).GetValue(),   2);
    QCOMPARE(BoolExpression("100>50" , true).GetValue(),   1);
    QCOMPARE(BoolExpression("100>101", true).GetValue(),   0);
    QCOMPARE(BoolExpression("100<50" , true).GetValue(),   0);
    QCOMPARE(BoolExpression("100<101", true).GetValue(),   1);
    QCOMPARE(BoolExpression("100=50" , true).GetValue(),   0);
    QCOMPARE(BoolExpression("50=50"  , true).GetValue(),   1);
    QCOMPARE(BoolExpression("(1+5)*5-6/2" ,true).GetValue(), 27);

    QCOMPARE(BoolExpression(" ( 1 + 5 ) * 5 - 6 / 2 " , true).GetValue(), 27);


    QCOMPARE(BoolExpression("1&1"  , false).GetValue(),   1);
    QCOMPARE(BoolExpression("0&1"  , false).GetValue(),   0);
    QCOMPARE(BoolExpression("1&0"  , false).GetValue(),   0);
    QCOMPARE(BoolExpression("!0&1" , false).GetValue(),   1);
    QCOMPARE(BoolExpression("0|1"  , false).GetValue(),    1);
    QCOMPARE(BoolExpression("1|0"  , false).GetValue(),    1);
    QCOMPARE(BoolExpression("0|0"  , false).GetValue(),    0);
    QCOMPARE(BoolExpression("1^0"  , false).GetValue(),    1);
    QCOMPARE(BoolExpression("0^1"  , false).GetValue(),    1);
    QCOMPARE(BoolExpression("0^0"  , false).GetValue(),    0);
    QCOMPARE(BoolExpression("0^0"  , false).GetValue(),    0);
    QCOMPARE(BoolExpression("(0|1)&1", false).GetValue(),  1);

    QCOMPARE(BoolExpression("1&2АП"   , false).GetValue(),   0);
    QCOMPARE(BoolExpression("1|2АП"   , false).GetValue(),   1);
}
/*
void TestExpression::GetValue_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("+") << "100+50" << "150";
}
*/
