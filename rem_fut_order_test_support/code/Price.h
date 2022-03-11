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
	int			Accept;			// 1 ��̨���� -1 ��̨�ܾ� 2�г����� -2�г��ܾ�
};

class CCreatePrice : public EESTraderEvent
{
public:
	CCreatePrice(LongoParam* logonParam, CLog* log, Quote* quote, const vector<Order>& order);
	virtual ~CCreatePrice();

	void Run();

public:
	/// \brief ��ʼ��
	bool Init();
	/// \brief �ر�
	void Close();

private:
	/// \brief ����EES����API�Ķ�̬��
	bool LoadEESTrader();
	/// \brief �ͷ�EES����API�Ķ�̬��
	void UnloadEESTrader();

	/// \brief ��ʼ��������������
	void InputParam();

	/// \brief ��¼
	void Logon();
	/// \brief �µ�
	void NewOrder();
	/// \brief ����
	void CxlOrder();

	void querySymbol();

	void queryPos();

	void waitForQueryResp();

private:
	/// \brief �����������¼�
	virtual void OnConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	�����������Ͽ������յ������Ϣ
	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	��¼��Ϣ�Ļص�
	virtual void OnUserLogon(EES_LogonResponse* pLogon);
	/// \brief	�µ���REM���ܵ��¼�
	virtual void OnOrderAccept(EES_OrderAcceptField* pAccept);
	/// \brief	�µ����г����ܵ��¼�
	virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	///	\brief	�µ���REM�ܾ����¼�
	virtual void OnOrderReject(EES_OrderRejectField* pReject);
	///	\brief	�µ����г��ܾ����¼�
	virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
	///	\brief	�����ɽ�����Ϣ�¼�
	virtual void OnOrderExecution(EES_OrderExecutionField* pExec);
	///	\brief	�����ɹ������¼�
	virtual void OnOrderCxled(EES_OrderCxled* pCxled);
	///	\brief	�������ܾ�����Ϣ�¼�
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
	EESTraderApi*						m_tradeApi;				///< EES����API�ӿ�
	T_DLL_HANDLE						m_handle;				///< EES����API���
	funcDestroyEESTraderApi				m_distoryFun;			///< EES����API��̬�����ٺ���
	LongoParam							m_logonParam;
	int									m_logonStatus;			///< 0-��δ��¼; 1-���������ڵ�¼; 2-��¼�ɹ�; 3-��¼ʧ�ܣ������Ѿ��Ͽ�����
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

