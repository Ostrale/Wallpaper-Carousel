#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QScreen>
#include <QGuiApplication>

class ScreenHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentScreenIndex READ currentScreenIndex NOTIFY currentScreenIndexChanged)

public:
    explicit ScreenHelper(QQuickWindow *window, QObject *parent = nullptr);

    int currentScreenIndex() const { return m_currentScreenIndex; }

signals:
    void currentScreenIndexChanged();

private slots:
    void updateScreenIndex();

private:
    int findWindowsIndexForScreen(QScreen *screen);

    QQuickWindow *m_window;
    int m_currentScreenIndex;
};

