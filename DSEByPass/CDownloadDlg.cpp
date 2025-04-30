// CDownloadDlg.cpp: 实现文件
//

#include "pch.h"
#include "DSEByPass.h"
#include "afxdialogex.h"
#include "CDownloadDlg.h"


// CDownloadDlg 对话框

IMPLEMENT_DYNAMIC(CDownloadDlg, CDialogEx)

CDownloadDlg::CDownloadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DOWNLOAD_DIALOG, pParent)
{
	
}

CDownloadDlg::~CDownloadDlg()
{
}

void CDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDownloadDlg, CDialogEx)
	ON_WM_PAINT()

END_MESSAGE_MAP()


// CDownloadDlg 消息处理程序

void CDownloadDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()


	if (m_pBgBitmap && m_pBgBitmap->GetLastStatus() == Gdiplus::Ok)
	{
		Gdiplus::Graphics graphics(dc);
		graphics.DrawImage(m_pBgBitmap, 0, 0,
			m_pBgBitmap->GetWidth()*0.5,
			m_pBgBitmap->GetHeight()*0.5);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

BOOL CDownloadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pBgBitmap = Gdiplus::Bitmap::FromFile(L"res\\loading.jpg");
    
	if (m_pBgBitmap && m_pBgBitmap->GetLastStatus() == Gdiplus::Ok)
	{
		SetWindowPos(NULL, 0, 0,
			m_pBgBitmap->GetWidth()*0.5, m_pBgBitmap->GetHeight()*0.5,
			SWP_NOMOVE | SWP_NOZORDER);
	}

	m_Progress.SubclassDlgItem(IDC_DOWNLOADPROGRESS,this);
	m_ProgressText.SubclassDlgItem(IDC_STATICPRECENT,this);


	m_Progress.SetRange(0, 100);
	CRect rect;
	GetClientRect(&rect);

	// 计算进度条位置和大小 - 改进版
	int progressHeight = 20;  // 固定高度
	int progressWidth = rect.Width() * 2 / 3;  // 宽度占窗口2/3
	int progressLeft = (rect.Width() - progressWidth) / 2;  // 水平居中
	int progressTop = rect.Height() *  6/7;  // 垂直位置在下部2/3处

	// 设置进度条位置和大小
	m_Progress.SetWindowPos(NULL,
		progressLeft,
		progressTop,
		progressWidth,
		progressHeight,
		SWP_NOZORDER);

	// 计算文本位置 - 居中在进度条上
	CRect progressRect(progressLeft, progressTop, progressLeft + progressWidth, progressTop + progressHeight);
	m_ProgressText.SetWindowPos(NULL,
		progressRect.left,
		progressRect.top + (progressHeight - 20) / 2, // 垂直居中
		progressRect.Width(),
		20, // 文本高度
		SWP_NOZORDER);

	// 设置文本控件透明背景
	m_ProgressText.ModifyStyle(0, SS_CENTER); // 文本居中
	m_ProgressText.SetWindowText(_T("0%")); // 设置默认文本


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
