#ifndef _PARSE_H_
#define _PARSE_H_

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "operate.h"
using namespace std;


class Parse
{
public:
	Parse();
	~Parse();
	void CmdParse(string str);
	void FunctionConstruct(string str, int ptr);
	void FunctionTest(string str, int ptr);
	double funParaExtract(string str, int &ptr);
	void NumericalIntegration(string, int ptr);
	double Intergral(Function f, vector<double> li);
	void FunctionMcint(string str, int ptr);

	void GradientAscent(string str, int ptr);
	void GradientDescent(string str, int ptr);
	vector<double> PartialDifferential(Function f, vector<double> pot);
	double StandardDeviation(vector<double> pdif);
	vector<double> movCompute(vector<double> pdif, double gamma);
	


private:
	Operate *opt;
};


#endif



