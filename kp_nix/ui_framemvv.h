/********************************************************************************
** Form generated from reading UI file 'framemvv.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMEMVV_H
#define UI_FRAMEMVV_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "qled.h"

QT_BEGIN_NAMESPACE

class Ui_FrameMVV
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_MVV;
    QLabel *label_7;
    QLabel *label_BT1;
    QLabel *label_6;
    QLed *label_M4;
    QLabel *label_BT1_beg;
    QLed *label_M6;
    QLed *label_M2;
    QLed *label_M8;
    QLabel *label_BT2;
    QLed *label_M3;
    QLabel *label_4;
    QLed *label_M1;
    QLed *label_M7;
    QLabel *label_BT2_beg;
    QLabel *label_8;
    QLed *label_M5;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label;
    QLed *label_M20;
    QLed *label_M19;
    QLed *label_M21;
    QLed *label_M23;
    QLed *label_M9;
    QLed *label_M13;
    QLed *label_M22;
    QLed *label_M11;
    QLed *label_M16;
    QLed *label_M24;
    QLed *label_M17;
    QLed *label_M10;
    QLed *label_M15;
    QLabel *label_BT3;
    QLabel *label_BT3_beg;
    QLed *label_M18;
    QLed *label_M14;
    QLed *label_M12;

    void setupUi(QFrame *FrameMVV)
    {
        if (FrameMVV->objectName().isEmpty())
            FrameMVV->setObjectName(QStringLiteral("FrameMVV"));
        FrameMVV->resize(168, 64);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FrameMVV->sizePolicy().hasHeightForWidth());
        FrameMVV->setSizePolicy(sizePolicy);
        FrameMVV->setMinimumSize(QSize(168, 64));
        FrameMVV->setFrameShape(QFrame::StyledPanel);
        FrameMVV->setFrameShadow(QFrame::Raised);
        layoutWidget = new QWidget(FrameMVV);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 164, 61));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(4);
        gridLayout->setVerticalSpacing(2);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_MVV = new QLabel(layoutWidget);
        label_MVV->setObjectName(QStringLiteral("label_MVV"));

        gridLayout->addWidget(label_MVV, 0, 0, 1, 1, Qt::AlignRight);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 0, 8, 1, 1);

        label_BT1 = new QLabel(layoutWidget);
        label_BT1->setObjectName(QStringLiteral("label_BT1"));

        gridLayout->addWidget(label_BT1, 1, 0, 1, 1, Qt::AlignRight);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 0, 7, 1, 1);

        label_M4 = new QLed(layoutWidget);
        label_M4->setObjectName(QStringLiteral("label_M4"));
        sizePolicy.setHeightForWidth(label_M4->sizePolicy().hasHeightForWidth());
        label_M4->setSizePolicy(sizePolicy);
        label_M4->setBaseSize(QSize(11, 11));
        label_M4->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M4, 1, 5, 1, 1);

        label_BT1_beg = new QLabel(layoutWidget);
        label_BT1_beg->setObjectName(QStringLiteral("label_BT1_beg"));

        gridLayout->addWidget(label_BT1_beg, 1, 1, 1, 1, Qt::AlignRight);

        label_M6 = new QLed(layoutWidget);
        label_M6->setObjectName(QStringLiteral("label_M6"));
        sizePolicy.setHeightForWidth(label_M6->sizePolicy().hasHeightForWidth());
        label_M6->setSizePolicy(sizePolicy);
        label_M6->setBaseSize(QSize(11, 11));
        label_M6->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M6, 1, 7, 1, 1);

        label_M2 = new QLed(layoutWidget);
        label_M2->setObjectName(QStringLiteral("label_M2"));
        sizePolicy.setHeightForWidth(label_M2->sizePolicy().hasHeightForWidth());
        label_M2->setSizePolicy(sizePolicy);
        label_M2->setBaseSize(QSize(11, 11));
        label_M2->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M2, 1, 3, 1, 1);

        label_M8 = new QLed(layoutWidget);
        label_M8->setObjectName(QStringLiteral("label_M8"));
        sizePolicy.setHeightForWidth(label_M8->sizePolicy().hasHeightForWidth());
        label_M8->setSizePolicy(sizePolicy);
        label_M8->setBaseSize(QSize(11, 11));
        label_M8->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M8, 1, 9, 1, 1);

        label_BT2 = new QLabel(layoutWidget);
        label_BT2->setObjectName(QStringLiteral("label_BT2"));

        gridLayout->addWidget(label_BT2, 2, 0, 1, 1, Qt::AlignRight);

        label_M3 = new QLed(layoutWidget);
        label_M3->setObjectName(QStringLiteral("label_M3"));
        sizePolicy.setHeightForWidth(label_M3->sizePolicy().hasHeightForWidth());
        label_M3->setSizePolicy(sizePolicy);
        label_M3->setBaseSize(QSize(11, 11));
        label_M3->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M3, 1, 4, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 5, 1, 1);

        label_M1 = new QLed(layoutWidget);
        label_M1->setObjectName(QStringLiteral("label_M1"));
        sizePolicy.setHeightForWidth(label_M1->sizePolicy().hasHeightForWidth());
        label_M1->setSizePolicy(sizePolicy);
        label_M1->setBaseSize(QSize(11, 11));
        label_M1->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M1, 1, 2, 1, 1);

        label_M7 = new QLed(layoutWidget);
        label_M7->setObjectName(QStringLiteral("label_M7"));
        sizePolicy.setHeightForWidth(label_M7->sizePolicy().hasHeightForWidth());
        label_M7->setSizePolicy(sizePolicy);
        label_M7->setBaseSize(QSize(11, 11));
        label_M7->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M7, 1, 8, 1, 1);

        label_BT2_beg = new QLabel(layoutWidget);
        label_BT2_beg->setObjectName(QStringLiteral("label_BT2_beg"));

        gridLayout->addWidget(label_BT2_beg, 2, 1, 1, 1, Qt::AlignRight);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 0, 9, 1, 1);

        label_M5 = new QLed(layoutWidget);
        label_M5->setObjectName(QStringLiteral("label_M5"));
        sizePolicy.setHeightForWidth(label_M5->sizePolicy().hasHeightForWidth());
        label_M5->setSizePolicy(sizePolicy);
        label_M5->setBaseSize(QSize(11, 11));
        label_M5->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M5, 1, 6, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 3, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 0, 4, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 0, 6, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 2, 1, 1);

        label_M20 = new QLed(layoutWidget);
        label_M20->setObjectName(QStringLiteral("label_M20"));
        sizePolicy.setHeightForWidth(label_M20->sizePolicy().hasHeightForWidth());
        label_M20->setSizePolicy(sizePolicy);
        label_M20->setBaseSize(QSize(11, 11));
        label_M20->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M20, 3, 5, 1, 1);

        label_M19 = new QLed(layoutWidget);
        label_M19->setObjectName(QStringLiteral("label_M19"));
        sizePolicy.setHeightForWidth(label_M19->sizePolicy().hasHeightForWidth());
        label_M19->setSizePolicy(sizePolicy);
        label_M19->setBaseSize(QSize(11, 11));
        label_M19->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M19, 3, 4, 1, 1);

        label_M21 = new QLed(layoutWidget);
        label_M21->setObjectName(QStringLiteral("label_M21"));
        sizePolicy.setHeightForWidth(label_M21->sizePolicy().hasHeightForWidth());
        label_M21->setSizePolicy(sizePolicy);
        label_M21->setBaseSize(QSize(11, 11));
        label_M21->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M21, 3, 6, 1, 1);

        label_M23 = new QLed(layoutWidget);
        label_M23->setObjectName(QStringLiteral("label_M23"));
        sizePolicy.setHeightForWidth(label_M23->sizePolicy().hasHeightForWidth());
        label_M23->setSizePolicy(sizePolicy);
        label_M23->setBaseSize(QSize(11, 11));
        label_M23->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M23, 3, 8, 1, 1);

        label_M9 = new QLed(layoutWidget);
        label_M9->setObjectName(QStringLiteral("label_M9"));
        sizePolicy.setHeightForWidth(label_M9->sizePolicy().hasHeightForWidth());
        label_M9->setSizePolicy(sizePolicy);
        label_M9->setBaseSize(QSize(11, 11));
        label_M9->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M9, 2, 2, 1, 1);

        label_M13 = new QLed(layoutWidget);
        label_M13->setObjectName(QStringLiteral("label_M13"));
        sizePolicy.setHeightForWidth(label_M13->sizePolicy().hasHeightForWidth());
        label_M13->setSizePolicy(sizePolicy);
        label_M13->setBaseSize(QSize(11, 11));
        label_M13->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M13, 2, 6, 1, 1);

        label_M22 = new QLed(layoutWidget);
        label_M22->setObjectName(QStringLiteral("label_M22"));
        sizePolicy.setHeightForWidth(label_M22->sizePolicy().hasHeightForWidth());
        label_M22->setSizePolicy(sizePolicy);
        label_M22->setBaseSize(QSize(11, 11));
        label_M22->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M22, 3, 7, 1, 1);

        label_M11 = new QLed(layoutWidget);
        label_M11->setObjectName(QStringLiteral("label_M11"));
        sizePolicy.setHeightForWidth(label_M11->sizePolicy().hasHeightForWidth());
        label_M11->setSizePolicy(sizePolicy);
        label_M11->setBaseSize(QSize(11, 11));
        label_M11->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M11, 2, 4, 1, 1);

        label_M16 = new QLed(layoutWidget);
        label_M16->setObjectName(QStringLiteral("label_M16"));
        sizePolicy.setHeightForWidth(label_M16->sizePolicy().hasHeightForWidth());
        label_M16->setSizePolicy(sizePolicy);
        label_M16->setBaseSize(QSize(11, 11));
        label_M16->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M16, 2, 9, 1, 1);

        label_M24 = new QLed(layoutWidget);
        label_M24->setObjectName(QStringLiteral("label_M24"));
        sizePolicy.setHeightForWidth(label_M24->sizePolicy().hasHeightForWidth());
        label_M24->setSizePolicy(sizePolicy);
        label_M24->setBaseSize(QSize(11, 11));
        label_M24->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M24, 3, 9, 1, 1);

        label_M17 = new QLed(layoutWidget);
        label_M17->setObjectName(QStringLiteral("label_M17"));
        sizePolicy.setHeightForWidth(label_M17->sizePolicy().hasHeightForWidth());
        label_M17->setSizePolicy(sizePolicy);
        label_M17->setBaseSize(QSize(11, 11));
        label_M17->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M17, 3, 2, 1, 1);

        label_M10 = new QLed(layoutWidget);
        label_M10->setObjectName(QStringLiteral("label_M10"));
        sizePolicy.setHeightForWidth(label_M10->sizePolicy().hasHeightForWidth());
        label_M10->setSizePolicy(sizePolicy);
        label_M10->setBaseSize(QSize(11, 11));
        label_M10->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M10, 2, 3, 1, 1);

        label_M15 = new QLed(layoutWidget);
        label_M15->setObjectName(QStringLiteral("label_M15"));
        sizePolicy.setHeightForWidth(label_M15->sizePolicy().hasHeightForWidth());
        label_M15->setSizePolicy(sizePolicy);
        label_M15->setBaseSize(QSize(11, 11));
        label_M15->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M15, 2, 8, 1, 1);

        label_BT3 = new QLabel(layoutWidget);
        label_BT3->setObjectName(QStringLiteral("label_BT3"));

        gridLayout->addWidget(label_BT3, 3, 0, 1, 1, Qt::AlignRight);

        label_BT3_beg = new QLabel(layoutWidget);
        label_BT3_beg->setObjectName(QStringLiteral("label_BT3_beg"));

        gridLayout->addWidget(label_BT3_beg, 3, 1, 1, 1, Qt::AlignRight);

        label_M18 = new QLed(layoutWidget);
        label_M18->setObjectName(QStringLiteral("label_M18"));
        sizePolicy.setHeightForWidth(label_M18->sizePolicy().hasHeightForWidth());
        label_M18->setSizePolicy(sizePolicy);
        label_M18->setBaseSize(QSize(11, 11));
        label_M18->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M18, 3, 3, 1, 1);

        label_M14 = new QLed(layoutWidget);
        label_M14->setObjectName(QStringLiteral("label_M14"));
        sizePolicy.setHeightForWidth(label_M14->sizePolicy().hasHeightForWidth());
        label_M14->setSizePolicy(sizePolicy);
        label_M14->setBaseSize(QSize(11, 11));
        label_M14->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M14, 2, 7, 1, 1);

        label_M12 = new QLed(layoutWidget);
        label_M12->setObjectName(QStringLiteral("label_M12"));
        sizePolicy.setHeightForWidth(label_M12->sizePolicy().hasHeightForWidth());
        label_M12->setSizePolicy(sizePolicy);
        label_M12->setBaseSize(QSize(11, 11));
        label_M12->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_M12, 2, 5, 1, 1);


        retranslateUi(FrameMVV);

        QMetaObject::connectSlotsByName(FrameMVV);
    } // setupUi

    void retranslateUi(QFrame *FrameMVV)
    {
        FrameMVV->setWindowTitle(QApplication::translate("FrameMVV", "Frame", Q_NULLPTR));
        label_MVV->setText(QApplication::translate("FrameMVV", "\320\234\320\222\320\2221", Q_NULLPTR));
        label_7->setText(QApplication::translate("FrameMVV", "7", Q_NULLPTR));
        label_BT1->setText(QApplication::translate("FrameMVV", "\320\221\320\2421", Q_NULLPTR));
        label_6->setText(QApplication::translate("FrameMVV", "6", Q_NULLPTR));
        label_M4->setText(QString());
        label_BT1_beg->setText(QApplication::translate("FrameMVV", "1", Q_NULLPTR));
        label_M6->setText(QString());
        label_M2->setText(QString());
        label_M8->setText(QString());
        label_BT2->setText(QApplication::translate("FrameMVV", "\320\221\320\2422", Q_NULLPTR));
        label_M3->setText(QString());
        label_4->setText(QApplication::translate("FrameMVV", "4", Q_NULLPTR));
        label_M1->setText(QString());
        label_M7->setText(QString());
        label_BT2_beg->setText(QApplication::translate("FrameMVV", "9", Q_NULLPTR));
        label_8->setText(QApplication::translate("FrameMVV", "8", Q_NULLPTR));
        label_M5->setText(QString());
        label_2->setText(QApplication::translate("FrameMVV", "2", Q_NULLPTR));
        label_3->setText(QApplication::translate("FrameMVV", "3", Q_NULLPTR));
        label_5->setText(QApplication::translate("FrameMVV", "5", Q_NULLPTR));
        label->setText(QApplication::translate("FrameMVV", "1", Q_NULLPTR));
        label_M20->setText(QString());
        label_M19->setText(QString());
        label_M21->setText(QString());
        label_M23->setText(QString());
        label_M9->setText(QString());
        label_M13->setText(QString());
        label_M22->setText(QString());
        label_M11->setText(QString());
        label_M16->setText(QString());
        label_M24->setText(QString());
        label_M17->setText(QString());
        label_M10->setText(QString());
        label_M15->setText(QString());
        label_BT3->setText(QApplication::translate("FrameMVV", "\320\221\320\2423", Q_NULLPTR));
        label_BT3_beg->setText(QApplication::translate("FrameMVV", "17", Q_NULLPTR));
        label_M18->setText(QString());
        label_M14->setText(QString());
        label_M12->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FrameMVV: public Ui_FrameMVV {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEMVV_H
