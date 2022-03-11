#include "Price.h"

#include <iostream>
using namespace std;

OrderManager::OrderManager()
	:ClientOrderToken(0)
	,MktOrderToken(0)
	,Qty(0)
	,FillQty(0)
	,IsSendCxl(0)
	,IsClose(0)
	,Side(0)
	,Accept(0)

{}

CCreatePrice::CCreatePrice(LongoParam* logonParam, CLog* log, Quote* quote, const vector<Order>& order)
	:m_logonStatus(0)
	,m_log(log)
	,m_logonParam(*logonParam)
	,m_acceptOrderCount(0)
	,m_enterOrderCount(0)
	,m_quote(quote)
	,m_orderList(order)
{}


CCreatePrice::~CCreatePrice()
{
}

void CCreatePrice::Run()
{
	bool ret = Init();
	if (!ret)
	{
		_sleep_(2000);
		Close();
		return;
	}

	// 不报单或未登录成功
	if(!m_logonParam.IsOrder || m_logonStatus != 2)
	{
		return;
	}

	querySymbol();
	/*rewind();

	if(m_logonParam.IsPrecloseOrder)
	{
		perCloseOrder();
	}*/

	_sleep_(1000);

	while(1)
	{
		for(map<int, OrderManager>::const_iterator iter = m_order.begin(); iter!= m_order.end(); ++iter)
		{
			if(0 == iter->second.IsClose)
			{
				cxlOrder(iter->second.MktOrderToken);
			}
		}

		queryPos();
		closePos();

		if(m_orderList.empty())
		{
			openPos();
		}
		else
		{
			openPosByConfigInfo();
		}
		
		_sleep_(10);
		if(m_enterOrderCount >= m_logonParam.OrderCount)
		{
			return;
		}
	}
	

}

void CCreatePrice::perCloseOrder()
{
	EES_ClientToken order_token = 0;
	EES_EnterOrderField temp;
	memset(&temp, 0, sizeof(EES_EnterOrderField));
	temp.m_Tif = EES_OrderTif_Day;
	temp.m_HedgeFlag = EES_HedgeFlag_Speculation;
	strcpy(temp.m_Account, m_logonParam.Account.c_str());
	temp.m_SecType = EES_SecType_fut;
	RESULT ret = 0;

	
	
}

void CCreatePrice::orderManager(EES_EnterOrderField* order)
{
	OrderManager o;
	o.ClientOrderToken = order->m_ClientOrderToken;
	o.Price = order->m_Price;
	o.Qty = order->m_Qty;
	o.Symbol = order->m_Symbol;
	o.Side = order->m_Side;

	m_order[order->m_ClientOrderToken] = o;
	++m_enterOrderCount;
};

void CCreatePrice::openPosByConfigInfo()
{
	EES_ClientToken order_token = 0;
	EES_EnterOrderField temp;
	memset(&temp, 0, sizeof(EES_EnterOrderField));
	temp.m_Tif = EES_OrderTif_Day;
	temp.m_HedgeFlag = EES_HedgeFlag_Speculation;
	strcpy(temp.m_Account, m_logonParam.Account.c_str());
	RESULT ret = 0;

	for(size_t i = 0; i < m_orderList.size(); ++i)
	{
		strcpy(temp.m_Symbol, m_orderList[i].Symbol.c_str());
		temp.m_HedgeFlag = m_orderList[i].HedgFlage;
		temp.m_SecType = m_orderList[i].SecType;
		temp.m_Tif =  m_orderList[i].Tif;
		temp.m_Price = m_orderList[i].Price;
		temp.m_Side = m_orderList[i].Side;
		temp.m_Exchange = m_orderList[i].ExchangeID;
		temp.m_Qty = rand() % 5 + 1;
		m_tradeApi->GetMaxToken(&order_token);
		temp.m_ClientOrderToken = order_token + 1;

		ret = m_tradeApi->EnterOrder(&temp);
		if(ret != NO_ERROR)
		{
			m_log->log("order failed! symbol:%s reason:%d", temp.m_Symbol, ret );
		}
		else
		{
			orderManager(&temp);
		}

		_sleep_(m_logonParam.OrderWaitTime);

	}

	_sleep_(10);

}

void CCreatePrice::openPos()
{
	EES_ClientToken order_token = 0;
	EES_EnterOrderField temp;
	memset(&temp, 0, sizeof(EES_EnterOrderField));
	temp.m_Tif = EES_OrderTif_Day;
	temp.m_HedgeFlag = EES_HedgeFlag_Speculation;
	strcpy(temp.m_Account, m_logonParam.Account.c_str());
	temp.m_SecType = EES_SecType_fut;
	RESULT ret = 0;

	for(map<string, EES_SymbolField>::const_iterator iter = m_symbol.begin(); iter != m_symbol.end(); ++iter)
	{
		map<string, EESMarketDepthQuoteData>::const_iterator iterQuote = m_quote->getQuoteMap().find(iter->first);
		if(iterQuote != m_quote->getQuoteMap().end())
		{
			// 取到合约价格信息
			strcpy(temp.m_Symbol, iter->first.c_str());
			if(0 != iterQuote->second.LastPrice)
			{
				temp.m_Price = iterQuote->second.LastPrice;
			}
			else if(0 != iterQuote->second.AskPrice1)
			{
				temp.m_Price = iterQuote->second.AskPrice1;
			}
			else if(0 != iterQuote->second.BidPrice1)
			{
				temp.m_Price = iterQuote->second.BidPrice1;
			}
			else
			{
				continue;
			}

			temp.m_Side = EES_SideType_open_long;
			temp.m_Exchange =  iter->second.m_ExchangeID;
			temp.m_Qty = rand() % 5 + 1;
			m_tradeApi->GetMaxToken(&order_token);
			temp.m_ClientOrderToken = order_token + 1;

			ret = m_tradeApi->EnterOrder(&temp);
			if(ret != NO_ERROR)
			{
				m_log->log("order failed! symbol:%s reason:%d", iter->first.c_str(), ret );
			}
			else
			{
				orderManager(&temp);
			}

			_sleep_(m_logonParam.OrderWaitTime);

			temp.m_Side = EES_SideType_open_short;
			temp.m_Qty = rand() % 5 + 1;
			m_tradeApi->GetMaxToken(&order_token);
			temp.m_ClientOrderToken = order_token + 1;

			ret = m_tradeApi->EnterOrder(&temp);
			if(ret != NO_ERROR)
			{
				m_log->log("order failed! symbol:%s reason:%d", iter->first.c_str(), ret );
			}
			else
			{
				orderManager(&temp);
			}

			_sleep_(m_logonParam.OrderWaitTime);
		}
		else
		{
			m_log->log("get price failed! symbol: %s", iter->first.c_str());
		}
	}
}

void CCreatePrice::closePos()
{
	EES_ClientToken order_token = 0;
	EES_EnterOrderField temp;
	memset(&temp, 0, sizeof(EES_EnterOrderField));
	temp.m_Tif = EES_OrderTif_Day;
	temp.m_HedgeFlag = EES_HedgeFlag_Speculation;
	strcpy(temp.m_Account, m_logonParam.Account.c_str());
	temp.m_SecType = EES_SecType_fut;
	RESULT ret = 0;

	for(size_t i = 0; i < m_postion.size(); ++i)
	{
		string symbol = m_postion[i].m_Symbol;
		map<string, EESMarketDepthQuoteData>::const_iterator iter = m_quote->getQuoteMap().find(symbol);
		if(iter == m_quote->getQuoteMap().end())
		{
			m_log->log("cant find price when close postion, symbol:%s", symbol.c_str());
			continue;
		}

		if(0 == iter->second.LastPrice)
		{
			if(0 == iter->second.BidPrice1)
			{
				temp.m_Price = iter->second.PreClosePrice;
			}
			else
			{
				temp.m_Price = iter->second.BidPrice1;
			}
		}
		else
		{
			temp.m_Price = iter->second.LastPrice;
		}

		strcpy(temp.m_Symbol, symbol.c_str());
		temp.m_Exchange = m_symbol[symbol].m_ExchangeID;
		temp.m_Qty = m_postion[i].m_TodayQty + m_postion[i].m_OvnQty - m_postion[i].m_FrozenOvnQty - m_postion[i].m_FrozenTodayQty;
		
		// 多仓
		if(m_postion[i].m_PosiDirection == EES_PosiDirection_long)
		{
			if(EES_ExchangeID_cffex == temp.m_Exchange)
			{
				temp.m_Side = EES_SideType_close_long;
			}
			else
			{
				// 昨仓
				if(m_postion[i].m_OvnQty - m_postion[i].m_FrozenOvnQty >= 0)
				{
					temp.m_Side = EES_SideType_close_ovn_long;
					temp.m_Qty = m_postion[i].m_OvnQty - m_postion[i].m_FrozenOvnQty;
				}
				else
				{
					// 今仓
					temp.m_Side = EES_SideType_close_today_long;
					temp.m_Qty = m_postion[i].m_TodayQty - m_postion[i].m_FrozenTodayQty;
				}
			}
		}
		else
		{
			// 空仓
			if(EES_ExchangeID_cffex == temp.m_Exchange)
			{
				temp.m_Side = EES_SideType_close_short;
			}
			else
			{
				// 昨仓
				if(m_postion[i].m_OvnQty - m_postion[i].m_FrozenOvnQty > 0)
				{
					temp.m_Side = EES_SideType_close_ovn_short;
					temp.m_Qty = m_postion[i].m_OvnQty - m_postion[i].m_FrozenOvnQty;
				}
				else
				{
					// 今仓
					temp.m_Side = EES_SideType_close_today_short;
					temp.m_Qty = m_postion[i].m_TodayQty - m_postion[i].m_FrozenTodayQty;
				}
			}
		}

		if(0 == temp.m_Qty || 0 == temp.m_Price)
		{
			continue;
		}

		m_tradeApi->GetMaxToken(&order_token);
		temp.m_ClientOrderToken = order_token + 1;

		ret = m_tradeApi->EnterOrder(&temp);
		if(ret != NO_ERROR)
		{
			m_log->log("order failed! symbol:%s reason:%d", symbol.c_str(), ret );
		}
		else
		{
			orderManager(&temp);
		}

		_sleep_(m_logonParam.OrderWaitTime);
	}
}

bool CCreatePrice::Init()
{

	bool ret = LoadEESTrader();
	if (!ret)
	{
		return false;
	}
	EES_TradeSvrInfo svrInfo;
	strcpy(svrInfo.m_remoteTradeIp, m_logonParam.TradeIP.c_str());
	svrInfo.m_remoteTradeTCPPort = m_logonParam.TradePort;
	svrInfo.m_remoteTradeUDPPort = m_logonParam.TradeUDPPort;
	strcpy(svrInfo.m_remoteQueryIp, m_logonParam.QueryIP.c_str());
	svrInfo.m_remoteQueryTCPPort = m_logonParam.QueryPort;
	strcpy(svrInfo.m_LocalTradeIp, m_logonParam.LocalIP.c_str());
	svrInfo.m_LocalTradeUDPPort = m_logonParam.LocalUDPPort;
	RESULT ret_err = m_tradeApi->ConnServer(svrInfo, this);
	if (ret_err != NO_ERROR)
	{
		m_log->log("connect to rem server failed!");
		return false;
	}

	m_log->log("info  init connect trade %s:%d udp port:%d local udp port:%s:%d query %s:%d", 
		m_logonParam.TradeIP.c_str(), m_logonParam.TradePort, m_logonParam.TradeUDPPort, 
		m_logonParam.LocalIP.c_str(), m_logonParam.LocalUDPPort,
		m_logonParam.QueryIP.c_str(), m_logonParam.QueryPort);

	int waitTime = 0;//等待超时
	while (m_logonStatus != 2 && m_logonStatus != 3)
	{
		_sleep_(100);
		waitTime++;
		if (waitTime >= 50)//5秒超时
		{
			m_log->log("wait for logon response timeout!");
			return false;
		}
	}
	
	return (2 == m_logonStatus);
}

void CCreatePrice::Close()
{
	if (m_tradeApi)
	{
		m_tradeApi->DisConnServer();
	}

	UnloadEESTrader();
}



bool CCreatePrice::LoadEESTrader()
{
	m_tradeApi = CreateEESTraderApi();
	return m_tradeApi ? true : false;
}

void CCreatePrice::UnloadEESTrader()
{
	DestroyEESTraderApi(m_tradeApi);
}



void CCreatePrice::Logon()
{
	if (!m_tradeApi)
	{
		m_log->log("invalid api object");
		return;
	}

	m_tradeApi->UserLogon(m_logonParam.LogonID.c_str(), m_logonParam.Pwd.c_str(), m_logonParam.AppID.c_str(), m_logonParam.AuthCode.c_str());
}

void CCreatePrice::cxlOrder(int mktOrderToken)
{
	EES_CancelOrder cxl;
	memset(&cxl, 0, sizeof(EES_CancelOrder));
	memcpy(cxl.m_Account, m_logonParam.Account.c_str(), sizeof(cxl.m_Account));
	cxl.m_origOrderUserId = m_userID;
	cxl.m_MarketOrderToken = mktOrderToken;

	int ret = m_tradeApi->CancelOrder(&cxl);
	if(ret != 0)
	{
		m_log->log("error cxl order failed, mkt order token:%d", mktOrderToken);
	}

	_sleep_(m_logonParam.OrderWaitTime);
}

void CCreatePrice::OnConnection(ERR_NO errNo, const char* pErrStr)
{
	if (errNo != NO_ERROR)
	{
		m_log->log("connect to rem server failed(%d), %s!", errNo, pErrStr);
		return;
	}
	m_logonStatus = 1;
	Logon();
}


void CCreatePrice::OnDisConnection(ERR_NO errNo, const char* pErrStr)
{
	m_log->log("disconnect from rem server:%d, %s!", errNo, pErrStr);
	m_logonStatus = 3;
}


void CCreatePrice::OnUserLogon(EES_LogonResponse* pLogon)
{	
	if (pLogon->m_Result != NO_ERROR)
	{
		m_logonStatus = 3;
		m_log->log("logon failed, result=%d", pLogon->m_Result);
		return;
	}
	m_logonStatus = 2;
	m_userID = pLogon->m_UserId;
	m_log->log("logon successfully, trading date:%u, max token:%d", pLogon->m_TradingDate, pLogon->m_MaxToken);
}

void CCreatePrice::OnOrderAccept(EES_OrderAcceptField* pAccept)
{
	if(pAccept->m_UserID == m_userID)
	{
		++m_acceptOrderCount;
		if(m_acceptOrderCount % 100 == 0)
		{
			m_log->log("on order accept, count:%d", m_acceptOrderCount);
		}

		m_order[pAccept->m_ClientOrderToken].Accept = 1;
		m_order[pAccept->m_ClientOrderToken].MktOrderToken = pAccept->m_MarketOrderToken;
	}
}

void CCreatePrice::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
	if(pAccept->m_UserID == m_userID)
	{
		m_order[pAccept->m_ClientOrderToken].Accept = 2;
	}	
}

void CCreatePrice::OnOrderReject(EES_OrderRejectField* pReject)
{
	if(pReject->m_Userid == m_userID)
	{
		m_order[pReject->m_ClientOrderToken].Accept = -1;
		m_order[pReject->m_ClientOrderToken].IsClose = -1;
		m_log->log("on order rej, client order token:%d reason code:%d grammar result:%d risk result:%s grammer text:%s risk text:%s", 
			pReject->m_ClientOrderToken, pReject->m_ReasonCode, pReject->m_GrammerResult, pReject->m_RiskResult, pReject->m_GrammerText, pReject->m_RiskText);
	}	
}

void CCreatePrice::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
	if(pReject->m_UserID == m_userID)
	{
		m_order[pReject->m_ClientOrderToken].Accept = -2;
		m_order[pReject->m_ClientOrderToken].IsClose = -2;
		m_log->log("on order mkt rej, client order token:%d reason:%s", pReject->m_ClientOrderToken, pReject->m_ReasonText);
	}	
}

void CCreatePrice::OnOrderExecution(EES_OrderExecutionField* pExec)
{
	if(pExec->m_Userid == m_userID)
	{
		m_order[pExec->m_ClientOrderToken].FillQty += pExec->m_Quantity;
		if(m_order[pExec->m_ClientOrderToken].FillQty == m_order[pExec->m_ClientOrderToken].Qty)
		{
			m_order[pExec->m_ClientOrderToken].IsClose = 1;
		}
	}	
}

void CCreatePrice::OnOrderCxled(EES_OrderCxled* pCxled)
{
	if(pCxled->m_Userid == m_userID)
	{
		m_order[pCxled->m_ClientOrderToken].IsClose = 2;
	}
}

void CCreatePrice::OnCxlOrderReject(EES_CxlOrderRej* pReject)
{	
	if(pReject->m_UserID == m_userID)
	{
		m_log->log("on cxl order failed! client order token:%d reason code:%d reason:%s", pReject->m_ClientOrderToken, pReject->m_ReasonCode, pReject->m_ReasonText);
	}
}

void CCreatePrice::OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish)
{
	if(bFinish)
	{
		m_log->log("on query symbol finish! symbol count:%d", m_symbol.size());
	}
	else
	{
			m_symbol[pSymbol->m_symbol] = *pSymbol;
			m_queryFinish = true;
	}
}


void CCreatePrice::OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish)
{
	if(bFinish)
	{
		m_log->log("on query postion finish! postion count:%d", m_postion.size());
		m_queryFinish = true;
	}
	else
	{
		m_postion.push_back(*pAccoutnPosition);
	}
}

void CCreatePrice::OnQueryAccountOptionPosition(const char* pAccount, EES_AccountOptionPosition* pAccoutnOptionPosition, int nReqId, bool bFinish)
{
	if(bFinish)
	{
		
		m_queryFinish = true;
	}
	else
	{
		
	}
}

void CCreatePrice::queryPos()
{
	m_postion.clear();
	int ret = m_tradeApi->QueryAccountPosition(m_logonParam.Account.c_str(), 0);
	if(0 != ret)
	{
		m_log->log("query postion failed!, ret: %d", ret);
	}

	waitForQueryResp();
}

void CCreatePrice::querySymbol()
{
	m_queryFinish = false;
	int ret = m_tradeApi->QuerySymbolList();
	if(0 != ret)
	{
		m_log->log("query symbol failed!, ret: %d", ret);
	}

	waitForQueryResp();
}

void CCreatePrice::waitForQueryResp()
{
	for(size_t i = 0; i < 1000; ++i)
	{
		_sleep_(100);
		if(m_queryFinish)
		{
			break;
		}
	}
}

void CCreatePrice::loadOrder()
{/*
	CLog order("order"+ CLog::getDateTimeString(), false, false);

	for(size_t i = 0; i < m_order.size(); ++i)
	{
		order.log("clt token:%d symbol:%s qty:%d fillqty:%d price:%f side:%d mkt token:%d accept:%d close:%d sendcxl:%d ",
			m_order[i].ClientOrderToken,
			m_order[i].Symbol,
			m_order[i].Qty,
			m_order[i].FillQty,
			m_order[i].Price,
			m_order[i].Side,
			m_order[i].MktOrderToken,
			m_order[i].Accept,
			m_order[i].IsClose,
			m_order[i].IsSendCxl);
	}*/
}


void CCreatePrice::rewind()
{
	int ret = m_tradeApi->QueryAccountOrder(m_logonParam.Account.c_str());
	if(0 != ret)
	{
		m_log->log("query symbol failed!, ret: %d", ret);
	}

	waitForQueryResp();
}

void CCreatePrice::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish)
{
	if(bFinish)
	{
		m_log->log("query order finish order count:%d", m_orderRewind.size());
		m_queryFinish = true;
	}
	else
	{
		m_orderRewind.push_back(*pQueryOrder);
	}
} 
