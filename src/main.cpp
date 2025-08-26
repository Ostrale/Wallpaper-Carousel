#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "wallpapercontroller.h"
#include "imageloader.h"
#include "infinitelistmodel.h"
#include "screenhelper.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Ostrale");
    app.setApplicationName("Wallpaper Carousel");

    WallpaperController controller;
    ImageLoader loader(controller.wallpaperDir());
    InfiniteListModel infiniteModel;
    infiniteModel.setImages(loader.imageList());

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("controller", &controller);
    engine.rootContext()->setContextProperty("infiniteModel", &infiniteModel);

    engine.loadFromModule("CarouselUI", "Main");
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    auto screenHelper = new ScreenHelper(window);
    engine.rootContext()->setContextProperty("screenHelper", screenHelper);

    QObject::connect(&loader, &ImageLoader::imagesChanged, &infiniteModel, &InfiniteListModel::setImages);

    return app.exec();
}

