#include "ListModelFileExplorer.h"

#include "Utility/JsonDtoFormat.h"

#include <QColor>

ListModelFileExplorer::ListModelFileExplorer(QJsonObject fileJson, QObject *parent)
    : QAbstractListModel(parent), fileJson{fileJson}
{
    for(int number = 1; number <= fileJson[JsonKeys::File::MaxVersionNumber].toInt(); number++)
        stringList.append(QString::number(number));
}

QString ListModelFileExplorer::getFileSymbolPath() const
{
    return fileJson[JsonKeys::File::SymbolFilePath].toString();
}

int ListModelFileExplorer::rowCount(const QModelIndex &parent) const
{
    return stringList.count();
}

QVariant ListModelFileExplorer::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= stringList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return stringList.at(index.row());
    else if(role == Qt::ItemDataRole::CheckStateRole)
    {
        if(this->checkedItems.contains(index))
            return Qt::CheckState::Checked;
        else
            return Qt::CheckState::Unchecked;
    }
    else
        return QVariant();
}

QVariant ListModelFileExplorer::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags ListModelFileExplorer::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsUserCheckable;
}

bool ListModelFileExplorer::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;

    if (index.isValid())
    {
        if(role == Qt::ItemDataRole::EditRole)
        {
            stringList.replace(index.row(), value.toString());

            emit dataChanged(index, index);
            result = true;
        }
        else if(role == Qt::ItemDataRole::CheckStateRole)
        {
            if(value == Qt::CheckState::Checked)
                this->checkedItems.insert(index);
            else
                this->checkedItems.remove(index);

            emit dataChanged(index, index);
            result = true;
        }
    }

    return result;
}

bool ListModelFileExplorer::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        stringList.insert(position, "");
    }

    endInsertRows();
    return true;
}

bool ListModelFileExplorer::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        stringList.removeAt(position);
    }

    endRemoveRows();
    return true;
}
