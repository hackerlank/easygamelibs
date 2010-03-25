/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DCombo.h                                               */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "d3dedit.h"


namespace D3DGUI{

class CD3DCombo :
	public CD3DEdit
{
protected:
	CD3DButton *			m_pExpandButton;
	CD3DSimpleList *		m_pComboList;

	bool					m_IsListShow;

	DECLARE_CLASS_INFO(CD3DCombo)
public:
	CD3DCombo();
	CD3DCombo(CD3DGUI * pGUI);
	virtual ~CD3DCombo(void);
	virtual void InitWnd(CD3DGUI *  pGUI);

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void SetMultiLine(bool IsMultiLine){}
	virtual void EnableScrollBar(bool Enable){}
	virtual void SetVisible(bool IsVisible);

	virtual CEasyRect GetCenterRect();

	virtual void UpdateRects();

	CD3DButton * GetExpandButton()
	{
		return m_pExpandButton;
	}
	
	CD3DSimpleList * GetComboList()
	{
		return m_pComboList;
	}

	void SelectItem(int Index);	

	void AddItem(LPCTSTR ItemText);
	void InsertItem(int Index,LPCTSTR ItemText);
	bool DeleteItem(int Index);
	void DeleteAllItem();

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

protected:
	virtual void GetMiniSize(int& Width,int& Height);
};

}