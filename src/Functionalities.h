/*
 * @Author: ZJG
 * @Date: 2022-03-18 11:01:20
 * @LastEditors: ZJG
 * @LastEditTime: 2022-03-24 11:16:50
 */
#pragma once
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;

extern void start_time();
extern void start_communication();
extern void end_time(string str);
extern void end_communication(string str);

void funcReconstruct2PC(const vector<myType> &a, size_t size, string str);
void funcReconstructBit(const smallType &a, smallType &b);
void funcGetShares(vector<myType> &a, const vector<myType> &data);
void funcTruncate2PC(vector<myType> &a, size_t power, size_t size, size_t party_1, size_t party_2);
void funcDotProductMPC(const vector<myType> &a, const vector<myType> &b, 
					   vector<myType> &c, size_t size);
void funcBitAnd(const vector<smallType> &x, const vector<smallType> &y, 
						   vector<smallType> &z, size_t size);
void funcBitAdd(const vector<smallType> &v, const vector<smallType> &r, vector<smallType> &u, size_t size);
void funcBitExtraction(const int64_t &a, smallType &c, size_t size);
void funcReLU(int64_t &x, int64_t & result);
void debugDotProduct();
void debugTest();