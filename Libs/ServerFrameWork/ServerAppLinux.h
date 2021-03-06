/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerAppLinux.h                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once




class CServerApp
{
protected:
	volatile UINT		m_WantExist;
	IBaseServer *		m_pServer;
public:
	CServerApp(void);
	~CServerApp(void);

public:

	virtual BOOL OnStartUp();
	virtual void OnShutDown();

	virtual int Run();

protected:
	void GetProgamVersion();

};
