#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"

#include <QFileInfo>
#include <QDir>

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);

    QList<TableModelFileMonitor::TableItem> sampleFileMonitorTableData;

    sampleFileMonitorTableData.insert(0, {"first_file.txt",
                                          "C:/Users/<USER>/AppData/Roaming/<APPNAME>/",
                                          TableModelFileMonitor::TableItemStatus::Deleted,
                                          QDateTime::currentDateTime()
                                         });

    sampleFileMonitorTableData.insert(1, {"second_file.zip",
                                          "C:/Users/<USER>/Desktop/",
                                          TableModelFileMonitor::TableItemStatus::Moved,
                                          QDateTime::fromString("2021-03-19 10:40:30", "yyyy-MM-dd HH:mm:ss")
                                         });

    sampleFileMonitorTableData.insert(2, {"third_file.pdf",
                                          "C:/Users/<USER>/Desktop/",
                                          TableModelFileMonitor::TableItemStatus::Updated,
                                          QDateTime::fromString("2019-12-27 03:50:00", "yyyy-MM-dd HH:mm:ss")
                                         });

    sampleFileMonitorTableData.insert(3, {"fourth_file.mp4",
                                          "C:/Users/<USER>/Videos/",
                                          TableModelFileMonitor::TableItemStatus::NewAdded,
                                          QDateTime::fromString("2022-01-15 07:00:00", "yyyy-MM-dd HH:mm:ss")
                                         });

    this->tableModelFileMonitor = new TableModelFileMonitor(sampleFileMonitorTableData, this);
    this->ui->tableViewFileMonitor->setModel(this->tableModelFileMonitor);
    this->ui->tableViewFileMonitor->horizontalHeader()->setMinimumSectionSize(110);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
    this->ui->tableViewFileMonitor->resizeColumnsToContents();

    this->comboBoxItemDelegateNote = new ComboBoxItemDelegateNote(this->ui->tableViewFileMonitor);
    this->comboBoxItemDelegateFileAction = new ComboBoxItemDelegateFileAction(this->ui->tableViewFileMonitor);

    for(int rowIndex = 0; rowIndex < sampleFileMonitorTableData.size(); rowIndex++)
    {
        int columnActionIndex = 4;
        int columnNoteIndex = 5;

        this->ui->tableViewFileMonitor->setItemDelegateForColumn(columnNoteIndex, this->comboBoxItemDelegateNote);
        this->ui->tableViewFileMonitor->openPersistentEditor(this->tableModelFileMonitor->index(rowIndex, columnNoteIndex));


        this->ui->tableViewFileMonitor->setItemDelegateForColumn(columnActionIndex, this->comboBoxItemDelegateFileAction);
        this->ui->tableViewFileMonitor->openPersistentEditor(this->tableModelFileMonitor->index(rowIndex, columnActionIndex));
    }
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

void TabFileMonitor::slotOnPredictedFileNotFound(const QString &pathToFile)
{
    QFileInfo fileInfo(pathToFile);
    auto fileDir = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
    TableModelFileMonitor::TableItem item {fileInfo.fileName(),
                                           fileDir,
                                           TableModelFileMonitor::TableItemStatus::Missing,
                                           QDateTime::currentDateTime()};
}
