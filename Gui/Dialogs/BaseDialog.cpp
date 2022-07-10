#include "BaseDialog.h"

void BaseDialog::showStatusNormal(const QString &message, QLabel *label)
{
    this->showStatus(message, label);
}

void BaseDialog::showStatusInfo(const QString &message, QLabel *label)
{
    this->showStatus(message, label, "#7ed6df");
}

void BaseDialog::showStatusWarning(const QString &message, QLabel *label)
{
    this->showStatus(message, label, "#f6e58d");
}

void BaseDialog::showStatusError(const QString &message, QLabel *label)
{
    this->showStatus(message, label, "#ff3838");
}

void BaseDialog::showStatusSuccess(const QString &message, QLabel *label)
{
    this->showStatus(message, label, "#b8e994");
}

void BaseDialog::showStatus(const QString &message, QLabel *label, const QString &bgColorCode)
{
    auto labelStatus = label;

    QPalette palette;

    if(!bgColorCode.isEmpty())
        palette.setColor(QPalette::ColorRole::Window, bgColorCode);

    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}
