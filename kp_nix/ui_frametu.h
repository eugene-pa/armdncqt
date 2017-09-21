/********************************************************************************
** Form generated from reading UI file 'frametu.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMETU_H
#define UI_FRAMETU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FrameTU
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTableWidget *table_TU;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QTableWidget *table_SYS;

    void setupUi(QFrame *FrameTU)
    {
        if (FrameTU->objectName().isEmpty())
            FrameTU->setObjectName(QStringLiteral("FrameTU"));
        FrameTU->resize(351, 303);
        FrameTU->setFrameShape(QFrame::StyledPanel);
        FrameTU->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(FrameTU);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(2, 2, 2, 2);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(FrameTU);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label, 0, Qt::AlignHCenter);

        table_TU = new QTableWidget(FrameTU);
        if (table_TU->columnCount() < 4)
            table_TU->setColumnCount(4);
        table_TU->setObjectName(QStringLiteral("table_TU"));
        table_TU->setEnabled(false);
        QFont font;
        font.setPointSize(9);
        table_TU->setFont(font);
        table_TU->setShowGrid(false);
        table_TU->setColumnCount(4);
        table_TU->horizontalHeader()->setDefaultSectionSize(45);
        table_TU->horizontalHeader()->setStretchLastSection(true);
        table_TU->verticalHeader()->setVisible(false);
        table_TU->verticalHeader()->setDefaultSectionSize(14);
        table_TU->verticalHeader()->setMinimumSectionSize(14);

        verticalLayout->addWidget(table_TU);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_2 = new QLabel(FrameTU);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_2->addWidget(label_2, 0, Qt::AlignHCenter);

        table_SYS = new QTableWidget(FrameTU);
        table_SYS->setObjectName(QStringLiteral("table_SYS"));
        table_SYS->setEnabled(false);

        verticalLayout_2->addWidget(table_SYS);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalLayout->setStretch(0, 3);
        horizontalLayout->setStretch(1, 2);

        verticalLayout_3->addLayout(horizontalLayout);


        gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);


        retranslateUi(FrameTU);

        QMetaObject::connectSlotsByName(FrameTU);
    } // setupUi

    void retranslateUi(QFrame *FrameTU)
    {
        FrameTU->setWindowTitle(QApplication::translate("FrameTU", "Frame", Q_NULLPTR));
        label->setText(QApplication::translate("FrameTU", "\320\242\320\243", Q_NULLPTR));
        label_2->setText(QApplication::translate("FrameTU", "\320\224\320\270\321\200\320\265\320\272\321\202\320\270\320\262\321\213", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FrameTU: public Ui_FrameTU {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMETU_H
