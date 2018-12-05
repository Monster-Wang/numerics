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
	/*���ݲ�ʹ�����Ľṹ����֪��һ��������Ϊ���⺯���壬����ȡ���⺯�����ַ�����eg: "(f (g 6 2) (h 2.5 8))"����
	��Ϊ���⺯������Ƕ�ף����Կ���ʹ�õݹ鷨�����������ݹ����ʱ���ɵõ�����㺯�������в�����eg: (f v1 v2 v2)����
	Ȼ����� "getFunctionValue(Function &fun)" ���ɵõ�����ֵ*/

	//��ȡ���⺯�����ַ����������������ŵ�������λ���⺯����ķ�Χ
	int p;
	ptr = ptrcnt;//�ַ���ָ��ص�cmd��test��֮���Ա��ȡ�������⺯����
	while (++ptr){
		int lbra = 0, rbra = 0;
		/*���������ַ����������ո�������һ����(��������lbra+1��Ȼ�����ѭ����
		ֱ�����������������������ѭ��*/
		if ('(' == str[ptr]){
			lbra++;
			p = ptr;//p��¼���⺯���������ַ���str�еĿ�ʼλ��
			while (++ptr){
				//����������+1
				if ('(' == str[ptr])
					lbra++;
				//����������+1
				if (')' == str[ptr])
					rbra++;

				//������������������������ѭ��
				if (lbra == rbra){
					ptr++;//ptrΪ���⺯���������ַ���str�еĽ���λ��
					break;
				}
			}
		}

		//�������������������Ҳ�Ϊ0����˵���ҵ��˱��⺯����
		if ((lbra == rbra) && lbra>0){

			/*���ݱ��⺯������ʼλ�ã���ȡ���⺯�����ַ���*/
			string ss(str.substr(p, ptr - p));
			int a = 0;
			
			//ʹ�õݹ麯����ȡ���⺯��������ֵ
			testdata.expVal = funParaExtract(ss, a);

			//�ѱ��⺯�������spara���������ָʾ��Ϣ��ʵ�����㲢û���õ�
			testdata.spara.push_back(ss);
			break;
		}
	}

	/*��ȡactual value�����actual value�Ǻ�����Ѹú����������⺯��һ���Դ�����ȡ������̺�����һ��
	���actual valueֱ��Ϊ��ֵ�������testdata.actVal��*/
	while (++ptr < str.length()){
		int lbra = 0, rbra = 0;
		if ('(' == str[ptr]){ //actual valueΪ������			
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
		else if (' ' != str[ptr]){//actual valueΪ��ֵ
			int p = ptr;
			while (++ptr < str.length());
			string ss(str.substr(p, ptr - p));
			testdata.actVal = stringToNum<double>(ss.c_str());
			testdata.spara.push_back(ss);
			break;
		}
	}

	//������뺯���Ĳ�����Ŀ�Ͷ���ĺ���������Ŀ���벻һ������������
	if (opt->ParaError){
		opt->ParaError = false;
		return;
	}

	opt->resultPrint("test", testdata);
}

/********************************************************************
fun name: funParaExtract(string str, int &ptr);
function: ʹ�õݹ���㺯��ֵ
parameter: 
     str: �������ַ���  
	 ptr: �ַ����Ŀ�ʼλ�ã���������Ϊ0����
return��������excepted value
*********************************************************************/
double Parse::funParaExtract(string str, int &ptr){
	
	Function f;

	//��ȡ������
	while (++ptr){
		if (' ' != str[ptr]){
			int p = ptr;
			while (' ' != str[++ptr]);
			string stmp(str.substr(p, ptr - p));
			//f.name = stmp;
			//���ݺ�������ȡ���⺯����Ϣ�����ں����ṹ��f��
			f = opt->getFunction(stmp, opt->funset);
			break;
		}
	}


	while (++ptr < str.length()){
		//��˳����������壬����ں�������������"("˵������Ϊ����������õݹ麯������ȡ��ǰ����ֵ
		if ('(' == str[ptr]){
			//�ݹ��ȡ����ֵ
			double fnum = funParaExtract(str, ptr);
			//�Ѳ���ֵ����f.dpara��
			f.dpara.push_back(fnum);
		}
		//����Ϊ��ֵ����ֱ����ȡ��������f.dpara
		else if ((' ' != str[ptr]) && ')' != str[ptr]){
			int p = ptr;
			while (' ' != str[++ptr] && ')' != str[ptr]);
			string stmp(str.substr(p, ptr - p));
			f.dpara.push_back(stringToNum<double>(stmp));

			//���������������˵��������������������Ĳ�����ȡ��ɣ�����ѭ����ֱ�ӵ��ú���ֵ���㺯��
			if (')' == str[ptr])
				break;
		}
	}

	//���ú���ֵ���㺯�������㺯��ֵ�����˳���ǰ��ĵݹ�����
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

	//���ݺ�������ȡ������Ϣ����fun
	fun = opt->getFunction(res[0], opt->funset);
	
	//��ȡ���ֲ���
	fun.step = stringToNum<double>(res[1]);

	//��ȡ����������
	vector<double> limit;
	for (int i = 2; i < res.size(); i++)
		limit.push_back(stringToNum<double>(res[i]));

	/*ʹ�õݹ麯���Ժ������֣������ػ��ֽ����
	��Ϊ���ػ����д���Ƕ�׹�ϵ�������ڲ�����С����������ϴ�ʱ���û��ַ�ʽ�ǳ���ʱ��
	�ò��ֹ���û�����Ż�
	*/
	double ret = Intergral(fun,limit);

	cout << "The intergral result is: " << ret << endl;

}


double Parse::Intergral(Function f,vector<double> li){

	int p = f.dpara.size();//��ȡ�����Ĳ�������
	int panum = f.spara.size();

	double ret = 0;

	//���ڸ�ά���֣���һ������һ�������Ļ��֣���Ҫʹ�õݹ�
	/*�ڻ��ּ��������������ֻ��Ҫ����һ�Σ������м��ÿ�����������Ҫ��������ֵ��
	������Ը��ݻ��ֵļ�����������
	*/

	//�������>1��ʹ�õݹ���ã��ֲ�λ���
	if (p < panum-1){
		f.dpara.push_back(li[2 * p]);
		ret += Intergral(f, li);//�ݹ���ã���������
		//��Ϊʹ��double������ѭ�����������ܴ������⣬�������ж�ѭ������ʱҪ+0.0000000000001
		for (double stp = li[2 * p] + f.step; stp < li[2 * p + 1] + 0.0000000000001; stp += f.step){
			f.dpara.erase(f.dpara.end() - 1);
			f.dpara.push_back(stp);
			ret += Intergral(f, li)* 2.0;//�ݹ����
		}
		ret -= Intergral(f, li);//ѭ���ж������һ�λ������ޣ������ȥ
	}
	//���ֻ����������ʱ����ֱ�ӻ���
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
		random_device rd;	//������ӵ����ɣ�
		mt19937 gen(rd()); //����������ת���㷨��������������棻
		uniform_int_distribution <int> dis(1, 100000);	//1-100000�ϵľ��ȷֲ���
		int randnum = dis(gen);	//�������1-100�����֣�
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


