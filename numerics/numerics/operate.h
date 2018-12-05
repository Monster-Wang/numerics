#ifndef _OPERATE_H_
#define _OPERATE_H_

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;



template<typename Ty>
Ty stringToNum(const string str){
	Ty ret = (Ty) atof(str.c_str());
	return ret;
};

template<typename Ty1>
string NumToString(Ty1 num)
{
	stringstream ss;
	string str;
	ss << num;
	ss >> str;
	return str;
}


/*A struct for function info storager*/
struct Function{
	string name;/*function name*/
	vector<string> spara;/*function parameter name, the storage order is the same as the input order */
	vector<double> dpara;/*function parameter value, the stroageorder is the same as the "spara"*/
	vector<string> function;/*Actual function of function */
	double actVal; /*the input value of the function*/
	double expVal; /*the expect value of the function*/
	double step; /*the step used in integral*/
};


class Operate
{
public:
	Operate();
	~Operate();
	double getFunctionValue(Function &fun);
	bool isOperator(string str);
	double getValue(double a, double b, string op);
	void resultPrint(string cmd, Function f);
	bool isSpecialOperator(string str);
	Function getFunction(string fname, vector<Function> funset);
	bool isFunction(string fname, vector<Function> funset);
	
	/*the flag used to return from function "getFunctionValue(Function &fun)",
	when the input parameters of function are incorrect*/
	bool ParaError;
	//the vector store functions definded
	vector<Function> funset;

};


#endif