#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include "Crypto/base64.h"
#include "nlohmann/json.hpp"

class API{
	public:
		std::string key;
		std::string seceret;
		std::string uri;
		std::string nonce;
		std::string command;
};

//cross platform sleep function
void sp(int sec)
{	
	#ifdef _WIN32
		Sleep(sec*1000);
	#else
		sleep(sec);
	#endif

}

//runs command without popup cross platform
void run(const char* command)
{
    #ifdef __linux__
		system(command);
    #elif _WIN32
        WinExec(command,SW_HIDE);
    #else
    #endif
}

std::string sign(API api)
{
	std::string com = ("python3 include/Krak/Crypto/sign.py -S "+api.seceret+" -N "+api.nonce+" -P "+api.uri+" -C \""+api.command+"\"");
	
	//run program to sign
	run(com.c_str());

	sp(1);

	std::ifstream f("sign.txt");
	
	std::stringstream buffer;
	buffer << f.rdbuf();
	std::string sig = buffer.str();
	std::remove("sign.txt");

	return sig;

}

nlohmann::json PostKraken(std::string uri,nlohmann::json comma,std::string key,std::string sec) {
	API a;

  const auto p1 = std::chrono::system_clock::now();

	//build request
	a.seceret = sec;
	a.nonce =  std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count());
	a.uri = uri;
	a.key = key;
	a.command = ("nonce="+a.nonce);

	//build command to sign
	for(int i = 0; i != comma.size(); i++)
	{
		a.command += "&"+comma[i][0].get<std::string>()+"="+comma[i][1].get<std::string>();
	}

	//sign request
	std::string sig = sign(a);

	//build command
	std::string coms = ("curl -s -X \"POST\" \"https://api.kraken.com"+a.uri+"\" -H \"API-Key: "+a.key+"\" -H \"API-Sign: "+sig+"\" -H \"Content-Type: application/x-www-form-urlencoded; charset=utf-8\"  --data-urlencode \"nonce="+a.nonce+"\"");

	//check if a additiononal parmaters are add
	for(int i = 0; i != comma.size(); i++)
	{
		coms += " --data-urlencode \""+comma[i][0].get<std::string>()+"="+comma[i][1].get<std::string>()+"\"";
	}
	
	std::string file = "out.json";

	//send the output of curl ot out.json
	coms += " -o "+file;

	//send command
	run(coms.c_str());

	//load json data
	nlohmann::json j;
	std::ifstream ifs(file);
	j = nlohmann::json::parse(ifs);

	std::remove(file.c_str());

	return j;
} 


using namespace nlohmann;

//blank json
json blank;

//kraken api class
class Krak
{
	private:
		std::string key;
		std::string sec; 
	public:
		//set api key
		void SetKey(std::string k)
		{
			this->key = k;
		}

		//set api secret
		void SetSecret(std::string sec)
		{
			this->sec = sec;
		}
		
		//api to get current balances of account (Requires api key and seceret)
		json GetAccountBalances()
		{
			return PostKraken("/0/private/Balance",blank,this->key,this->sec)["result"];
		}

		//api get account trade balance
		json GetTradeBalance()
		{
			return PostKraken("/0/private/TradeBalance",blank,this->key,this->sec)["result"];
		}

		//api get account open orders
		json GetOpenOrders()
		{
			return PostKraken("/0/private/OpenOrders",blank,this->key,this->sec)["result"];
		}

		//api get account closed orders
		json GetClosedOrders()
		{
			return PostKraken("/0/private/ClosedOrders",blank,this->key,this->sec)["result"];
		}


		//api simple buy order that buys at current market price (amount 0 buys as much as possible) (Requires api key and seceret)
		json SimpleBuy(int amount,std::string symbol)
		{
			json j;
			json pair;

			//set buy parms
			pair[0] = "type";
			pair[1] = "buy";
			j[0] = pair;

			pair[0] = "ordertype";
			pair[1] = "market";
			j[1] = pair;

			pair[0] = "pair";
			pair[1] = symbol;
			j[2] = pair;

			pair[0] = "volume";
			pair[1] = std::to_string(amount);
			j[3] = pair;

			return PostKraken("/0/private/AddOrder",j,this->key,this->sec);
		}

		//api simple sell order that sells at current market price (amount 0 buys as much as possible)(Requires api key and seceret)
		json SimpleSell(int amount,std::string symbol)
		{
			json j;
			json pair;

			//set buy parms
			pair[0] = "type";
			pair[1] = "sell";
			j[0] = pair;

			pair[0] = "ordertype";
			pair[1] = "market";
			j[1] = pair;

			pair[0] = "pair";
			pair[1] = symbol;
			j[2] = pair;

			pair[0] = "volume";
			pair[1] = std::to_string(amount);
			j[3] = pair;

			return PostKraken("/0/private/AddOrder",j,this->key,this->sec);
		}

		//api complex order
		//reference: https://docs.kraken.com/rest/#operation/addOrder
		//negative prices will ignore price set (Requires api key and seceret)
		json ComplexOrder(int amount,std::string symbol,std::string type,std::string ordertype,double price1 = -1,double price2 = -1,bool validate = false)
		{
			json j;
			json pair;

			//set buy parms
			pair[0] = "type";
			pair[1] = type;
			j[0] = pair;

			pair[0] = "ordertype";
			pair[1] = ordertype;
			j[1] = pair;

			pair[0] = "pair";
			pair[1] = symbol;
			j[2] = pair;

			pair[0] = "volume";
			pair[1] = std::to_string(amount);
			j[3] = pair;

			//set price if entered
			if(price1 > 0)
			{
				pair[0] = "price";
				pair[1] = std::to_string(price1);
				j[4] = pair;
			}

			if(price2 > 0)
			{
				pair[0] = "price2";
				pair[1] = std::to_string(price2);
				j[5] = pair;
			}

			if(validate)
			{
				pair[0] = "validate";
				pair[1] = "true";
				j[6] = pair;
			}

			return PostKraken("/0/private/AddOrder",j,this->key,this->sec);
		}

		//api to get current value of coin by each minute
		json GetValue(std::string symbol)
		{
			std::string file = base64_encode((const unsigned char*)symbol.c_str(),symbol.size())+".json";
			std::string command = "curl -s \"https://api.kraken.com/0/public/OHLC?pair="+symbol+"\" -o "+file;

			run(command.c_str());


			//load json data
			nlohmann::json j;
			std::ifstream ifs(file);
			j = nlohmann::json::parse(ifs);

			std::remove(file.c_str());

			return j["result"][symbol];
		}

		//Get specific balance from account (Requires api key and seceret)
		double GetBalance(std::string Currency)
		{
			return std::stod(GetAccountBalances()[Currency].get<std::string>());
		}

};
