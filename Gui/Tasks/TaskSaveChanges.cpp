#include "TaskSaveChanges.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/JsonDtoFormat.h"

#include <QFile>

TaskSaveChanges::TaskSaveChanges(const QHash<QString, TreeItem *> folderItemMap,
                                 const QHash<QString, TreeItem *> fileItemMap,
                                 QObject *parent)
    : QThread{parent}, folderItemIterator(folderItemMap), fileItemIterator(fileItemMap)
{
}

TaskSaveChanges::~TaskSaveChanges()
{

}

void TaskSaveChanges::run()
{
    auto fsm = FileStorageManager::instance();

    while(fileItemIterator.hasNext())
    {
        fileItemIterator.next();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(fileItemIterator.key());
        QString symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();

        if(fileJson[JsonKeys::IsExist].toBool())
        {
            TreeItem *item = fileItemIterator.value();
            TreeItem::Action action = item->getAction();

            if(action == TreeItem::Action::Delete)
                fsm->deleteFile(symbolFilePath);

            else if(action == TreeItem::Action::Save)
                fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());

            else if(action == TreeItem::Action::Freeze)
            {
                fileJson[JsonKeys::File::IsFrozen] = true;
                fsm->updateFileEntity(fileJson);
            }

            else if(action == TreeItem::Action::Restore)
            {
                qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                auto internalFilePath = fsm->getBackupFolderPath() + internalFileName;
                QString userFilePath = fileJson[JsonKeys::File::UserFilePath].toString();

                QFile::copy(internalFilePath, userFilePath);
            }
        }
    }
}
