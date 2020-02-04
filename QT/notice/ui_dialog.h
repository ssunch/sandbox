/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->setWindowModality(Qt::ApplicationModal);
        Dialog->setEnabled(false);
        Dialog->resize(800, 800);
        Dialog->setMinimumSize(QSize(750, 750));
        Dialog->setMaximumSize(QSize(800, 800));
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu Thin"));
        Dialog->setFont(font);
        Dialog->setContextMenuPolicy(Qt::DefaultContextMenu);
        Dialog->setModal(true);
        Dialog->setMouseTracking(true);
        plainTextEdit = new QPlainTextEdit(Dialog);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 0, 800, 700));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Ubuntu"));
        font1.setPointSize(12);
        plainTextEdit->setFont(font1);
        plainTextEdit->setMouseTracking(true);
        plainTextEdit->setFrameShape(QFrame::NoFrame);
        plainTextEdit->setFrameShadow(QFrame::Plain);
        plainTextEdit->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextEditable|Qt::TextEditorInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "notice", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
