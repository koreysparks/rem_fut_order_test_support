#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <map>
#include <string>

#include "EESQuoteApi.h"
#include "../../base/log.hpp"

#ifndef _WIN32
#include <dlfcn.h>
#endif

using namespace std;
using namespace pp;

typedef void*		T_DLL_HANDLE;

struct Order 
{
	string		Symbol;
	int			ExchangeID;
	double		Price;
	int			Side;
	int			PriceType;	
	int			Tif;
	int			HedgFlage;
	int			SecType;

};

struct LongoParam
{
	LongoParam();

	string		TradeIP;
	int			TradePort;
	int			TradeUDPPort;
	string		QueryIP;
	int			QueryPort;
	string		QuoteIP;
	int			QuotePort;

	string		LogonID;
	string		Pwd;
	string		AppID;
	string		AuthCode;
	string		LocalIP;
	int			LocalUDPPort;
	
	string		Account;
	int			OrderWaitTime;
	bool		IsPrecloseOrder;
	int			OrderCount;
	bool		IsOrder;
};



class Quote : public EESQuoteEvent
{
public:
	Quote();
	Quote(LongoParam* p, CLog* log);
	virtual ~Quote();
	
	/// \brief ������ں���
	void Run();
	void init(LongoParam* p, CLog* log);
	map<string, EESMarketDepthQuoteData>& getQuoteMap();

private:
	/// \brief ��ʼ��
	bool Init();
	/// \brief �ر�
	void Close();

	/// \brief ��ͣ
	void Pause();

private:
	/// \brief ����EES����API�Ķ�̬��
	bool LoadEESQuote();
	/// \brief �ͷ�EES����API�Ķ�̬��
	void UnloadEESQuote();

	/// \brief Windows���������API�Ķ�̬��
	bool Windows_LoadEESQuote();
	/// \brief Windows���ͷ�����API�Ķ�̬��
	void Windows_UnloadEESQuote();

	/// \brief Linux�汾��������API�Ķ�̬��
	bool Linux_LoadEESQuote();
	/// \brief Linux�汾�ͷ�����API�Ķ�̬��
	void Linux_UnloadEESQuote();


	/// \brief ��¼
	void Logon();
	/// \brief ��ѯ���к�Լ
	void QueryAllSymbol();
	/// \brief ע��ָ����Լ������
	void RegisterSymbol(EesEqsIntrumentType chInstrumentType, const char* pSymbol);
	/// \brief ��ʾ������Ϣ
	void updateQuote(EESMarketDepthQuoteData* pQuote);

	
private:
	/// \brief ������������Ӧ
	virtual void OnEqsConnected();
	/// \brief ���������ӶϿ���Ӧ
	virtual void OnEqsDisconnected();
	/// \brief ��¼��������Ӧ
	virtual void OnLoginResponse(bool bSuccess, const char* pReason);
	/// \brief ���յ�������Ϣ����Ӧ
	virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);
	/// \brief ע���Լ����Ӧ
	virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	/// \brief ע����Լ����Ӧ
	virtual void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	/// \brief ��ѯ��Լ�б����Ӧ
	virtual void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast);

private:
	EESQuoteApi*										m_eesApi;				///< EES����API�ӿ�
	T_DLL_HANDLE										m_handle;				///< EES����API���
	funcDestroyEESQuoteApi								m_distoryFun;			///< EES����API�������ٺ���

	LongoParam											m_logonParam;
	CLog*												m_log;
	map<string, EESMarketDepthQuoteData>				m_futureQuote;
};

