#ifndef DIALOGFILEORDIREDITOR_H
#define DIALOGFILEORDIREDITOR_H

#include <QDialog>

namespace Ui {
class DialogFileOrDirEditor;
}

class DialogFileOrDirEditor : public QDialog
{
    Q_OBJECT

public:
     DialogFileOrDirEditor(QWidget *parent = nullptr);
    ~DialogFileOrDirEditor();

private:
    Ui::DialogFileOrDirEditor *ui;
};

#endif // DIALOGFILEORDIREDITOR_H
