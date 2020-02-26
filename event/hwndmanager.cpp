#include "hwndmanager.h"

BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
{
    Q_UNUSED(lParam)

    char WindowTitle[100] = { 0 };
    ::GetWindowText(hWnd, LPWSTR(WindowTitle), 100);

    QString title = QString::fromStdWString(LPWSTR(WindowTitle));

    if(title.contains(HwndManager::instance()->filter()))
    {
        HwndManager::instance()->addWinWidget(TagWinWidget(hWnd, title));
    }

    return true;
}

////EnumWindows回调函数，hwnd为发现的顶层窗口
//BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
//{
//	if (GetParent(hWnd) == nullptr && IsWindowVisible(hWnd))  //判断是否顶层窗口并且可见
//	{
//		char WindowTitle[100] = { 0 };
//		::GetWindowText(hWnd, WindowTitle, 100);
//		printf("%s\n", WindowTitle);

//		EnumChildWindows(hWnd, EnumChildWindowsProc, NULL); //获取父窗口的所有子窗口
//		ok = false;
//	}

//	return true;
//}

HwndManager *HwndManager::instance()
{
    static HwndManager hwndManager;
    return &hwndManager;
}

QList<TagWinWidget> HwndManager::getWinWidget(const QString &title)
{
    m_listHwnd.clear();
    m_filter = title;

    EnumChildWindows(nullptr, EnumChildWindowsProc, 0);

    return m_listHwnd;
}

void HwndManager::addWinWidget(const TagWinWidget &winWidget)
{
    m_listHwnd.append(winWidget);
}

const QString &HwndManager::filter() const
{
    return m_filter;
}

HwndManager::HwndManager()
{
    //
}
