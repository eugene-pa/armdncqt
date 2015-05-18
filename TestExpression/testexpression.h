#ifndef TESTEXPRESSION_H
#define TESTEXPRESSION_H

#include <QObject>

class TestExpression : public QObject
{
    Q_OBJECT
public:
    TestExpression();
    ~TestExpression();

private slots:
    void GetValue();

};

#endif // TESTEXPRESSION_H
