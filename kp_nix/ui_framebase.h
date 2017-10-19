/********************************************************************************
** Form generated from reading UI file 'framebase.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMEBASE_H
#define UI_FRAMEBASE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include "qled.h"

QT_BEGIN_NAMESPACE

class Ui_FrameBase
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *labelAddr;
    QLabel *labelMainRsrv;
    QLabel *labelBMStatus;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelAddr_2;
    QLed *labelReady;
    QHBoxLayout *horizontalLayout_9;
    QLabel *labelAddr_3;
    QLed *labelBypass;
    QHBoxLayout *horizontalLayout_10;
    QLabel *labelAddr_4;
    QLed *labelReady2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_Seans;
    QLabel *label_AllSeans;
    QLabel *label_Restart;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_Mku160;
    QLabel *label_Mku161;
    QLabel *label_Mku162;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_11;
    QLabel *label_4;
    QLabel *label_5;
    QVBoxLayout *verticalLayout_12;
    QLabel *label_12;
    QLabel *label_13;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLabel *label_10;
    QLed *label_testMTU;
    QLabel *label_11;
    QLed *label_testMTS;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_14;
    QLabel *label_15;
    QHBoxLayout *horizontalLayout_7;
    QLed *label_Watchdog;
    QLabel *label_17;
    QLabel *label_18;

    void setupUi(QFrame *FrameBase)
    {
        if (FrameBase->objectName().isEmpty())
            FrameBase->setObjectName(QStringLiteral("FrameBase"));
        FrameBase->resize(790, 61);
        FrameBase->setMinimumSize(QSize(0, 60));
        FrameBase->setMaximumSize(QSize(790, 61));
        FrameBase->setFrameShape(QFrame::StyledPanel);
        FrameBase->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(FrameBase);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 4, -1, 4);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(4, 0, -1, 0);
        labelAddr = new QLabel(FrameBase);
        labelAddr->setObjectName(QStringLiteral("labelAddr"));

        verticalLayout->addWidget(labelAddr);

        labelMainRsrv = new QLabel(FrameBase);
        labelMainRsrv->setObjectName(QStringLiteral("labelMainRsrv"));

        verticalLayout->addWidget(labelMainRsrv);

        labelBMStatus = new QLabel(FrameBase);
        labelBMStatus->setObjectName(QStringLiteral("labelBMStatus"));

        verticalLayout->addWidget(labelBMStatus);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(2);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(10, -1, -1, -1);
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        labelAddr_2 = new QLabel(FrameBase);
        labelAddr_2->setObjectName(QStringLiteral("labelAddr_2"));

        horizontalLayout_5->addWidget(labelAddr_2, 0, Qt::AlignRight);

        labelReady = new QLed(FrameBase);
        labelReady->setObjectName(QStringLiteral("labelReady"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelReady->sizePolicy().hasHeightForWidth());
        labelReady->setSizePolicy(sizePolicy);
        labelReady->setBaseSize(QSize(11, 11));
        labelReady->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_5->addWidget(labelReady);


        verticalLayout_8->addLayout(horizontalLayout_5);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(2);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        labelAddr_3 = new QLabel(FrameBase);
        labelAddr_3->setObjectName(QStringLiteral("labelAddr_3"));

        horizontalLayout_9->addWidget(labelAddr_3, 0, Qt::AlignRight);

        labelBypass = new QLed(FrameBase);
        labelBypass->setObjectName(QStringLiteral("labelBypass"));
        sizePolicy.setHeightForWidth(labelBypass->sizePolicy().hasHeightForWidth());
        labelBypass->setSizePolicy(sizePolicy);
        labelBypass->setBaseSize(QSize(11, 11));
        labelBypass->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_9->addWidget(labelBypass);


        verticalLayout_8->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        labelAddr_4 = new QLabel(FrameBase);
        labelAddr_4->setObjectName(QStringLiteral("labelAddr_4"));

        horizontalLayout_10->addWidget(labelAddr_4);

        labelReady2 = new QLed(FrameBase);
        labelReady2->setObjectName(QStringLiteral("labelReady2"));
        sizePolicy.setHeightForWidth(labelReady2->sizePolicy().hasHeightForWidth());
        labelReady2->setSizePolicy(sizePolicy);
        labelReady2->setBaseSize(QSize(11, 11));
        labelReady2->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_10->addWidget(labelReady2);


        verticalLayout_8->addLayout(horizontalLayout_10);


        horizontalLayout_3->addLayout(verticalLayout_8);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, -1, -1, -1);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label = new QLabel(FrameBase);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_4->addWidget(label, 0, Qt::AlignRight);

        label_2 = new QLabel(FrameBase);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_4->addWidget(label_2, 0, Qt::AlignRight);

        label_3 = new QLabel(FrameBase);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_4->addWidget(label_3, 0, Qt::AlignRight);


        horizontalLayout->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_Seans = new QLabel(FrameBase);
        label_Seans->setObjectName(QStringLiteral("label_Seans"));

        verticalLayout_5->addWidget(label_Seans);

        label_AllSeans = new QLabel(FrameBase);
        label_AllSeans->setObjectName(QStringLiteral("label_AllSeans"));

        verticalLayout_5->addWidget(label_AllSeans);

        label_Restart = new QLabel(FrameBase);
        label_Restart->setObjectName(QStringLiteral("label_Restart"));

        verticalLayout_5->addWidget(label_Restart);


        horizontalLayout->addLayout(verticalLayout_5);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(4);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(20, -1, -1, -1);
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(2);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        label_7 = new QLabel(FrameBase);
        label_7->setObjectName(QStringLiteral("label_7"));

        verticalLayout_6->addWidget(label_7, 0, Qt::AlignRight);

        label_8 = new QLabel(FrameBase);
        label_8->setObjectName(QStringLiteral("label_8"));

        verticalLayout_6->addWidget(label_8, 0, Qt::AlignRight);

        label_9 = new QLabel(FrameBase);
        label_9->setObjectName(QStringLiteral("label_9"));

        verticalLayout_6->addWidget(label_9, 0, Qt::AlignRight);


        horizontalLayout_2->addLayout(verticalLayout_6);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(2);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        label_Mku160 = new QLabel(FrameBase);
        label_Mku160->setObjectName(QStringLiteral("label_Mku160"));

        verticalLayout_7->addWidget(label_Mku160);

        label_Mku161 = new QLabel(FrameBase);
        label_Mku161->setObjectName(QStringLiteral("label_Mku161"));

        verticalLayout_7->addWidget(label_Mku161);

        label_Mku162 = new QLabel(FrameBase);
        label_Mku162->setObjectName(QStringLiteral("label_Mku162"));

        verticalLayout_7->addWidget(label_Mku162);


        horizontalLayout_2->addLayout(verticalLayout_7);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(2);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        verticalLayout_13->setContentsMargins(20, -1, -1, -1);
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(2);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        label_4 = new QLabel(FrameBase);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_11->addWidget(label_4);

        label_5 = new QLabel(FrameBase);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_11->addWidget(label_5);


        horizontalLayout_8->addLayout(verticalLayout_11);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(2);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        label_12 = new QLabel(FrameBase);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_12->addWidget(label_12);

        label_13 = new QLabel(FrameBase);
        label_13->setObjectName(QStringLiteral("label_13"));

        verticalLayout_12->addWidget(label_13);


        horizontalLayout_8->addLayout(verticalLayout_12);


        verticalLayout_13->addLayout(horizontalLayout_8);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_6 = new QLabel(FrameBase);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_4->addWidget(label_6);

        label_10 = new QLabel(FrameBase);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_4->addWidget(label_10);

        label_testMTU = new QLed(FrameBase);
        label_testMTU->setObjectName(QStringLiteral("label_testMTU"));
        sizePolicy.setHeightForWidth(label_testMTU->sizePolicy().hasHeightForWidth());
        label_testMTU->setSizePolicy(sizePolicy);
        label_testMTU->setBaseSize(QSize(11, 11));
        label_testMTU->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_4->addWidget(label_testMTU);

        label_11 = new QLabel(FrameBase);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_4->addWidget(label_11);

        label_testMTS = new QLed(FrameBase);
        label_testMTS->setObjectName(QStringLiteral("label_testMTS"));
        sizePolicy.setHeightForWidth(label_testMTS->sizePolicy().hasHeightForWidth());
        label_testMTS->setSizePolicy(sizePolicy);
        label_testMTS->setBaseSize(QSize(11, 11));
        label_testMTS->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_4->addWidget(label_testMTS);


        verticalLayout_13->addLayout(horizontalLayout_4);


        horizontalLayout_3->addLayout(verticalLayout_13);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(20, -1, 20, -1);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_14 = new QLabel(FrameBase);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_6->addWidget(label_14, 0, Qt::AlignRight);

        label_15 = new QLabel(FrameBase);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_6->addWidget(label_15);


        verticalLayout_10->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_Watchdog = new QLed(FrameBase);
        label_Watchdog->setObjectName(QStringLiteral("label_Watchdog"));
        sizePolicy.setHeightForWidth(label_Watchdog->sizePolicy().hasHeightForWidth());
        label_Watchdog->setSizePolicy(sizePolicy);
        label_Watchdog->setBaseSize(QSize(11, 11));
        label_Watchdog->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        horizontalLayout_7->addWidget(label_Watchdog);

        label_17 = new QLabel(FrameBase);
        label_17->setObjectName(QStringLiteral("label_17"));

        horizontalLayout_7->addWidget(label_17);


        verticalLayout_10->addLayout(horizontalLayout_7);


        horizontalLayout_3->addLayout(verticalLayout_10);

        label_18 = new QLabel(FrameBase);
        label_18->setObjectName(QStringLiteral("label_18"));
        sizePolicy.setHeightForWidth(label_18->sizePolicy().hasHeightForWidth());
        label_18->setSizePolicy(sizePolicy);
        label_18->setBaseSize(QSize(32, 32));
        label_18->setPixmap(QPixmap(QString::fromUtf8(":/images/images/icon1.ico")));

        horizontalLayout_3->addWidget(label_18);


        retranslateUi(FrameBase);

        QMetaObject::connectSlotsByName(FrameBase);
    } // setupUi

    void retranslateUi(QFrame *FrameBase)
    {
        FrameBase->setWindowTitle(QApplication::translate("FrameBase", "Frame", Q_NULLPTR));
        labelAddr->setText(QApplication::translate("FrameBase", "\320\220\320\264\321\200\320\265\321\201: 1", Q_NULLPTR));
        labelMainRsrv->setText(QApplication::translate("FrameBase", "\320\236\321\201\320\275\320\276\320\262\320\275\320\276\320\271", Q_NULLPTR));
        labelBMStatus->setText(QApplication::translate("FrameBase", "\320\220\320\272\321\202\320\270\320\262\320\265\320\275", Q_NULLPTR));
        labelAddr_2->setText(QApplication::translate("FrameBase", "\320\223\320\276\321\202\320\276\320\262", Q_NULLPTR));
        labelReady->setText(QString());
        labelAddr_3->setText(QApplication::translate("FrameBase", "\320\242\321\200\320\260\320\275\320\267\320\270\321\202", Q_NULLPTR));
        labelBypass->setText(QString());
        labelAddr_4->setText(QApplication::translate("FrameBase", "\320\241\320\276\321\201\320\265\320\264\320\275\320\270\320\271", Q_NULLPTR));
        labelReady2->setText(QString());
        label->setText(QApplication::translate("FrameBase", "\320\241\320\265\320\260\320\275\321\201", Q_NULLPTR));
        label_2->setText(QApplication::translate("FrameBase", "\320\236\320\261\321\211.\321\201\320\265\320\260\320\275\321\201", Q_NULLPTR));
        label_3->setText(QApplication::translate("FrameBase", "\320\237\320\265\321\200\320\265\320\267\320\260\320\277\321\203\321\201\320\272\320\276\320\262", Q_NULLPTR));
        label_Seans->setText(QApplication::translate("FrameBase", "3", Q_NULLPTR));
        label_AllSeans->setText(QApplication::translate("FrameBase", "33/34", Q_NULLPTR));
        label_Restart->setText(QApplication::translate("FrameBase", "3", Q_NULLPTR));
        label_7->setText(QApplication::translate("FrameBase", "\320\234\320\232\320\243 160:", Q_NULLPTR));
        label_8->setText(QApplication::translate("FrameBase", "\320\234\320\232\320\243 161:", Q_NULLPTR));
        label_9->setText(QApplication::translate("FrameBase", "\320\234\320\232\320\243 162:", Q_NULLPTR));
        label_Mku160->setText(QApplication::translate("FrameBase", "00000011", Q_NULLPTR));
        label_Mku161->setText(QApplication::translate("FrameBase", "00000001", Q_NULLPTR));
        label_Mku162->setText(QApplication::translate("FrameBase", "00001001", Q_NULLPTR));
        label_4->setText(QApplication::translate("FrameBase", "\320\234\320\222\320\222 111", Q_NULLPTR));
        label_5->setText(QApplication::translate("FrameBase", "\320\234\320\222\320\222 131", Q_NULLPTR));
        label_12->setText(QApplication::translate("FrameBase", "00000110", Q_NULLPTR));
        label_13->setText(QApplication::translate("FrameBase", "00000101", Q_NULLPTR));
        label_6->setText(QApplication::translate("FrameBase", "\320\242\320\265\321\201\321\202:   ", Q_NULLPTR));
        label_10->setText(QApplication::translate("FrameBase", "\320\234\320\242\320\243", Q_NULLPTR));
        label_testMTU->setText(QString());
        label_11->setText(QApplication::translate("FrameBase", "\320\234\320\242\320\241", Q_NULLPTR));
        label_testMTS->setText(QString());
        label_14->setText(QApplication::translate("FrameBase", "\320\237\320\260\320\274\321\217\321\202\321\214", Q_NULLPTR));
        label_15->setText(QApplication::translate("FrameBase", "000000000000", Q_NULLPTR));
        label_Watchdog->setText(QString());
        label_17->setText(QApplication::translate("FrameBase", "\320\241\321\202\320\276\321\200\320\276\320\266\320\265\320\262\320\276\320\271 \321\202\320\260\320\271\320\274\320\265\321\200", Q_NULLPTR));
        label_18->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FrameBase: public Ui_FrameBase {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEBASE_H
