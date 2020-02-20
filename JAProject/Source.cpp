#include "pch.h"
using namespace std;
//void CppOnlyBlur(unsigned char* Tablica, int Szer, int szerokoscbajt, int start, int koniec);
void Trapezoid(float* a, float* b, float* c, int max,float step);
//extern "C" int _stdcall AsmVal();
struct IUnknown;
int CallMyDLLC(float* tab1, float* tab2, float* wynik, int max, float step)
{
	HINSTANCE hGetProcIDDLL = LoadLibrary("JADLLC.dll");
	FARPROC lpfnGetProcessID = GetProcAddress(HMODULE(hGetProcIDDLL), "Trapezoid");
	typedef void(__cdecl* pICFUNC)(float*, float*, float*, int, float);
	pICFUNC Algorithm;
	Algorithm = pICFUNC(lpfnGetProcessID);

	Algorithm(tab1, tab2, wynik, max,step);

	FreeLibrary(hGetProcIDDLL);
	return 0;
}
float func(float x) {
	return x * x -x- (float)5;
}
void prepareArgumentAndFunctionValues(int lowerIntegrationBoundary, int higherIntegrationBoundary, int amountOfTrapezes)
{
	fstream fileValues1;
	fstream fileValues2;
	fileValues1.open("f(x)1.txt",ios::out|ios::trunc);
	float step = abs(lowerIntegrationBoundary - higherIntegrationBoundary) / ((float)amountOfTrapezes);
	for (int i = 0; i <= amountOfTrapezes; i++) {
		fileValues1 << func((float)lowerIntegrationBoundary+float(i)*step);
		//cout << std::setprecision(8)<<float(i)*step << endl;
		fileValues1 << " ";
	}
	fileValues1.close();
	fileValues2.close();
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		cout << argv[i];
		cout << endl;
	}
	bool got = false;
	int numberOfThreads;
	std::cout << "Chcesz podac ilosc watkow jakie maja zostac uruchomione? y/n: ";
	char znak = _getch();
	if (znak == 'y')
	{
		std::cout << "\nPodaj ilosc watkow: ";
		std::cin >> numberOfThreads;
		got = true;
	}
	else {
		std::cout << "\n";
	}
	typedef void(_fastcall *ProcAsm)(float*, float*, float*, int,float,float);
    HINSTANCE dllHandleAsm = NULL;
	dllHandleAsm = LoadLibrary("JADLL.dll");
	ProcAsm TrapezoidAsm = (ProcAsm)GetProcAddress(dllHandleAsm, "MyProc1");
	if(!got) numberOfThreads = thread::hardware_concurrency();
	fstream fileValues1;
	fileValues1.open("f(x)1.txt", std::ios::in);
	fstream result;
	result.open("result.txt", ios::app|ios::out);
	//matrix arrays	
	char* p;
	errno = 0; // not 'int errno', because the '#include' already defined it
	long a = strtol(argv[1], &p, 10);
	if (*p != '\0' || errno != 0) {
		return 1; // In main(), returning non-zero means failure
	}

	if (a < INT_MIN || a > INT_MAX) {
		return 1;
	}
	int b = strtol(argv[2], &p, 10);
	if (*p != '\0' || errno != 0) {
		return 1; 
	}

	if (b < INT_MIN || b > INT_MAX) {
		return 1;
	}
	int n = strtol(argv[3], &p, 10);
	if (*p != '\0' || errno != 0) {
		return 1; 
	}

	if (n < INT_MIN || n > INT_MAX) {
		return 1;
	}
	int arg_a = a;
	int arg_b = b;
	int arg_n = n;
	int matrixLenght = n;
	float step = abs(arg_a - arg_b) / ((float)n);
	float division = 2.00000000;
	result << "a: " << arg_a << " b: " << arg_b << " Ilosc trapezow: " << n << " Ilosc watkow: "<<numberOfThreads;
	prepareArgumentAndFunctionValues(arg_a, arg_b, arg_n);
		cout << "Matrix lenght: " << matrixLenght << endl;
		float **values1 = new float *[numberOfThreads];
		float **values2 = new float *[numberOfThreads];
		float **matrixArrayResultAsm = new float*[numberOfThreads];
		float **matrixArrayResultC = new float*[numberOfThreads];
		int arrayLenght = matrixLenght / numberOfThreads;
		int moduloLenghtThreads = matrixLenght % numberOfThreads;
		int lenghtLastThread = arrayLenght + moduloLenghtThreads;
		bool lastThreadHasMore = false;
		cout << "amount of threads: " << numberOfThreads << endl;
		cout << "lenght for each (w/o last) arrays: " << arrayLenght << endl;
		cout << "lenght for last array: " << lenghtLastThread << endl;
		if (moduloLenghtThreads != 0) {
			cout << "Last array will have: " << moduloLenghtThreads << " more numbers" << endl;
			lastThreadHasMore = true;
		}
		//Prepare for sse
		int lenghtAssembler;
		lenghtAssembler = arrayLenght;
		lenghtAssembler = arrayLenght + ((4 - arrayLenght % 4) % 4);
		int lenghtAssemblerLastThread;
		if (lastThreadHasMore) {
			//last thread 
			lenghtAssemblerLastThread = arrayLenght + moduloLenghtThreads;
			lenghtAssemblerLastThread = lenghtAssemblerLastThread + ((4 - lenghtAssemblerLastThread % 4) % 4);
		}
		else {
			//
			lenghtAssemblerLastThread = lenghtAssembler;
		}
		std::cout << "Every thread assembler iteration: " << lenghtAssembler << endl;
		cout << "Last thread assembler iteration: " << lenghtAssemblerLastThread << endl;
		for (int i = 0; i < numberOfThreads - 1; ++i)
		{
			values1[i] = new float[lenghtAssembler];
			values2[i] = new float[lenghtAssembler];
			matrixArrayResultAsm[i] = new float[lenghtAssembler];
			matrixArrayResultC[i] = new float[lenghtAssembler];
		}
		//last thread
		values1[numberOfThreads - 1] = new float[lenghtAssemblerLastThread];
		values2[numberOfThreads - 1] = new float[lenghtAssemblerLastThread];
		matrixArrayResultAsm[numberOfThreads - 1] = new float[lenghtAssemblerLastThread];
		matrixArrayResultC[numberOfThreads - 1] = new float[lenghtAssemblerLastThread];
		streampos oldPos;
		//loading from file 
		for (int i = 0; i < numberOfThreads - 1; ++i)
		{
			for (int j = 0; j < arrayLenght; ++j)
			{
				
				fileValues1 >> values1[i][j];
				oldPos = fileValues1.tellg();
				fileValues1 >> values2[i][j];
				fileValues1.seekg(oldPos);
			}
		}
		for (int j = 0; j < lenghtLastThread; ++j)
		{
			fileValues1 >> values1[numberOfThreads - 1][j];
			oldPos = fileValues1.tellg();
			fileValues1 >> values2[numberOfThreads - 1][j];
			fileValues1.seekg(oldPos);
		}


		//obliczenia
		std::thread *t = new std::thread[numberOfThreads];
		int amountOfTimesFirst = lenghtAssembler / 4;
		int amountOfTimesSecond = lenghtAssemblerLastThread / 4;
		
		for (int i = 0; i < numberOfThreads - 1; i++)
		{
			t[i] = thread(TrapezoidAsm, values1[i], values2[i], matrixArrayResultAsm[i], amountOfTimesFirst, step, division);
		}

		t[numberOfThreads - 1] = thread(TrapezoidAsm, values1[numberOfThreads - 1],
			values2[numberOfThreads - 1], matrixArrayResultAsm[numberOfThreads - 1], amountOfTimesSecond, step, division);
		auto startTime = chrono::steady_clock::now();
		for (int i = 0; i < numberOfThreads; ++i)
		{
			t[i].join();
		}
		auto endTime = chrono::steady_clock::now();
		std::cout << "czas Asm: " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count()<<endl;
		result << " czas Asm: " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count()<<" microseconds";
		std::thread *t1 = new std::thread[numberOfThreads];
		for (int i = 0; i < numberOfThreads - 1; i++)
		{
			t1[i] = thread(CallMyDLLC, values1[i], values2[i], matrixArrayResultC[i], arrayLenght,step);
		}

		t1[numberOfThreads - 1] = thread(CallMyDLLC, values1[numberOfThreads - 1],
			values2[numberOfThreads - 1], matrixArrayResultC[numberOfThreads - 1], lenghtLastThread,step);
		startTime = chrono::steady_clock::now();
		for (int i = 0; i < numberOfThreads; ++i)
		{
			t1[i].join();
		}
		endTime = chrono::steady_clock::now();
		std::cout << "czas C: " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() << endl;
		result << " czas C: " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() <<" microseconds ";
	
		long double sum = 0;
		long double localSum = 0;
		//policzenie pol dla asm
		for (int i = 0; i < numberOfThreads - 1; ++i)
		{
		for (int j = 0; j < 4; j++)
		{
			localSum+= matrixArrayResultAsm[i][j];
			//cout << matrixArrayResultAsm[i][5];
		}
		sum += localSum;
		localSum = 0;		
		}
		for (int j = 0; j < 4; j++)
		{
		localSum += matrixArrayResultAsm[numberOfThreads - 1][j];
		}
		sum += localSum;
		result << "Pole: " << sum << endl;
		cout << sum<<endl;
		sum = 0;
		localSum = 0;
		//policzenie pol dla c, powinno byc takie same
		for (int i = 0; i < numberOfThreads - 1; ++i)
		{
			for (int j = 0; j < arrayLenght; ++j)
			{
				localSum += matrixArrayResultC[i][j];
			}
			sum += localSum;
			localSum = 0;
		}
		for (int j = 0; j < lenghtLastThread; ++j)
		{
			localSum += matrixArrayResultC[numberOfThreads - 1][j];
		}
		sum += localSum;
		cout << sum;

	for (int i = 0; i < numberOfThreads; ++i)
	{
		delete[] values1[i];
		delete[] values2[i];
		delete[] matrixArrayResultAsm[i];
	}
	delete[]values1;
	delete[]values2;
	delete[]matrixArrayResultAsm;
	delete[]matrixArrayResultC;
	fileValues1.close();
	result.close();
	return 0;
}