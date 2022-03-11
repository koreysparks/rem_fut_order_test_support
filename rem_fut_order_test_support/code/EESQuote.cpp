#include "EESQuote.h"
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

LongoParam::LongoParam()
	:TradePort(0)
	,QueryPort(0)
	,QuotePort(0)
	,OrderCount(0)
	,LocalUDPPort(0)
	,IsPrecloseOrder(true)
	,IsOrder(false)
{}

Quote::Quote()
{}

Quote::Quote(LongoParam* p, CLog* log)
	:m_eesApi(NULL)
	,m_handle(NULL)
	,m_logonParam(*p)
	,m_log(log)
{}


Quote::~Quote()
{}

void Quote::init(LongoParam* p, CLog* log)
{
	m_logonParam = *p;
	m_log = log;
}


void Quote::Run()
{
	bool ret = Init();
	if (!ret)
	{	
		return;
	}

	Pause();
}

bool Quote::Init()
{
	bool ret = LoadEESQuote();
	if (!ret)
	{
		return false;
	}

	EqsTcpInfo			info;
	vector<EqsTcpInfo>	vec_info;

	memset(&info, 0, sizeof(EqsTcpInfo));
	strcpy(info.m_eqsIp, m_logonParam.QuoteIP.c_str());
	info.m_eqsPort = m_logonParam.QuotePort;
	vec_info.push_back(info);

	ret = m_eesApi->ConnServer(vec_info, this);
	if (!ret)
	{
		m_log->log("Connect Quote Server Failed!");
		return false;
	}
	else
	{
		m_log->log("Connect Quote Server OK!");
	}

	return true;
}

void Quote::Close()
{
	if (m_eesApi)
	{
		m_eesApi->DisConnServer();
	}

	UnloadEESQuote();
}


void Quote::Pause()
{
	while(1)
	{}
}


void Quote::OnEqsConnected()
{
	Logon();
}

void Quote::OnEqsDisconnected()
{
	m_log->log("Eqs Disconnected.");
}

void Quote::OnLoginResponse(bool bSuccess, const char* pReason)
{
	if (!bSuccess)
	{
		m_log->log("Login Quote Server Failed: %s!",pReason);
		return;
	}

	m_log->log("Login Quote Server OK!");

	QueryAllSymbol();
}

void Quote::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData)
{
	updateQuote(pDepthQuoteData);
}


void Quote::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
	if (bSuccess)
	{
		m_log->log("Symbol(%s) Registered.", pSymbol);
	}
	else
	{
		m_log->log("Symbol(%s) Register Failed.", pSymbol);
	}
	
}

void Quote::OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
	if (bSuccess)
	{
		m_log->log("Symbol(%s) Unregistered.", pSymbol);
	}
	else
	{
		m_log->log("Symbol(%s) Unregister Failed.", pSymbol);
	}
}

void Quote::OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast)
{
	string strSymbol = pSymbol;
	if (!strSymbol.empty())
	{
		RegisterSymbol(chInstrumentType, pSymbol);
	}

	if (bLast)
	{
		m_log->log("Symbo List finished");
	}
}

bool Quote::LoadEESQuote()
{
	m_eesApi = CreateEESQuoteApi();
	return m_eesApi ? true : false;
}

void Quote::UnloadEESQuote()
{
	DestroyEESQuoteApi(m_eesApi);
}


void Quote::Logon()
{
	EqsLoginParam temp;

	strcpy(temp.m_loginId, m_logonParam.LogonID.c_str());
	strcpy(temp.m_password, m_logonParam.Pwd.c_str());

	if (!m_eesApi)
	{
		printf("Invalid Quote Api Instance\n");
		return;
	}

	m_eesApi->LoginToEqs(temp);
}

void Quote::QueryAllSymbol()
{
	if (!m_eesApi)
	{
		printf("Invalid Quote Api Instance\n");
		return;
	}

	m_eesApi->QuerySymbolList();
}

void Quote::RegisterSymbol(EesEqsIntrumentType chInstrumentType, const char* pSymbol)
{
	if (!m_eesApi)
	{
		printf("Invalid Quote Api Instance\n");
		return;
	}

	m_eesApi->RegisterSymbol(chInstrumentType, pSymbol);
}


void Quote::updateQuote(EESMarketDepthQuoteData* pQuote)
{
	/*printf("------------------------------\n");
	printf("Symbol:     %s\n", pQuote->InstrumentID);
	printf("LastPrice:  %.4f\n", pQuote->LastPrice);
	printf("Volume:     %d\n", pQuote->Volume);
	printf("Bid:        %.4f\n", pQuote->BidPrice1);
	printf("Ask:        %.4f\n", pQuote->AskPrice1);
	printf("\n");*/

	m_futureQuote[pQuote->InstrumentID] = *pQuote;
}

map<string, EESMarketDepthQuoteData>& Quote::getQuoteMap()
{
	return m_futureQuote;
}

