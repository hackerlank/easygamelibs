/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerThread.cpp                                         */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


IMPLEMENT_CLASS_INFO_STATIC(CServerThread,CNetServer);

CServerThread::CServerThread()
	:m_CommandExecutor(128,128)
{
	FUNCTION_BEGIN
	m_pSysNetLinkManager=NULL;
	m_pUDPSystemControlPort=NULL;
	FUNCTION_END;
}

CServerThread::~CServerThread(void)
{
	FUNCTION_BEGIN
	
	FUNCTION_END;
}

void CServerThread::Execute()
{
	FUNCTION_BEGIN;

	//装载系统配置
	CSystemConfig::GetInstance()->LoadConfig(MakeModuleFullPath(NULL,GetConfigFileName()));


#ifndef _DEBUG
	CMainGuardThread::GetInstance()->SetTargetThreadID(GetThreadID());
	CMainGuardThread::GetInstance()->SetKeepAliveTime(
		CSystemConfig::GetInstance()->GetGuardThreadKeepAliveTime(),
		CSystemConfig::GetInstance()->GetGuardThreadKeepAliveCount());
	CMainGuardThread::GetInstance()->Start();
#endif
	while((!m_WantTerminate)&&(OnRun()))
	{
#ifndef _DEBUG
		CMainGuardThread::GetInstance()->MakeKeepAlive();
#endif
	}	
#ifndef _DEBUG
	CMainGuardThread::GetInstance()->SafeTerminate();
#endif
	OnBeginTerminate();
	DWORD Time=CEasyTimer::GetTime();
	while(GetTimeToTime(Time,CEasyTimer::GetTime())<SERVER_ENDING_TIME&&OnTerminating())
	{
	}
	
	FUNCTION_END;
}


BOOL CServerThread::OnStart()
{
	FUNCTION_BEGIN;

	
	m_CycleCount=0;
	m_TCPRecvBytes=0;
	m_TCPSendBytes=0;
	m_UDPRecvBytes=0;
	m_UDPSendBytes=0;
	m_TCPRecvCount=0;
	m_TCPSendCount=0;
	m_UDPRecvCount=0;
	m_UDPSendCount=0;
	m_CountTimer.SaveTime();

	


	CEasyString LogFileName;
	CEasyString ModulePath=GetModulePath(NULL);

	CServerLogPrinter * pLog;



	LogFileName.Format("%s/Log/%s",(LPCTSTR)ModulePath,g_ProgramName);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_CONSOLE|CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(SERVER_LOG_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/%s.Status",(LPCTSTR)ModulePath,g_ProgramName);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(SERVER_STATUS_LOG_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	

	LogFileName.Format("%s/Log/%s.NetLib",(LPCTSTR)ModulePath,g_ProgramName);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_NET_CHANNEL,pLog);
	SAFE_RELEASE(pLog);

	LogFileName.Format("%s/Log/%s.DBLib",(LPCTSTR)ModulePath,g_ProgramName);
	pLog=new CServerLogPrinter(this,CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(),LogFileName);
	CLogManager::GetInstance()->AddChannel(LOG_DB_ERROR_CHANNEL,pLog);
	SAFE_RELEASE(pLog);	



	Log("开始启动服务器,当前版本:%u.%u.%u.%u",
		g_ProgramVersion[3],
		g_ProgramVersion[2],
		g_ProgramVersion[1],
		g_ProgramVersion[0]);


	


	m_CommandExecutor.AddFaction("StartLog",3,(INT_PTR)this,StartLog);
	m_CommandExecutor.AddFaction("StopLog",2,(INT_PTR)this,StopLog);
	m_CommandExecutor.AddFaction("TestLog",1,(INT_PTR)this,TestLog);
	m_CommandExecutor.AddFaction("RebuildUDPControlPort",0,(INT_PTR)this,RebuildUDPControlPort);

	if(!CNetServer::OnStart())
		return FALSE;

	//初始化系统连接
	m_pSysNetLinkManager=new CSystemNetLinkManager();
	m_pSysNetLinkManager->SetServerThread(this);

	xml_parser Parser;

	if(Parser.parse_file(MakeModuleFullPath(NULL,GetConfigFileName()),pug::parse_trim_attribute))
	{
		xml_node Config=Parser.document();
		if(Config.moveto_child("SystemLink"))
		{
			if(m_pSysNetLinkManager->Init(this,Config))
			{
				Log("初始化系统连接管理器成功");
			}
			else
			{
				Log("初始化系统连接管理器失败");
			}
		}
		else
		{
			Log("不合法的系统连接配置文件%s",GetConfigFileName());
		}
		
	}
	else
	{
		Log("未找到系统连接配置文件%s",GetConfigFileName());
	}
	
	m_pUDPSystemControlPort=new CSystemControlPort();
	if(!m_pUDPSystemControlPort->Init(this))
	{
		Log("初始化UDP系统控制端口失败");
	}
	

	m_ServerStatus.Create(SERVER_STATUS_BLOCK_SIZE);

	ULONG64_CONVERTER Version;
	memcpy(Version.Words,g_ProgramVersion,sizeof(ULONG64_CONVERTER));

	SetServerStatus(SC_SST_SS_PROGRAM_VERSION,CSmartValue(Version.QuadPart));
	SetServerStatusName(SC_SST_SS_CLIENT_COUNT,"客户端数量");
	SetServerStatusName(SC_SST_SS_CYCLE_TIME,"循环时间(毫秒)");
	SetServerStatusName(SC_SST_SS_TCP_RECV_FLOW,"TCP接收流量(Byte/S)");
	SetServerStatusName(SC_SST_SS_TCP_SEND_FLOW,"TCP发送流量(Byte/S)");
	SetServerStatusName(SC_SST_SS_UDP_RECV_FLOW,"UDP接收流量(Byte/S)");
	SetServerStatusName(SC_SST_SS_UDP_SEND_FLOW,"UDP发送流量(Byte/S)");
	SetServerStatusName(SC_SST_SS_TCP_RECV_COUNT,"TCP接收次数(次/S)");
	SetServerStatusName(SC_SST_SS_TCP_SEND_COUNT,"TCP发送次数(次/S)");
	SetServerStatusName(SC_SST_SS_UDP_RECV_COUNT,"UDP接收次数(次/S)");
	SetServerStatusName(SC_SST_SS_UDP_SEND_COUNT,"UDP发送次数(次/S)");
	SetServerStatusName(SC_SST_SS_PROGRAM_VERSION,"服务器版本");

	Log("服务器成功启动");
	
	FUNCTION_END;
	return TRUE;
}

void CServerThread::OnBeginTerminate()
{
	
}

void CServerThread::OnTerminate()
{
	FUNCTION_BEGIN;	
	
	SAFE_RELEASE(m_pSysNetLinkManager);
	SAFE_RELEASE(m_pUDPSystemControlPort);
	CNetServer::OnTerminate();
	Log("服务器关闭");
	FUNCTION_END;
}

BOOL CServerThread::OnRun()
{
	FUNCTION_BEGIN;		

	CNetServer::OnRun();

	//计算服务器循环时间
	m_CycleCount++;
	if(m_CountTimer.IsTimeOut(SERVER_INFO_COUNT_TIME))
	{		
		m_CountTimer.SaveTime();
		DoServerStat();
	}

	//执行控制台命令
	PANEL_MSG * pCommand=CControlPanel::GetInstance()->GetCommand();
	if(pCommand)
	{
		
		ExecCommand(pCommand->Msg);
		CControlPanel::GetInstance()->ReleaseCommand(pCommand->ID);
	}

	FUNCTION_END;
	return TRUE;
}

int CServerThread::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int Process=0;	
	Process+=CNetServer::Update(ProcessPacketLimit);		
	Process+=m_pSysNetLinkManager->Update(ProcessPacketLimit);
	Process+=m_pUDPSystemControlPort->Update(ProcessPacketLimit);
	return Process;
	FUNCTION_END;
	return 0;
}

BOOL CServerThread::OnTerminating()
{
	return FALSE;
}


int CServerThread::GetClientCount()
{
	return 0;
}

LPCTSTR CServerThread::GetConfigFileName()
{
	return SYSTEM_NET_LINK_CONFIG_FILE_NAME;
}

BOOL CServerThread::PrintConsoleLog(LPCTSTR szLogMsg)
{
	FUNCTION_BEGIN;
#ifdef WIN32
	CControlPanel::GetInstance()->PushMsg(szLogMsg);
#else
	printf("%s\n",szLogMsg);
#endif
	if(m_pSysNetLinkManager)
		m_pSysNetLinkManager->SendLogMsg(szLogMsg);
	FUNCTION_END;
	return FALSE;
}

void CServerThread::ExecCommand(LPCTSTR szCommand)
{
	FUNCTION_BEGIN;
	CBolan Result;
	int RetCode=m_CommandExecutor.ExecScript(szCommand,Result);
	if(RetCode)
	{
		Log("执行命令[%s]出错[%d][%s]",szCommand,RetCode,
			m_CommandExecutor.GetErrorMsg(RetCode));
	}
	else
	{
		if(Result.ValueType==VALUE_TYPE_NUMBER)
			Log("执行命令[%s]结果[%g]",szCommand,Result.value);
		else
			Log("执行命令[%s]结果[%s]",szCommand,(LPCTSTR)Result.StrValue);
	}
	FUNCTION_END;
}

BOOL CServerThread::SetServerStatus(WORD StatusID,const CSmartValue& Value)
{
	FUNCTION_BEGIN;
	if(m_ServerStatus.IDToIndex(StatusID)==CSmartStruct::INVALID_MEMBER_ID)
	{
		m_ServerStatus.AddMember(StatusID,Value);
	}
	else
	{
		m_ServerStatus.GetMember(StatusID).SetValue(Value);
	}
	FUNCTION_END;
	return FALSE;
}

void CServerThread::SetServerStatusName(WORD StatusID,LPCTSTR szStatusName)
{
	FUNCTION_BEGIN;
	CControlPanel::GetInstance()->SetServerStatusName(StatusID,szStatusName);
	FUNCTION_END;
}

int CServerThread::StartLog(INT_PTR FnParam,CVariableList* pVarList,CBolan* pResult,CBolan* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_LOG_CHANNEL);
	}	
	else if(_stricmp(pParams[0].StrValue,"Status")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_STATUS_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"DB")==0)
	{		
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_DB_ERROR_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Net")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_NET_CHANNEL);	
	}
	if(pLog)
	{
		int WitchMode=0;
		if(_stricmp(pParams[1].StrValue,"VS")==0)
		{
			WitchMode=CServerLogPrinter::LOM_VS;
		}
		else if(_stricmp(pParams[1].StrValue,"Console")==0)
		{
			WitchMode=CServerLogPrinter::LOM_CONSOLE;
		}
		else if(_stricmp(pParams[1].StrValue,"File")==0)
		{
			WitchMode=CServerLogPrinter::LOM_FILE;
		}
		if(WitchMode)
		{
			UINT Mode=pLog->GetLogMode();
			Mode|=WitchMode;

			int Level=pLog->GetLogLevel();

			CEasyString LogFileName;
			if(pParams[2].StrValue.IsEmpty())
			{
				LogFileName=pLog->GetLogFileName();
			}
			else
			{
				CEasyString ModulePath=GetModulePath(NULL);
				LogFileName.Format("%s/Log/%s",(LPCTSTR)ModulePath,(LPCTSTR)pParams[2].StrValue);
			}
			

			pLog->SetLogMode(Mode,Level,LogFileName);
			Log("Log模块[%s],模式[%s]的输出已被开启",
				(LPCTSTR)(pParams[0].StrValue),
				(LPCTSTR)(pParams[1].StrValue));
		}
	}
	FUNCTION_END;
	return 0;
}

int CServerThread::StopLog(INT_PTR FnParam,CVariableList* pVarList,CBolan* pResult,CBolan* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_LOG_CHANNEL);
	}	
	else if(_stricmp(pParams[0].StrValue,"Status")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(SERVER_STATUS_LOG_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"DB")==0)
	{
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_DB_ERROR_CHANNEL);
	}
	else if(_stricmp(pParams[0].StrValue,"Net")==0)
	{		
		pLog=(CServerLogPrinter *)CLogManager::GetInstance()->
			GetChannel(LOG_NET_CHANNEL);
	}
	if(pLog)
	{
		int WitchMode=0;
		if(_stricmp(pParams[1].StrValue,"VS")==0)
		{
			WitchMode=CServerLogPrinter::LOM_VS;
		}
		else if(_stricmp(pParams[1].StrValue,"Console")==0)
		{
			WitchMode=CServerLogPrinter::LOM_CONSOLE;
		}
		else if(_stricmp(pParams[1].StrValue,"File")==0)
		{
			WitchMode=CServerLogPrinter::LOM_FILE;
		}
		if(WitchMode)
		{
			UINT Mode=pLog->GetLogMode();
			Mode&=~WitchMode;
			int Level=pLog->GetLogLevel();
			pLog->SetLogMode(Mode,Level,NULL);
			Log("Log模块[%s],模式[%s]的输出已被关闭",
				(LPCTSTR)(pParams[0].StrValue),
				(LPCTSTR)(pParams[1].StrValue));
		}
	}
	FUNCTION_END;
	return 0;
}

int CServerThread::TestLog(INT_PTR FnParam,CVariableList* pVarList,CBolan* pResult,CBolan* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerLogPrinter * pLog=NULL;
	if(_stricmp(pParams[0].StrValue,"Normal")==0)
	{
		Log("Normal");
	}
	else if(_stricmp(pParams[0].StrValue,"Debug")==0)
	{
		LogDebug("Debug");
	}
	else if(_stricmp(pParams[0].StrValue,"Status")==0)
	{		
		LogServerInfo("Status");
	}
	else if(_stricmp(pParams[0].StrValue,"DB")==0)
	{
		PrintDBLog(0,"DB");
	}
	else if(_stricmp(pParams[0].StrValue,"Net")==0)
	{
		PrintNetLog(0,"Net");
	}
	
	FUNCTION_END;
	return 0;
}

int CServerThread::RebuildUDPControlPort(INT_PTR FnParam,CVariableList* pVarList,CBolan* pResult,CBolan* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	CServerThread * pServer=(CServerThread *)FnParam;
	if(pServer)
	{
		if(!pServer->m_pUDPSystemControlPort->Init(pServer))
		{
			Log("重建UDP系统控制端口失败");
		}
		else
		{
			Log("重建UDP系统控制端口成功");
		}
	}
	else
	{
		Log("服务器指针参数异常");
	}
	FUNCTION_END;
	return 0;
}

void CServerThread::DoServerStat()
{
	FUNCTION_BEGIN;
	int ClientCount=GetClientCount();
	float CycleTime=(float)SERVER_INFO_COUNT_TIME/m_CycleCount;
	float TCPRecvFlow=(float)m_TCPRecvBytes*1000/1024/SERVER_INFO_COUNT_TIME;
	float TCPSendFlow=(float)m_TCPSendBytes*1000/1024/SERVER_INFO_COUNT_TIME;
	float UDPRecvFlow=(float)m_UDPRecvBytes*1000/1024/SERVER_INFO_COUNT_TIME;
	float UDPSendFlow=(float)m_UDPSendBytes*1000/1024/SERVER_INFO_COUNT_TIME;

	float TCPRecvCount=(float)m_TCPRecvCount*1000/SERVER_INFO_COUNT_TIME;
	float TCPSendCount=(float)m_TCPSendCount*1000/SERVER_INFO_COUNT_TIME;
	float UDPRecvCount=(float)m_UDPRecvCount*1000/SERVER_INFO_COUNT_TIME;
	float UDPSendCount=(float)m_UDPSendCount*1000/SERVER_INFO_COUNT_TIME;


	LogServerInfo("CycleTime=%06.6g,"
		"TCPRecvFlow=%06.6g,TCPSendFlow=%06.6g,UDPRecvFlow=%06.6g,UDPSendFlow=%06.6g,"
		"TCPRecvCount=%06.6g,TCPSendCount=%06.6g,UDPRecvCount=%06.6g,UDPSendCount=%06.6g,"
		"ClientCount=%04d",
		CycleTime,
		TCPRecvFlow,
		TCPSendFlow,
		UDPRecvFlow,
		UDPSendFlow,
		TCPRecvCount,
		TCPSendCount,
		UDPRecvCount,
		UDPSendCount,
		ClientCount);

	SetServerStatus(SC_SST_SS_CLIENT_COUNT,CSmartValue(ClientCount));
	SetServerStatus(SC_SST_SS_CYCLE_TIME,CSmartValue(CycleTime));
	SetServerStatus(SC_SST_SS_TCP_RECV_FLOW,CSmartValue(TCPRecvFlow));
	SetServerStatus(SC_SST_SS_TCP_SEND_FLOW,CSmartValue(TCPSendFlow));
	SetServerStatus(SC_SST_SS_UDP_RECV_FLOW,CSmartValue(UDPRecvFlow));
	SetServerStatus(SC_SST_SS_UDP_SEND_FLOW,CSmartValue(UDPSendFlow));
	SetServerStatus(SC_SST_SS_TCP_RECV_COUNT,CSmartValue(TCPRecvCount));
	SetServerStatus(SC_SST_SS_TCP_SEND_COUNT,CSmartValue(TCPSendCount));
	SetServerStatus(SC_SST_SS_UDP_RECV_COUNT,CSmartValue(UDPRecvCount));
	SetServerStatus(SC_SST_SS_UDP_SEND_COUNT,CSmartValue(UDPSendCount));

	CControlPanel::GetInstance()->SetServerStatus(m_ServerStatus.GetData(),m_ServerStatus.GetDataLen());

	m_CycleCount=0;
	ResetFluxStat();

	if(CSystemConfig::GetInstance()->IsLogServerObjectUse())
	{
		PrintObjectStatus();
	}		
	FUNCTION_END;
}