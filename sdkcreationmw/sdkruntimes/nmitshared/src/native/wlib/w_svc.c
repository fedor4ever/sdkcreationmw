/*
 * $Id: w_svc.c,v 1.2 2005/01/08 05:11:28 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "w_lib.h"

#ifndef _WIN32_WCE

/**
 * Note that all functions take NULL as a handle to the service control
 * manager, in which case they open a temporary connection to the SCM
 * and close it. The caller only needs to open its own connection if it
 * does many service control operations that can reuse the same handle.
 */

#define SC_LOCAL_SYSTEM         TEXT("LocalSystem")
#define SC_DATABASE             SERVICES_ACTIVE_DATABASE
#define SERVICE_READ_ACCESS    (STANDARD_RIGHTS_REQUIRED     | \
                                SERVICE_QUERY_CONFIG         | \
                                SERVICE_QUERY_STATUS         | \
                                SERVICE_ENUMERATE_DEPENDENTS)

/**
 * Installs the service. If the service already exists, makes sure that
 * it's associated with the right executable. Otherwise, creates a new
 * service.
 */
Bool SERVICE_Install(SC_HANDLE hScMgr, const NT_SERVICE * svc)
{
    Bool ok = False;
    SC_HANDLE hService = NULL;
    SC_HANDLE hScMgrTmp = NULL;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
    
#ifdef _USE_EXCEPTION_HANDLING
    TRY_FINALLY
#endif /*_USE_EXCEPTION_HANDLING */
    {
        /* check if the service is already installed */
        ASSERT(svc->lpServiceName);
        hService = OpenService(hScMgr,svc->lpServiceName,SERVICE_ALL_ACCESS);

        if (hService) {
            /* if it's already installed, configure the existing one */
            ok = SERVICE_Configure(hService, svc);
        } else {
            /* create a new service */
            ok = SERVICE_Create(hScMgr, svc);
        }
    } 
#ifdef _USE_EXCEPTION_HANDLING
    FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        ULONG lastError = GetLastError();
        if (hService) CloseServiceHandle(hService);
        if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
        SetLastError(lastError);
    }

    return ok;
}

/**
 * Deletes the service. 
 */
Bool SERVICE_Delete(SC_HANDLE hScMgr, LPCTSTR szService)
{
    Bool ok = False;
    SC_HANDLE hService = NULL;
    SC_HANDLE hScMgrTmp = NULL;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
    
#ifdef _USE_EXCEPTION_HANDLING
    TRY_FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        /* check if the service is already installed */
        hService = OpenService(hScMgr, szService, SERVICE_ALL_ACCESS);
        if (hService) {
            ok = DeleteService(hService);
            if (!ok && GetLastError() == ERROR_SERVICE_MARKED_FOR_DELETE) {
                SetLastError(NO_ERROR);
                ok = True;
            }
        } else if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST) {
            SetLastError(NO_ERROR);
            ok = True;
        }
    } 
#ifdef _USE_EXCEPTION_HANDLING
    FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        ULONG lastError = GetLastError();
        if (hService) CloseServiceHandle(hService);
        if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
        SetLastError(lastError);
    }

    return ok;
}

/**
 * Shortcut to ChangeServiceConfig. 
 */
Bool SERVICE_Configure(SC_HANDLE hService, const NT_SERVICE * service)
{
    LPCTSTR account = ((service->dwServiceType & (SERVICE_KERNEL_DRIVER | 
        SERVICE_FILE_SYSTEM_DRIVER)) ? NULL : SC_LOCAL_SYSTEM);
    if (ChangeServiceConfig(hService, service->dwServiceType, 
        service->dwStartType, SERVICE_ERROR_NORMAL, service->lpExeName,                 // service's binary
        NULL, NULL, service->lpDependencies, account, NULL, 
        service->lpDisplayName)) {
        return True;
    } else {
        TRACE2("failed to config %s, error %lu\n", service->lpServiceName,
            GetLastError());
        return False;
    }
}

/**
 * Registers a Windows NT service or kernel mode driver.
 */
Bool SERVICE_Create(SC_HANDLE hScMgr, const NT_SERVICE * service)
{
    Bool ok = False;
    SC_HANDLE hService = NULL;
    SC_HANDLE hScMgrTmp = NULL;
    LPCTSTR account = ((service->dwServiceType & (SERVICE_KERNEL_DRIVER | 
        SERVICE_FILE_SYSTEM_DRIVER)) ? NULL : SC_LOCAL_SYSTEM);

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
    
#ifdef _USE_EXCEPTION_HANDLING
    TRY_FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        hService = CreateService(hScMgr, service->lpServiceName,
            service->lpDisplayName, SERVICE_ALL_ACCESS, 
            service->dwServiceType, service->dwStartType, 
            SERVICE_ERROR_NORMAL, service->lpExeName, NULL,
            NULL, service->lpDependencies, NULL, NULL);

        /* check the return value. Ignore ERROR_SERVICE_EXISTS */
        if (hService) {
            TRACE1("%s installed successfully\n",service->lpServiceName);
            SetLastError(NO_ERROR);
            ok = True;
        } else if ( GetLastError() == ERROR_SERVICE_EXISTS) {
            TRACE1("%s already installed\n",service->lpServiceName);
            SetLastError(NO_ERROR);
            ok = True;
        } else {
            TRACE2("failed to install %s, error %lu\n", 
                service->lpServiceName,GetLastError());
        }
    } 
#ifdef _USE_EXCEPTION_HANDLING
    FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        ULONG lastError = GetLastError();
        if (hService) CloseServiceHandle(hService);
        if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
        SetLastError(lastError);
    }

    return ok;
}

/**
 * Starts the service
 */
Bool SERVICE_Start(SC_HANDLE hScMgr, LPCTSTR szService)
{
    ULONG status;
    SC_HANDLE hScMgrTmp = NULL;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
        
    /* get the current status. If already running, why bother */
    status = SERVICE_State(hScMgr, szService);    
    if (status == SERVICE_RUNNING) {
        TRACE1("%s is running\n",szService);
        SetLastError(NO_ERROR);
        return True;
    } else {
        SC_HANDLE hService = NULL;
        Bool ok = False;
#ifdef _USE_EXCEPTION_HANDLING
        TRY_FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
        {
            TRACE1("starting %s\n", szService);
            hService = OpenService(hScMgr, szService, SERVICE_ALL_ACCESS);
            if (hService) {
                if (StartService(hService, 0, NULL)) {
                    SetLastError(NO_ERROR);
                    ok = True;
                } else {
                    /* If service is already running, it's not an error. */
                    if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING) {
                        TRACE1("%s is already running\n", szService);
                        SetLastError(NO_ERROR);
                        ok = TRUE;
                    } else {
                        TRACE2("failed to start %s, error %lu\n",szService,
                            GetLastError());
                    }
                }
            } else {
                TRACE2("failed to open %s, error %lu\n",szService,
                    GetLastError());
            }
        } 
#ifdef _USE_EXCEPTION_HANDLING
        FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
        {
            ULONG lastError = GetLastError();
            if (hService) CloseServiceHandle(hService);
            if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
            SetLastError(lastError);
        }
        return ok;
    }
}

/**
 * Sends control code to the service
 */
Bool SERVICE_Control(SC_HANDLE hScMgr, LPCTSTR szService, ULONG nCode)
{
    Bool ok = False;
    SC_HANDLE hService = NULL;
    SC_HANDLE hScMgrTmp = NULL;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
        
#ifdef _USE_EXCEPTION_HANDLING
    TRY_FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        hService = OpenService(hScMgr, szService, SERVICE_ALL_ACCESS);
        if (hService) {
            SERVICE_STATUS serviceStatus;
            if (ControlService(hService, nCode, &serviceStatus)) {
                SetLastError(NO_ERROR);
                ok = True;
            } else {
                TRACE2("failed to control %s, error %lu\n",szService,
                    GetLastError());
            }
        } else {
            TRACE2("failed to open %s, error %lu\n",szService,GetLastError());
        }
    } 
#ifdef _USE_EXCEPTION_HANDLING
    FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        ULONG lastError = GetLastError();
        if (hService) CloseServiceHandle(hService);
        if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
        SetLastError(lastError);
    }
    return ok;
}

/**
 * Returns the service state, zero in case of error
 */
ULONG SERVICE_State(SC_HANDLE hScMgr, LPCTSTR szService)
{
    ULONG status = 0;
    SC_HANDLE hService = NULL;
    SC_HANDLE hScMgrTmp = NULL;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        hScMgrTmp = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgrTmp);
        hScMgr = hScMgrTmp;
    }
        
#ifdef _USE_EXCEPTION_HANDLING
    TRY_FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        hService = OpenService(hScMgr, szService, SERVICE_READ_ACCESS);
        if (hService) {
            SERVICE_STATUS serviceStatus;
            if (QueryServiceStatus(hService, &serviceStatus)) {
                status = serviceStatus.dwCurrentState;
                TRACE2("%s: status %lu\n",szService,status);
                SetLastError(NO_ERROR);
            } else {
                TRACE2("failed to get status for %s, error %lu\n",
                    szService,GetLastError());
            }
        } else {
            TRACE2("failed to open %s, error %lu\n",szService,GetLastError());
        }
    } 
#ifdef _USE_EXCEPTION_HANDLING
    FINALLY 
#endif /*_USE_EXCEPTION_HANDLING */
    {
        ULONG lastError = GetLastError();
        if (hService) CloseServiceHandle(hService);
        if (hScMgrTmp) CloseServiceHandle(hScMgrTmp);
        SetLastError(lastError);
    }
    return status;
}


/**
 * Stops the service
 */
Bool SERVICE_Stop(SC_HANDLE hScMgr, LPCTSTR szService)
{
    int i;
    int nAttempts = 60;
    ULONG timeout = 500;

    /* open temporary connection to the service control manager */
    if (!hScMgr) {
        Bool ok = False;
        hScMgr = OpenSCManager(NULL, SC_DATABASE, SC_MANAGER_ALL_ACCESS);
        ASSERT(hScMgr);
        if (hScMgr) {
            ok = SERVICE_Stop(hScMgr, szService);
            CloseServiceHandle(hScMgr);
        }
        return ok;
    }
        
    for (i=0; i<nAttempts; i++) {
        ULONG error;
        ULONG status = SERVICE_State( hScMgr, szService );
        if (status == SERVICE_STOPPED) {
            TRACE1("%s stopped\n",szService);
            SetLastError(NO_ERROR);
            return True;
        }

        /* looks like it's running */
        TRACE1("stopping %s\n",szService); 
        if (SERVICE_Control(hScMgr, szService, SERVICE_CONTROL_STOP)) {
            status = SERVICE_State(hScMgr, szService);
            if (status == SERVICE_STOPPED) {
                TRACE1("%s has been stopped\n",szService);
                SetLastError(NO_ERROR);
                return True;
            }

            if (i < (nAttempts-1)) {
                TRACE1("%s not stopped yet, keep trying\n",szService);
                Sleep(timeout);
                continue;
            }
        }

        /* check the error code */
        error = GetLastError();
        switch (error) {
        /*
         * The service cannot accept control messages at this time.
         * It could be in the process of stopping or something
         */
        case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
            TRACE1("%s is not accepting STOP. keep trying...\n",szService);
            Sleep(timeout);
            continue;

        /* If dependent service is running, give it some time to exit. */
        case ERROR_DEPENDENT_SERVICES_RUNNING:
            if (i < (nAttempts-1)) {
                TRACE("dependent service is running, wait a second\n");
                Sleep(timeout);
            }
            continue;

        /* 
         * ERROR_SERVICE_NOT_ACTIVE probably means that it's not
         * running. Treat it as a success
         */
        case ERROR_SERVICE_NOT_ACTIVE:
            TRACE1("%s is not running\n",szService);
            SetLastError( NO_ERROR );
            return True;

        /*
         * ERROR_INVALID_SERVICE_CONTROL usually means that the service 
         * cannot be stopped. Consider it to be error
         */
        case ERROR_INVALID_SERVICE_CONTROL:
            status = SERVICE_State(hScMgr, szService);
            if (status == SERVICE_STOPPED) {
                TRACE1("%s has died\n",szService);
                SetLastError(NO_ERROR);
                return True;
            }
            /* fall through */

        /* everything else is considered to be a REAL error */
        default:
            if (GetLastError() == NO_ERROR) { 
                SetLastError(ERROR_INVALID_SERVICE_CONTROL);
            }
            return False;
        }
    }

    /* give up and return to the caller. Make sure we return an error */
    TRACE1("gave up on %s\n",szService);
    if (GetLastError() == NO_ERROR) { 
        SetLastError(ERROR_INVALID_SERVICE_CONTROL);
    }
    return False;
}

#endif /* !_WIN32_WCE */
