
// xcopyfileDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CxcopyfileDlg 对话框
class CxcopyfileDlg : public CDialogEx
{
// 构造
public:
	CxcopyfileDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_XCOPYFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
