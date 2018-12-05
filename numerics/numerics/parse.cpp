#include "parse.h"
#include <sstream>
#include <time.h>
#include <random>

Parse::Parse(){
	opt = new Operate;
}


Parse::~Parse(){
	
}


void Parse::CmdParse(string str){

	char strcmd[11];
	int ptr = 0;

	//in front of the first space is command, get the cmd
	while (' ' != str[ptr]){
		strcmd[ptr] = str[ptr];
		ptr++;
		if (ptr >= 10)
			break;
	}
	strcmd[ptr] = '\0';

	if (!strcmp(strcmd, "define")){
		FunctionConstruct(str, ptr);
	}
	else if (!strcmp(strcmd, "test")){
		FunctionTest(str, ptr);
	} 
	else if (!strcmp(strcmd, "numint")){
		NumericalIntegration(str, ptr);
	}
	else if (!strcmp(strcmd, "mcint")){
		FunctionMcint(str, ptr);
	}
	else if (!strcmp(strcmd, "min")){
		GradientDescent(str, ptr);
	}
	else if (!strcmp(strcmd, "max")){
		GradientAscent(str, ptr);
	}
	else {
		std::cerr << "Incorrect command: " << str << "\n";
		return;
	}
}


void Parse::FunctionConstruct(string str, int ptr){
	Function Fun;
	//extracting the function name
	while (++ptr){
		if ((' ' != str[ptr]) && ('(' != str[ptr])){
			int p = ptr;
			while (' ' != str[++ptr]);
			string stmp(str.substr(p, ptr - p));
			Fun.name = stmp;
			break;
		}
	}

	if (opt->isFunction(Fun.name, opt->funset)){
		std::cerr << "The function " << Fun.name << " was defined!\n";
		return;
	}


	//extracting the function parameters
	while ('=' != str[++ptr]){
		if ((' ' != str[ptr]) && ')' != str[ptr]){
			int p = ptr;
			while (' ' != str[++ptr] && ')' != str[ptr]);
			string stmp(str.substr(p, ptr - p));
			Fun.spara.push_back(stmp);
		}
	}

	//extracting the function
	while (++ptr < str.length()){
		string s1(str.substr(ptr, 1));
		if (opt->isOperator(s1)){
			Fun.function.push_back(s1);
		}
		else{
			if (' ' != str[ptr]){
				int p = ptr;
				while (' ' != str[++ptr] && ')' != str[ptr] && '(' != str[ptr]);
				string s2(str.substr(p, ptr - p));
				Fun.function.push_back(s2);
				--ptr;
			}
		}
	}

	opt->funset.push_back(Fun);
	opt->resultPrint("define", Fun);
}


void Parse::FunctionTest(string str, int ptr){

	Function testdata;
	//extracting the function name
	int ptrcnt = ptr;
	while (++ptr){
		if ((' ' != str[ptr]) && ('(' != str[ptr])){
			int p = ptr;
			while (' ' != str[++ptr]);
			string stmp(str.substr(p, ptr - p));
			testdata.name = stmp;
			break;
		}
	}

	/**/
	Function f = opt->getFunction(testdata.name, opt->funset);
	if (f.name == "\0"){
		std::cerr << "The function " << testdata.name << " is not defined!\n";
		return;
	}


	/*test (f (g 6 2) (h 2.5 8)) (f 6 2)*/
	/*根据测使用例的结构，可知第一个函数体为被测函数体，先提取被测函数体字符串（eg: "(f (g 6 2) (h 2.5 8))"），
	因为被测函数存在嵌套，所以可以使用递归法解析函数，递归结束时即可得到最外层函数的所有参数（eg: (f v1 v2 v2)），
	然后调用 "getFunctionValue(Function &fun)" 即可得到函数值*/

	//提取被测函数体字符串，根据左右括号的数量定位被测函数体的范围
	int p;
	ptr = ptrcnt;//字符串指针回到cmd“test”之后，以便获取整个被测函数体
	while (++ptr){
		int lbra = 0, rbra = 0;
		/*解析输入字符串，跳过空格，遇到第一个“(”左括号lbra+1，然后进入循环，
		直到左右括号数量相等则跳出循环*/
		if ('(' == str[ptr]){
			lbra++;
			p = ptr;//p记录被测函数在输入字符串str中的开始位置
			while (++ptr){
				//左括号数量+1
				if ('(' == str[ptr])
					lbra++;
				//右括号数量+1
				if (')' == str[ptr])
					rbra++;

				//如果左右括号数量相等则跳出循环
				if (lbra == rbra){
					ptr++;//ptr为被测函数在输入字符串str中的结束位置
					break;
				}
			}
		}

		//如果左右括号数量相等且不为0，则说明找到了被测函数体
		if ((lbra == rbra) && lbra>0){

			/*根据被测函数体起始位置，获取被测函数体字符串*/
			string ss(str.substr(p, ptr - p));
			int a = 0;
			
			//使用递归函数获取被测函数的期望值
			testdata.expVal = funParaExtract(ss, a);

			//把被测函数体存入spara中用于输出指示信息，实际运算并没有用到
			testdata.spara.push_back(ss);
			break;
		}
	}

	/*获取actual value，如果actual value是函数则把该函数当做被测函数一样对待，获取计算过程和上述一样
	如果actual value直接为数值，则存入testdata.actVal中*/
	while (++ptr < str.length()){
		int lbra = 0, rbra = 0;
		if ('(' == str[ptr]){ //actual value为函数体			
			lbra++;
			p = ptr;
			while (++ptr){
				if ('(' == str[ptr])
					lbra++;
				if (')' == str[ptr])
					rbra++;
				if (lbra == rbra){
					ptr++;
					break;
				}
			}
			string ss(str.substr(p, ptr - p));
			int a = 0;
			testdata.actVal = funParaExtract(ss, a);
			testdata.spara.push_back(ss);
			break;
		}
		else if (' ' != str[ptr]){//actual value为数值
			int p = ptr;
			while (++ptr < str.length());
			string ss(str.substr(p, ptr - p));
			testdata.actVal = stringToNum<double>(ss.c_str());
			testdata.spara.push_back(ss);
			break;
		}
	}

	//如果输入函数的参数数目和定义的函数参数数目输入不一致则跳出函数
	if (opt->ParaError){
		opt->ParaError = false;
		return;
	}

	opt->resultPrint("test", testdata);
}

/********************************************************************
fun name: funParaExtract(string str, int &ptr);
function: 使用递归计算函数值
parameter: 
     str: 函数体字符串  
	 ptr: 字符串的开始位置，在这里设为0即可
return：函数的excepted value
*********************************************************************/
double Parse::funParaExtract(string str, int &ptr){
	
	Function f;

	//提取函数名
	while (++ptr){
		if (' ' != str[ptr]){
			int p = ptr;
			while (' ' != str[++ptr]);
			string stmp(str.substr(p, ptr - p));
			//f.name = stmp;
			//根据函数名获取被测函数信息，存在函数结构体f中
			f = opt->getFunction(stmp, opt->funset);
			break;
		}
	}


	while (++ptr < str.length()){
		//按顺序解析函数体，如果在函数参数中遇到"("说明参数为函数，则调用递归函数，获取当前参数值
		if ('(' == str[ptr]){
			//递归获取参数值
			double fnum = funParaExtract(str, ptr);
			//把参数值存入f.dpara中
			f.dpara.push_back(fnum);
		}
		//参数为数值，则直接提取出来存入f.dpara
		else if ((' ' != str[ptr]) && ')' != str[ptr]){
			int p = ptr;
			while (' ' != str[++ptr] && ')' != str[ptr]);
			string stmp(str.substr(p, ptr - p));
			f.dpara.push_back(stringToNum<double>(stmp));

			//如果遇到右括号则说明，函数体结束，函数的参数提取完成，跳出循环，直接调用函数值计算函数
			if (')' == str[ptr])
				break;
		}
	}

	//调用函数值计算函数，计算函数值，并退出当前层的递归运算
	return opt->getFunctionValue(f);
}




void Parse::NumericalIntegration(string str, int ptr){
	Function fun;
	string stmp(str.substr(ptr, str.length() - ptr));
	stringstream sst(stmp);
	string s1;
	vector<string> res;
	while (sst >> s1)
		res.push_back(s1);

	//根据函数名获取函数信息存入fun
	fun = opt->getFunction(res[0], opt->funset);
	
	//获取积分步长
	fun.step = stringToNum<double>(res[1]);

	//提取积分上下限
	vector<double> limit;
	for (int i = 2; i < res.size(); i++)
		limit.push_back(stringToNum<double>(res[i]));

	/*使用递归函数对函数积分，并返回积分结果，
	因为多重积分中存在嵌套关系，所以在步长较小，积分区间较大时，该积分方式非常耗时，
	该部分功能没有做优化
	*/
	double ret = Intergral(fun,limit);

	cout << "The intergral result is: " << ret << endl;

}


double Parse::Intergral(Function f,vector<double> li){

	int p = f.dpara.size();//获取函数的参数数量
	int panum = f.spara.size();

	double ret = 0;

	//对于高维积分，则一个变量一个变量的积分，需要使用递归
	/*在积分计算过程中上下限只需要计算一次，但是中间的每个步长间隔需要计算两次值，
	具体可以根据积分的计算过程来理解
	*/

	//如果参数>1则使用递归调用，分层次积分
	if (p < panum-1){
		f.dpara.push_back(li[2 * p]);
		ret += Intergral(f, li);//递归调用，计算下限
		//因为使用double类型做循环变量，可能存在问题，所以在判断循环结束时要+0.0000000000001
		for (double stp = li[2 * p] + f.step; stp < li[2 * p + 1] + 0.0000000000001; stp += f.step){
			f.dpara.erase(f.dpara.end() - 1);
			f.dpara.push_back(stp);
			ret += Intergral(f, li)* 2.0;//递归调用
		}
		ret -= Intergral(f, li);//循环中多计算了一次积分上限，这里减去
	}
	//如果只有最外层积分时，则直接积分
	else if (p == panum-1){
		f.dpara.push_back(li[2 * p]);
		ret += abs(opt->getFunctionValue(f));

		for (double stp = li[2 * p] + f.step; stp < li[2 * p + 1] + 0.0000000000001; stp += f.step){
			f.dpara.erase(f.dpara.end() - 1);
			f.dpara.push_back(stp);
			ret += abs(opt->getFunctionValue(f) * 2.0);
		}
		ret -= abs(opt->getFunctionValue(f));
	}
	ret = ret / 2.0 * f.step;
	return ret;
}


void Parse::FunctionMcint(string str, int ptr) {
	Function mcintdata;

	string stmp(str.substr(ptr, str.length() - ptr));
	stringstream sst(stmp);
	string s1;
	vector<string> res;
	while (sst >> s1)
		res.push_back(s1);

	mcintdata = opt->getFunction(res[0], opt->funset);

	int N;
	N = stringToNum<int>(res[1]);

	vector<double> pMin;
	vector<double> pMax;

	for (int i = 2; i < res.size(); i += 2) {
		pMin.push_back(stringToNum<double>(res[i]));
		pMax.push_back(stringToNum<double>(res[i + 1]));
	}

	int M = 100000;
	vector<int> randnumber;
	for (int i = 0; i < N; i++) {
		random_device rd;	//随机种子的生成；
		mt19937 gen(rd()); //马特赛特旋转演算法，随机数生成引擎；
		uniform_int_distribution <int> dis(1, 100000);	//1-100000上的均匀分布；
		int randnum = dis(gen);	//随机产生1-100的数字；
		vector<int> ::iterator it;
		it = find(randnumber.begin(), randnumber.end(), randnum);
		if (it != randnumber.end()) {
			i--;
		}
		else
		{
			randnumber.push_back(randnum);
		}
	}

	mcintdata.dpara.clear();
	double avg = 0;
	for (int k = 0; k < N; k++) {
		for (int i = 0; i < pMin.size(); i++) {
			double 	paramet = pMin[i] + (pMax[i] - pMin[i]) / M*randnumber[k];
			mcintdata.dpara.push_back(paramet);
		}
		avg += opt->getFunctionValue(mcintdata);
		mcintdata.dpara.clear();
	}
	avg = avg / N;

	for (int i = 0; i < pMin.size(); i++) {
		avg = avg*(pMax[i] - pMin[i]);
	}

	cout << "The Monte Carlo Integration is:" << avg << endl;

}



vector<double> Parse::PartialDifferential(Function f, vector<double> pot){
	double h = 0.000000001;
	vector<double> ret;

	f.dpara.clear();
	f.dpara.insert(f.dpara.begin(), pot.begin(), pot.end());
	double x1 = opt->getFunctionValue(f);

	for (int i = 0; i < f.spara.size(); i++){
		vector<double> potnew = pot;
		potnew[i] += h;
		f.dpara.clear();
		f.dpara.insert(f.dpara.begin(), potnew.begin(), potnew.end());
		double x2 = opt->getFunctionValue(f);

		ret.push_back((x2 - x1) / h);
	}
	return ret;
}


void Parse::GradientAscent(string str, int ptr){
	
	string stmp(str.substr(ptr, str.length() - ptr));
	stringstream sst(stmp);
	string s1;
	vector<string> res;
	while (sst >> s1)
		res.push_back(s1);
	
	Function fun = opt->getFunction(res[0], opt->funset);

	double gamma = stringToNum<double>(res[1]);
	double convDist = stringToNum<double>(res[2]);
	vector<double> startpot(fun.spara.size(), 0);
	for (int i = 0; i < fun.spara.size(); i++){
		startpot[i]=stringToNum<double>(res[i + 3]);
	}

	unsigned int mns = stringToNum<unsigned int>(res[fun.spara.size() + 3]);
	
	vector<double> newpoint(startpot.size(), 0);
	vector<double> step = PartialDifferential(fun,startpot);

	while (StandardDeviation(step) > convDist){
		vector<double> pdif = movCompute(step, gamma);
		for (int i = 0; i < startpot.size(); i++)
			newpoint[i] = startpot[i] + pdif[i];

		startpot = newpoint;
		step = PartialDifferential(fun, startpot);
	}

	fun.dpara = startpot;
	double ret = opt->getFunctionValue(fun);
	cout << "The maxmum of the function " << fun.name << " is: " << ret << endl;
}


void Parse::GradientDescent(string str, int ptr){
	string stmp(str.substr(ptr, str.length() - ptr));
	stringstream sst(stmp);
	string s1;
	vector<string> res;
	while (sst >> s1)
		res.push_back(s1);

	Function fun = opt->getFunction(res[0], opt->funset);

	double gamma = stringToNum<double>(res[1]);
	double convDist = stringToNum<double>(res[2]);
	vector<double> startpot(fun.spara.size(), 0);
	for (int i = 0; i < fun.spara.size(); i++){
		startpot[i] = stringToNum<double>(res[i + 3]);
	}

	unsigned int mns = stringToNum<unsigned int>(res[fun.spara.size() + 3]);
	vector<double> newpoint(startpot.size(), 0);
	vector<double> step = PartialDifferential(fun, startpot);

	while (StandardDeviation(step) > convDist){
		vector<double> pdif = movCompute(step, gamma);
		for (int i = 0; i < startpot.size(); i++){
			newpoint[i] = startpot[i] - pdif[i];
		}
		startpot = newpoint;
		step = PartialDifferential(fun, startpot);
	}

	fun.dpara = startpot;
	double ret = opt->getFunctionValue(fun);
	cout << "The minimum of the function " << fun.name << " is: " << ret << endl;
}


double Parse::StandardDeviation(vector<double> pdif){
	double dis = 0;
	vector<double> tmp(pdif.size(), 0);

	for (unsigned i = 0; i<pdif.size(); i++){
		tmp[i] = pdif[i] * pdif[i];
		dis += tmp[i];
	}
	
	dis = sqrt(dis);
	return dis;
}


vector<double> Parse::movCompute(vector<double> pdif, double gamma){
	
	vector<double> tmp(pdif.size(), 0);
	for (unsigned i = 0; i<pdif.size(); i++)
		tmp[i] = pdif[i] * gamma;

	return tmp;
}


