#ifndef TABRELATEDFILES_H
#define TABRELATEDFILES_H

#include <QWidget>

namespace Ui {
class TabRelatedFiles;
}

class TabRelatedFiles : public QWidget
{
    Q_OBJECT

public:
    explicit TabRelatedFiles(QWidget *parent = nullptr);
    ~TabRelatedFiles();

private:
    Ui::TabRelatedFiles *ui;
};

#endif // TABRELATEDFILES_H
