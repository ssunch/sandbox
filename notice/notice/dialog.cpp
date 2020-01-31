#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setFilePath(QString path)
{
    _path = path;
}

QString Dialog::getFilePath(void)
{
    return _path;
}

void Dialog::show()
{
    QFile fd(getFilePath());

    fd.open(QIODevice::ReadOnly);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(fd.readAll());
    fd.close();

    return QDialog::show();
}

void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    if(Qt::RightButton == event->button())
    {
        ui->plainTextEdit->clear();
    }
    //QWidget::mouseReleaseEvent(e);
}
