/********************************************************************************
** Form generated from reading UI file 'dbgtu.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBGTU_H
#define UI_DBGTU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DbgTu
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_MTU;
    QPushButton *pushButton_RPC;
    QPushButton *pushButton_Ebl;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DbgTu)
    {
        if (DbgTu->objectName().isEmpty())
            DbgTu->setObjectName(QStringLiteral("DbgTu"));
        DbgTu->resize(400, 107);
        gridLayout = new QGridLayout(DbgTu);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_MTU = new QPushButton(DbgTu);
        pushButton_MTU->setObjectName(QStringLiteral("pushButton_MTU"));

        horizontalLayout->addWidget(pushButton_MTU);

        pushButton_RPC = new QPushButton(DbgTu);
        pushButton_RPC->setObjectName(QStringLiteral("pushButton_RPC"));

        horizontalLayout->addWidget(pushButton_RPC);

        pushButton_Ebl = new QPushButton(DbgTu);
        pushButton_Ebl->setObjectName(QStringLiteral("pushButton_Ebl"));

        horizontalLayout->addWidget(pushButton_Ebl);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DbgTu);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(DbgTu);
        QObject::connect(buttonBox, SIGNAL(accepted()), DbgTu, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DbgTu, SLOT(reject()));

        QMetaObject::connectSlotsByName(DbgTu);
    } // setupUi

    void retranslateUi(QDialog *DbgTu)
    {
        DbgTu->setWindowTitle(QApplication::translate("DbgTu", "Dialog", Q_NULLPTR));
        pushButton_MTU->setText(QApplication::translate("DbgTu", "\320\234\320\242\320\243", Q_NULLPTR));
        pushButton_RPC->setText(QApplication::translate("DbgTu", "\320\240\320\237\320\246 \320\224\320\270\320\260\320\273\320\276\320\263", Q_NULLPTR));
        pushButton_Ebl->setText(QApplication::translate("DbgTu", "Ebilock", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DbgTu: public Ui_DbgTu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBGTU_H
