/********************************************************************************
** Form generated from reading UI file 'framemvvstatus.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMEMVVSTATUS_H
#define UI_FRAMEMVVSTATUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "qled.h"

QT_BEGIN_NAMESPACE

class Ui_FrameMvvStatus
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label_12;
    QLabel *label;
    QLabel *label_2;
    QLed *label_TU1;
    QLabel *label_4;
    QLabel *label_5;
    QLed *label_OUT1;
    QLed *label_ATU1;
    QLed *label_TU2;
    QLed *label_OUT2;
    QLed *label_ATU2;
    QLabel *label_13;

    void setupUi(QGroupBox *FrameMvvStatus)
    {
        if (FrameMvvStatus->objectName().isEmpty())
            FrameMvvStatus->setObjectName(QStringLiteral("FrameMvvStatus"));
        FrameMvvStatus->resize(109, 75);
        widget = new QWidget(FrameMvvStatus);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 10, 93, 65));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(8);
        gridLayout->setVerticalSpacing(2);
        gridLayout->setContentsMargins(0, 6, 0, 0);
        label_12 = new QLabel(widget);
        label_12->setObjectName(QStringLiteral("label_12"));

        gridLayout->addWidget(label_12, 0, 0, 1, 1, Qt::AlignRight);

        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 1, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        label_TU1 = new QLed(widget);
        label_TU1->setObjectName(QStringLiteral("label_TU1"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_TU1->sizePolicy().hasHeightForWidth());
        label_TU1->setSizePolicy(sizePolicy);
        label_TU1->setMinimumSize(QSize(11, 11));
        label_TU1->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_TU1, 1, 1, 1, 1);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1, Qt::AlignRight);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1, Qt::AlignRight);

        label_OUT1 = new QLed(widget);
        label_OUT1->setObjectName(QStringLiteral("label_OUT1"));
        sizePolicy.setHeightForWidth(label_OUT1->sizePolicy().hasHeightForWidth());
        label_OUT1->setSizePolicy(sizePolicy);
        label_OUT1->setMinimumSize(QSize(11, 11));
        label_OUT1->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_OUT1, 2, 1, 1, 1);

        label_ATU1 = new QLed(widget);
        label_ATU1->setObjectName(QStringLiteral("label_ATU1"));
        sizePolicy.setHeightForWidth(label_ATU1->sizePolicy().hasHeightForWidth());
        label_ATU1->setSizePolicy(sizePolicy);
        label_ATU1->setMinimumSize(QSize(11, 11));
        label_ATU1->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_ATU1, 3, 1, 1, 1);

        label_TU2 = new QLed(widget);
        label_TU2->setObjectName(QStringLiteral("label_TU2"));
        sizePolicy.setHeightForWidth(label_TU2->sizePolicy().hasHeightForWidth());
        label_TU2->setSizePolicy(sizePolicy);
        label_TU2->setMinimumSize(QSize(11, 11));
        label_TU2->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_TU2, 1, 2, 1, 1);

        label_OUT2 = new QLed(widget);
        label_OUT2->setObjectName(QStringLiteral("label_OUT2"));
        sizePolicy.setHeightForWidth(label_OUT2->sizePolicy().hasHeightForWidth());
        label_OUT2->setSizePolicy(sizePolicy);
        label_OUT2->setMinimumSize(QSize(11, 11));
        label_OUT2->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_OUT2, 2, 2, 1, 1);

        label_ATU2 = new QLed(widget);
        label_ATU2->setObjectName(QStringLiteral("label_ATU2"));
        sizePolicy.setHeightForWidth(label_ATU2->sizePolicy().hasHeightForWidth());
        label_ATU2->setSizePolicy(sizePolicy);
        label_ATU2->setMinimumSize(QSize(11, 11));
        label_ATU2->setPixmap(QPixmap(QString::fromUtf8(":/images/images/box_grn.png")));

        gridLayout->addWidget(label_ATU2, 3, 2, 1, 1);

        label_13 = new QLabel(widget);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout->addWidget(label_13, 1, 0, 1, 1, Qt::AlignRight);


        retranslateUi(FrameMvvStatus);

        QMetaObject::connectSlotsByName(FrameMvvStatus);
    } // setupUi

    void retranslateUi(QGroupBox *FrameMvvStatus)
    {
        FrameMvvStatus->setWindowTitle(QApplication::translate("FrameMvvStatus", "GroupBox", Q_NULLPTR));
        FrameMvvStatus->setTitle(QApplication::translate("FrameMvvStatus", "\320\241\320\276\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\234\320\222\320\222", Q_NULLPTR));
        label_12->setText(QApplication::translate("FrameMvvStatus", "\320\234\320\222\320\222:", Q_NULLPTR));
        label->setText(QApplication::translate("FrameMvvStatus", "1", Q_NULLPTR));
        label_2->setText(QApplication::translate("FrameMvvStatus", "2", Q_NULLPTR));
        label_TU1->setText(QString());
        label_4->setText(QApplication::translate("FrameMvvStatus", "\320\222\321\213\321\205\320\276\320\264 \320\242\320\243:", Q_NULLPTR));
        label_5->setText(QApplication::translate("FrameMvvStatus", "\320\220\320\242\320\243:", Q_NULLPTR));
        label_OUT1->setText(QString());
        label_ATU1->setText(QString());
        label_TU2->setText(QString());
        label_OUT2->setText(QString());
        label_ATU2->setText(QString());
        label_13->setText(QApplication::translate("FrameMvvStatus", "\320\232\320\273\321\216\321\207 \320\242\320\243:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FrameMvvStatus: public Ui_FrameMvvStatus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEMVVSTATUS_H
