/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test utility
 *
*/


#include "s_lib.h"
#include "bt_ioctl.h"
#include "vid.h"

static char pname[] = "sbt";
static char BtControlDeviceName[] = "\\\\.\\HciUsbControl";

/**
 * Opens a handle to the driver's control device object.
 * Prints error message in case of failure, which usually indicates
 * that driver is not loaded.
 */
STATIC HANDLE SBT_OpenDriver()
{
    HANDLE hDriver = CreateFile(BtControlDeviceName, 
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (hDriver == INVALID_HANDLE_VALUE) {
        PRINT_Verbose("%s: failed to open %s, error %d\n",pname,
            BtControlDeviceName, GetLastError());
        PRINT_Error("%s: driver is not loaded\n",pname);
    } else {
        BtVersion v;
        ULONG nbytes = 0;
        ZeroMemory(&v, sizeof(v));
        if (DeviceIoControl(hDriver, IOCTL_S60BT_VERSION, NULL, 0, 
            &v, sizeof(v), &nbytes, NULL)) {
            ASSERT(nbytes == sizeof(v));
            PRINT_Verbose("%s: driver version %d.%d.%d\n",pname,
                v.major, v.minor, v.micro);
        } else {
            PRINT_Error("%s: failed to query driver version\n",pname);
        }
    }
    return hDriver;
}

/**
 * Lists the ports created by the driver and their status.
 */
STATIC void SBT_ListPorts(HANDLE hDriver)
{
    ULONG nbytes = 0;
    BtPortList* list = MEM_New(BtPortList);
    if (list) {
        ZeroMemory(list, sizeof(*list));
        if (DeviceIoControl(hDriver, IOCTL_S60BT_GET_PORT_LIST, NULL, 0, 
            list, sizeof(*list), &nbytes, NULL)) {

            BtUint32 i;
            while (list && list->totalCount > list->infoCount) {
                int n = list->totalCount;
                int size = sizeof(BtPortList) + n*sizeof(BtPortInfo);
                MEM_Free(list);
                list = MEM_Alloc(size);
                if (list) {
                    ZeroMemory(list, sizeof(BtPortList));
                    nbytes = 0;
                    DeviceIoControl(hDriver, IOCTL_S60BT_GET_PORT_LIST, NULL, 0, 
                        list, size, &nbytes, NULL);
                }
            }
            
            if (list) {
                PRINT_Output("%s: found %d port%s\n", pname, list->totalCount,
                    ((list->totalCount % 10) == 1)? "" : "s");
                for (i=0; i<list->totalCount; i++) {
                    BtPortInfo* p = ((BtPortInfo*)(list+1)) + i;
                    PRINT_Output("COM%d: %savailable\n", p->portNum, 
                        p->openCount ? "not " : "");
                    if (PRINT_GetMask() & TRACE_VERBOSE) {
                        Str vendorName = VID_GetVendorName(p->vid);
                        if (vendorName) {
                            PRINT_Verbose("  USB Vendor ID:  0x%04X (%s)\n",
                                p->vid, vendorName);
                        } else {
                            PRINT_Verbose("  USB Vendor ID:  0x%04X\n",
                                p->vid);
                        }
                        PRINT_Verbose("  USB Product ID: 0x%04X\n", p->pid);
                        if (p->manufacturer[0]) {
                            PRINT_Verbose("  Manufacturer:   %ls\n", 
                                p->manufacturer);
                        }
                        if (p->productName[0]) {
                            PRINT_Verbose("  Product name:   %ls\n", 
                                p->productName);
                        }
                        if (p->serialNumber[0]) {
                            PRINT_Verbose("  Serial number:  %ls\n",
                                p->serialNumber);
                        }
                    }
                }
                MEM_Free(list);
            }

        } else {
            PRINT_Verbose("%s: DeviceIoControl error %d\n",pname,
                GetLastError());
            PRINT_Error("%s: failed to query port list\n",pname);
        }
    }
}

/**
 * The program entry point.
 */
int main(int argc, char* argv[])
{
    CmdLine * c;
    SLIB_InitModules();

    /* Parse the comment line */
    c = CMDLINE_Create(pname);
    if (c) {
        Bool help = False;
        Bool verbose = False;
        CMDLINE_AddTrueOpt(c,'h',"help","print this help and exit",&help);
        CMDLINE_AddTrueOpt(c,'v',"verbose","produce verbose output",&verbose);
        CMDLINE_SetMaxArgs(c, 0);
        CMDLINE_SetApp(c, pname);
        if (CMDLINE_Parse(c, argv+1, argc-1, PARSE_NO_DUP, NULL) && !help) {
            HANDLE hDriver;

            /* Set TRACE_ALL flags if verbose mode is selected */
            if (verbose) PRINT_SetMask(TRACE_ALL);

            /* Open the driver */
            hDriver = SBT_OpenDriver();
            if (hDriver != INVALID_HANDLE_VALUE) {
                SBT_ListPorts(hDriver);
                CloseHandle(hDriver);
            }
        } else {

            /* Print the usage summary. */
            CMDLINE_Usage(c, NULL, 0);
        }
    }
    SLIB_Shutdown();
}
