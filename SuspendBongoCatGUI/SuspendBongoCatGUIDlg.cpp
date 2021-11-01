
// SuspendBongoCatGUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SuspendBongoCatGUI.h"
#include "SuspendBongoCatGUIDlg.h"
#include "afxdialogex.h"
#include <string>
#include <TlHelp32.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSuspendBongoCatGUIDlg::CSuspendBongoCatGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUSPENDBONGOCATGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSuspendBongoCatGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMIND, m_txtRemind);
}

BEGIN_MESSAGE_MAP(CSuspendBongoCatGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CSuspendBongoCatGUIDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


string WideStringToAscii(wstring pwstring)
{
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, pwstring.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (dwNum == 0)
	{
		return "<error while convert from wide string to ascii>";
	}
	char* pastring = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, 0, pwstring.c_str(), -1, pastring, dwNum, NULL, NULL);
	string res = pastring;
	delete[]pastring;
	return res;
}

// CSuspendBongoCatGUIDlg 消息处理程序

BOOL CSuspendBongoCatGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_txtRemind.SetWindowTextW(L"Bongo Cat MVer已暂停工作\n，请点击恢复运行...");

	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnapshot)
	{
		return 0;
	}
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(hProcessSnapshot, &ProcessEntry);
	bool bFound = false;
	while (bRet)
	{
		string ExeName = WideStringToAscii(ProcessEntry.szExeFile);
		if (ExeName.find("Bongo Cat Mver") != string::npos)
		{
			bFound = true;
			break;
		}
		bRet = Process32Next(hProcessSnapshot, &ProcessEntry);
	}
	CloseHandle(hProcessSnapshot);
	if (!bFound)
	{
		MessageBox(L"你还没有运行Bongo Cat!", L"错误");
		exit(0);
	}
	hBongoCatWnd = ::FindWindow(NULL, L"Bongo Cat Mver v0.1.6");
	::ShowWindow(hBongoCatWnd, FALSE);
	if (hBongoCatWnd == NULL)
	{
		MessageBox(L"无法定位窗口",L"错误");
		exit(0);
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessEntry.th32ProcessID);
	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	bRet = Thread32First(hThreadSnapshot, &te);
	while (bRet)
	{
		if (te.th32OwnerProcessID == ProcessEntry.th32ProcessID)
		{
			HANDLE hThread = OpenThread(READ_CONTROL | THREAD_SUSPEND_RESUME, false, te.th32ThreadID);
			SuspendThread(hThread);
			vecThreadList.push_back(hThread);

		}
		bRet = Thread32Next(hThreadSnapshot, &te);
	}
	
	CloseHandle(hProcess);
	CloseHandle(hThreadSnapshot);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSuspendBongoCatGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSuspendBongoCatGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSuspendBongoCatGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSuspendBongoCatGUIDlg::OnBnClickedButton1()
{
	for (int i = 0; i < vecThreadList.size(); i++)
	{
		ResumeThread(vecThreadList[i]);
		CloseHandle(vecThreadList[i]);
	}
	::ShowWindow(hBongoCatWnd, TRUE);
	exit(0);
}


BOOL CSuspendBongoCatGUIDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
