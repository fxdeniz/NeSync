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
    return QFileSystemModel::columnCount() + 1;
}

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const
{
    if(index.column() == ColumnIndex::AutoSync)
    {
        switch(role)
        {
           case(Qt::DisplayRole):
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
           case(Qt::TextAlignmentRole):
               return Qt::AlignHCenter;

           default:{}
        }
    }

    return QFileSystemModel::data(index,role);
}

QVariant CustomFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == ColumnIndex::AutoSync)
        return tr("Auto-Sync");

    return QFileSystemModel::headerData(section, orientation, role);
}
