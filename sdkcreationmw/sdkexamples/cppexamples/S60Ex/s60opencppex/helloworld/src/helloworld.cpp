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

#include "helloworld.h"
#include <iostream>
#include <cstring>


// This is a GCCE toolchain workaround needed when compiling with GCCE
// and using main() entry point
#ifdef __GCCE__
#include <staticlibinit_gcce.h>
#endif

using namespace std;

class myclass {
public:
  void show(){cout<<"Hello World\n"; }
} ;

int main()
{
  myclass obj;
  obj.show();
  cout<<"Press a character to exit!";
  int c = getchar();
  return 0;
}
