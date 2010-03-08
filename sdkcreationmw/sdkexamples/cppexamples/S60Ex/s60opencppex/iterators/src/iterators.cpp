/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


//  Include Files  

#include "iterators.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>

// This is a GCCE toolchain workaround needed when compiling with GCCE
// and using main() entry point
#ifdef __GCCE__
#include <staticlibinit_gcce.h>
#endif

using namespace std;

int usage1()
{
	cout<<"Example to demonstrate insert Iterators\n";
	vector<int> v;
	vector<int>::iterator itr;
	int i;
	for(i=0; i<5; i++)
	v.push_back(i);
	cout << "Original array: ";
	itr = v.begin();
	while(itr != v.end())
	cout << *itr++ << " ";
	cout << endl;
	itr = v.begin();
	itr += 2; // point to element 2
	// create insert_iterator to element 2
	insert_iterator<vector<int> > i_itr(v, itr);
	// insert rather than overwrite
	*i_itr++ = 100;
	*i_itr++ = 200;
	cout << "Array after insertion: ";
	itr = v.begin();
	while(itr != v.end())
	cout << *itr++ << " ";
	cout<<"\nPress a character to continue!";
	int c= getchar();
	cout<<"\n\n\n";
	return 0;
}


int usage2 () 
{
	cout<<"Example to demonstrate os_stream iterator\n";
	int ary[] = {2,5,7,2,8,9};
    ofstream ofile("TEST.DAT");
    
    // write to STDOUT
    copy(ary,ary+6,ostream_iterator<int>(cout," - "));
    // write into file "TEST.DAT"
    copy(ary,ary+6,ostream_iterator<int>(ofile,"\n"));
    ofile.close();
    cout << endl;

    ifstream ifile("TEST.DAT");
	 
    istream_iterator<int> start_file(ifile);
    istream_iterator<int> end_file;
    
    // read from file and write to STDOUT
    copy(start_file,end_file,
        ostream_iterator<int>(cout," * "));
    ifile.close();
    cout << endl;
	cout<<"\nPress a character to exit!";
	int c= getchar();
	
    return 0;
}

int main()
{
	usage1();
	usage2();
	return 0;
}