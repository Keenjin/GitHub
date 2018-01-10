#pragma once


// CPageBasic dialog

class CPageBasic : public CDialogEx
{
	DECLARE_DYNAMIC(CPageBasic)

public:
	CPageBasic(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageBasic();

	BOOL Create(CWnd* pParent);
	BOOL Show(BOOL bShow);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_BASIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
