/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "farmeline.h"
#include "framebase.h"
#include "framemvv.h"
#include "framemvvstatus.h"
#include "frameplugin.h"
#include "frametu.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_TS;
    QAction *action_Log;
    QAction *action_Ini;
    QAction *action_Lock;
    QAction *action_Refresh;
    QAction *action_Clear;
    QAction *action_TU;
    QAction *action_Toolbar;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    FrameBase *frameBase;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_MT;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_MT;
    FrameMVV *frameMVV1;
    FrameMVV *frameMVV2;
    FrameMvvStatus *groupBox_MVV;
    QWidget *widget;
    QGroupBox *groupBox_DBG;
    QHBoxLayout *horizontalLayout_2;
    FarmeLine *groupBox_PSV;
    FarmeLine *groupBox_ACT;
    QHBoxLayout *horizontalLayout_3;
    FrameTU *frameTU;
    FramePlugin *groupBox;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 600);
        action_TS = new QAction(MainWindow);
        action_TS->setObjectName(QStringLiteral("action_TS"));
        action_TS->setCheckable(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/images/3.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_TS->setIcon(icon);
        action_Log = new QAction(MainWindow);
        action_Log->setObjectName(QStringLiteral("action_Log"));
        action_Log->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/images/6.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Log->setIcon(icon1);
        action_Ini = new QAction(MainWindow);
        action_Ini->setObjectName(QStringLiteral("action_Ini"));
        action_Ini->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/images/5.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Ini->setIcon(icon2);
        action_Lock = new QAction(MainWindow);
        action_Lock->setObjectName(QStringLiteral("action_Lock"));
        action_Lock->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/images/1.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Lock->setIcon(icon3);
        action_Refresh = new QAction(MainWindow);
        action_Refresh->setObjectName(QStringLiteral("action_Refresh"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/images/2.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Refresh->setIcon(icon4);
        action_Clear = new QAction(MainWindow);
        action_Clear->setObjectName(QStringLiteral("action_Clear"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/images/4.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Clear->setIcon(icon5);
        action_TU = new QAction(MainWindow);
        action_TU->setObjectName(QStringLiteral("action_TU"));
        action_TU->setCheckable(true);
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/images/images/7.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_TU->setIcon(icon6);
        action_Toolbar = new QAction(MainWindow);
        action_Toolbar->setObjectName(QStringLiteral("action_Toolbar"));
        action_Toolbar->setCheckable(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(2);
        frameBase = new FrameBase(centralWidget);
        frameBase->setObjectName(QStringLiteral("frameBase"));
        frameBase->setMinimumSize(QSize(780, 60));
        frameBase->setFrameShape(QFrame::StyledPanel);
        frameBase->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(frameBase, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox_MT = new QGroupBox(centralWidget);
        groupBox_MT->setObjectName(QStringLiteral("groupBox_MT"));
        groupBox_MT->setMinimumSize(QSize(350, 90));
        groupBox_MT->setBaseSize(QSize(350, 90));
        gridLayout = new QGridLayout(groupBox_MT);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 2, 2);
        horizontalLayout_MT = new QHBoxLayout();
        horizontalLayout_MT->setSpacing(6);
        horizontalLayout_MT->setObjectName(QStringLiteral("horizontalLayout_MT"));
        frameMVV1 = new FrameMVV(groupBox_MT);
        frameMVV1->setObjectName(QStringLiteral("frameMVV1"));
        frameMVV1->setFrameShape(QFrame::StyledPanel);
        frameMVV1->setFrameShadow(QFrame::Raised);

        horizontalLayout_MT->addWidget(frameMVV1);

        frameMVV2 = new FrameMVV(groupBox_MT);
        frameMVV2->setObjectName(QStringLiteral("frameMVV2"));
        frameMVV2->setFrameShape(QFrame::StyledPanel);
        frameMVV2->setFrameShadow(QFrame::Raised);

        horizontalLayout_MT->addWidget(frameMVV2);


        gridLayout->addLayout(horizontalLayout_MT, 0, 0, 1, 1);


        horizontalLayout->addWidget(groupBox_MT);

        groupBox_MVV = new FrameMvvStatus(centralWidget);
        groupBox_MVV->setObjectName(QStringLiteral("groupBox_MVV"));

        horizontalLayout->addWidget(groupBox_MVV);

        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));

        horizontalLayout->addWidget(widget);

        groupBox_DBG = new QGroupBox(centralWidget);
        groupBox_DBG->setObjectName(QStringLiteral("groupBox_DBG"));

        horizontalLayout->addWidget(groupBox_DBG);


        gridLayout_2->addLayout(horizontalLayout, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox_PSV = new FarmeLine(centralWidget);
        groupBox_PSV->setObjectName(QStringLiteral("groupBox_PSV"));
        groupBox_PSV->setMinimumSize(QSize(0, 94));

        horizontalLayout_2->addWidget(groupBox_PSV);

        groupBox_ACT = new FarmeLine(centralWidget);
        groupBox_ACT->setObjectName(QStringLiteral("groupBox_ACT"));
        groupBox_ACT->setMinimumSize(QSize(0, 94));

        horizontalLayout_2->addWidget(groupBox_ACT);


        gridLayout_2->addLayout(horizontalLayout_2, 2, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(4);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        frameTU = new FrameTU(centralWidget);
        frameTU->setObjectName(QStringLiteral("frameTU"));
        frameTU->setFrameShape(QFrame::StyledPanel);
        frameTU->setFrameShadow(QFrame::Raised);

        horizontalLayout_3->addWidget(frameTU);

        groupBox = new FramePlugin(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));

        horizontalLayout_3->addWidget(groupBox);

        horizontalLayout_3->setStretch(0, 2);
        horizontalLayout_3->setStretch(1, 3);

        gridLayout_2->addLayout(horizontalLayout_3, 3, 0, 1, 1);

        gridLayout_2->setRowStretch(3, 1);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(action_TS);
        menu->addAction(action_Log);
        menu->addAction(action_Ini);
        menu_2->addAction(action_Lock);
        menu_2->addAction(action_Refresh);
        menu_2->addAction(action_Clear);
        menu_2->addAction(action_TU);
        menu_2->addSeparator();
        menu_3->addAction(action_Toolbar);
        mainToolBar->addAction(action_Lock);
        mainToolBar->addAction(action_Refresh);
        mainToolBar->addAction(action_TS);
        mainToolBar->addAction(action_Clear);
        mainToolBar->addAction(action_Ini);
        mainToolBar->addAction(action_Log);
        mainToolBar->addAction(action_TU);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        action_TS->setText(QApplication::translate("MainWindow", "\320\242\320\241", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_TS->setShortcut(QApplication::translate("MainWindow", "3", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Log->setText(QApplication::translate("MainWindow", "\320\237\321\200\320\276\321\202\320\276\320\272\320\276\320\273", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Log->setShortcut(QApplication::translate("MainWindow", "6", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Ini->setText(QApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Ini->setShortcut(QApplication::translate("MainWindow", "5", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Lock->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\261\320\273\320\276\320\272\320\270\321\200\320\276\320\262\320\260\321\202\321\214", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Lock->setShortcut(QApplication::translate("MainWindow", "1", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Refresh->setText(QApplication::translate("MainWindow", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Refresh->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Clear->setText(QApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Clear->setShortcut(QApplication::translate("MainWindow", "4", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_TU->setText(QApplication::translate("MainWindow", "\320\242\320\243", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_TU->setShortcut(QApplication::translate("MainWindow", "7", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        action_Toolbar->setText(QApplication::translate("MainWindow", "\320\242\321\203\320\273\320\261\320\260\321\200", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        action_Toolbar->setShortcut(QApplication::translate("MainWindow", "0", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        groupBox_MT->setTitle(QApplication::translate("MainWindow", "\320\241\320\276\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\274\320\276\320\264\321\203\320\273\320\265\320\271 \320\242\320\243/\320\242\320\241", Q_NULLPTR));
        groupBox_MVV->setTitle(QApplication::translate("MainWindow", "\320\241\320\276\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\234\320\222\320\222", Q_NULLPTR));
        groupBox_DBG->setTitle(QApplication::translate("MainWindow", "\320\236\321\202\320\273\320\260\320\264\320\272\320\260", Q_NULLPTR));
        groupBox_PSV->setTitle(QApplication::translate("MainWindow", "\320\234\320\276\320\264\320\265\320\274 COM3 (\320\277\320\260\321\201)", Q_NULLPTR));
        groupBox_ACT->setTitle(QApplication::translate("MainWindow", "\320\234\320\276\320\264\320\265\320\274 COM4 (\320\260\320\272\321\202)", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "\320\241\320\276\320\277\321\200\321\217\320\266\320\265\320\275\320\270\320\265 \321\201 \321\201\320\270\321\201\321\202\320\265\320\274\320\260\320\274\320\270 \320\226\320\220\320\242", Q_NULLPTR));
        menu->setTitle(QApplication::translate("MainWindow", "\320\237\321\200\320\276\321\201\320\274\320\276\321\202\321\200", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("MainWindow", "\320\222\321\213\320\277\320\276\320\273\320\275\320\270\321\202\321\214", Q_NULLPTR));
        menu_3->setTitle(QApplication::translate("MainWindow", "\320\222\320\270\320\264", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
