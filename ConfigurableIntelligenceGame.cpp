
// ConfigurableIntelligenceGame.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
//#include "vld.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ConfigurableIntelligenceGame.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConfigurableIntelligenceGameApp

BEGIN_MESSAGE_MAP(CConfigurableIntelligenceGameApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CConfigurableIntelligenceGameApp::OnAppAbout)
END_MESSAGE_MAP()


// CConfigurableIntelligenceGameApp 构造

CConfigurableIntelligenceGameApp::CConfigurableIntelligenceGameApp()
{
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ConfigurableIntelligenceGame.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CConfigurableIntelligenceGameApp 对象

CConfigurableIntelligenceGameApp theApp;


// CConfigurableIntelligenceGameApp 初始化

BOOL CConfigurableIntelligenceGameApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;

	if (!pFrame)
	{
		return FALSE;
	}

	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,
					  WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
					  NULL);





	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

int CConfigurableIntelligenceGameApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	return CWinApp::ExitInstance();
}

// CConfigurableIntelligenceGameApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
	public:
		CAboutDlg();

		// 对话框数据
		enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

		// 实现
	protected:
		DECLARE_MESSAGE_MAP()
	public:
	protected:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CConfigurableIntelligenceGameApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CConfigurableIntelligenceGameApp 消息处理程序
