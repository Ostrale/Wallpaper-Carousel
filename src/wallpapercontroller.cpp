#include "wallpapercontroller.h"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QGuiApplication>
#include <QDBusReply>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QMimeDatabase>
#include <toml++/toml.h>

WallpaperController::WallpaperController(QObject *parent)
    : QObject(parent)
{
    // 🔹 Initialisation des correspondances extensions -> catégories
    m_extensionCategoryMap["png"] = "image";
    m_extensionCategoryMap["jpg"] = "image";
    m_extensionCategoryMap["jpeg"] = "image";
    m_extensionCategoryMap["bmp"] = "image";
    m_extensionCategoryMap["gif"] = "animation";
    m_extensionCategoryMap["webp"] = "animation";
    m_extensionCategoryMap["mp4"] = "video";
    m_extensionCategoryMap["webm"] = "video";

    QString configDir = QDir::homePath() + "/.config/wallpaper-carousel";
    QString configPath = configDir + "/config.toml";
    QString pictures = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString defaultDir = pictures + "/Wallpapers";

    // 🔹 Création du dossier de configuration
    QDir().mkpath(configDir);

    QString wallpaperDir;

    if (!QFile::exists(configPath)) {
        // 🔹 Si le fichier n'existe pas, on le crée avec les valeurs par défaut
        QFile file(configPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "# Wallpaper Carousel configuration\n";
            out << "# wallpaper_dir: path to folder with wallpapers\n";
            out << "wallpaper_dir = \"" << defaultDir << "\"\n";
            out << "\n# [plugins] section allows you to define custom wallpaper plugins\n";
            out << "# format: [plugins.category]\n";
            out << "# type = \"PluginType\"\n";
            out << "# write_key = \"WriteKey\"\n";
            out << "\n[plugins.image]\n";
            out << "type = \"org.kde.image\"\n";
            out << "write_key = \"Image\"\n";
            out << "\n[plugins.video]\n";
            out << "type = \"luisbocanegra.smart.video.wallpaper.reborn\"\n";
            out << "write_key = \"VideoUrls\"\n";
            out << "\n[plugins.animation]\n";
            out << "type = \"org.kde.image\"\n";
            out << "write_key = \"Image\"\n";
            file.close();
            qDebug() << "[WallpaperController] Created default config at" << configPath;
        }
        wallpaperDir = defaultDir;
    } else {
        try {
            toml::table tbl = toml::parse_file(configPath.toStdString());

            if (auto val = tbl["wallpaper_dir"].value<std::string>()) {
                QString candidate = QString::fromStdString(*val);
                candidate.replace("$HOME", QDir::homePath());
                wallpaperDir = candidate;
            }

            // 🔹 Lecture de la configuration des plugins (par catégorie)
            if (auto pluginsTable = tbl.get_as<toml::table>("plugins")) {
                for (auto&& [key, val] : *pluginsTable) {
                    if (auto plugin = val.as_table()) {
                        if (auto typeVal = plugin->get_as<std::string>("type")) {
                            if (auto writeKeyVal = plugin->get_as<std::string>("write_key")) {
                                QString category = QString::fromStdString(std::string(key.str()));
                                QString pluginType = QString::fromStdString(std::string(*typeVal));
                                QString writeKey = QString::fromStdString(std::string(*writeKeyVal));
                                m_pluginConfig[category] = qMakePair(pluginType, writeKey);
                                qDebug() << "[WallpaperController] Loaded custom plugin config for category:" << category;
                            }
                        }
                    }
                }
            }
        }
        catch (const toml::parse_error &err) {
            qWarning() << "[WallpaperController] Failed to parse config:" << err.description().data();
            wallpaperDir = defaultDir;
        }
    }

    if (wallpaperDir.isEmpty()) {
        wallpaperDir = defaultDir;
    }

    m_wallpaperDir = wallpaperDir;

    qDebug() << "[WallpaperController] Initialized with wallpaperDir:" << m_wallpaperDir;
}

QString WallpaperController::wallpaperDir() const {
    qDebug() << "[wallpaperDir] Returning:" << m_wallpaperDir;
    return m_wallpaperDir;
}

void WallpaperController::applyWallpaperForScreen(int screenIndex, const QString &imagePath)
{
    qDebug() << "[applyWallpaperForScreen] Called with screenIndex:" << screenIndex << "imagePath:" << imagePath;

    QString path = imagePath;
    if (path.startsWith("file://")) {
        path = path.mid(7);
        qDebug() << "[applyWallpaperForScreen] Stripped file:// ->" << path;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        QString errorMsg = QString("Image not found: %1").arg(path);
        qWarning() << "[applyWallpaperForScreen] " << errorMsg;
        emit error(errorMsg);
        return;
    }

    QString fileExtension = fileInfo.suffix().toLower();
    QString pluginType = "org.kde.image";
    QString writeKey = "Image";

    // 🔹 Logique de déduction : on cherche la catégorie à partir de l'extension
    QString category;
    if (m_extensionCategoryMap.contains(fileExtension)) {
        category = m_extensionCategoryMap[fileExtension];
    } else {
        qWarning() << "[applyWallpaperForScreen] No category found for extension:" << fileExtension << "Using default 'image' category.";
        category = "image";
    }

    // 🔹 On utilise la catégorie pour trouver les informations du plugin
    if (m_pluginConfig.contains(category)) {
        pluginType = m_pluginConfig[category].first;
        writeKey = m_pluginConfig[category].second;
        qDebug() << "[applyWallpaperForScreen] Found plugin config for category:" << category << "-> Type:" << pluginType << "WriteKey:" << writeKey;
    } else {
        qDebug() << "[applyWallpaperForScreen] Plugin config not found for category:" << category << "Using default 'org.kde.image'.";
    }

    if (!setPlasmaWallpaperJavaScript(screenIndex, path, pluginType, writeKey)) {
        QString errorMsg = QString("Failed to set wallpaper for screen %1").arg(screenIndex);
        qWarning() << "[applyWallpaperForScreen] " << errorMsg;
        emit error(errorMsg);
    } else {
        qDebug() << "[applyWallpaperForScreen] Wallpaper applied successfully.";
    }
}

// Reste du code pour setPlasmaWallpaperJavaScript...
bool WallpaperController::setPlasmaWallpaperJavaScript(int screenIndex, const QString &imagePath, const QString &pluginType, const QString &writeKey)
{
    // Le code de cette fonction est inchangé par rapport à la réponse précédente.
    // ...
    qDebug() << "[setPlasmaWallpaperJavaScript] Called with screenIndex:" << screenIndex << "imagePath:" << imagePath << "pluginType:" << pluginType << "writeKey:" << writeKey;

    QString path = imagePath;
    if (path.startsWith("file://")) {
        path = path.mid(7);
        qDebug() << "[setPlasmaWallpaperJavaScript] Stripped file:// ->" << path;
    }

    QString js = QString(R"(
const allDesktops = desktops()
    .filter(d => d.screen !== -1)
    .sort((a, b) => {
        const ga = screenGeometry(a.screen);
        const gb = screenGeometry(b.screen);
        if (ga.top === gb.top) {
            return ga.left - gb.left;
        }
        return ga.top - gb.top;
    });

const targetDesktop = allDesktops[%1];
if (targetDesktop) {
    targetDesktop.wallpaperPlugin = "%2";
    targetDesktop.currentConfigGroup = ["Wallpaper", "%2", "General"];
    targetDesktop.writeConfig("Image", "file:///dev/null");
    targetDesktop.writeConfig("%3", "file://%4");
}
)").arg(screenIndex).arg(pluginType).arg(writeKey).arg(path);

    qDebug() << "[setPlasmaWallpaperJavaScript] JavaScript being sent to DBus:\n" << js;

    QDBusInterface iface("org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell");
    if (!iface.isValid()) {
        qWarning() << "[setPlasmaWallpaperJavaScript] plasmashell DBus interface not available";
        return false;
    }

    QDBusReply<QString> reply = iface.call("evaluateScript", js);
    if (!reply.isValid()) {
        qWarning() << "[setPlasmaWallpaperJavaScript] evaluateScript call failed:" << reply.error().message();
        return false;
    }

    qDebug() << "[setPlasmaWallpaperJavaScript] Wallpaper script executed successfully.";
    return true;
}
