#include "operate.h"
#include <sstream>
#include <math.h>

Operate::Operate(){
	ParaError = false;
	
}


Operate::~Operate(){

}


bool Operate::isOperator(string str){
	if ("+" == str || "-" == str || "*" == str || "/" == str
		|| "%" == str || "(" == str || ")" == str ){
		return true;
	}
	else{
		return false;
	}
}



bool Operate::isSpecialOperator(string str){
	if ("sin" == str || "cos" == str || "sqrt" == str || "ln" == str
		|| "pow" == str){
		return true;
	}
	else{
		return false;
	}
}



double Operate::getValue(double a, double b, string op){
	if ("+" == op)
		return a + b;
	else if ("-" == op)
		return a - b;
	else if ("*" == op)
		return a * b;
	else if ("/" == op)
		return a / b;
	else if ("%" == op)
		return (double)((int)a % (int)b);
	else if ("sin" == op)
		return sin(a);
	else if ("cos" == op)
		return cos(a);
	else if ("ln" == op)
		return log(a);
	else if ("sqrt" == op)
		return sqrt(a);
	else if ("pow" == op)
		return pow(a,b);
	else
		std::cerr << "The operator " << op << " not exist" << endl;
}



void Operate::resultPrint(string cmd, Function f){
	if ("define" == cmd){
		cout << "defined" << " " << "(" << f.name;
		for (int i = 0; i < f.spara.size(); i++)
			cout << " " << f.spara[i];
		cout << ")" << endl;
	}

	if ("test" == cmd){
		string restmp;
		vector<string> res;
		stringstream tmp(f.spara[0]);
		while (tmp >> restmp)
			res.push_back(restmp);

		if ("(" == res[0]){
			cout << f.name << "(";
			for (int i = 2; i < res.size(); i++){
				cout << res[i] << " ";
			}
		}
		else{
			cout << f.name << "(";
			for (int i = 1; i < res.size(); i++){
				cout << res[i] << " ";
			}
		}
		cout << "= " << f.spara[1];

		if (abs(f.expVal - f.actVal) <= 0.0000000000001)
			cout << " [correct]" << endl;
		else
			cout << " [" << "INCORRECT: expected " << f.expVal << "]" << endl;
	}
}


double Operate::getFunctionValue(Function &fun){
	double result;
	stack<string> optorStk;
	stack<double> opndStk;
	string strtmp;
	double a, b, c;

	Function f = fun;

	if (fun.dpara.size() != f.spara.size()){
		std::cerr << "Incorrect number of input parameters with function: " << f.name << "\n";
		ParaError = true;
		return -1;
		//abort();
	}

	for (int i = f.function.size() - 1; i >= 0; i--){

		if (")" == f.function[i]){
			optorStk.push(f.function[i]);
		}
		else if ("(" == f.function[i]){
			optorStk.pop();
		}
		else if (isOperator(f.function[i])){
			strtmp = f.function[i];
			a = opndStk.top();
			opndStk.pop();
			b = opndStk.top();
			opndStk.pop();
			c = getValue(a, b, strtmp);
			opndStk.push(c);
		}
		else if (isSpecialOperator(f.function[i])){			
			strtmp = f.function[i];
			a = opndStk.top();
			opndStk.pop();
			if ("pow" == strtmp){
				b = opndStk.top();
				opndStk.pop();
			}
			else{
				b = 0;
			}
			c = getValue(a, b, strtmp);
			opndStk.push(c);
		}
		else if(isFunction(f.function[i],funset)){
			Function ftmp = getFunction(f.function[i], funset);
			ftmp.dpara.clear();
			for (int j = 0; j < ftmp.spara.size(); j++){
				ftmp.dpara.push_back(opndStk.top());
				opndStk.pop();
			}
			opndStk.push(getFunctionValue(ftmp));
		}
		else{
			bool ispar = false;
			for (int j = 0; j < f.spara.size(); j++){
				if (f.function[i] == f.spara[j]){
					opndStk.push(f.dpara[j]);
					ispar = true;
					break;
				}
			}
			if (!ispar)
				opndStk.push(stringToNum<double>(f.function[i]));
		}
	}

	result = opndStk.top();
	return result;
}


Function Operate::getFunction(string fname, vector<Function> funset){
	Function f;
	f.name = "\0";
	for (int i = 0; i < funset.size(); i++){
		if (fname == funset[i].name){
			f = funset[i];
			break;
		}
	}
	return f;
}


bool Operate::isFunction(string fname, vector<Function> funset){
	for (int i = 0; i < funset.size(); i++){
		if (fname == funset[i].name){
			return true;
		}
	}
	return false;
}


