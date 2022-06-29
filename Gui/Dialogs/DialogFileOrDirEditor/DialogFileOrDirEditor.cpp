#include "DialogFileOrDirEditor.h"
#include "ui_DialogFileOrDirEditor.h"

DialogFileOrDirEditor::DialogFileOrDirEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFileOrDirEditor)
{
    ui->setupUi(this);
}

DialogFileOrDirEditor::~DialogFileOrDirEditor()
{
    delete ui;
}
