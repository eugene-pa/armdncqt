/********************************************************************************
** Form generated from reading UI file 'farmeline.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FARMELINE_H
#define UI_FARMELINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qled.h"

QT_BEGIN_NAMESPACE

class Ui_FarmeLine
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLed *label_COM;
    QLabel *label_20;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_13;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_15;
    QLabel *label_16;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_17;
    QLabel *label_19;
    QLabel *label_18;

    void setupUi(QGroupBox *FarmeLine)
    {
        if (FarmeLine->objectName().isEmpty())
            FarmeLine->setObjectName(QStringLiteral("FarmeLine"));
        FarmeLine->resize(390, 100);
        FarmeLine->setMinimumSize(QSize(390, 100));
        FarmeLine->setMaximumSize(QSize(390, 120));
        layoutWidget = new QWidget(FarmeLine);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 10, 381, 81));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 4, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_COM = new QLed(layoutWidget);
        label_COM->setObjectName(QStringLiteral("label_COM"));
        label_COM->setMinimumSize(QSize(11, 11));
        label_COM->setMaximumSize(QSize(11, 11));
        label_COM->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout->addWidget(label_COM);

        label_20 = new QLabel(layoutWidget);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setMinimumSize(QSize(0, 0));
        label_20->setBaseSize(QSize(60, 0));

        horizontalLayout->addWidget(label_20);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(0, 0));

        horizontalLayout->addWidget(label_3);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout->addWidget(label_4);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout->addWidget(label_5);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout->addWidget(label_6);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout->addWidget(label_7);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout->addWidget(label_8);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setMinimumSize(QSize(60, 0));

        horizontalLayout_2->addWidget(label_9);

        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setMinimumSize(QSize(140, 0));

        horizontalLayout_2->addWidget(label_10);

        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_2->addWidget(label_11);

        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_2->addWidget(label_12);

        horizontalLayout_2->setStretch(2, 1);

        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(2);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName(QStringLiteral("label_13"));

        horizontalLayout_3->addWidget(label_13);

        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_3->addWidget(label_14);

        horizontalLayout_3->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_15 = new QLabel(layoutWidget);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_4->addWidget(label_15);

        label_16 = new QLabel(layoutWidget);
        label_16->setObjectName(QStringLiteral("label_16"));

        horizontalLayout_4->addWidget(label_16);

        horizontalLayout_4->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_17 = new QLabel(layoutWidget);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setMinimumSize(QSize(60, 0));

        horizontalLayout_5->addWidget(label_17);

        label_19 = new QLabel(layoutWidget);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setMinimumSize(QSize(140, 0));

        horizontalLayout_5->addWidget(label_19);

        label_18 = new QLabel(layoutWidget);
        label_18->setObjectName(QStringLiteral("label_18"));

        horizontalLayout_5->addWidget(label_18);

        horizontalLayout_5->setStretch(2, 1);

        verticalLayout->addLayout(horizontalLayout_5);


        retranslateUi(FarmeLine);

        QMetaObject::connectSlotsByName(FarmeLine);
    } // setupUi

    void retranslateUi(QGroupBox *FarmeLine)
    {
        FarmeLine->setWindowTitle(QApplication::translate("FarmeLine", "GroupBox", Q_NULLPTR));
        FarmeLine->setTitle(QApplication::translate("FarmeLine", "\320\234\320\276\320\264\320\265\320\274 COM3 (\320\277\320\260\321\201)", Q_NULLPTR));
        label_COM->setText(QString());
        label_20->setText(QApplication::translate("FarmeLine", "V=57600", Q_NULLPTR));
        label_3->setText(QApplication::translate("FarmeLine", "\320\240\320\260\320\267\321\200\321\213\320\262\321\213:", Q_NULLPTR));
        label_4->setText(QApplication::translate("FarmeLine", "0", Q_NULLPTR));
        label_5->setText(QApplication::translate("FarmeLine", "\320\236\321\210\320\261:", Q_NULLPTR));
        label_6->setText(QApplication::translate("FarmeLine", "0", Q_NULLPTR));
        label_7->setText(QApplication::translate("FarmeLine", "\320\242\320\270\320\277 \320\276\321\210\320\261:", Q_NULLPTR));
        label_8->setText(QApplication::translate("FarmeLine", "...", Q_NULLPTR));
        label_9->setText(QApplication::translate("FarmeLine", "0/0", Q_NULLPTR));
        label_10->setText(QApplication::translate("FarmeLine", "\320\241=00 \320\242\320\243=00 \320\224=00 \320\236=00", Q_NULLPTR));
        label_11->setText(QApplication::translate("FarmeLine", "0->5", Q_NULLPTR));
        label_12->setText(QApplication::translate("FarmeLine", "t=23:59:59 10.02", Q_NULLPTR));
        label_13->setText(QApplication::translate("FarmeLine", "\320\237\320\240\320\234:", Q_NULLPTR));
        label_14->setText(QApplication::translate("FarmeLine", "...", Q_NULLPTR));
        label_15->setText(QApplication::translate("FarmeLine", "\320\237\320\240\320\224:", Q_NULLPTR));
        label_16->setText(QApplication::translate("FarmeLine", "...", Q_NULLPTR));
        label_17->setText(QApplication::translate("FarmeLine", "0/0", Q_NULLPTR));
        label_19->setText(QApplication::translate("FarmeLine", "\320\241=0 \320\242\320\243=0 \320\224=0 \320\236=0", Q_NULLPTR));
        label_18->setText(QApplication::translate("FarmeLine", "0->5", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FarmeLine: public Ui_FarmeLine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FARMELINE_H
