#pragma once
#include "afxdialogex.h"


// CDownloadDlg 对话框

class CDownloadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDownloadDlg)

public:
	CDownloadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DOWNLOAD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void SetControlItemPos();
	Gdiplus::Bitmap* m_pBgBitmap;  // 背景图像对象
	CProgressCtrl m_Progress;
	CStatic m_ProgressText;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

};
