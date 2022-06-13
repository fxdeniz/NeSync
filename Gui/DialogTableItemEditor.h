#ifndef DIALOGTABLEITEMEDITOR_H
#define DIALOGTABLEITEMEDITOR_H

#include <QDialog>

namespace Ui {
class DialogTableItemEditor;
}

class DialogTableItemEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTableItemEditor(QWidget *parent = nullptr);
    ~DialogTableItemEditor();

private:
    Ui::DialogTableItemEditor *ui;
};

#endif // DIALOGTABLEITEMEDITOR_H
