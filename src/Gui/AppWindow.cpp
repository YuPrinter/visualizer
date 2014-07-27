#include <QtQuick/QQuickWindow>

#include "Gui/AppWindow.h"

#include "Quick/ModelViewer.h"

namespace Gui {
    AppWindow::AppWindow(const QString & qmlSource, QObject * parent) :
        QObject(parent) {

        registerQmlTypes();

        _engine = new QQmlApplicationEngine(QUrl(qmlSource));

        // path to db
        qDebug() << _engine->offlineStoragePath();

        fetchConnections();
    }

    AppWindow::~AppWindow() {
        delete _engine;
    }

    void AppWindow::fetchConnections() {
        QObject * appWindow = _engine->rootObjects().at(0);

        _appWindow = qobject_cast<QQuickWindow *>(appWindow);

        QSurfaceFormat surfaceFormat = _appWindow->format();
        surfaceFormat.setVersion(4, 1);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        surfaceFormat.setSwapInterval(10);

        _appWindow->setFormat(surfaceFormat);

        QObject::connect(appWindow, SIGNAL(fileOpenedDcm(const QUrl &)), this, SIGNAL(fileOpenedDcm(const QUrl &)));
        QObject::connect(appWindow, SIGNAL(fileOpenedStl(const QUrl &)), this, SIGNAL(fileOpenedStl(const QUrl &)));

        QObject::connect(appWindow, SIGNAL(filesOpened(QVariant)), this, SLOT(readFiles(QVariant)));

        QObject::connect(appWindow, SIGNAL(sliceNumberChanged(const int &)), this, SIGNAL(sliceNumberChanged(const int &)));

        foreach (QObject * modelItem, _appWindow->findChild<QQuickItem *>("modelRow")->children()) {
            Quick::ModelViewer * modelViewer = modelItem->findChild<Quick::ModelViewer *>("modelViewer");

            QObject::connect(this, &AppWindow::slicesProcessed, modelViewer, &Quick::ModelViewer::drawSlices);
            QObject::connect(this, &AppWindow::modelRead, modelViewer, &Quick::ModelViewer::modelRead);

            QObject::connect(modelViewer, &Quick::ModelViewer::minHUChanged, this, &AppWindow::minHUChanged);
            QObject::connect(modelViewer, &Quick::ModelViewer::maxHUChanged, this, &AppWindow::maxHUChanged);

            QObject::connect(modelViewer, SIGNAL(appearedSmthToDraw()), modelItem, SLOT(show()));

            QObject::connect(_appWindow, &QQuickWindow::heightChanged, [=](const int & height) {
                modelViewer->setHeight(height);
                modelViewer->update();
            });

            QObject::connect(_appWindow, &QQuickWindow::widthChanged, [=](const int & width) {
                modelViewer->setWidth(width * appWindow->property("sideBarWidth").toFloat());
                modelViewer->update();
            });

            QObject::connect(_appWindow, &QQuickWindow::visibilityChanged, [=](const QWindow::Visibility & visibility) {
                if (visibility != (QWindow::Minimized || QWindow::Hidden)) {
                    modelViewer->update();
                }
            });
        }
    }

    void AppWindow::readFiles(QVariant fileNames) {
        QStringList fileNamesStr;

        foreach (QVariant item, fileNames.value<QList<QUrl> >()) {
            fileNamesStr.append(item.toUrl().toLocalFile());
        }

        emit filesOpened(fileNamesStr);
    }

    void AppWindow::registerQmlTypes() {
        qmlRegisterType<Quick::ModelViewer>("RenderTools", 1, 0, "ModelViewer");
    }

    void AppWindow::show() {
        _appWindow->show();
    }
}
