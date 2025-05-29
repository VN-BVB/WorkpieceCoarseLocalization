/********************************************************************************
** Form generated from reading UI file 'WorkpieceCoarseLocalization.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORKPIECECOARSELOCALIZATION_H
#define UI_WORKPIECECOARSELOCALIZATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>
#include "utils/image_widget/QImageWidget.h"
#include "utils/image_widget/ScalableGraphicsView.h"

QT_BEGIN_NAMESPACE

class Ui_WorkpieceCoarseLocalization
{
public:
    QGridLayout *gridLayout_2;
    QImageWidget *qImageWidget;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QGridLayout *gridLayout;
    QLineEdit *lineEditInferTime;
    QPushButton *btnSaveImage;
    QPushButton *btnStartInfer;
    QLabel *label_2;
    QComboBox *comboCameras;
    QLabel *label;
    QComboBox *comboSaveImage;
    QPushButton *btnDisconnectCamera;
    QPushButton *btnConnectCamera;
    QPushButton *btnInferPath;
    QWidget *tab_4;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_4;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEditX;
    QLabel *label_6;
    QLabel *label_8;
    QLineEdit *lineEditZ;
    QLineEdit *lineEditY;
    QLabel *label_7;
    QLineEdit *lineEditRX;
    QLineEdit *lineEditRY;
    QLabel *label_9;
    QLineEdit *lineEditRZ;
    QPushButton *btnSaveImage_2;
    QPushButton *btnCalibratateCamera;
    QPushButton *btnCalibEyetoHand;
    QPushButton *btnCalibTrack;
    QWidget *tab;
    QGridLayout *gridLayout_6;
    ScalableGraphicsView *mapView;
    QGridLayout *gridLayout_3;
    QLabel *coordinateLabel;
    QPushButton *btn_VerifyCoordinates;
    QWidget *tab_2;
    QGridLayout *gridLayout_5;
    QLabel *label_3;
    QComboBox *comboWorkpieceNum;
    QPushButton *btnGetWorkpieceInfo;
    QTextBrowser *textCalibratation;

    void setupUi(QWidget *WorkpieceCoarseLocalization)
    {
        if (WorkpieceCoarseLocalization->objectName().isEmpty())
            WorkpieceCoarseLocalization->setObjectName(QString::fromUtf8("WorkpieceCoarseLocalization"));
        WorkpieceCoarseLocalization->resize(1536, 1200);
        gridLayout_2 = new QGridLayout(WorkpieceCoarseLocalization);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qImageWidget = new QImageWidget(WorkpieceCoarseLocalization);
        qImageWidget->setObjectName(QString::fromUtf8("qImageWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qImageWidget->sizePolicy().hasHeightForWidth());
        qImageWidget->setSizePolicy(sizePolicy);
        qImageWidget->setMinimumSize(QSize(800, 0));

        gridLayout_2->addWidget(qImageWidget, 0, 0, 2, 1);

        tabWidget = new QTabWidget(WorkpieceCoarseLocalization);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMinimumSize(QSize(0, 0));
        tabWidget->setMaximumSize(QSize(800, 16777215));
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tab_3->sizePolicy().hasHeightForWidth());
        tab_3->setSizePolicy(sizePolicy1);
        tab_3->setMinimumSize(QSize(0, 250));
        gridLayout = new QGridLayout(tab_3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEditInferTime = new QLineEdit(tab_3);
        lineEditInferTime->setObjectName(QString::fromUtf8("lineEditInferTime"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lineEditInferTime->sizePolicy().hasHeightForWidth());
        lineEditInferTime->setSizePolicy(sizePolicy2);
        QFont font;
        font.setPointSize(26);
        lineEditInferTime->setFont(font);

        gridLayout->addWidget(lineEditInferTime, 4, 1, 1, 2);

        btnSaveImage = new QPushButton(tab_3);
        btnSaveImage->setObjectName(QString::fromUtf8("btnSaveImage"));
        sizePolicy2.setHeightForWidth(btnSaveImage->sizePolicy().hasHeightForWidth());
        btnSaveImage->setSizePolicy(sizePolicy2);
        btnSaveImage->setMinimumSize(QSize(0, 0));
        btnSaveImage->setFont(font);

        gridLayout->addWidget(btnSaveImage, 3, 0, 1, 1);

        btnStartInfer = new QPushButton(tab_3);
        btnStartInfer->setObjectName(QString::fromUtf8("btnStartInfer"));
        sizePolicy2.setHeightForWidth(btnStartInfer->sizePolicy().hasHeightForWidth());
        btnStartInfer->setSizePolicy(sizePolicy2);
        btnStartInfer->setFont(font);

        gridLayout->addWidget(btnStartInfer, 1, 0, 1, 1);

        label_2 = new QLabel(tab_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);
        label_2->setFont(font);
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        comboCameras = new QComboBox(tab_3);
        comboCameras->setObjectName(QString::fromUtf8("comboCameras"));
        comboCameras->setMinimumSize(QSize(0, 40));
        QFont font1;
        font1.setPointSize(20);
        comboCameras->setFont(font1);

        gridLayout->addWidget(comboCameras, 2, 1, 1, 2);

        label = new QLabel(tab_3);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setFont(font);
        label->setLayoutDirection(Qt::LeftToRight);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 4, 0, 1, 1);

        comboSaveImage = new QComboBox(tab_3);
        comboSaveImage->addItem(QString());
        comboSaveImage->addItem(QString());
        comboSaveImage->addItem(QString());
        comboSaveImage->addItem(QString());
        comboSaveImage->setObjectName(QString::fromUtf8("comboSaveImage"));
        comboSaveImage->setMinimumSize(QSize(0, 40));
        comboSaveImage->setFont(font1);

        gridLayout->addWidget(comboSaveImage, 3, 1, 1, 2);

        btnDisconnectCamera = new QPushButton(tab_3);
        btnDisconnectCamera->setObjectName(QString::fromUtf8("btnDisconnectCamera"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnDisconnectCamera->sizePolicy().hasHeightForWidth());
        btnDisconnectCamera->setSizePolicy(sizePolicy3);
        btnDisconnectCamera->setMinimumSize(QSize(300, 0));
        btnDisconnectCamera->setMaximumSize(QSize(16777215, 16777215));
        btnDisconnectCamera->setFont(font);

        gridLayout->addWidget(btnDisconnectCamera, 0, 1, 1, 2);

        btnConnectCamera = new QPushButton(tab_3);
        btnConnectCamera->setObjectName(QString::fromUtf8("btnConnectCamera"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(btnConnectCamera->sizePolicy().hasHeightForWidth());
        btnConnectCamera->setSizePolicy(sizePolicy4);
        btnConnectCamera->setMinimumSize(QSize(300, 0));
        btnConnectCamera->setMaximumSize(QSize(16777215, 16777215));
        btnConnectCamera->setFont(font);

        gridLayout->addWidget(btnConnectCamera, 0, 0, 1, 1);

        btnInferPath = new QPushButton(tab_3);
        btnInferPath->setObjectName(QString::fromUtf8("btnInferPath"));
        sizePolicy4.setHeightForWidth(btnInferPath->sizePolicy().hasHeightForWidth());
        btnInferPath->setSizePolicy(sizePolicy4);
        btnInferPath->setMinimumSize(QSize(0, 0));
        btnInferPath->setFont(font);

        gridLayout->addWidget(btnInferPath, 1, 1, 1, 2);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_7 = new QGridLayout(tab_4);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label_4 = new QLabel(tab_4);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy2);
        label_4->setFont(font);
        label_4->setLayoutDirection(Qt::LeftToRight);
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_4, 1, 0, 1, 1);

        label_5 = new QLabel(tab_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);
        label_5->setFont(font);
        label_5->setLayoutDirection(Qt::LeftToRight);
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_5, 2, 0, 1, 1);

        lineEditX = new QLineEdit(tab_4);
        lineEditX->setObjectName(QString::fromUtf8("lineEditX"));
        sizePolicy2.setHeightForWidth(lineEditX->sizePolicy().hasHeightForWidth());
        lineEditX->setSizePolicy(sizePolicy2);
        lineEditX->setFont(font);

        gridLayout_4->addWidget(lineEditX, 1, 1, 1, 1);

        label_6 = new QLabel(tab_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy2.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy2);
        label_6->setFont(font);
        label_6->setLayoutDirection(Qt::LeftToRight);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_6, 3, 0, 1, 1);

        label_8 = new QLabel(tab_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);
        label_8->setFont(font);
        label_8->setLayoutDirection(Qt::LeftToRight);
        label_8->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_8, 5, 0, 1, 1);

        lineEditZ = new QLineEdit(tab_4);
        lineEditZ->setObjectName(QString::fromUtf8("lineEditZ"));
        sizePolicy2.setHeightForWidth(lineEditZ->sizePolicy().hasHeightForWidth());
        lineEditZ->setSizePolicy(sizePolicy2);
        lineEditZ->setFont(font);

        gridLayout_4->addWidget(lineEditZ, 3, 1, 1, 1);

        lineEditY = new QLineEdit(tab_4);
        lineEditY->setObjectName(QString::fromUtf8("lineEditY"));
        sizePolicy2.setHeightForWidth(lineEditY->sizePolicy().hasHeightForWidth());
        lineEditY->setSizePolicy(sizePolicy2);
        lineEditY->setFont(font);

        gridLayout_4->addWidget(lineEditY, 2, 1, 1, 1);

        label_7 = new QLabel(tab_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy2);
        label_7->setFont(font);
        label_7->setLayoutDirection(Qt::LeftToRight);
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_7, 4, 0, 1, 1);

        lineEditRX = new QLineEdit(tab_4);
        lineEditRX->setObjectName(QString::fromUtf8("lineEditRX"));
        sizePolicy2.setHeightForWidth(lineEditRX->sizePolicy().hasHeightForWidth());
        lineEditRX->setSizePolicy(sizePolicy2);
        lineEditRX->setFont(font);

        gridLayout_4->addWidget(lineEditRX, 4, 1, 1, 1);

        lineEditRY = new QLineEdit(tab_4);
        lineEditRY->setObjectName(QString::fromUtf8("lineEditRY"));
        sizePolicy2.setHeightForWidth(lineEditRY->sizePolicy().hasHeightForWidth());
        lineEditRY->setSizePolicy(sizePolicy2);
        lineEditRY->setFont(font);

        gridLayout_4->addWidget(lineEditRY, 5, 1, 1, 1);

        label_9 = new QLabel(tab_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        sizePolicy2.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy2);
        label_9->setFont(font);
        label_9->setLayoutDirection(Qt::LeftToRight);
        label_9->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_9, 6, 0, 1, 1);

        lineEditRZ = new QLineEdit(tab_4);
        lineEditRZ->setObjectName(QString::fromUtf8("lineEditRZ"));
        sizePolicy2.setHeightForWidth(lineEditRZ->sizePolicy().hasHeightForWidth());
        lineEditRZ->setSizePolicy(sizePolicy2);
        lineEditRZ->setFont(font);

        gridLayout_4->addWidget(lineEditRZ, 6, 1, 1, 1);


        gridLayout_7->addLayout(gridLayout_4, 0, 0, 1, 1);

        btnSaveImage_2 = new QPushButton(tab_4);
        btnSaveImage_2->setObjectName(QString::fromUtf8("btnSaveImage_2"));
        sizePolicy2.setHeightForWidth(btnSaveImage_2->sizePolicy().hasHeightForWidth());
        btnSaveImage_2->setSizePolicy(sizePolicy2);
        btnSaveImage_2->setMinimumSize(QSize(0, 100));
        btnSaveImage_2->setFont(font);

        gridLayout_7->addWidget(btnSaveImage_2, 1, 0, 1, 1);

        btnCalibratateCamera = new QPushButton(tab_4);
        btnCalibratateCamera->setObjectName(QString::fromUtf8("btnCalibratateCamera"));
        sizePolicy4.setHeightForWidth(btnCalibratateCamera->sizePolicy().hasHeightForWidth());
        btnCalibratateCamera->setSizePolicy(sizePolicy4);
        btnCalibratateCamera->setMinimumSize(QSize(0, 100));
        btnCalibratateCamera->setMaximumSize(QSize(16777215, 100));
        btnCalibratateCamera->setFont(font);

        gridLayout_7->addWidget(btnCalibratateCamera, 2, 0, 1, 1);

        btnCalibEyetoHand = new QPushButton(tab_4);
        btnCalibEyetoHand->setObjectName(QString::fromUtf8("btnCalibEyetoHand"));
        sizePolicy4.setHeightForWidth(btnCalibEyetoHand->sizePolicy().hasHeightForWidth());
        btnCalibEyetoHand->setSizePolicy(sizePolicy4);
        btnCalibEyetoHand->setMinimumSize(QSize(0, 0));
        btnCalibEyetoHand->setMaximumSize(QSize(16777215, 100));
        btnCalibEyetoHand->setFont(font);

        gridLayout_7->addWidget(btnCalibEyetoHand, 3, 0, 1, 1);

        btnCalibTrack = new QPushButton(tab_4);
        btnCalibTrack->setObjectName(QString::fromUtf8("btnCalibTrack"));
        sizePolicy4.setHeightForWidth(btnCalibTrack->sizePolicy().hasHeightForWidth());
        btnCalibTrack->setSizePolicy(sizePolicy4);
        btnCalibTrack->setMinimumSize(QSize(0, 0));
        btnCalibTrack->setMaximumSize(QSize(16777215, 100));
        btnCalibTrack->setFont(font);

        gridLayout_7->addWidget(btnCalibTrack, 4, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_6 = new QGridLayout(tab);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        mapView = new ScalableGraphicsView(tab);
        mapView->setObjectName(QString::fromUtf8("mapView"));

        gridLayout_6->addWidget(mapView, 0, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        coordinateLabel = new QLabel(tab);
        coordinateLabel->setObjectName(QString::fromUtf8("coordinateLabel"));

        gridLayout_3->addWidget(coordinateLabel, 0, 1, 1, 1);

        btn_VerifyCoordinates = new QPushButton(tab);
        btn_VerifyCoordinates->setObjectName(QString::fromUtf8("btn_VerifyCoordinates"));
        btn_VerifyCoordinates->setMinimumSize(QSize(0, 0));

        gridLayout_3->addWidget(btn_VerifyCoordinates, 0, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_3, 1, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_5 = new QGridLayout(tab_2);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy2);
        label_3->setFont(font);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(label_3, 0, 0, 1, 1);

        comboWorkpieceNum = new QComboBox(tab_2);
        comboWorkpieceNum->setObjectName(QString::fromUtf8("comboWorkpieceNum"));
        comboWorkpieceNum->setMinimumSize(QSize(0, 40));
        comboWorkpieceNum->setFont(font1);

        gridLayout_5->addWidget(comboWorkpieceNum, 0, 1, 1, 1);

        btnGetWorkpieceInfo = new QPushButton(tab_2);
        btnGetWorkpieceInfo->setObjectName(QString::fromUtf8("btnGetWorkpieceInfo"));
        sizePolicy2.setHeightForWidth(btnGetWorkpieceInfo->sizePolicy().hasHeightForWidth());
        btnGetWorkpieceInfo->setSizePolicy(sizePolicy2);
        btnGetWorkpieceInfo->setFont(font);

        gridLayout_5->addWidget(btnGetWorkpieceInfo, 1, 0, 1, 2);

        tabWidget->addTab(tab_2, QString());

        gridLayout_2->addWidget(tabWidget, 0, 1, 1, 1);

        textCalibratation = new QTextBrowser(WorkpieceCoarseLocalization);
        textCalibratation->setObjectName(QString::fromUtf8("textCalibratation"));
        sizePolicy2.setHeightForWidth(textCalibratation->sizePolicy().hasHeightForWidth());
        textCalibratation->setSizePolicy(sizePolicy2);
        textCalibratation->setMinimumSize(QSize(0, 250));
        textCalibratation->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_2->addWidget(textCalibratation, 1, 1, 1, 1);


        retranslateUi(WorkpieceCoarseLocalization);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(WorkpieceCoarseLocalization);
    } // setupUi

    void retranslateUi(QWidget *WorkpieceCoarseLocalization)
    {
        WorkpieceCoarseLocalization->setWindowTitle(QCoreApplication::translate("WorkpieceCoarseLocalization", "MainWindow", nullptr));
        btnSaveImage->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\344\277\235\345\255\230\345\233\276\345\203\217", nullptr));
        btnStartInfer->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\274\200\345\247\213\346\216\250\347\220\206", nullptr));
        label_2->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\347\233\270\346\234\272\351\200\211\346\213\251", nullptr));
        label->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\216\250\347\220\206\347\224\250\346\227\266", nullptr));
        comboSaveImage->setItemText(0, QCoreApplication::translate("WorkpieceCoarseLocalization", "\347\233\270\346\234\272\346\211\213\347\234\274\346\240\207\345\256\232", nullptr));
        comboSaveImage->setItemText(1, QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\271\263\351\235\242\346\213\237\345\220\210", nullptr));
        comboSaveImage->setItemText(2, QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\234\260\350\275\250\346\240\207\345\256\232", nullptr));
        comboSaveImage->setItemText(3, QCoreApplication::translate("WorkpieceCoarseLocalization", "\344\277\235\345\255\230\345\233\276\345\203\217", nullptr));

        btnDisconnectCamera->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        btnConnectCamera->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\350\277\236\346\216\245\347\233\270\346\234\272", nullptr));
        btnInferPath->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\216\250\347\220\206\346\226\207\344\273\266\345\244\271", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\213\215\346\221\204\346\216\250\347\220\206", nullptr));
        label_4->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "X:", nullptr));
        label_5->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "Y:", nullptr));
        label_6->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "Z:", nullptr));
        label_8->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "RY:", nullptr));
        label_7->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "RX:", nullptr));
        label_9->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "RZ:", nullptr));
        btnSaveImage_2->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\344\277\235\345\255\230\346\240\207\345\256\232\345\233\276\345\203\217", nullptr));
        btnCalibratateCamera->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\347\233\270\346\234\272\346\240\207\345\256\232", nullptr));
        btnCalibEyetoHand->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\211\213\347\234\274\346\240\207\345\256\232", nullptr));
        btnCalibTrack->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\234\260\350\275\250\346\240\207\345\256\232", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("WorkpieceCoarseLocalization", "\346\240\207\345\256\232", nullptr));
        coordinateLabel->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "coordinateLabel", nullptr));
        btn_VerifyCoordinates->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\347\241\256\350\256\244\345\267\245\344\273\266\345\235\220\346\240\207", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\205\250\345\261\200\346\243\200\346\237\245", nullptr));
        label_3->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\267\245\344\273\266\351\200\211\346\213\251", nullptr));
        btnGetWorkpieceInfo->setText(QCoreApplication::translate("WorkpieceCoarseLocalization", "\350\216\267\345\217\226\345\267\245\344\273\266\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("WorkpieceCoarseLocalization", "\345\215\225\345\274\240\346\243\200\346\237\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WorkpieceCoarseLocalization: public Ui_WorkpieceCoarseLocalization {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORKPIECECOARSELOCALIZATION_H
