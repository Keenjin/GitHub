
// xcopyfileDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CxcopyfileDlg �Ի���
class CxcopyfileDlg : public CDialogEx
{
// ����
public:
	CxcopyfileDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_XCOPYFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_oInput;
	CEdit m_oDstDir;
	afx_msg void OnBnClickedButton1();
	CEdit m_oOutputFile;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton3();
	CEdit m_oSrcRoot;
};
