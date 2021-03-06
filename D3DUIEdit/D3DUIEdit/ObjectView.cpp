/****************************************************************************/
/*                                                                          */
/*      文件名:    ObjectView.cpp                                           */
/*      创建日期:  2011年10月07日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// ObjectView.cpp : 实现文件
//

#include "stdafx.h"
#include "D3DUIEdit.h"
#include "D3DUIEditDoc.h"
#include "ObjectView.h"
#include "ObjectTreeView.h"
#include "ObjectPropertyView.h"
#include ".\objectview.h"

// CObjectView

IMPLEMENT_DYNCREATE(CObjectView, CView)

CObjectView::CObjectView()
{
	m_pObjectTreeView=NULL;
	m_pObjectPropertyView=NULL;
}

CObjectView::~CObjectView()
{
}

BEGIN_MESSAGE_MAP(CObjectView, CView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CObjectView 绘图

void CObjectView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CObjectView 诊断

#ifdef _DEBUG
void CObjectView::AssertValid() const
{
	CView::AssertValid();
}

void CObjectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CObjectView 消息处理程序

void CObjectView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CRect rect;	
	GetClientRect(rect);
	m_wndSplitter.SetRowInfo(0,rect.Height()/2,0);
	m_wndSplitter.RecalcLayout();
}

void CObjectView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	int row1,row2,col1,col2,SizeMin;
	if(::IsWindow(m_wndSplitter.GetSafeHwnd()))
	{
		CRect Rect;
		m_wndSplitter.GetWindowRect(&Rect);
		if(m_wndSplitter.GetRowCount()>1)
		{
			m_wndSplitter.GetRowInfo(0,row1,SizeMin);
			m_wndSplitter.GetRowInfo(1,row2,SizeMin);
		}
		if(m_wndSplitter.GetColumnCount()>1)
		{
			m_wndSplitter.GetColumnInfo(0,col1,SizeMin);
			m_wndSplitter.GetColumnInfo(1,col2,SizeMin);
		}
		m_wndSplitter.MoveWindow(0,0,cx,cy);
		if(m_wndSplitter.GetRowCount()>1&&Rect.Height())
		{
			m_wndSplitter.SetRowInfo(0,(int)floor(cy*(double)row1/Rect.Height()+0.5),0);			
			m_wndSplitter.RecalcLayout();
		}
		if(m_wndSplitter.GetColumnCount()>1&&Rect.Width())
		{
			m_wndSplitter.SetColumnInfo(0,(int)floor(cx*(double)col1/Rect.Width()+0.5),0);			
			m_wndSplitter.RecalcLayout();
		}
	}
}

BOOL CObjectView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(!CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return false;

	if(!m_wndSplitter.CreateStatic(this,2,1))
		return false;

	if(!m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CObjectTreeView),CSize(200,300),pContext))
		return false;
	if(!m_wndSplitter.CreateView(1,0,RUNTIME_CLASS(CObjectPropertyView),CSize(0,0),pContext))
		return false;

	m_pObjectTreeView=(CObjectTreeView *)m_wndSplitter.GetPane(0,0);
	m_pObjectPropertyView=(CObjectPropertyView *)m_wndSplitter.GetPane(1,0);

	//GetDocument()->SetItemTreeView((CObjectTreeView *)m_wndSplitter.GetPane(0,0));
	//GetDocument()->SetPropertyView((CObjectPropertyView *)m_wndSplitter.GetPane(1,0));
	m_wndSplitter.SetActivePane(0,0);
	return true;
}
