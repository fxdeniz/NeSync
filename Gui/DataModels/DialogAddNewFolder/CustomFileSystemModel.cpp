#include "CustomFileSystemModel.h"

CustomFileSystemModel::CustomFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{

}

void CustomFileSystemModel::updateAutoSyncStatusOfItem(const QModelIndex &index)
{
    QFileInfo info = fileInfo(index);
    bool isFile = info.isFile();

    if(isFile)
    {
        auto key = info.absoluteFilePath();
        bool isContains = autoSyncDisabledFiles.contains(key);

        if(isContains)
            autoSyncDisabledFiles.remove(key);
        else
            autoSyncDisabledFiles.insert(key);
    }

    //emit dataChanged(index, index);
    emit layoutChanged();
}

int CustomFileSystemModel::columnCount(const QModelIndex &parent) const
{
    return QFileSystemModel::columnCount() + 2;
}

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};

    int column = index.column();

    if(role == Qt::DisplayRole)
    {
        if(column == ColumnIndex::AutoSync)
        {
            QFileInfo info = fileInfo(index);
            bool isFile = info.isFile();

            if(!isFile)
                return tr("Not Applicable");
            else
            {
                auto key = info.absoluteFilePath();
                bool isContains = autoSyncDisabledFiles.contains(key);

                if(isContains)
                    return "✘";
                else
                    return "✔";

            }
        }
        else if(column == ColumnIndex::Status)
        {
            auto key = index.siblingAtColumn(ColumnIndex::Name).data().toString();
            bool isContains = statusOfFiles.contains(key);

            if(isContains)
                return itemStatusToString(statusOfFiles.value(key));
            else
                return itemStatusToString(ItemStatus::Waiting);
        }
    }
    else if(role == Qt::TextAlignmentRole)
    {
        if(column == ColumnIndex::AutoSync || column == ColumnIndex::Status)
            return Qt::AlignHCenter;

    }

    return QFileSystemModel::data(index,role);
}

QVariant CustomFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == ColumnIndex::AutoSync)
        return tr("Auto-Sync");

    else if(section == ColumnIndex::Status)
        return tr("Status");

    else
        return QFileSystemModel::headerData(section, orientation, role);
}

bool CustomFileSystemModel::isAutoSyncEnabledFor(const QString &pathToFile)
{
    bool isContains = autoSyncDisabledFiles.contains(pathToFile);

    if(isContains)
        return false;
    else
        return true;
}

QString CustomFileSystemModel::itemStatusToString(ItemStatus status)
{
    if(status == ItemStatus::Waiting)
        return tr("Waiting");

    else if(status == ItemStatus::Pending)
        return tr("Pending");

    else if(status == ItemStatus::Successful)
        return tr("Successful");

    else if(status == ItemStatus::Failed)
        return tr("Failed");

    else
        return "NaN";
}
