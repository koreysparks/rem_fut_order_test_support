#include "EESQuote.h"
#include "Price.h"
#include "../../base/thread.hpp"


#include <iostream>

#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>

#ifdef _WIN32

	#ifdef _DEBUG
		#pragma comment(lib, "base_jys_d.lib")
	
		#else
		#pragma comment(lib, "base_jys.lib")

#endif

	#pragma comment(lib, "EESQuoteApi.lib")
	#pragma comment(lib, "EESTraderApi.lib")
	
#else

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


#endif


using namespace pp;
using namespace std;


LongoParam			g_param;
Quote				g_quote;
CLog				g_log("rem_fut_order_test_support", true);
vector<Order>		g_order;

bool loadParam()
{
	string paramName = "rem_fut_order_test_support.xml";
	ptree pt;
	read_xml(paramName, pt, boost::property_tree::xml_parser::trim_whitespace);
	BOOST_AUTO(child, pt.get_child("config.param"));//获取根节点的子节点
	BOOST_AUTO(pos, child.begin());
	for (; pos != child.end(); ++pos) 
	{ 
		if( "server_param" == pos->first)
		{
			g_param.TradeIP = pos->second.get<string>("<xmlattr>.trade_ip");
			g_param.TradePort = pos->second.get<int>("<xmlattr>.trade_port");
			g_param.TradeUDPPort = pos->second.get<int>("<xmlattr>.trade_udp_port");
			g_param.QueryIP = pos->second.get<string>("<xmlattr>.query_ip");
			g_param.QueryPort = pos->second.get<int>("<xmlattr>.query_port");
			g_param.QuoteIP = pos->second.get<string>("<xmlattr>.quote_ip");
			g_param.QuotePort = pos->second.get<int>("<xmlattr>.quote_port");
			g_param.QuoteIP = pos->second.get<string>("<xmlattr>.local_ip");
			g_param.QuotePort = pos->second.get<int>("<xmlattr>.local_udp_port");
		}	

		if( "user_info" == pos->first)
		{
			g_param.LogonID = pos->second.get<string>("<xmlattr>.logon_id");
			g_param.Pwd = pos->second.get<string>("<xmlattr>.pwd");
			g_param.AppID = pos->second.get<string>("<xmlattr>.appid");
			g_param.AuthCode = pos->second.get<string>("<xmlattr>.auth_code");
			g_param.Account = pos->second.get<string>("<xmlattr>.account");
		}


		if( "trade_param" == pos->first)
		{
			g_param.IsOrder = pos->second.get<int>("<xmlattr>.is_order") == 1 ? true : false;
			g_param.OrderWaitTime = pos->second.get<int>("<xmlattr>.order_wait_time");
			g_param.OrderCount = pos->second.get<int>("<xmlattr>.order_count");
		}
		

		if( "order_info" == pos->first)
		{
			Order order;
			order.Symbol =pos->second.get<string>("<xmlattr>.symbol");
			order.ExchangeID = pos->second.get<int>("<xmlattr>.exchenge_id");
			order.Price = pos->second.get<double>("<xmlattr>.price");
			order.Side =  pos->second.get<int>("<xmlattr>.side");
			order.PriceType =  pos->second.get<int>("<xmlattr>.price_type");
			order.SecType =  pos->second.get<int>("<xmlattr>.sec_type");
			order.HedgFlage =  pos->second.get<int>("<xmlattr>.hedg_flag");
			order.Tif =  pos->second.get<int>("<xmlattr>.tif");
			g_order.push_back(order);
		}
	}

	return true;
}



#ifdef _WIN32

	DWORD WINAPI tradeFun(void* lParam)
	{
		CCreatePrice createPrice(&g_param, &g_log, &g_quote, g_order);
		createPrice.Run();
	
	
		return NULL;
	}
	
	DWORD WINAPI  quoteFun(void* lParam)
	{
		g_quote.Run();
		return NULL;
	}
	
	DWORD WINAPI  execCollRptr(void* lParam)
	{
		//system("./coll_rptr rem_fut_order_test_support.xml");
		return NULL;
	}

#else

	void* tradeFun(void* lParam)
	{
		CCreatePrice createPrice(&g_param, &g_log, &g_quote, g_order);
		createPrice.Run();
	
	
		return NULL;
	}
	
	void* quoteFun(void* lParam)
	{
		g_quote.Run();
		return NULL;
	}
	
	void* execCollRptr(void* lParam)
	{
		system("./coll_rptr rem_fut_order_test_support.xml");
		return NULL;
	}
#endif


int main()
{
	if(!loadParam())
	{
		return -1;
	}

	CThread tradeThread, quoteThread, collRptrThread;
	
	// 运行采集工具 
	collRptrThread.init(execCollRptr, (void*)NULL);
	_sleep_(3000);

	if(g_param.IsOrder)
	{
		// 收取行情
		g_quote.init(&g_param, &g_log);	
		quoteThread.init(quoteFun, (void*)NULL);
		_sleep_(3000);
	}
	
	// 交易
	tradeThread.init(tradeFun, (void*)NULL);
	

	string quit;
	while(1)
	{
		cin >> quit;
		if(quit == "quit")
		{
			break;
		}

		_sleep_(1000);
	}

	collRptrThread.Cancl();
	tradeThread.Cancl();
	
	return 0;
}









