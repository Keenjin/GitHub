#pragma once


// CPageAbout �Ի���

class CPageAbout : public CDialogEx
{
	DECLARE_DYNAMIC(CPageAbout)

public:
	CPageAbout(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageAbout();

// �Ի�������
	enum { IDD = IDD_PAGE_ABOUT };

	BOOL Create(CWnd* pParent);
	BOOL Show(BOOL bShow);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
