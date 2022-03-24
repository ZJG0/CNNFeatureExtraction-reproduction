/*
 * @Author: ZJG
 * @Date: 2022-03-18 10:59:18
 * @LastEditors: ZJG
 * @LastEditTime: 2022-03-21 17:17:52
 */
#include <iostream>
#include <string>
#include "secondary.h"
#include "connect.h"
#include "AESObject.h"
#include "ParallelAESObject.h"
#include "Functionalities.h"
#include "tools.h"

using namespace std;
extern int partyNum;
int NUM_OF_PARTIES;	

AESObject* aes_common;
AESObject* aes_indep;
AESObject* aes_a_1;
AESObject* aes_a_2;
AESObject* aes_b_1;
AESObject* aes_b_2;
AESObject* aes_c_1;
ParallelAESObject* aes_parallel;

int main(int argc, char** argv)
{

/****************************** PREPROCESSING ******************************/ 
	parseInputs(argc, argv);
	string whichNetwork = "No Network";

/****************************** AES SETUP and SYNC ******************************/ 
	aes_indep = new AESObject(argv[4]);
	aes_common = new AESObject(argv[5]);
	aes_a_1 = new AESObject("files/keyD");
	aes_a_2 = new AESObject("files/keyD");
	aes_b_1 = new AESObject("files/keyD");
	aes_b_2 = new AESObject("files/keyD");
	aes_c_1 = new AESObject("files/keyD");
	aes_parallel = new ParallelAESObject(argv[5]);


	// vector<myType> A(5, 0);
	// populateRandomVector<myType>(A, 5, "c_1", "POSITIVE");
	// for (size_t i = 0; i < 5; i++)
	// {
	// 	/* code */
	// 	cout<<A[i]<<endl;
	// }
	// return 0;
	
	initializeMPC();

	if (!STANDALONE)
	{
		initializeCommunication(argv[3], partyNum);
		synchronize(2000000);
	}
	
	if (PARALLEL)
		aes_parallel->precompute();

/****************************** RUN NETWORK/BENCHMARKS ******************************/ 
	
	start_m();

	whichNetwork = "Debug DotProduct";
	//debugDotProduct();
	debugTest();
	//debugReLU();
	// debugReLUPrime();
	// debugSS();
	// debugDotProd();

	end_m(whichNetwork);
	cout << "----------------------------------" << endl;  	
	cout << NUM_OF_PARTIES << "PC code, P" << partyNum << endl;
	cout << NUM_ITERATIONS << " iterations, " << whichNetwork << ", batch size " << MINI_BATCH_SIZE << endl;
	cout << "----------------------------------" << endl << endl;  

/****************************** CLEAN-UP ******************************/ 
	delete aes_common;
	delete aes_indep;
	delete aes_a_1;
	delete aes_a_2;
	delete aes_b_1;
	delete aes_b_2;
	delete aes_c_1;
	delete aes_parallel;
	if (partyNum != PARTY_S)
		deleteObjects();
	return 0;
}

