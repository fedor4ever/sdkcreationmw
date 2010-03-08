/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#include "EcmtGwUI.h"
#include "GwEngine.h"

STATIC const Str SERIALCOMM_KEY = "HARDWARE\\DEVICEMAP\\SERIALCOMM";

/***************************************************************************
 * Sorts the list items
 ***************************************************************************/
STATIC int CALLBACK GUI_PortSort(LPARAM lp1, LPARAM lp2, LPARAM ctx)
{
    EcmtGatewayPort* port1 = ((EcmtGwPort*)lp1)->port;
    EcmtGatewayPort* port2 = ((EcmtGwPort*)lp2)->port;
    EcmtGwPortType type1 = ECMTGW_GetPortType(port1);
    EcmtGwPortType type2 = ECMTGW_GetPortType(port2);
    Str p1, p2;
    if (type1 > type2) return 1;
    if (type2 > type1) return -1;
    p1 = ECMTGW_GetPortName(port1);
    p2 = ECMTGW_GetPortName(port2);
    while (*p1 == *p2 && *p1 && *p2) p1++, p2++;
    if (!*p1 && !*p2) return 0;     /* equal strings */
    if (type1 == EcmtGwPortSerial) { /* special case for COM port names */
        int len1 = strlen(p1);      /* length of the remaining part #1 */
        int len2 = strlen(p2);      /* length of the remaining part #2 */
        if (len2 > len1) return -1; /* #2 is longer */
        if (len1 > len2) return 1;  /* #1 is longer */
    }
    return (*p1 - *p2);
}

/***************************************************************************
 * Window proc for the port list. Forwards WM_COMMAND messages (translated
 * by TranslateAccelerator) to the main window.
 ***************************************************************************/
STATIC LRESULT CALLBACK GUI_ListWndProc(HWND wnd, UINT m, WPARAM w, LPARAM l)
{
    EcmtGwUI* ui = (EcmtGwUI*)GetWindowLong(wnd, GWL_USERDATA);
    ASSERT(ui);
    if (ui) {
        if (m == WM_COMMAND) {
            SendMessage(ui->mainWindow, m, w, l);

        /* Windows messages keep coming to the ListView control even if
         * it's not visible. I don't know why, perhaps this has something
         * to do with the fact that we use dialog as a child window. We need
         * to ignore all keyboard messages in such case, so that the port 
         * selection does not change when we are in the "connected" state.
         */
        } else if (m < WM_KEYFIRST || m > WM_KEYLAST || IsWindowVisible(wnd)) {
            return CallWindowProc(ui->listWndProc, wnd, m, w, l);
        }
    }
    return 0;
}

/***************************************************************************
 * Returns the port icon index in the image list of the list view control.
 ***************************************************************************/
STATIC int GUI_DlgPortIconIndex(EcmtGwUI* ui, EcmtGwPort* port)
{
    switch (ECMTGW_GetPortType(port->port)) {
    case EcmtGwPortBtSocket: return ui->btPortIconIndex;
    case EcmtGwPortSerial: return ui->comPortIconIndex;
	case EcmtGwPortSocket: return ui->wlanPortIconIndex;
    default: return ui->otherPortIconIndex;
    }
}

/***************************************************************************
 * Returns the handle to the port icon
 ***************************************************************************/
STATIC HICON GUI_DlgPortIcon(EcmtGwUI* ui, EcmtGwPort* port)
{
    switch (ECMTGW_GetPortType(port->port)) {
    case EcmtGwPortBtSocket: return ui->btPortIcon;
    case EcmtGwPortSerial: return ui->comPortIcon;
	case EcmtGwPortSocket: return ui->lanPortIcon;
    default: return ui->otherPortIcon;
    }
}

/***************************************************************************
 * Updates port name and icon controls in the dialog
 ***************************************************************************/
STATIC void GUI_DlgUpdatePortInfo(EcmtGwUI* ui, EcmtGwPort* port)
{
    Str portName = "";
    HICON hPortIcon = ui->emptyIcon;
    HWND hIconCtl = GetDlgItem(ui->mainDialog, IDC_PORT_ICON);

    ui->port = port;
    if (port) {
        hPortIcon = GUI_DlgPortIcon(ui, port);
        portName = ECMTGW_GetPortName(port->port);
    }

    /* update the dialog controls */
    SetDlgItemText(ui->mainDialog,IDC_PORT_NAME, portName);
    SetDlgItemText(ui->mainDialog,IDC_PORT_NAME2, portName);
    Static_SetIcon(hIconCtl, hPortIcon);
}

/***************************************************************************
 * Creates a list item for the specified port at specified position.
 * If position is negative, adds new item to the end of the list.
 ***************************************************************************/
void GUI_DlgAddPort(EcmtGwUI* ui, EcmtGwPort* port, int i)
{
    LVITEM item;
    ZeroMemory(&item, sizeof(item));
    QUEUE_InsertTail(&ui->ports, &port->entry);

    /* insert the list item */
    item.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
    item.iItem = (i >= 0) ? i : ListView_GetItemCount(ui->listView);
    item.iSubItem = 0;
    item.pszText = (LPSTR)ECMTGW_GetPortName(port->port);
    item.lParam = (LPARAM)port;
    item.iImage = GUI_DlgPortIconIndex(ui, port);
    item.iItem = ListView_InsertItem(ui->listView, &item);

    item.mask = LVIF_TEXT;
    item.iSubItem = 1;
    item.pszText = (LPSTR)ECMTGW_GetPortComment(port->port);
    ListView_SetItem(ui->listView, &item);
}

/***************************************************************************
 * Updates port indices in the list view
 ***************************************************************************/
STATIC void GUI_DlgUpdatePortIndices(EcmtGwUI* ui)
{
    int i, n = ListView_GetItemCount(ui->listView);
    for (i=0; i<n; i++) {
        LVITEM item;
        ZeroMemory(&item, sizeof(item));
        item.mask = LVIF_PARAM;
        item.iItem = i;
        item.lParam = 0;
        if (ListView_GetItem(ui->listView, &item) && item.lParam) {
            EcmtGwPort* port = (EcmtGwPort*)item.lParam;
            port->iItem = item.iItem;
        }
    }
}

/***************************************************************************
 * Sorts the port list
 ***************************************************************************/
STATIC void GUI_DlgSortPortList(EcmtGwUI* ui)
{
    ListView_SortItems(ui->listView, GUI_PortSort, 0);
    GUI_DlgUpdatePortIndices(ui);

    /* make sure that selected item is still visible */
    if (ui->port) {
        ListView_EnsureVisible(ui->listView, ui->port->iItem, FALSE);
    }
}

/***************************************************************************
 * Handles WM_INITDIALOG message. Returns True if the window has been created
 * successfully, False on failure
 ***************************************************************************/
STATIC void GUI_DlgInit(HWND hDlg, EcmtGwUI* ui)
{
    RECT clientRect;
    LVCOLUMN col;
    StrBuf sb;
    HANDLE hInst = (HANDLE)GetWindowLong(hDlg, GWL_HINSTANCE);
    HIMAGELIST imgList;
    STRBUF_Init(&sb);

    /* get handle to the port list */
    ASSERT(!ui->mainDialog);
    ui->mainDialog = hDlg;
    ui->listView = GetDlgItem(hDlg, IDC_PORT_LIST);
    ASSERT(ui->listView);

    ListView_SetExtendedListViewStyleEx(ui->listView,
        LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    GetClientRect(ui->listView, &clientRect);
    ZeroMemory(&col, sizeof(col));
    col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    col.fmt = LVCFMT_LEFT;
    col.iSubItem = 0;
    col.cx = 80;
    WIN32_LoadString(hInst, IDS_HEADER_PORT_NAME, &sb);
    col.pszText = (LPSTR)STRBUF_Text(&sb);
    ListView_InsertColumn(ui->listView, col.iSubItem, &col);
    col.iSubItem++;
    col.cx = clientRect.right - col.cx - GetSystemMetrics(SM_CXVSCROLL);
    WIN32_LoadString(hInst, IDS_HEADER_PORT_COMMENT, &sb);
    col.pszText = (LPSTR)STRBUF_Text(&sb);
    ListView_InsertColumn(ui->listView, col.iSubItem, &col);

    /* Load icons */
    ui->emptyIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EMPTY));
    ui->btPortIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BLUETOOTH));
    ui->comPortIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_COMPORT));
	ui->lanPortIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WLAN));
    ui->otherPortIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_UNKNOWN));

    /* Create image list, it will be destroyed by the list view control */ 	 
    imgList = ImageList_Create(16,16,ILC_MASK,2,0);
    ASSERT(imgList); 	 
    if (imgList) {
        ui->btPortIconIndex = ImageList_AddIcon(imgList,ui->btPortIcon);
        ui->comPortIconIndex = ImageList_AddIcon(imgList,ui->comPortIcon);
		ui->wlanPortIconIndex = ImageList_AddIcon(imgList,ui->lanPortIcon);
        ui->otherPortIconIndex = ImageList_AddIcon(imgList,ui->otherPortIcon);
        ListView_SetImageList(ui->listView, imgList, LVSIL_SMALL );
    }

    /* associate pointer with the window */
    SetLastError(NO_ERROR);
    SetWindowLong(hDlg, DWL_USER, (LONG)ui);

    /* collect port information */
    ECMTGW_RefreshPorts(ui->engine);

    /* sort the items */
    GUI_DlgSortPortList(ui);

    /* subclass the port list */
    ui->listWndProc = (WNDPROC)GetWindowLong(ui->listView, GWL_WNDPROC);
    SetWindowLong(ui->listView, GWL_USERDATA, (LONG)ui);
    SetWindowLong(ui->listView, GWL_WNDPROC, (LONG)GUI_ListWndProc);

    /* cleanup */
    STRBUF_Destroy(&sb);
}


STATIC BOOL CALLBACK GUI_wlansetDlgProc(HWND hwndDlg, UINT message, 
                             WPARAM wParam, LPARAM lParam)
{ 
	char wlanIP[20];
	int wlanPort;
	BOOL portReadOk;
	char* ip = (char*)malloc(IP_ADDR_LEN*sizeof(char));
	int* port = malloc(sizeof(int));


    switch (message) 
    {	case WM_INITDIALOG:
			//Read values from file (call engine's fn)
			if( ECMTGW_GetWlanIpPort(ip, port) != EcmtTrue )
			{	

				free(ip);
				free(port);
				SetDlgItemText(hwndDlg, IDC_WLANIPADDRESS, "127.0.0.1");
				SetDlgItemInt(hwndDlg, IDC_WLANPORT, 49359, FALSE);

			}
			else
			{
				SetDlgItemText(hwndDlg, IDC_WLANIPADDRESS, ip);
				SetDlgItemInt(hwndDlg, IDC_WLANPORT, *port, FALSE);
			}
		
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK:
				{
					HWND parentDlg = GetParent(hwndDlg);
					//HWND owner = GetWindow(hwndDlg, GW_OWNER);

					EcmtGwUI* ui = (EcmtGwUI*)GetWindowLong(parentDlg, GWL_USERDATA);

                    if (GetDlgItemText(hwndDlg, IDC_WLANIPADDRESS, wlanIP, 20))
					TRACE1("WLAN IP is set: %s", wlanIP);	
					wlanPort = GetDlgItemInt(hwndDlg, IDC_WLANPORT, &portReadOk, FALSE);
					TRACE1("WLAN Ecmt Port is set: %d", wlanPort);	
					//write values to file
					ECMTGW_SetWlanIpPort(wlanIP, wlanPort, ui->engine);
					
				}	
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
}

STATIC void GUI_ShowWlanSettingsDlg(EcmtGwUI* ui)
{
	DialogBox(ui->module, MAKEINTRESOURCE(IDD_WLANSET), 
            ui->mainWindow, (DLGPROC)GUI_wlansetDlgProc);

	//SetWindowLong(hDlg, DWL_USER, (LONG)ui);
}
 
/***************************************************************************
 * Handles WM_NOTIFY message.
 ***************************************************************************/
STATIC void GUI_DlgNotify(EcmtGwUI* ui, NMLISTVIEW* nm)
{
    switch (nm->hdr.code) {
    case LVN_ITEMCHANGED:
        if (nm->uChanged & LVIF_STATE) {
            int change = (nm->uNewState ^ nm->uOldState);
            if (change & LVIS_SELECTED) {
                EcmtGwPort* port = NULL;
                int select = ListView_GetNextItem(ui->listView,-1,LVNI_SELECTED);
                if (select >= 0) {
                    LVITEM item;
                    ZeroMemory(&item, sizeof(item));
                    item.mask = LVIF_PARAM;
                    item.iItem = nm->iItem;
                    VERIFY(ListView_GetItem(ui->listView, &item));
                    ASSERT(item.lParam);
                    port = (EcmtGwPort*)item.lParam;
                    ASSERT(QUEUE_Contains(&ui->ports, &port->entry));
                }

                if (ui->port != port) {
                    ui->port = port;
                    GUI_DlgUpdatePortInfo(ui, port);
                    SendMessage(ui->mainWindow,WM_ECMTGW_SELECT_PORT,
                        (WPARAM)port, 0);
                }
            }
        }
        break;

    case NM_DBLCLK:
        ASSERT(!ECMTGW_IsActive(ui->engine));
        if (ui->port) {
            EcmtGatewayPort* gp = ui->port->port;
            TRACE1("GUI: connecting to %s\n",ECMTGW_GetPortName(gp));
            ECMTGW_Activate(gp);
        }
	case NM_RCLICK:
		/*ASSERT(!ECMTGW_IsActive(ui->engine));
		if (ui->port) {
			const char ecmtLanPortName [] = "WLAN";
            EcmtGatewayPort* gp = ui->port->port;
			if(strcmp( ECMTGW_GetPortName(gp), ecmtLanPortName ) == 0)
			{
				//GUI_ShowWlanSettingsDlg(ui);
				char editString[] = "Edit...";
				//LoadString(ui->module, IDS_EDIT_WLAN, buf, 15);
					HMENU wlanMenu;
					HMENU wlanCMenu;// = CreatePopupMenu();
					wlanMenu = LoadMenu(ui->module, MAKEINTRESOURCE(IDR_WLAN_POPUP) );
					wlanCMenu = GetSubMenu(wlanMenu, 0);
				if(wlanCMenu)
				{
					POINTS pt;
					//pt.x=GET_X_LPARAM(nm->lParam);
					//pt.y=GET_Y_LPARAM(nm->lParam);
					
					pt = MAKEPOINTS(*nm);
					ClientToScreen(ui->mainDialog, (LPPOINT)&pt);
					
					TrackPopupMenuEx(wlanCMenu, TPM_LEFTBUTTON|TPM_LEFTALIGN, pt.x, pt.y, ui->mainDialog, NULL);

				}

				TRACE("show");
			}
		}*/
        break;
    }
}



/***************************************************************************
 * Selects the specified port in the list view. Note that ui->port pointer
 * is updated by GUI_DlgNotify handler as a result of selection change in
 * the list view.
 ***************************************************************************/
STATIC void GUI_DlgSelectPort(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    if (port && (!ui->port || ui->port->port != port)) {
        QEntry* e = QUEUE_First(&ui->ports);
        int mask = LVIS_SELECTED|LVIS_FOCUSED;
        while (e) {
            EcmtGwPort* p = QCAST(e,EcmtGwPort,entry);
            int state = (p->port == port) ? mask : 0;
            ListView_SetItemState(ui->listView, p->iItem, state, mask);
            if (state != 0) {
                ListView_EnsureVisible(ui->listView, p->iItem, FALSE);
            }
            e = QUEUE_Next(e);
        }

        /* GUI_DlgNotify should have handled it */
        ASSERT(ui->port->port == port);
    }
}

/***************************************************************************
 * Finds EcmtGwPort* structure associated with the specified EcmtGatewayPort.
 ***************************************************************************/
STATIC EcmtGwPort* GUI_DlgFindPort(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    QEntry* e;
    for (e = QUEUE_First(&ui->ports); e; e = QUEUE_Next(e)) {
        EcmtGwPort* p = QCAST(e, EcmtGwPort, entry);
        if (p->port == port) {
            return p;
        }
    }
    return NULL;
}

/***************************************************************************
 * Update the dialog UI
 ***************************************************************************/
STATIC void GUI_DlgUpdateUI(EcmtGwUI* ui, int sw1, int sw2, int status)
{
    StrBuf32 buf;
    EcmtGatewayPort* port = ECMTGW_GetActivePort(ui->engine);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_PORT_LIST), sw1);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_SEPARATOR2), sw1);

    if (sw2) {
        UINT rxLed = ui->rxLedOn ? IDC_RX_LED_ON : IDC_RX_LED_OFF;
        UINT txLed = ui->txLedOn ? IDC_TX_LED_ON : IDC_TX_LED_OFF;
        ShowWindow(GetDlgItem(ui->mainDialog,rxLed), TRUE);
        ShowWindow(GetDlgItem(ui->mainDialog,txLed), TRUE);
    } else {
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_ON), SW_HIDE);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_ON), SW_HIDE);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_OFF), SW_HIDE);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_OFF), SW_HIDE);
    }
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_PORT_NAME), sw1);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_PORT_NAME2), sw2);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LABEL), sw2);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LABEL), sw2);
    ShowWindow(GetDlgItem(ui->mainDialog,IDC_SEPARATOR1), sw2);

    STRBUF_InitBufXXX(&buf);
    VERIFY(WIN32_LoadString(ui->module, status, &buf.sb));
    SetDlgItemText(ui->mainDialog, IDC_STATUS, STRBUF_Text(&buf.sb));
    STRBUF_Destroy(&buf.sb);

    GUI_DlgSelectPort(ui, port);
}

/***************************************************************************
 * Handles WM_ECMTGW_ACTIVATE message.
 ***************************************************************************/
STATIC void GUI_DlgActivate(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    GUI_DlgUpdateUI(ui, SW_HIDE, SW_SHOW, IDS_STATUS_CONNECTED);
}

/***************************************************************************
 * Handles WM_ECMTGW_DEACTIVATE message.
 ***************************************************************************/
STATIC void GUI_DlgDeactivate(EcmtGwUI* ui, EcmtGwDeactReason why)
{
    KillTimer(ui->mainDialog, TX_LED_OFF_TIMER);
    KillTimer(ui->mainDialog, RX_LED_OFF_TIMER);
    GUI_DlgUpdateUI(ui, SW_SHOW, SW_HIDE, IDS_STATUS_DISCONNECTED);
    ui->txLedOn = False;
    ui->rxLedOn = False;
}

/***************************************************************************
 * Handles WM_ECMTGW_PORT_FOUND message.
 ***************************************************************************/
STATIC void GUI_DlgPortFound(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    EcmtGwPort* p = MEM_New(EcmtGwPort);
    if (p) {
        ZeroMemory(p, sizeof(*p));
        p->iItem = -1;
        p->port = port;
        GUI_DlgAddPort(ui, p, -1);

        /*
         * If the main window is not visible, then this function
         * is probably called from GUI_DlgInit, which will invoke
         * GUI_DlgSortPortList after ECMTGW_RefreshPorts returns.
         * There's no need to do it here.
         */
        if (IsWindowVisible(ui->mainWindow)) {
            GUI_DlgSortPortList(ui);
            if (!ui->port && ECMTGW_GetPortType(port) == EcmtGwPortBtSocket) {
                /*
                 * If user hasn't selected any port yet (possibly, because
                 * he can't figure out what he's supposed to do) select the
                 * first Bluetooth device that we have found. Because I like
                 * Bluetooth sockets better than COM ports.
                 */
                GUI_DlgSelectPort(ui, port);
            }
        }
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_PORT_MODIFIED message.
 ***************************************************************************/
STATIC void GUI_DlgPortModified(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    EcmtGwPort* p = GUI_DlgFindPort(ui, port);
    ASSERT(p);
    if (p) {
        LVITEM item;
        ZeroMemory(&item, sizeof(item));

        item.mask = LVIF_TEXT | LVIF_IMAGE;
        item.iItem = p->iItem;
        item.pszText = (LPSTR)ECMTGW_GetPortName(port);
        item.iImage = GUI_DlgPortIconIndex(ui, p);
        ListView_SetItem(ui->listView, &item);

        item.mask = LVIF_TEXT;
        item.iSubItem = 1;
        item.pszText = (LPSTR)ECMTGW_GetPortComment(port);
        ListView_SetItem(ui->listView, &item);

        /* Sort order may change */
        GUI_DlgSortPortList(ui);

        /* Update information about the selected port */
        if (p == ui->port) {
            GUI_DlgUpdatePortInfo(ui, p);
        }
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_PORT_DISAPPEARED message.
 ***************************************************************************/
STATIC void GUI_DlgPortDisappeared(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    EcmtGwPort* p = GUI_DlgFindPort(ui, port);
    ASSERT(p);
    if (p) {
        ListView_DeleteItem(ui->listView,p->iItem);
        GUI_DlgUpdatePortIndices(ui);
        ASSERT(ui->port != p);
        QUEUE_RemoveEntry(&p->entry);
        MEM_Free(p);
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_SEND_PACKET message.
 ***************************************************************************/
STATIC void GUI_DlgSendPacket(EcmtGwUI* ui)
{
    KillTimer(ui->mainDialog, TX_LED_OFF_TIMER);
    if (!ui->txLedOn) {
        ui->txLedOn = True;
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_ON), SW_SHOW);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_OFF), SW_HIDE);
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_SEND_PACKET_COMPLETE message.
 ***************************************************************************/
STATIC void GUI_DlgSendPacketComplete(EcmtGwUI* ui)
{
    if (ui->txLedOn) {
        SetTimer(ui->mainDialog, TX_LED_OFF_TIMER, LED_OFF_TIMEOUT, NULL);
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_RECEIVE_PACKET message.
 ***************************************************************************/
STATIC void GUI_DlgReceivePacket(EcmtGwUI* ui)
{
    KillTimer(ui->mainDialog, RX_LED_OFF_TIMER);
    if (!ui->rxLedOn) {
        ui->rxLedOn = True;
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_ON), SW_SHOW);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_OFF), SW_HIDE);
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_RECEIVE_PACKET_COMPLETE message.
 ***************************************************************************/
STATIC void GUI_DlgReceivePacketComplete(EcmtGwUI* ui)
{
    if (ui->rxLedOn) {
        SetTimer(ui->mainDialog, RX_LED_OFF_TIMER, LED_OFF_TIMEOUT, NULL);
    }
}

/***************************************************************************
 * Handles WM_TIMER message. Return True if the message has been handled,
 * False otherwise.
 ***************************************************************************/
STATIC Bool GUI_DlgTimer(EcmtGwUI* ui, UINT id)
{
    switch (id) {
    case TX_LED_OFF_TIMER:
        ASSERT(ui->txLedOn);
        ui->txLedOn = False;
        KillTimer(ui->mainDialog, id);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_OFF), SW_SHOW);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_TX_LED_ON), SW_HIDE);
        return True;
    case RX_LED_OFF_TIMER:
        ASSERT(ui->rxLedOn);
        ui->rxLedOn = False;
        KillTimer(ui->mainDialog, id);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_OFF), SW_SHOW);
        ShowWindow(GetDlgItem(ui->mainDialog,IDC_RX_LED_ON), SW_HIDE);
        return True;
    default:
        return False;
    }
}

/***************************************************************************
 * Main dialog proc
 ***************************************************************************/
BOOL CALLBACK GUI_MainDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_INITDIALOG) {
        GUI_DlgInit(hDlg,(EcmtGwUI*)lp);
        return TRUE;
    } else {
        EcmtGwUI* ui = (EcmtGwUI*)GetWindowLong(hDlg, DWL_USER);
        if (ui) {
            ASSERT(ui->mainDialog == hDlg);
            switch (msg) {
            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLORDLG:
                switch (GetDlgCtrlID((HWND)lp)) {
                case IDC_PORT_NAME:
                case IDC_PORT_NAME2:
                    SetTextColor((HDC)wp, GetSysColor(COLOR_GRAYTEXT));
                    break;
                }
                return (BOOL)GetStockObject(WHITE_BRUSH);

            case WM_NOTIFY:
                if (((NMHDR*)lp)->hwndFrom == ui->listView) {
                    /* Notification from the list view */
                    GUI_DlgNotify(ui, (LPNMLISTVIEW)lp);
                    return TRUE;
                }
                break;

            case WM_TIMER:
                if (GUI_DlgTimer(ui, wp)) return TRUE;
                break;

            case WM_ECMTGW_ACTIVATE:
                GUI_DlgActivate(ui, (EcmtGatewayPort*)wp);
                return TRUE;

            case WM_ECMTGW_DEACTIVATE:
                GUI_DlgDeactivate(ui, wp);
                return TRUE;

            case WM_ECMTGW_PORT_FOUND:
                GUI_DlgPortFound(ui, (EcmtGatewayPort*)wp);
                return TRUE;

            case WM_ECMTGW_PORT_MODIFIED:
                GUI_DlgPortModified(ui, (EcmtGatewayPort*)wp);
                return TRUE;

            case WM_ECMTGW_PORT_DISAPPEARED:
                GUI_DlgPortDisappeared(ui, (EcmtGatewayPort*)wp);
                return TRUE;

            case WM_ECMTGW_SEND_PACKET:
                GUI_DlgSendPacket(ui);
                return TRUE;

            case WM_ECMTGW_SEND_PACKET_COMPLETE:
                GUI_DlgSendPacketComplete(ui);
                return TRUE;

            case WM_ECMTGW_RECEIVE_PACKET:
                GUI_DlgReceivePacket(ui);
                return TRUE;

            case WM_ECMTGW_RECEIVE_PACKET_COMPLETE:
                GUI_DlgReceivePacketComplete(ui);
                return TRUE;
			case WM_WLAN_SET:
				GUI_ShowWlanSettingsDlg(ui);
				return TRUE;
            }
        }
        return FALSE;
    }
}
