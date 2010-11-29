#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include <QTextEdit>

namespace Ui {
    class reportDialog;
}

class reportDialog : public QDialog {
    Q_OBJECT
public:
    reportDialog(QWidget *parent = 0);
    ~reportDialog();
    void addText(QString text);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::reportDialog *ui;
};

#endif // REPORTDIALOG_H
