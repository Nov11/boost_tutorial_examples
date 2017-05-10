#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

struct TradingRecord {
	std::string InstrumentID;
	std::string TradingDay;
	std::string UpdateTime;
	long UpdateMillisec;
	double LastPrice;
	double BidPrice1;
	long  BidVolume1;
	double AskPrice1;
	long AskVolume1;
	long Volume;
	double OpenInterest;
	double PreOpenInterest;
	double PreSettlementPrice;
	double PreClosePrice;
	double OpenPrice;
	double HighestPrice;
	double LowestPrice;
	double ClosePrice;
	double SettlementPrice;
	double UpperLimitPrice;
	double LowerLimitPrice;
	double Turnover;
	long TID;
	long ms;
	static const int memberCount = 24;
};

void fillMembers(std::string s, TradingRecord& tr)
{
	std::replace(s.begin(), s.end(), ',', ' ');
	std::stringstream ss(s);
	std::string tmp;
	std::vector<std::string> items;
	while (ss >> tmp)
	{
		items.push_back(tmp);
	}
	if (items.size() != TradingRecord::memberCount) {
		std::cerr << "invalid input: input field count != demanding struct's member count" << std::endl;
		return;
	}
	int i = 0;
	//std::string InstrumentID;
	tr.InstrumentID = items[i++];
	//std::string TradingDay;
	tr.TradingDay = items[i++];
	//std::string UpdateTime;
	tr.UpdateTime = items[i++];
	//long UpdateMillisec;
	tr.UpdateMillisec = std::stol(items[i++]);
	//double LastPrice;
	tr.LastPrice = std::stod(items[i++]);
	//double BidPrice1;
	tr.BidPrice1 = std::stod(items[i++]);
	//long  BidVolume1;
	tr.BidVolume1 = std::stol(items[i++]);
	//double AskPrice1;
	tr.AskPrice1 = std::stod(items[i++]);
	//long AskVolume1;
	tr.AskVolume1 = std::stol(items[i++]);
	//long Volume;
	tr.Volume = std::stol(items[i++]);
	//double OpenInterest;
	tr.OpenInterest = std::stod(items[i++]);
	//double PreOpenInterest;
	tr.PreOpenInterest = std::stod(items[i++]);
	//double PreSettlementPrice;
	tr.PreSettlementPrice = std::stod(items[i++]);
	//double PreClosePrice;
	tr.PreClosePrice = std::stod(items[i++]);
	//double OpenPrice;
	tr.OpenPrice = std::stod(items[i++]);
	//double HighestPrice;
	tr.HighestPrice = std::stod(items[i++]);
	//double LowestPrice;
	tr.LowestPrice = std::stod(items[i++]);
	//double ClosePrice;
	tr.ClosePrice = std::stod(items[i++]);
	//double SettlementPrice;
	tr.SettlementPrice = std::stod(items[i++]);
	//double UpperLimitPrice;
	tr.UpperLimitPrice = std::stod(items[i++]);
	//double LowerLimitPrice;
	tr.LowerLimitPrice = std::stod(items[i++]);
	//double Turnover;
	tr.Turnover = std::stod(items[i++]);
	//double TID;
	tr.TID = std::stol(items[i++]);
	//double ms;
	tr.ms = std::stol(items[i++]);
}

std::string loadCSVFile(std::string path, std::vector<TradingRecord>& result, bool hasHeader = true)
{
	std::ifstream ifs(path);
	if (!ifs) { std::cerr << "error : cannot open file: " << path << std::endl; return "Error : can't open file"; }
	std::string header;
	if (hasHeader)
	{
		getline(ifs, header);
	}

	std::string line;
	while (getline(ifs, line))
	{
		TradingRecord tr;
		fillMembers(line, tr);
		result.push_back(tr);
	}
	return header;
}
template< class T>
inline void fill(std::stringstream& ss, T t, bool appendComma = true)
{
	ss << t;
	if (appendComma) { ss << ","; }
}
std::string makeStringFromTradingRecord(TradingRecord& r)
{
	std::stringstream s;
	s << std::fixed << std::setprecision(6);
	s << r.InstrumentID << ",";
	s << r.TradingDay << ",";
	s << r.UpdateTime << ",";
	fill(s, r.UpdateMillisec);
	fill(s, r.LastPrice);
	fill(s, r.BidPrice1);
	fill(s, r.BidVolume1);
	fill(s, r.AskPrice1);
	fill(s, r.AskVolume1);
	fill(s, r.Volume);
	fill(s, r.OpenInterest);
	fill(s, r.PreOpenInterest);
	fill(s, r.PreSettlementPrice);
	fill(s, r.PreClosePrice);
	fill(s, r.OpenPrice);
	fill(s, r.HighestPrice);
	fill(s, r.LowestPrice);
	fill(s, r.ClosePrice);
	fill(s, r.SettlementPrice);
	fill(s, r.UpperLimitPrice);
	fill(s, r.LowerLimitPrice);
	fill(s, r.Turnover);
	fill(s, r.TID);
	fill(s, r.ms, false);

	return s.str();
}
void saveCSVFile(std::string path, std::vector<TradingRecord>& records, const std::string& head)
{
	std::ofstream ofs(path);
	if (!ofs) { std::cerr << "error : cannot open file (path :" << path << " ) to write" << std::endl; return; }
	ofs << head << std::endl;
	for (auto & item : records)
	{
		ofs << makeStringFromTradingRecord(item) << std::endl;
	}
}
int main()
{
	//std::ifstream ifs("./fff1234");
	//if (ifs) { std::cout << "ifs created" << std::endl; 		std::string s;
	//	ifs>>s;
	//	std::cout <<s << std::endl;}
	//else {
	//	std::cout << "ifs failed" << std::endl;

	//}
	//std::ofstream ofs("fff1234");
	//ofs << "ouput";
	//return 0;
	std::string path = "./IF1506.txt";
	std::vector<TradingRecord> result;
	std::string header = loadCSVFile(path, result);
	for (auto& item : result)
	{
		std::cout << item.UpdateTime << " : " << item.UpdateMillisec << std::endl;
	}
	saveCSVFile("./wIF1506.txt", result, header);

	std::stringstream ss("123456");
	ss.unget();
	std::cout << ss.str() << std::endl;
}