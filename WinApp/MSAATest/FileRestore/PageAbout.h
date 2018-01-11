#pragma once


// CPageAbout 对话框

class CPageAbout : public CDialogEx
{
	DECLARE_DYNAMIC(CPageAbout)

public:
	CPageAbout(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageAbout();

// 对话框数据
	enum { IDD = IDD_PAGE_ABOUT };

	BOOL Create(CWnd* pParent);
	BOOL Show(BOOL bShow);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
