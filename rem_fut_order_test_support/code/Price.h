#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <string>


typedef void*		T_DLL_HANDLE;

#include "EesTraderApi.h"
#include "EESQuote.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

using std::string;

struct OrderManager 
{
	OrderManager();

	int			ClientOrderToken;
	int			MktOrderToken;
	string		Symbol;
	double		Price;
	int			Side;
	int			Qty;
	int			FillQty;
	int			IsClose;
	bool		IsSendCxl;
	int			Accept;			// 1 柜台接受 -1 柜台拒绝 2市场接受 -2市场拒绝
};

class CCreatePrice : public EESTraderEvent
{
public:
	CCreatePrice(LongoParam* logonParam, CLog* log, Quote* quote, const vector<Order>& order);
	virtual ~CCreatePrice();

	void Run();

public:
	/// \brief 初始化
	bool Init();
	/// \brief 关闭
	void Close();

private:
	/// \brief 加载EES交易API的动态库
	bool LoadEESTrader();
	/// \brief 释放EES交易API的动态库
	void UnloadEESTrader();

	/// \brief 初始化参数调整方法
	void InputParam();

	/// \brief 登录
	void Logon();
	/// \brief 下单
	void NewOrder();
	/// \brief 撤单
	void CxlOrder();

	void querySymbol();

	void queryPos();

	void waitForQueryResp();

private:
	/// \brief 服务器连接事件
	virtual void OnConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	服务器主动断开，会收到这个消息
	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	登录消息的回调
	virtual void OnUserLogon(EES_LogonResponse* pLogon);
	/// \brief	下单被REM接受的事件
	virtual void OnOrderAccept(EES_OrderAcceptField* pAccept);
	/// \brief	下单被市场接受的事件
	virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	///	\brief	下单被REM拒绝的事件
	virtual void OnOrderReject(EES_OrderRejectField* pReject);
	///	\brief	下单被市场拒绝的事件
	virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
	///	\brief	订单成交的消息事件
	virtual void OnOrderExecution(EES_OrderExecutionField* pExec);
	///	\brief	订单成功撤销事件
	virtual void OnOrderCxled(EES_OrderCxled* pCxled);
	///	\brief	撤单被拒绝的消息事件
	virtual void OnCxlOrderReject(EES_CxlOrderRej* pReject);

	virtual void OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish);
	virtual void OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish);
	virtual void OnQueryAccountOptionPosition(const char* pAccount, EES_AccountOptionPosition* pAccoutnOptionPosition, int nReqId, bool bFinish);
	virtual void OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish);


	void	perCloseOrder();
	void	openPos();
	void	openPosByConfigInfo();
	void	cxlOrder(int mktOrderToken);
	void	closePos();
	void	orderManager(EES_EnterOrderField* order);
	void	loadOrder();
	void	rewind();


private:
	EESTraderApi*						m_tradeApi;				///< EES交易API接口
	T_DLL_HANDLE						m_handle;				///< EES交易API句柄
	funcDestroyEESTraderApi				m_distoryFun;			///< EES交易API动态库销毁函数
	LongoParam							m_logonParam;
	int									m_logonStatus;			///< 0-尚未登录; 1-已连接正在登录; 2-登录成功; 3-登录失败，或者已经断开连接
	int									m_userID;

	Quote*								m_quote;
	CLog*								m_log;
	map<string, EES_SymbolField>		m_symbol;
	vector<EES_AccountPosition>			m_postion;
	map<int, OrderManager>				m_order;
	vector<Order>						m_orderList;
	vector<EES_QueryAccountOrder>		m_orderRewind;
	bool								m_queryFinish;

	int									m_acceptOrderCount;
	int									m_enterOrderCount;
};

