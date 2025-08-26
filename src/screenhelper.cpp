#include "screenhelper.h"

ScreenHelper::ScreenHelper(QQuickWindow *window, QObject *parent)
    : QObject(parent), m_window(window), m_currentScreenIndex(-1)
{
    if (m_window) {
        connect(m_window, &QWindow::screenChanged,
                this, &ScreenHelper::updateScreenIndex);
        updateScreenIndex();
    }
}

void ScreenHelper::updateScreenIndex() 
{
    if (!m_window) {
        return;
    }

    const QPoint center = m_window->geometry().center();
    auto screens = QGuiApplication::screens();

    std::sort(screens.begin(), screens.end(), [](QScreen *a, QScreen *b) {
        const QRect ga = a->geometry();
        const QRect gb = b->geometry();
        if (ga.top() == gb.top()) {
            return ga.left() < gb.left();
        }
        return ga.top() < gb.top();
    });

    for (int i = 0; i < screens.size(); ++i) {
        if (screens[i]->geometry().contains(center)) {
            if (m_currentScreenIndex != i) {
                m_currentScreenIndex = i;
                emit currentScreenIndexChanged();
            }
            return;
        }
    }

    if (m_currentScreenIndex != -1) {
        m_currentScreenIndex = -1;
        emit currentScreenIndexChanged();
    }
}

