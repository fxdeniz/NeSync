#include "CustomFileSystemModel.h"

CustomFileSystemModel::CustomFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    QObject::connect(this, &QAbstractItemModel::rowsInserted, this, &CustomFileSystemModel::addToStatusColumn);
}

void CustomFileSystemModel::updateFrozenStatusOfItem(const QModelIndex &index)
{
    QFileInfo info = fileInfo(index);
    bool isFile = info.isFile();

    if(isFile)
    {
        auto key = info.absoluteFilePath();
        bool isContains = frozenFiles.contains(key);

        if(isContains)
            frozenFiles.remove(key);
        else
            frozenFiles.insert(key);
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
        if(column == ColumnIndex::Frozen)
        {
            QFileInfo info = fileInfo(index);
            bool isFile = info.isFile();

            if(!isFile)
                return tr("Not Applicable");
            else
            {
                auto key = info.absoluteFilePath();
                bool isContains = frozenFiles.contains(key);

                if(isContains)
                    return "✔";
                else
                    return "✘";

            }
        }
        else if(column == ColumnIndex::Status)
        {
            ItemStatus statusCode = statusOfFiles.value(filePath(index));
            return itemStatusToString(statusCode);
        }
    }
    else if(role == Qt::TextAlignmentRole)
    {
        if(column == ColumnIndex::Frozen || column == ColumnIndex::Status)
            return Qt::AlignHCenter;

    }

    return QFileSystemModel::data(index,role);
}

QVariant CustomFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == ColumnIndex::Frozen)
        return tr("Frozen");

    else if(section == ColumnIndex::Status)
        return tr("Status");

    else
        return QFileSystemModel::headerData(section, orientation, role);
}

bool CustomFileSystemModel::isFileMarkedAsFrozen(const QString &pathToFile)
{
    bool isContains = frozenFiles.contains(pathToFile);

    if(isContains)
        return true;
    else
        return false;
}

void CustomFileSystemModel::markItemAsPending(const QString &pathToFile)
{
    markItem(pathToFile, ItemStatus::Pending);
}

void CustomFileSystemModel::markItemAsSuccessful(const QString &pathToFile)
{
    markItem(pathToFile, ItemStatus::Successful);
}

void CustomFileSystemModel::markItemAsFailed(const QString &pathToFile)
{
    markItem(pathToFile, ItemStatus::Failed);
}

void CustomFileSystemModel::addToStatusColumn(const QModelIndex &index, int first, int last)
{
    auto info = fileInfo(index);

    if(info.isFile())
        statusOfFiles.insert(filePath(index), ItemStatus::Waiting);
    else if(info.isDir())
        statusOfFiles.insert(filePath(index), ItemStatus::NotApplicable);
}

QString CustomFileSystemModel::itemStatusToString(ItemStatus status)
{
    if(status == ItemStatus::NotApplicable)
        return "";
    else if(status == ItemStatus::Waiting)
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

void CustomFileSystemModel::markItem(const QString &pathToFile, ItemStatus status)
{
    statusOfFiles.insert(pathToFile, status);
    QModelIndex index = this->index(pathToFile, ColumnIndex::Status);
    emit dataChanged(index, index);
    emit layoutChanged();
}
