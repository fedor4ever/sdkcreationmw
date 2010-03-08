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

#include "stl.h"
#include <iostream>
#include <vector>
#include <string>

// This is a GCCE toolchain workaround needed when compiling with GCCE
// and using main() entry point
#ifdef __GCCE__
#include <staticlibinit_gcce.h>
#endif


using namespace std;

int main()
{
   vector<string> hello;

   hello.push_back("The number is 10");
   hello.push_back("The number is 20");
   hello.push_back("The number is 30");

   cout << "Loop by index:" << endl;

   int i;
   for(i=0; i < hello.size(); i++)
   {
      cout << hello[i] << endl;
   }

   cout << endl << "Constant Iterator:" << endl;

   vector<string>::const_iterator cite;
   for(cite=hello.begin(); cite!=hello.end(); cite++)
   {
      cout << *cite << endl;
   }

   cout << endl << "Reverse Iterator:" << endl;

   vector<string>::reverse_iterator rite;
   for(rite=hello.rbegin(); rite!=hello.rend(); ++rite)
   {
      cout << *rite << endl;
   }

   cout << endl << "Sample Output:"<< endl <<"Size of vector = " ;

   cout << hello.size() << endl;
   cout << hello[2] << endl;

   swap(hello[0], hello[2]);
   cout << hello[2] << endl;
   getchar();
   return 0;
}
