/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_pose;
    QPushButton *pushButton_reset;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_targetX;
    QDoubleSpinBox *doubleSpinBox_targetY;
    QPushButton *pushButton_sendToGoal;
    QLabel *label_3;
    QLineEdit *lineEdit_status;
    QProgressBar *progressBar_goal;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 20, 141, 17));
        label_pose = new QLabel(centralwidget);
        label_pose->setObjectName(QString::fromUtf8("label_pose"));
        label_pose->setGeometry(QRect(220, 20, 411, 17));
        pushButton_reset = new QPushButton(centralwidget);
        pushButton_reset->setObjectName(QString::fromUtf8("pushButton_reset"));
        pushButton_reset->setGeometry(QRect(670, 20, 89, 25));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(60, 70, 141, 17));
        doubleSpinBox_targetX = new QDoubleSpinBox(centralwidget);
        doubleSpinBox_targetX->setObjectName(QString::fromUtf8("doubleSpinBox_targetX"));
        doubleSpinBox_targetX->setGeometry(QRect(230, 70, 65, 26));
        doubleSpinBox_targetY = new QDoubleSpinBox(centralwidget);
        doubleSpinBox_targetY->setObjectName(QString::fromUtf8("doubleSpinBox_targetY"));
        doubleSpinBox_targetY->setGeometry(QRect(350, 70, 65, 26));
        pushButton_sendToGoal = new QPushButton(centralwidget);
        pushButton_sendToGoal->setObjectName(QString::fromUtf8("pushButton_sendToGoal"));
        pushButton_sendToGoal->setGeometry(QRect(670, 70, 89, 25));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(70, 130, 67, 17));
        lineEdit_status = new QLineEdit(centralwidget);
        lineEdit_status->setObjectName(QString::fromUtf8("lineEdit_status"));
        lineEdit_status->setGeometry(QRect(160, 120, 601, 25));
        progressBar_goal = new QProgressBar(centralwidget);
        progressBar_goal->setObjectName(QString::fromUtf8("progressBar_goal"));
        progressBar_goal->setGeometry(QRect(80, 170, 681, 23));
        progressBar_goal->setValue(0);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\237\321\203\320\273\321\214\321\202 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\321\217 \321\207\320\265\321\200\320\265\320\277\320\260\321\205\320\276\320\271", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\267\320\270\321\206\320\270\321\217 \321\207\320\265\321\200\320\265\320\277\320\260\321\205\320\270", nullptr));
        label_pose->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        pushButton_reset->setText(QCoreApplication::translate("MainWindow", "\320\241\320\261\321\200\320\276\321\201", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\246\320\265\320\273\320\265\320\262\320\260\321\217 \321\202\320\276\321\207\320\272\320\260", nullptr));
        pushButton_sendToGoal->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\277\321\203\321\201\320\272", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\260\321\202\321\203\321\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
