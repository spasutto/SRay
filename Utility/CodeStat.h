#pragma once
/*
#define foo 4
#define MACRO_STR(s) #s // ex : MACRO_STR(foo) ==> "foo" 
#define MACRO_XSTR(s) MACRO_STR(s) // ex : MACRO_XSTR(foo) ==> "4"
#define my_macro(x) x##_macrofied // ex : MACRO_XSTR(my_macro(foo)) ==> "foo_macrofied"
#define callfunc(x) func##x // ex : callfunc(test)(); ==> functest();
*/

#ifndef DEBUG_CODESTAT

#define CS_FSTART(funcname) ;
#define CS_FEND() ;

#else

#define CS_FSTART(funcname) auto cs_func_start = steady_clock::now();\
std::string cs_func_name = #funcname
#define CS_FEND() auto cs_func_end = steady_clock::now();\
CodeStat::codestat.AddMethodCall(cs_func_name, duration_cast<microseconds>(cs_func_end - cs_func_start))

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <execution>
#include <functional>

using namespace std;
using namespace chrono;

class CodeStat
{
	struct MethodCall
	{
		int hashcode;
		microseconds executiontime;
	};

protected:
	bool reportdone = false;
	std::map<int, string> methods;
	std::vector<MethodCall> calls;

	int HashCode(const string &str)
	{
		int h = 0;
		for (size_t i = 0; i < str.size(); ++i)
			h = h * 31 + static_cast<int>(str[i]);
		return h;
	}
	string us_to_str(microseconds us)
	{
		bool st = false;
		char buff[100];
		string retval = "";
		typedef duration<int, ratio<86400>> days;
		auto d = duration_cast<days>(us);
		us -= d;
		auto h = duration_cast<hours>(us);
		us -= h;
		auto m = duration_cast<minutes>(us);
		us -= m;
		auto s = duration_cast<seconds>(us);
		us -= s;
		auto ms = duration_cast<milliseconds>(us);
		us -= ms;


		if (d > days::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%dd", d.count());
			retval += buff;
		}
		if (st || h > hours::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%dh", h.count());
			retval += buff;
		}
		if (st || m > minutes::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%dm", m.count());
			retval += buff;
		}
		if (st || s > seconds::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%llds", s.count());
			retval += buff;
		}
		if (st || ms > milliseconds::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%lldms", ms.count());
			retval += buff;
		}
		if (st || us > microseconds::zero())
		{
			st = true;
			snprintf(buff, sizeof(buff), "%lldus", us.count());
			retval += buff;
		}
		return retval;

	}

public:
	CodeStat()
	{
	}
	~CodeStat()
	{
		if (!reportdone)
			Report();
	}

	void AddMethodCall(string methodname, microseconds time)
	{
		int hashcode = HashCode(methodname);
		if (methods.count(hashcode) <= 0)
			methods.insert({hashcode, methodname});
		reportdone = false;
		calls.push_back({hashcode, time });
	}

	static bool sortByName(const vector<MethodCall> &lhs, const  vector<MethodCall> &rhs) { return lhs.size() < rhs.size(); }

	void Report()
	{
		ofstream cslogfile;
		cslogfile.open("codestat_log.txt");
		microseconds totalsum = accumulate(begin(calls), end(calls), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; });
		cslogfile << calls.size() << " total calls in " << /*totalsum.count() << " µs "*/us_to_str(totalsum) << endl;

		map<int, vector<MethodCall> > calls_by_hashcode;
		for (const auto & call : calls)
			calls_by_hashcode[call.hashcode].push_back(call);

		typedef std::function<bool(std::pair<int, vector<MethodCall>>, std::pair<int, vector<MethodCall>>)> Comparator;

		/*Comparator compByCountFunctor = [](std::pair<int, vector<MethodCall>> elem1, std::pair<int, vector<MethodCall>> elem2)
		{
			cout << "sort fname : " << elem1.second[0].name << "("<< elem1.second.size() <<") / " << elem2.second[0].name << "(" << elem2.second.size() << ") ==> " << ((elem1.second.size() > elem2.second.size())?"true":"false") << endl;
			return elem1.second.size() > elem2.second.size();
		};
		std::set<std::pair<int, vector<MethodCall>>, Comparator> calls_by_hashcode_sorted_by_count(calls_by_hashcode.begin(), calls_by_hashcode.end(), compByCountFunctor);
		cslogfile << endl << "BY METHOD CALL COUNT :" << endl;
		for (std::pair<int, vector<MethodCall>> call : calls_by_hashcode_sorted_by_count)
		{
			microseconds methodsum = accumulate(begin(call.second), end(call.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; });
			cslogfile << " # function " << call.second[0].name.c_str() << "(" << hex << uppercase << call.first << nouppercase << dec << ") : " << call.second.size() << " calls in " << us_to_str(methodsum) << endl;
		}*/

		Comparator compByDurationFunctor = [](std::pair<int, vector<MethodCall>> elem1, std::pair<int, vector<MethodCall>> elem2)
		{
			microseconds methodsum1 = accumulate(begin(elem1.second), end(elem1.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; }) / elem1.second.size();
			microseconds methodsum2 = accumulate(begin(elem2.second), end(elem2.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; }) / elem2.second.size();
			return methodsum1 > methodsum2;
		};
		std::set<std::pair<int, vector<MethodCall>>, Comparator> calls_by_hashcode_sorted_by_duration(calls_by_hashcode.begin(), calls_by_hashcode.end(), compByDurationFunctor);
		cslogfile << endl << "BY METHOD DURATION :" << endl;
		for (std::pair<int, vector<MethodCall>> call : calls_by_hashcode_sorted_by_duration)
		{
			microseconds methodsum = accumulate(begin(call.second), end(call.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; });
			cslogfile << " # function " << methods[call.second[0].hashcode].c_str() << "(" << hex << uppercase << call.first << nouppercase << dec << ") : " << call.second.size() << " calls in " << us_to_str(methodsum) << " (avg=" << us_to_str(methodsum / call.second.size()) << ")" << endl;
		}

		Comparator compByDurationAvgFunctor = [](std::pair<int, vector<MethodCall>> elem1, std::pair<int, vector<MethodCall>> elem2)
		{
			microseconds methodsum1 = accumulate(begin(elem1.second), end(elem1.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; }) / elem1.second.size();
			microseconds methodsum2 = accumulate(begin(elem2.second), end(elem2.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; }) / elem2.second.size();
			return methodsum1 > methodsum2;
		};
		std::set<std::pair<int, vector<MethodCall>>, Comparator> calls_by_hashcode_sorted_by_durationavg(calls_by_hashcode.begin(), calls_by_hashcode.end(), compByDurationAvgFunctor);
		cslogfile << endl << "BY AVERAGE METHOD DURATION :" << endl;
		for (std::pair<int, vector<MethodCall>> call : calls_by_hashcode_sorted_by_durationavg)
		{
			microseconds methodsum = accumulate(begin(call.second), end(call.second), microseconds::zero(),
				[](microseconds i, const MethodCall& o) { return o.executiontime + i; });
			cslogfile << " # function " << methods[call.second[0].hashcode].c_str() << "(" << hex << uppercase << call.first << nouppercase << dec << ") : " << call.second.size() << " calls in " << us_to_str(methodsum) << " (avg=" << us_to_str(methodsum / call.second.size()) << ")" << endl;
		}

		/*
		cslogfile << endl << "DETAILS :" << endl;
		for (auto& call : calls)
		{
			cslogfile << " # function " << methods[call.hashcode].c_str() << " : " << us_to_str(call.executiontime) << endl;
		}
		*/
		cslogfile.close();
		reportdone = true;
	}

	static CodeStat codestat;
};
#endif