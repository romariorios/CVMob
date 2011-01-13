#include "aboutdialog.h"

#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent){
    ui.setupUi(this);
}

void AboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}
