#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QDialog *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QString _path;

public:
    void setFilePath(QString path);
    QString getFilePath(void);
    void show();
protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // DIALOG_H
