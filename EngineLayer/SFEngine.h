#pragma once
#include "ErrorCode.h"
#include <EngineInterface/IEngine.h>
#include <EngineInterface/INetworkEngine.h>
#include <EngineInterface/IPacketProtocol.h>
#include "SFConfigure.h"
#include "ILogicDispatcher.h"
#include "ILogicEntry.h"

class IRPCInterface;
class SFServerConnectionManager;
class SFPacketProtocolManager;


class SFEngine : public IEngine
{
	friend class SFTCPNetwork;

	friend class SFMsgPackProtocol;
	friend class SFProtobufProtocol;
	friend class SFAvroProtocol;
	friend class SFCGSFPacketProtocol;
	friend class SFJsonProtocol;

public:
	virtual ~SFEngine(void);

	static SFEngine* GetInstance();
	void SetLogFolder(TCHAR* szPath = NULL);

	bool Start(char* szIP = NULL, unsigned short port = 0);
	bool Activate();
	ERROR_CODE Intialize(ILogicEntry* pLogicEntry, IPacketProtocol* pProtocol = NULL, ILogicDispatcher* pDispatcher = NULL);
	bool ShutDown();

	virtual ISessionService* CreateSessionService(_SessionDesc& desc) override;
	
	virtual bool OnConnect(int serial, _SessionDesc& desc) override;
	virtual bool OnDisconnect(int serial, _SessionDesc& desc) override;
	virtual bool OnTimer(const void *arg) override;

	bool AddTimer(int timerID, DWORD period, DWORD delay);
	bool SendRequest(BasePacket* pPacket);
	bool SendRequest(BasePacket* pPacket, std::vector<int>& ownerList);

	bool SendDelayedRequest(BasePacket* pPacket);
	bool SendDelayedRequest(BasePacket* pPacket, std::vector<int>* pOwnerList = NULL);

	bool ReleasePacket(BasePacket* pPacket);

	SFServerConnectionManager* GetServerConnectionManager(){ return m_pServerConnectionManager; }
	bool SetupServerReconnectSys();
	bool LoadConnectorList(WCHAR* szFileName);
	int  AddListener(char* szIP, unsigned short port, int packetProtocolId);
	int  AddConnector(int connectorId, char* szIP, unsigned short port);
	void AddRPCService(IRPCService* pService);
	bool AddPacketProtocol(int packetProtocolId, IPacketProtocol* pProtocol);
	
	INetworkEngine* GetNetworkEngine(){return m_pNetworkEngine;}

	SFConfigure* GetConfig(){return &m_Config;}
	void SetConfig(SFConfigure& Config){m_Config = Config;}

	ILogicDispatcher* GetLogicDispatcher(){return m_pLogicDispatcher;}

	bool IsServer(){ return m_isServer; }

	SFPacketProtocolManager* GetPacketProtocolManager(){ return m_pPacketProtocolManager; }

protected:
	bool CreatePacketSendThread();
	ERROR_CODE CreateEngine(char* szModuleName, bool Server = false);
	
private:
	SFEngine();

	SFConfigure m_Config;
	int m_packetSendThreadId;

	HINSTANCE m_EngineHandle;
	INetworkEngine* m_pNetworkEngine;

	SFPacketProtocolManager* m_pPacketProtocolManager;
	ILogicDispatcher* m_pLogicDispatcher;
	SFServerConnectionManager* m_pServerConnectionManager;

	void SetLogicDispathcer(ILogicDispatcher* pDispatcher){m_pLogicDispatcher = pDispatcher;}

	static SFEngine* m_pEngine;

	bool m_isServer;
};