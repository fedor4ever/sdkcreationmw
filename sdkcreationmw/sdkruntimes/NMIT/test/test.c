/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#if 0

#include <s_mem.h>
#include <s_thread.h>
#include "nmitlib.h"

/**
 * Implementation of NmitThread referenced by NMITLIB
 */
struct _NmitThread {
    ThrID id;
};

/**
 * Called by NMITLIB to create a thread on which the remote command handlers
 * will be invoked.
 */
NmitThread* NMIT_ThreadStart(NmitThreadProc proc, void* arg)
{
    NmitThread* thread = MEM_New(NmitThread);
    if (thread) {
        if (THREAD_Create(&thread->id, proc, arg)) {
            return thread;
        }
        MEM_Free(thread);
    }
    return NULL;
}

/**
 * Called by NMITLIB to destroy the thread created by NMIT_StartThread
 * and wait until the thread is finished.
 */
void NMIT_ThreadJoin(NmitThread* thread)
{
    if (thread) {
        THREAD_Join(thread->id);
        MEM_Free(thread);
    }
}

/**
 * Called by NMITLIB to destroy the thread created by NMIT_StartThread
 * without waiting until the thread is finished.
 */
void NMIT_ThreadDetach(NmitThread* thread)
{
    if (thread) {
        THREAD_Detach(thread->id);
        MEM_Free(thread);
    }
}

/**
 * Handler for the toFront command
 */
static void NMIT_API TEST_ToFront(void* aContext)
{
    printf("test: toFront\n");
}

/**
 * Handler for the load command
 */
static void NMIT_API TEST_Load(void* aContext, const NmitUcs2* aURL)
{
    printf("test: load %ls\n",aURL);
}

/**
 * Handler for the exit command
 */
static void NMIT_API TEST_Exit(void* aContext)
{
    int* stop= aContext;
    printf("test: exit\n");
    *stop = 1;
}

int main(int argc, char* argv[])
{
    int stop = 0;
    NmitInterface* nmit;
    MEM_InitModule();
    THREAD_InitModule();
    nmit = NMIT_Create("Test",TEST_Load,TEST_ToFront,TEST_Exit, &stop);
    if (nmit) {
        printf("test: waiting for the exit command\n");
        while (!stop) sleep(1);
        printf("test: deleting NMIY interface\n");
        NMIT_Delete(nmit);
    }
    THREAD_Shutdown();
    MEM_Shutdown();
    return 0;
}
#endif
