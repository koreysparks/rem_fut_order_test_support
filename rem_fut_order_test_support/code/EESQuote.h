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
	
	/// \brief 测试入口函数
	void Run();
	void init(LongoParam* p, CLog* log);
	map<string, EESMarketDepthQuoteData>& getQuoteMap();

private:
	/// \brief 初始化
	bool Init();
	/// \brief 关闭
	void Close();

	/// \brief 暂停
	void Pause();

private:
	/// \brief 加载EES行情API的动态库
	bool LoadEESQuote();
	/// \brief 释放EES行情API的动态库
	void UnloadEESQuote();

	/// \brief Windows版加载行情API的动态库
	bool Windows_LoadEESQuote();
	/// \brief Windows版释放行情API的动态库
	void Windows_UnloadEESQuote();

	/// \brief Linux版本加载行情API的动态库
	bool Linux_LoadEESQuote();
	/// \brief Linux版本释放行情API的动态库
	void Linux_UnloadEESQuote();


	/// \brief 登录
	void Logon();
	/// \brief 查询所有合约
	void QueryAllSymbol();
	/// \brief 注册指定合约的行情
	void RegisterSymbol(EesEqsIntrumentType chInstrumentType, const char* pSymbol);
	/// \brief 显示行情信息
	void updateQuote(EESMarketDepthQuoteData* pQuote);

	
private:
	/// \brief 服务器连接响应
	virtual void OnEqsConnected();
	/// \brief 服务器连接断开响应
	virtual void OnEqsDisconnected();
	/// \brief 登录服务器响应
	virtual void OnLoginResponse(bool bSuccess, const char* pReason);
	/// \brief 接收到行情信息的响应
	virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);
	/// \brief 注册合约的响应
	virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	/// \brief 注销合约的响应
	virtual void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	/// \brief 查询合约列表的响应
	virtual void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast);

private:
	EESQuoteApi*										m_eesApi;				///< EES行情API接口
	T_DLL_HANDLE										m_handle;				///< EES行情API句柄
	funcDestroyEESQuoteApi								m_distoryFun;			///< EES行情API对象销毁函数

	LongoParam											m_logonParam;
	CLog*												m_log;
	map<string, EESMarketDepthQuoteData>				m_futureQuote;
};

