/****************************************************************************/
/*                                                                          */
/*      文件名:    DlgWorkDirBowser.h                                       */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "afxcmn.h"



// CDlgWorkDirBowser 对话框
class CServerManagerClientView;

class CDlgWorkDirBowser : public CDialog
{
protected:
	struct SORT_PARAM
	{
		CListCtrl *			pList;	
		int					SortCol;
		int					SortOrder;
	};
	CServerManagerClientView *	m_pView;
	CDlgTrannferProgress		m_TransferProgress;

	CImageList					m_SmallIcons;
	CImageList					m_LargeIcons;
	CListCtrl					m_lvFileList;
	CListCtrl					m_lvLocalFileList;
	CEasyString					m_ServerAddress;
	UINT						m_ServerIndex;
	CEasyString					m_CurDir;
	CEasyString					m_CurLocalDir;

	CEasyArray<TRANSFER_FILE_INFO>	m_TransferList;
	CEasyBuffer						m_TransferBuffer;
	UINT							m_TransferLeftSize;
	UINT							m_OriginSize;
	CEasyBuffer						m_PackProp;
	CEasyTime						m_CurTransferFileLastWriteTime;

	bool							m_IsInEdit;	
	CEasyString						m_CurEditFilePath;

	CEasyTimer						m_RefreshTimer;


	DECLARE_DYNAMIC(CDlgWorkDirBowser)

public:
	CDlgWorkDirBowser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWorkDirBowser();

// 对话框数据
	enum { IDD = IDD_DIALOG_WORK_DIR_BROWSER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnColumnclickListFilesLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListFilesLocal(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedUpload();
	afx_msg void OnBnClickedDownload();

	afx_msg void OnNMRClickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListFilesLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnServerCreateDir();

	afx_msg void OnServerEditFile();
	afx_msg void OnServerRunFile();

	afx_msg void OnServerDeleteFile();
	afx_msg void OnServerRefresh();
	afx_msg void OnLocalRefresh();
	afx_msg void OnServerRunScript();
	

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();

	void SetServerInfo(CServerManagerClientView * pView,LPCTSTR szServerAddress,UINT ServerIndex)
	{
		m_pView=pView;
		m_ServerAddress=szServerAddress;
		m_ServerIndex=ServerIndex;
	}


	void Browse(LPCTSTR szDir);
	void BrowseLocal(LPCTSTR szDir);

	int GetSHIcon(LPCTSTR FileType,DWORD FileAttr,BOOL Large);

	void OnBrowseResult(LPCTSTR szDir,LPCTSTR szFileName,DWORD FileAttr,ULONG64 FileSize,time_t CreateTime,time_t LastAccessTime,time_t LastWriteTime);
	void OnBrowseFinish();

	void NextUploadFile();
	void NextDownloadFile();	
	void ContinueUploadFile(bool IsSucceed);
	void ContinueDownloadFile(bool IsSucceed);

	void OnStartDownloadResult(int Result,UINT TotalSize,UINT OrgTotalSize,LPCVOID pPackProps,UINT PackPropSize,time_t LastWriteTime);
	void OnDownloadData(int Result,UINT TotalSize,UINT LeftSize,LPCVOID pData,UINT DataSize);

	void OnCreateDirResult(int Result);
	void OnDeleteFileResult(int Result);

	void DoEdit(LPCTSTR Text,UINT Size);
	
protected:
	static int CALLBACK ItemSortCallBack(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);


	
public:
	
	
	
	
};
