/****************************************************************************/
/*                                                                          */
/*      文件名:    BaseTCPConnection.cpp                                    */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CBaseTCPConnection,CNameObject);
CBaseTCPConnection::CBaseTCPConnection(void)
{
}

CBaseTCPConnection::~CBaseTCPConnection(void)
{
}

UINT CBaseTCPConnection::GetCurSendQueryCount()
{
	return 0;
}