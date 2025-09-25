#include "files.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

bool
Files::copyData(const std::string &src)
{
    QDir dstDir(base_dir().c_str());
    if (!dstDir.exists())
    {
        dstDir.mkdir(dstDir.path());
    }
    int src_ver = getVersion(src.c_str());
    int dst_ver = getVersion(base_dir().c_str());
    if (src_ver > dst_ver)
    {
        return copyRecursively(src.c_str(), base_dir().c_str());
    }
    return true;
}

int
Files::getVersion(const QString &dir)
{
    QString version_file = dir + "/version.json";
    QFile f(version_file);
    if (!f.exists()) return 0;
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) return 0;
    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return 0;

    QJsonObject root = doc.object();
    return root.value("version").toInt(0);
}

bool
Files::copyRecursively(const QString &srcPath, const QString &dstPath)
{
    QDir srcDir(srcPath);
    if (!srcDir.exists())
        return false;

    QDir dstDir(dstPath);
    if (!dstDir.exists()) {
        if (!dstDir.mkpath(".")) return false;
    }

    // ファイルをコピー
    for (const QFileInfo &fileInfo : srcDir.entryInfoList(QDir::Files)) {
        QString srcFilePath = fileInfo.absoluteFilePath();
        QString dstFilePath = dstDir.filePath(fileInfo.fileName());
        if (QFile::exists(dstFilePath)) {
            QFile::remove(dstFilePath); // 上書きしたい場合
        }
        if (!QFile::copy(srcFilePath, dstFilePath))
            return false;
    }

    // サブディレクトリを再帰的にコピー
    for (const QFileInfo &dirInfo : srcDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString srcSubDir = dirInfo.absoluteFilePath();
        QString dstSubDir = dstDir.filePath(dirInfo.fileName());
        if (!copyRecursively(srcSubDir, dstSubDir))
            return false;
    }

    return true;
}
