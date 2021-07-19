#pragma once

#include <iostream>
#include <fstream>

#include <SkelTreeData.h>
#include <SkelChainOpBase.h>
#include <SkelTreeByte.h>

NS_BEGIN

using namespace std;

struct File
{
	static void write(SkelTreeData& treeData, AniOpData& opData, CCharP fileName)
	{
		ofstream myfile(fileName, ios::out | ios::binary);
		if (myfile.is_open()) {
			Uint dataSize = SkelTreeFileByte::byteSize(treeData, opData);
			Char* data = new Char[dataSize];
			SkelTreeFileByte::toByte(treeData, opData, data);
			myfile.write((char*)data, dataSize);

			DELETE_ARRAY_POINTER(data);
		}
		myfile.close();
	}
	static void read(SkelTreeData& treeData, AniOpData& opData, CCharP fileName)
	{
		ifstream myfile(fileName, ios::in | ios::binary);
		if (myfile.is_open()) {
			myfile.seekg(0, ios::end);
			Uint dataSize = (Uint)myfile.tellg();
			Char* data = new Char[dataSize];
			memset(data, 0, dataSize);
			myfile.seekg(0, ios::beg);
			myfile.read(data, dataSize);

			SkelTreeFileByte::toObj(treeData, opData, data);
			DELETE_ARRAY_POINTER(data);
		}
		myfile.close();
	}
};

NS_END