#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

class options : public QWidget
{
    Q_OBJECT
public:
    options();
    int prec;           // floating point precision in all views
};

#endif // OPTIONS_H
