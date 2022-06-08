#include "UserContentListModel.h"

#include <QColor>

UserContentListModel::UserContentListModel(QObject *parent) : QAbstractListModel(parent)
{
}

UserContentListModel::UserContentListModel(const QStringList &itemList, QObject *parent)
    : QAbstractListModel(parent)
{
    this->stringList = itemList;
}

int UserContentListModel::rowCount(const QModelIndex &parent) const
{
    return stringList.count();
}


/*!
    Returns an appropriate value for the requested data.
    If the view requests an invalid index, an invalid variant is returned.
    Any valid index that corresponds to a string in the list causes that
    string to be returned.
*/


QVariant UserContentListModel::data(const QModelIndex &index, int role) const
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


/*!
    Returns the appropriate header string depending on the orientation of
    the header and the section. If anything other than the display role is
    requested, we return an invalid variant.
*/


QVariant UserContentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}


/*!
    Returns an appropriate value for the item's flags. Valid items are
    enabled, selectable, and editable.
*/


Qt::ItemFlags UserContentListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsUserCheckable;
}


/*!
    Changes an item in the string list, but only if the following conditions
    are met:

* The index supplied is valid.
        * The index corresponds to an item to be shown in a view.
            * The role associated with editing text is specified.

    The dataChanged() signal is emitted if the item is changed.
        */


bool UserContentListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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


/*!
    Inserts a number of rows into the model at the specified position.
*/


bool UserContentListModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        stringList.insert(position, "");
    }

    endInsertRows();
    return true;

}


/*!
    Removes a number of rows from the model at the specified position.
*/


bool UserContentListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        stringList.removeAt(position);
    }

    endRemoveRows();
    return true;

}
