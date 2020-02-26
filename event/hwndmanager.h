#ifndef HWNDMANAGER_H
#define HWNDMANAGER_H

#include <windows.h>
#include <QList>

struct TagWinWidget
{
    HWND    hwnd;
    QString title;

    TagWinWidget(HWND _hwnd, const QString &_title)
    {
        hwnd = _hwnd;
        title = _title;
    }
};

class HwndManager
{
public:
    static HwndManager* instance();

    QList<TagWinWidget> getWinWidget(const QString &title);

    void addWinWidget(const TagWinWidget &winWidget);

    const QString& filter() const;

protected:
    HwndManager();

private:
    QList<TagWinWidget> m_listHwnd;
    QString             m_filter;
};

#endif // HWNDMANAGER_H
