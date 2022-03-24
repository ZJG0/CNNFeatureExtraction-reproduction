/*
 * @Author: ZJG
 * @Date: 2022-03-18 11:04:38
 * @LastEditors: ZJG
 * @LastEditTime: 2022-03-20 09:07:28
 */

#pragma once
#include "TedKrovetzAesNiWrapperC.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include "ParallelAESObject.h"


using namespace std;

void swapSmallTypes(smallType *a, smallType *b)
{
    smallType temp = *a;
    *a = *b;
    *b = temp;
}

void ParallelAESObject::precompute()
{
	
	for (size_t	i = 0; i < PC_CALLS_MAX*SHUFFLE_MAX*NO_CORES; ++i)
		randomNumber[i] = 1;
	for (size_t i = 0; i < PC_CALLS_MAX*NONZERO_MAX*NO_CORES; ++i)
		randomNonZero[i] = 1;
}


smallType ParallelAESObject::randNonZeroModPrime(int t, int &offset)
{
	return randomNonZero[(counterPC*NONZERO_MAX*NO_CORES + t*NONZERO_MAX + offset++)%PC_CALLS_MAX*NONZERO_MAX*NO_CORES];
}


smallType ParallelAESObject::AES_random(int i, int t, int &offset)
{
	smallType ret;

	do
	{
		ret = randomNumber[(counterPC*SHUFFLE_MAX*NO_CORES + t*SHUFFLE_MAX + offset++)%PC_CALLS_MAX*SHUFFLE_MAX*NO_CORES];
	} while (ret >= ((256/i) * i));

	return ret;
}

void ParallelAESObject::AES_random_shuffle(smallType* vec, size_t begin_offset, 
							size_t end_offset, int t, int &offset)
{
    auto n = end_offset - begin_offset;

    for (auto i = n-1; i > 0; --i)
        swapSmallTypes(&vec[begin_offset + i], &vec[begin_offset + AES_random(i+1, t, offset)]);
}
