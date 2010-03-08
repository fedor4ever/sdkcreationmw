/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


/* The whole file is Win32 specific */
#ifdef _WIN32
#include <windows.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <errno.h>
#include "com_nokia_wap_sdk_util_HelperProcess.h"
#include "java_util.h"

#define ERR_MSG_LEN  80
static char gErrMsg[ERR_MSG_LEN];
static char extraDir[] = "lib\\i386";

/*
 * Class:     com.nokia.wap.sdk.util.HelperProcess
 * Method:    cCreateProcess
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 *
 * Creates a new process.  Parameters:
 *	jExecName: exececutable
 *	jExecDir: directory to find it
 *	
 *	Returns: Process Handle used later to terminate process.
 */
JNIEXPORT jint JNICALL Java_com_nokia_wap_sdk_util_HelperProcess_cCreateProcess (
	JNIEnv *env, 
	jobject obj, 
	jstring jExeName,
	jstring jExeDir)
{
	BOOL callSucceeded = FALSE;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION proc_info;
	LPVOID lpMsgBuf;
	HANDLE rtn_handle = 0;
	char *exeName = 0;
	char *exeDir = 0;

	JNI_DEBUG("In cCreateProc\n");

	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	memset(gErrMsg, 0, ERR_MSG_LEN);

	if (jExeName != 0) {
		exeName = (char *)(*env)->GetStringUTFChars(env, jExeName, 0);
	} 
	if (jExeDir != 0)
		exeDir  = (char *)(*env)->GetStringUTFChars(env, jExeDir, 0);
		
	JNI_DEBUG_ARG1("cCreateProc name: %s\n", exeName);
	JNI_DEBUG_ARG1("cCreateProc dir: %s\n", (jExeDir == 0 ? "<cur dir>" : exeDir));

	/**
	 * Append bin directory to PATH
	 */
	{
	char *curPath = 0;
	char *newPath = 0;
	DWORD bufsz;
	unsigned int i;
	char *frp, *top;

	bufsz = GetEnvironmentVariable("PATH", curPath, 0);	// find req buf len
	curPath = (char *)malloc(bufsz + 1);			// alloc buf
	bufsz = GetEnvironmentVariable("PATH", curPath, bufsz + 1);	// get cur path value
	newPath = (char *)malloc(bufsz + sizeof(extraDir) + 2);		// alloc new buf
	top = newPath, frp = extraDir;				// set pointers
	for (i = 0; i < strlen(extraDir); i++)			// copy new dir
		*top++ = *frp++;
	*top++ = ';';					// path separator
	frp = curPath;					//
	for (i = 0; i < bufsz; i++)
		*top++ = *frp++;
	*top = '\0';					// NULL terminate
	SetEnvironmentVariable("PATH", newPath);	// set cur path value
	JNI_DEBUG_ARG1("cCreateProc set new PATH to be %s", newPath);
	free(curPath);
	free(newPath);
	}
	
	callSucceeded = CreateProcess(
		NULL,	// (LPCTSTR) exeName, 
		exeName, // commandline
		NULL, // std. process security
		NULL, // std. thread security
		FALSE, // do not inherit handles from this proc
		CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP,
		NULL, // std. environment
		exeDir, // if NULL, uses current dir
		&startupInfo, // window info (all zero since console app.)
		&proc_info);

	//
	// create the error msg string if necessary
	//
	if (callSucceeded == FALSE)  
	{
		FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |     
			FORMAT_MESSAGE_IGNORE_INSERTS,    
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,    
			0,    
			NULL );

		strcpy(gErrMsg, (LPCTSTR)lpMsgBuf);

		JNI_DEBUG_ARG1("cCreateProc failed: %s\n", gErrMsg);

		// Free the buffer.
		LocalFree( lpMsgBuf );
		
		return -1;
	} else {
		JNI_DEBUG_ARG1("cCreateProc succeeded: id: %d", 
			proc_info.dwProcessId);
		JNI_DEBUG_ARG1(" hdl: %d\n", proc_info.hProcess);
	}
	//
	// Return the proces handle used to kill the process later
	//
	// return (jint)proc_info.dwProcessId;	// used to send a signal to
	return (jint)proc_info.hProcess;	// used in TerminateProcess
}

/*
 * Class:     com.nokia.wap.sdk.util.HelperProcess
 * Method:    cTerminateProcess
 * Signature: (I)I
 *
 * Creates a new process.  Parameters:
 *	proc_handle: process handle of process to kill.
 *	
 *	Returns: TRUE on success, FALSE on failure.
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_wap_sdk_util_HelperProcess_cTerminateProcess(
	JNIEnv *env, 
	jobject obj,
	jint proc_handle)
{
	BOOL callSucceeded = FALSE;
	JNI_DEBUG("In cTermProc\n");

	callSucceeded = TerminateProcess((HANDLE)proc_handle, 0);
	// callSucceeded = GenerateConsoleCtrlEvent( CTRL_C_EVENT, (DWORD)proc_handle);

	if (callSucceeded == FALSE)
	{
		LPVOID lpMsgBuf;

		JNI_DEBUG_ARG1("cTermProc: terminating proc %d failed\n", proc_handle);
		FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |     
			FORMAT_MESSAGE_IGNORE_INSERTS,    
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,    
			0,    
			NULL );

		memset(gErrMsg, 0, ERR_MSG_LEN);
		strcpy(gErrMsg, (LPCTSTR)lpMsgBuf);

		JNI_DEBUG_ARG1("cTermProc failed: %s\n", gErrMsg);

		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	else {
		memset(gErrMsg, 0, ERR_MSG_LEN);
		strcpy(gErrMsg, "destroyProcess succeeded");
		JNI_DEBUG("cTermProc succeeded\n");
	}
	return (callSucceeded)? JNI_TRUE : JNI_FALSE;
}
#endif /* _WIN32 */
