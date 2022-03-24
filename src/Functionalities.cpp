/*
 * @Author: ZJG
 * @Date: 2022-03-18 11:01:27
 * @LastEditors: ZJG
 * @LastEditTime: 2022-03-24 11:36:02
 */
#pragma once
#include "Functionalities.h"
#include <algorithm>    // std::rotate
#include <thread>
#include <chrono>
using namespace std;
using namespace chrono;

void funcReconstruct2PC(const vector<myType> &a, vector<myType> &b, size_t size, string str)
{
	assert((partyNum == PARTY_A or partyNum == PARTY_B) && "Reconstruct called by spurious parties");

	vector<myType> temp(size);
	if (partyNum == PARTY_B)
		sendVector<myType>(a, PARTY_A, size);
	if (partyNum == PARTY_A)
	{
		receiveVector<myType>(temp, PARTY_B, size);
		addVectors<myType>(temp, a, temp, size);
	
		cout << str << ": ";
		for (size_t i = 0; i < size; ++i)
			print_linear(temp[i]>>FLOAT_PRECISION, DEBUG_PRINT);
			// print_linear(temp[i], DEBUG_PRINT);
		cout << endl;
	}
}

void funcReconstructBit(const smallType &a, smallType &b)
{
	assert((partyNum == PARTY_A or partyNum == PARTY_B) && "Reconstruct called by spurious parties");

	smallType temp;
	sendMyType<smallType>(a, adversary(partyNum));
	receiveMyType<smallType>(temp, adversary(partyNum));

	b = a ^ temp;
}

void funcGetShares(vector<myType> &a, const vector<myType> &data)
{
	size_t size = a.size();

	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i] = data[i];
		}
	}
	else if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i] = 0;
		}
	}
}

void funcTruncate2PC(vector<myType> &a, size_t power, size_t size, size_t party_1, size_t party_2)
{
	assert((partyNum == party_1 or partyNum == party_2) && "Truncate called by spurious parties");

	if (partyNum == party_1)
		for (size_t i = 0; i < size; ++i)
			a[i] = static_cast<uint64_t>(static_cast<int64_t>(a[i]) >> power);

	if (partyNum == party_2)
		for (size_t i = 0; i < size; ++i)
			a[i] = - static_cast<uint64_t>(static_cast<int64_t>(- a[i]) >> power);
}

void funcDotProductMPC(const vector<myType> &a, const vector<myType> &b, 
						   vector<myType> &c, size_t size) 
{
	log_print("funcDotProductMPC");

	if (THREE_PC)
	{
		vector<myType> A(size, 0), B(size, 0), C(size, 0);

		if (HELPER)
		{
			vector<myType> A1(size, 0), A2(size, 0), 
						   B1(size, 0), B2(size, 0), 
						   C1(size, 0), C2(size, 0);

			populateRandomVector<myType>(A1, size, "a_1", "POSITIVE");
			populateRandomVector<myType>(A2, size, "a_2", "POSITIVE");
			populateRandomVector<myType>(B1, size, "b_1", "POSITIVE");
			populateRandomVector<myType>(B2, size, "b_2", "POSITIVE");
			populateRandomVector<myType>(C1, size, "c_1", "POSITIVE");

			// populateRandomVector<myType>(A1, size, "INDEP", "POSITIVE");
			// populateRandomVector<myType>(A2, size, "INDEP", "POSITIVE");
			// populateRandomVector<myType>(B1, size, "INDEP", "POSITIVE");
			// populateRandomVector<myType>(B2, size, "INDEP", "POSITIVE");

			addVectors<myType>(A1, A2, A, size);
			addVectors<myType>(B1, B2, B, size);

			for (size_t i = 0; i < size; ++i)
				C[i] = A[i] * B[i];

			// splitIntoShares(C, C1, C2, size);
			subtractVectors<myType>(C, C1, C2, size);
			sendVector<myType>(C2, PARTY_B, size);

			// sendThreeVectors<myType>(A1, B1, C1, PARTY_A, size, size, size);
			// sendThreeVectors<myType>(A2, B2, C2, PARTY_B, size, size, size);
		}

		if (PRIMARY)
		{
			if (partyNum == PARTY_A)
			{
				populateRandomVector<myType>(A, size, "a_1", "POSITIVE");
				populateRandomVector<myType>(B, size, "b_1", "POSITIVE");
				populateRandomVector<myType>(C, size, "c_1", "POSITIVE");
			}

			if (partyNum == PARTY_B)
			{
				populateRandomVector<myType>(A, size, "a_2", "POSITIVE");
				populateRandomVector<myType>(B, size, "b_2", "POSITIVE");
				receiveVector<myType>(C, PARTY_C, size);
			}			

			// receiveThreeVectors<myType>(A, B, C, PARTY_C, size, size, size);
			vector<myType> alpha(size), beta(size), temp_alpha(size), temp_beta(size);
			myType temp;
			
			subtractVectors<myType>(a, A, alpha, size);
			subtractVectors<myType>(b, B, beta, size);

			thread *threads = new thread[2];

			threads[0] = thread(sendTwoVectors<myType>, ref(alpha), ref(beta), adversary(partyNum), size, size);
			threads[1] = thread(receiveTwoVectors<myType>, ref(temp_alpha), ref(temp_beta), adversary(partyNum), size, size);
	
			for (int i = 0; i < 2; i++)
				threads[i].join();

			delete[] threads;

			
			addVectors<myType>(alpha, temp_alpha, alpha, size);
			addVectors<myType>(beta, temp_beta, beta, size);
			

			for (size_t i = 0; i < size; ++i)
			{
				c[i] = A[i] * beta[i] + alpha[i] * B[i] + C[i];

				if (partyNum == PARTY_B)
				{
					temp = alpha[i] * beta[i];
					c[i] = c[i] + temp;
				}
			}
			/*************************SecureNN************************/
			// for (size_t i = 0; i < size; ++i)
			// {
			// 	c[i] = a[i] * F[i];
			// 	temp = E[i] * b[i];
			// 	c[i] = c[i] + temp;

			// 	if (partyNum == PARTY_A)
			// 	{
			// 		temp = E[i] * F[i];
			// 		c[i] = c[i] - temp;
			// 	}
			// }
			// addVectors<myType>(c, C, c, size);
			/*********************************************************/
			funcTruncate2PC(c, FLOAT_PRECISION, size, PARTY_A, PARTY_B);
		}
	}
}
void funcBitAnd(const vector<smallType> &x, const vector<smallType> &y, 
						   vector<smallType> &z, size_t size) 
{
	log_print("funcBitAnd");

	if (THREE_PC)
	{
		vector<smallType> a(size, 0), b(size, 0), c(size, 0);

		if (HELPER)
		{
			vector<smallType> a1(size, 0), a2(size, 0), 
						   b1(size, 0), b2(size, 0), 
						   c1(size, 0), c2(size, 0);

			populateRandomVector<smallType>(a1, size, "r_1", "POSITIVE");
			populateRandomVector<smallType>(a2, size, "r_1", "POSITIVE");
			populateRandomVector<smallType>(b1, size, "r_2", "POSITIVE");
			populateRandomVector<smallType>(b2, size, "r_2", "POSITIVE");
			populateRandomVector<smallType>(c1, size, "r_1", "POSITIVE");

			xorVectors<smallType>(a1, a2, a, size);
			xorVectors<smallType>(b1, b2, b, size);

			for (size_t i = 0; i < size; ++i)
				c[i] = a[i] && b[i];

			// splitIntoShares(C, C1, C2, size);
			xorVectors<smallType>(c, c1, c2, size);
			// sendVector<smallType>(c2, PARTY_B, size);
			sendThreeVectors<smallType>(a1, b1, c1, PARTY_A, size, size, size);
			sendThreeVectors<smallType>(a2, b2, c2, PARTY_B, size, size, size);
		}

		if (PRIMARY)
		{
			receiveThreeVectors(a, b, c, PARTY_C, size, size, size);
			vector<smallType> e(size), f(size), temp_e(size), temp_f(size);
			
			xorVectors<smallType>(a, x, e, size);
			xorVectors<smallType>(b, y, f, size);

			thread *threads = new thread[2];

			threads[0] = thread(sendTwoVectors<smallType>, ref(e), ref(f), adversary(partyNum), size, size);
			threads[1] = thread(receiveTwoVectors<smallType>, ref(temp_e), ref(temp_f), adversary(partyNum), size, size);
	
			for (int i = 0; i < 2; i++)
				threads[i].join();

			delete[] threads;

			
			xorVectors<smallType>(e, temp_e, e, size);
			xorVectors<smallType>(f, temp_f, f, size);
			
			smallType temp1;
			smallType temp2;
			smallType temp3;

			for (size_t i = 0; i < size; ++i)
			{
				temp1 = partyNum && e[i] && f[i];
				temp2 = f[i] && a[i];
				temp3 = e[i] && b[i];
				z[i] = temp1 ^ temp2 ^ temp3 ^ c[i];
				//z[i] = smallType(partyNum) && e[i] && f[i] ^ f[i] && a[i] ^ e[i] && b[i] ^ c[i];
			}
			//funcTruncate2PC(c, FLOAT_PRECISION, size, PARTY_A, PARTY_B);
		}
	}
}
void funcBitAdd(const vector<smallType> &v, const vector<smallType> &r, vector<smallType> &u, size_t size)
{
	if(PRIMARY)
	{
		vector<smallType> alpha(size, 0);
		vector<smallType> beta(size, 0);
		for (size_t i = 0; i < size; i++)
		{
			alpha[i] = v[i] ^ r[i];
		}
		funcBitAnd(v, r, beta, size);
		vector<smallType> c(size, 0);
		u[size-1] = v[size-1] ^ r[size-1];
		//c[size-1] = 0;
		for (int i = size-2; i >=0; i--)
		{
			vector<smallType> alpha_temp(1);
			vector<smallType> c_temp(1);
			//vector<smallType> r_temp(1);
			alpha_temp[0] = alpha[i+1];
			c_temp[0] = c[i+1];
			funcBitAnd(alpha_temp, c_temp, alpha_temp, 1);
			alpha[i+1] = alpha_temp[0];
			c[i] = alpha[i+1] ^ beta[i+1];
			u[i] = v[i] ^ r[i] ^ c[i];
		}
	}
}

void funcBitExtraction(const int64_t &a, smallType &c, size_t size) 
{
	if(HELPER)
	{
		cout<<"-------------------------test------------------------"<<endl;
		//Generate random data
		vector<smallType> r_bits(size, 0);
		vector<smallType> r1_bits(size, 0);
		vector<smallType> r2_bits(size, 0);
		vector<int64_t> s1(1, 0);
		int64_t s2 = 0;
		int64_t r = 0;
		populateRandomVector<smallType>(r1_bits, size, "r_1", "POSITIVE");
		populateRandomVector<smallType>(r2_bits, size, "r_2", "POSITIVE");
		for (size_t i = 0; i < size; i++)
		{
			r_bits[i] = r1_bits[i] ^ r2_bits[i];
		}
		int64_t base = 1;
		for (size_t i = 0; i < size-1; i++)
		{
			r += r_bits[size-1-i]*(base << i);
		}
		r += (-1)*r_bits[0]*(base << (size-1));
		populateRandomVector<int64_t>(s1, 1, "s_1", "POSITIVE");
		s2 = r - s1[0];
		//Transer data to P1 and P2
		sendVector<smallType>(r1_bits, PARTY_A, size);
		sendVector<smallType>(r2_bits, PARTY_B, size);
		sendInt64<int64_t>(s1[0], PARTY_A);
		sendInt64<int64_t>(s2, PARTY_B);
	}
	if(PRIMARY)
	{
		vector<smallType> r_bits(size, 0);
		vector<smallType> v_bits(size);
		vector<smallType> u_bits(size, 0);
		int64_t s = 0;
		int64_t t = 0;
		//Receive data
		receiveVector<smallType>(r_bits, PARTY_C, size);
		receiveInt64<int64_t>(s, PARTY_C);
		//cout<<"s: "<<s<<endl;
		t = a - s;
		if (partyNum == PARTY_B)
		{
			sendInt64<int64_t>(t, PARTY_A);
			//vector<smallType> v2_bits(LENGTH);
			receiveVector<smallType>(v_bits, PARTY_A, size);

			// thread *threads = new thread[2];
			// threads[0] = thread(sendInt64<int64_t>, ref(t), PARTY_A);
			// threads[1] = thread(receiveVector<smallType>, ref(v_bits), PARTY_A, size);
			// for (int i = 0; i < 2; i++)
			// 	threads[i].join();
			// delete[] threads;
		}
		if (partyNum == PARTY_A)
		{
			vector<smallType> temp;
			int64_t t_temp = 0;
			receiveInt64<int64_t>(t_temp, PARTY_B);
			
			int64_t v = t + t_temp;
			//cout<<"v: "<<v<<endl;
			//Calculate two's complement
			binary(v, temp, LENGTH);
			inverseCode(temp);
			complement(temp, LENGTH);
			//v_bits = temp;
			copyVectors<smallType>(temp, v_bits, size);
			//cout<<temp.size()<<endl;
			//Generate v1
			vector<smallType> v1_bits(size);
			vector<smallType> v2_bits(size);
			populateRandomVector<smallType>(v1_bits, size, "v_1", "POSITIVE");
			
			//Calculate v2
			for (size_t i = 0; i < size; i++)
			{
				v2_bits[i] = v1_bits[i] ^ v_bits[i];
			}
			//Send v2 to P2
			sendVector<smallType>(v2_bits, PARTY_B, size);
			//v_bits = v1_bits;
			copyVectors<smallType>(v1_bits, v_bits, size);
		}
		funcBitAdd(v_bits, r_bits, u_bits, size);
		c = u_bits[0];
	}
}
void funcReLU(int64_t &x, int64_t & result)
{
	if(PRIMARY)
	{
		smallType result_share;
		funcBitExtraction(x, result_share, LENGTH);
		smallType msb;
		funcReconstructBit(result_share, msb);
		//cout<<+msb<<endl;
		if (msb == 0)
		{
			result = x;
		}
		else{
			result = 0;
		}
	}
	
	
}
extern CommunicationObject commObject;
void aggregateCommunication()
{
	vector<myType> vec(4, 0), temp(4, 0);
	vec[0] = commObject.getSent();
	vec[1] = commObject.getRecv();
	vec[2] = commObject.getRoundsSent();
	vec[3] = commObject.getRoundsRecv();

	if (THREE_PC)
	{
		if (partyNum == PARTY_B or partyNum == PARTY_C)
			sendVector<myType>(vec, PARTY_A, 4);

		if (partyNum == PARTY_A)
		{
			receiveVector<myType>(temp, PARTY_B, 4);
			addVectors<myType>(vec, temp, vec, 4);
			receiveVector<myType>(temp, PARTY_C, 4);
			addVectors<myType>(vec, temp, vec, 4);
		}
	}

	if (partyNum == PARTY_A)
	{
		cout << "------------------------------------" << endl;
		cout << "Total communication: " << (float)vec[0]/1000000 << "MB (sent) and " << (float)vec[1]/1000000 << "MB (recv)\n";
		cout << "Total calls: " << vec[2] << " (sends) and " << vec[3] << " (recvs)" << endl;
		cout << "------------------------------------" << endl;
	}
}

void debugDotProduct(){
	vector<myType> a(3, 0), b(3, 0), c(3, 0);
	vector<myType> a_rec(3, 0), b_rec(3, 0), c_rec(3, 0);
	vector<myType> data = {floatToMyType(3), floatToMyType(6), floatToMyType(4)};
	//vector<myType> data = {myType(3), myType(6), myType(4)};
	funcGetShares(a, data);
	funcGetShares(b, data);

	//Reconstruct
	// funcReconstruct2PC(a, a_rec, 3, "a");
	// funcReconstruct2PC(b, b_rec, 3, "b");
	if (partyNum == PARTY_A or partyNum == PARTY_B)
	{
		funcReconstruct2PC(a, a_rec, 3, "a");
		funcReconstruct2PC(b, b_rec, 3, "b");
	}

	funcDotProductMPC(a, b, c, 3);
	//cout<<c[0]<<endl;
	if (partyNum == PARTY_A or partyNum == PARTY_B)
	{
		funcReconstruct2PC(c, c_rec, 3, "c");
	}

	
}
void debugTest(){
	int64_t a = -1;
	int64_t c;
	funcReLU(a, c);
	//cout<<a<<endl;
	cout<<c<<endl;

	// vector<smallType> v = {1, 1, 0, 1};
	// vector<smallType> r = {0, 0, 1, 1};
	// vector<smallType> u(4);
	// funcBitAdd(v, r, u, 4);

	// for (size_t i = 0; i < 4; i++)
	// {
	// 	cout<<+u[i]<<" ";
	// }

	// for (size_t i = 0; i < 4-1; i++)
	// {
	// 		r += v[4-1-i]*(1 << i);
	// }
	// r += (-1)*v[0]*(1 << (4-1));
	// cout<<r<<endl;
	// vector<smallType> a = {smallType(1), smallType(0), smallType(1), smallType(0), smallType(1)};
	// vector<smallType> b = {smallType(0), smallType(0), smallType(1), smallType(0), smallType(0)};
	// vector<smallType> c(5);
	// funcBitAnd(a, b, c, 5);
	// for (size_t i = 0; i < 5; i++)
	// {
	// 	/* code */
	// 	cout<<+c[i]<<" ";
	// }
	// int64_t data = -1011;
	// vector<smallType> data_bits;
	// binary(data, data_bits, 32);
	// printVector(data_bits);
	// inverseCode(data_bits);
	// printVector(data_bits);
	// complement(data_bits, 32);
	// printVector(data_bits);
	// int64_t r = 0;
	// int64_t base = 1;
	// for (size_t i = 0; i < 32-1; i++)
	// {
	// 	r += data_bits[32-1-i]*(base << i);
	// }
	// r += (-1)*data_bits[0]*(base << (32-1));

	// cout<<r<<endl;
	// vector<smallType> data1(LENGTH, 0);
	// vector<smallType> data2(LENGTH, 0);
	// vector<smallType> data3(LENGTH, 0);
	// populateRandomVector(data1, LENGTH, "r_1", "POSITIVE");
	// populateRandomVector(data2, LENGTH, "r_2", "POSITIVE");
	// for (size_t i = 0; i < LENGTH; i++)
	// {
	// 	data3[i] = data1[i]^data2[i];
	// }
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	cout<<+data1[i]<<" ";
	// }
	// cout<<endl;
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	cout<<+data2[i]<<" ";
	// }
	// cout<<endl;
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	cout<<+data3[i]<<" ";
	// }
	
}