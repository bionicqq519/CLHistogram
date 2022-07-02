#include<iostream>
#include<fstream>
#include <string>
using namespace std;
 
int main(){
	//#define SIZE 100
	//char line[SIZE];
	string line;
	
    fstream fin;
    ofstream fout;
    fin.open("Histogram_CLKernel.cl",ios::in);
    fout.open("Histogram_CLKernel.txt",ios::out);
#if 0
    line = "#ifndef __HISTOGRAM_OPENCLKERNELCODE_H__";
    cout<<line<<endl;
    fout<<line<<endl;
	line = "#define __HISTOGRAM_OPENCLKERNELCODE_H__";
	cout<<line<<endl;
    fout<<line<<endl;
	line = "const char *sHis_CL_KernelCode =";
    cout<<line<<endl;
    fout<<line<<endl;
#endif
    while(getline(fin,line)){
    	cout<<"\""<<line<<"\\n\""<<endl;
        fout<<"\""<<line<<"\\n\""<<endl;
    }
#if 0
    line = ";";
    cout<<line<<endl;
    fout<<line<<endl;
    line = "#endif // __HISTOGRAM_OPENCLKERNELCODE_H__";
    cout<<line<<endl;
    fout<<line<<endl;
 #endif
    fin.close();//Ãö³¬ÀÉ®×
    fout.close();
    system("pause");
    return 0;
}
