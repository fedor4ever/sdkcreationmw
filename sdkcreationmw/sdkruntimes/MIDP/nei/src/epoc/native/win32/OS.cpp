/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#include <cstdlib.>
#include <iostream>
using namespace std;

#include <windows.h>


char dirSeparator = '\\';

int getExecutablePath(char* path, int maxPathLength)
    {
    return !::GetModuleFileName(0, path, maxPathLength);
    }


int getRegistryValue(HKEY hKeyIn, char* path, char* key, char* value, int len) 
{
	HKEY hKey;
	LONG res;
	DWORD buflen = len;
   int ret = 0;
 
	res = RegOpenKeyEx(hKeyIn, path, 0, KEY_QUERY_VALUE, &hKey);
	if( res != ERROR_SUCCESS ) 
   {
		ret = -1;
	}
   else
   { 
	   res = RegQueryValueEx( hKey, key, NULL, NULL, (LPBYTE) value, &buflen);
	   if( (res != ERROR_SUCCESS) || (buflen > (DWORD)len) ) 
      {
		   ret = -1;
	   }
   }

	RegCloseKey( hKey );

	return ret;
}

int getJavaRuntime(char* vmpath, int pathlen)
{
   char current_version[256];
	char regpath[1024];
	int ret = 0;
 
	ret = getRegistryValue(HKEY_LOCAL_MACHINE,"SOFTWARE\\JavaSoft\\Java Runtime Environment", 
			"CurrentVersion", current_version, sizeof(current_version));
	if(ret == 0) 
   {
	   wsprintf(regpath, "SOFTWARE\\JavaSoft\\Java Runtime Environment\\%s", current_version);
   	ret = getRegistryValue(HKEY_LOCAL_MACHINE, regpath, "RuntimeLib", vmpath, pathlen);
	}
 
   return ret;
}


void* getJavaVMCreateFn(char *vmpath)
 {
     HINSTANCE vm = LoadLibrary(vmpath);
     if (vm == NULL) {
         return NULL;
     }
     return GetProcAddress(vm, "JNI_CreateJavaVM");
 }

void reportError(char *error)
{
	::printf(error);
	::printf("\n");
	char* errorMsg = reinterpret_cast<char*>(::malloc(4096));
	::sprintf(errorMsg, "%s\nPlease check your SDK installation", error);
	MessageBox (NULL, errorMsg , "Symbian SDK", 0);
}


