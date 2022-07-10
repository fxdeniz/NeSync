#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QString>
#include <QLabel>

class BaseDialog
{

protected:
     void showStatusNormal(const QString &message, QLabel *label);
     void showStatusInfo(const QString &message, QLabel *label);
     void showStatusWarning(const QString &message, QLabel *label);
     void showStatusError(const QString &message, QLabel *label);
     void showStatusSuccess(const QString &message, QLabel *label);
     void showStatus(const QString &message, QLabel *label, const QString &bgColorCode = "");

};

#endif // BASEDIALOG_H
