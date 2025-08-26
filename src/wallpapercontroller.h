#pragma once
#include <QObject>
#include <QString>
#include <QScreen>
#include <QGuiApplication>
#include <QDBusInterface>
#include <QMap>

class WallpaperController : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperController(QObject *parent = nullptr);

    Q_INVOKABLE void applyWallpaperForScreen(int screenIndex, const QString &imagePath);
    Q_INVOKABLE QString wallpaperDir() const;

signals:
    void error(const QString &msg);
private:
    QString m_wallpaperDir;
    QString m_theme;
    QMap<QString, QPair<QString, QString>> m_pluginConfig;
    QMap<QString, QString> m_extensionCategoryMap = {};
    bool setPlasmaWallpaperJavaScript(int screenIndex, const QString &imagePath, const QString &pluginType, const QString &writeKey);

};
