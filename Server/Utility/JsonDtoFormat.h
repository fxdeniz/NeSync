#ifndef JSONDTOFORMAT_H
#define JSONDTOFORMAT_H

#include <QJsonArray>
#include <QJsonObject>

namespace JsonKeys
{
    const inline QString IsExist = QStringLiteral("isExist");

    namespace Folder
    {
        const inline QString ParentFolderPath = QStringLiteral("parentFolderPath");
        const inline QString SuffixPath = QStringLiteral("suffixPath");
        const inline QString SymbolFolderPath = QStringLiteral("symbolFolderPath");
        const inline QString UserFolderPath = QStringLiteral("userFolderPath");
        const inline QString IsFrozen = QStringLiteral("isFrozen");
        const inline QString ChildFolders = QStringLiteral("childFolders");
        const inline QString ChildFiles = QStringLiteral("childFiles");
    }

    namespace File
    {
        const inline QString FileName = QStringLiteral("fileName");
        const inline QString SymbolFolderPath = QStringLiteral("symbolFolderPath");
        const inline QString SymbolFilePath = QStringLiteral("symbolFilePath");
        const inline QString UserFilePath = QStringLiteral("userFilePath");
        const inline QString IsFrozen = QStringLiteral("isFrozen");
        const inline QString MaxVersionNumber = QStringLiteral("maxVersionNumber");
        const inline QString VersionList = QStringLiteral("versionList");
    }

    namespace FileVersion
    {
        const inline QString SymbolFilePath = QStringLiteral("symbolFilePath");
        const inline QString VersionNumber = QStringLiteral("versionNumber");
        const inline QString NewVersionNumber = QStringLiteral("newVersionNumber");
        const inline QString Size = QStringLiteral("size");
        const inline QString LastModifiedTimestamp = QStringLiteral("lastModifiedTimestamp");
        const inline QString Description = QStringLiteral("description");
        const inline QString Hash = QStringLiteral("hash");
        const inline QString InternalFileName = QStringLiteral("internalFileName");
    }
}

#endif // JSONDTOFORMAT_H
