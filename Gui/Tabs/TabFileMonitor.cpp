#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"

#include "DataModels/TabFileMonitor/TreeModelFileMonitor.h"
#include "Tasks/TaskSaveChanges.h"

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);
    ui->progressBar->hide();

    itemDelegateAction = new ItemDelegateAction(this);
    itemDelegateDescription = new ItemDelegateDescription(this);

    timer.setInterval(2000);
    timer.stop();

    QObject::connect(&timer, &QTimer::timeout,
                     this, &TabFileMonitor::displayFileMonitorContent);
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

void TabFileMonitor::saveChanges(FileMonitoringManager *fmm)
{
    ui->textEditDescription->setReadOnly(true);
    ui->buttonAddDescription->setDisabled(true);
    ui->buttonDeleteDescription->setDisabled(true);

    auto treeModel = (TreeModelFileMonitor *)(ui->treeView->model());
    Q_ASSERT(treeModel);

    TaskSaveChanges *task = new TaskSaveChanges(treeModel->getFolderItemMap(),
                                                treeModel->getFileItemMap(),
                                                this);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(task->getTotalItemCount());
    ui->progressBar->show();

    QObject::connect(task, &TaskSaveChanges::itemBeingProcessed,
                     ui->progressBar, &QProgressBar::setValue);

    QObject::connect(task, &QThread::started,
                     fmm, &FileMonitoringManager::pauseMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &QThread::finished,
                     fmm, &FileMonitoringManager::continueMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &TaskSaveChanges::folderRestored,
                     fmm, &FileMonitoringManager::addTargetAtRuntime,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &QThread::finished,
                     task, &QThread::deleteLater);

    treeModel->disableComboBoxes();

    task->start();
}

void TabFileMonitor::onEventDbUpdated()
{
    QString statusText = "Analyzing detected changes...";
    ui->labelStatus->setHidden(false);
    ui->labelStatus->setText(statusText);

    ui->textEditDescription->setDisabled(true);
    ui->textEditDescription->blockSignals(true);
    ui->textEditDescription->clear();
    ui->textEditDescription->blockSignals(false);

    ui->buttonAddDescription->setDisabled(true);
    ui->buttonDeleteDescription->setDisabled(true);

    timer.start();
}

void TabFileMonitor::displayFileMonitorContent()
{
    timer.stop();
    ui->labelStatus->setHidden(true);
    ui->progressBar->hide();
    ui->textEditDescription->setEnabled(true);
    ui->textEditDescription->setReadOnly(false);
    ui->buttonAddDescription->setEnabled(true);
    ui->buttonDeleteDescription->setEnabled(true);

    TreeModelFileMonitor *treeModel = new TreeModelFileMonitor();
    QAbstractItemModel *oldModel = ui->treeView->model();

    if(oldModel != nullptr)
        delete oldModel;

    ui->treeView->setModel(treeModel);
    ui->comboBoxDescriptionNumber->setModel(treeModel->getDescriptionNumberListModel());

    QHeaderView *header = ui->treeView->header();
    header->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    header->setSectionResizeMode(TreeModelFileMonitor::ColumnIndexUserPath, QHeaderView::ResizeMode::Interactive);
    header->setMinimumSectionSize(130);
    ui->treeView->setColumnWidth(TreeModelFileMonitor::ColumnIndexUserPath, 500);

    ui->treeView->setItemDelegateForColumn(TreeModelFileMonitor::ColumnIndexAction, itemDelegateAction);
    ui->treeView->setItemDelegateForColumn(TreeModelFileMonitor::ColumnIndexDescription, itemDelegateDescription);

    ui->treeView->expandAll();

    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    ui->treeView->selectAll();
    QModelIndexList selectedIndices = ui->treeView->selectionModel()->selectedIndexes();

    for(const QModelIndex &current : selectedIndices)
    {
        ui->treeView->openPersistentEditor(current.siblingAtColumn(TreeModelFileMonitor::ColumnIndexAction));
        ui->treeView->openPersistentEditor(current.siblingAtColumn(TreeModelFileMonitor::ColumnIndexDescription));
    }

    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->treeView->selectionModel()->clearSelection();
}

void TabFileMonitor::on_buttonAddDescription_clicked()
{
    QTextEdit *textEdit = ui->textEditDescription;
    textEdit->blockSignals(true); // Disable updating descriptionMap in TreeModel

    textEdit->clear();

    auto model = (TreeModelFileMonitor *) ui->treeView->model();
    model->appendDescription();

    int number = model->getMaxDescriptionNumber();
    textEdit->setText(model->getDescription(number));

    int lastIndex = model->getDescriptionNumberListModel()->stringList().size() - 1;
    ui->comboBoxDescriptionNumber->setCurrentIndex(lastIndex);

    textEdit->blockSignals(false);
}

void TabFileMonitor::on_buttonDeleteDescription_clicked()
{
    QTextEdit *textEdit = ui->textEditDescription;
    QComboBox *comboBox = ui->comboBoxDescriptionNumber;
    textEdit->blockSignals(true); // Disable updating descriptionMap in TreeModel

    textEdit->clear();

    auto model = (TreeModelFileMonitor *) ui->treeView->model();
    int number = comboBox->currentText().toInt();
    int previousIndex = comboBox->currentIndex();
    model->deleteDescription(number);

    auto list = model->getDescriptionNumberListModel()->stringList();

    if(list.size() > 1)
    {
        if(previousIndex == list.size())
            comboBox->setCurrentIndex(0);
        else
            comboBox->setCurrentIndex(previousIndex);
    }

    int descIndex = comboBox->currentText().toInt();
    textEdit->setText(model->getDescription(descIndex));

    textEdit->blockSignals(false);
}

void TabFileMonitor::on_textEditDescription_textChanged()
{
    auto model = (TreeModelFileMonitor *) ui->treeView->model();
    int number = ui->comboBoxDescriptionNumber->currentText().toInt();
    model->updateDescription(number, ui->textEditDescription->toPlainText());
}

void TabFileMonitor::on_comboBoxDescriptionNumber_activated(int index)
{
    Q_UNUSED(index);

    QTextEdit *textEdit = ui->textEditDescription;
    textEdit->blockSignals(true); // Disable updating descriptionMap in TreeModel

    textEdit->clear();
    auto model = (TreeModelFileMonitor *) ui->treeView->model();
    int number = ui->comboBoxDescriptionNumber->currentText().toInt();
    textEdit->setText(model->getDescription(number));

    textEdit->blockSignals(false);
}

