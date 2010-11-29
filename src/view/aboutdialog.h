#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_aboutDialog.h"

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AboutDialog ui;
};

#endif // ABOUTDIALOG_H
