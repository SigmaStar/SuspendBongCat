
// SuspendBongoCatGUIDlg.h: 头文件
//

#pragma once
#include <vector>

// CSuspendBongoCatGUIDlg 对话框
class CSuspendBongoCatGUIDlg : public CDialogEx
{
// 构造
public:
	CSuspendBongoCatGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUSPENDBONGOCATGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	std::vector<HANDLE> vecThreadList;
	HWND hBongoCatWnd;
public:
	CStatic m_txtRemind;
	afx_msg void OnBnClickedButton1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
