#include <QThread>

class BaseJob : public QThread
{
    Q_OBJECT
public:
    explicit BaseJob(QObject *parent);

signals:
    void progressRangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
};
