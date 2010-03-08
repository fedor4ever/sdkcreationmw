/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Open URL dialog
*
*/


static char rcsid[] = "$Id: LoadDialog.cpp,v 1.8 2005/11/20 20:06:37 slava Exp $";

// INCLUDE FILES

#include "WinMenubar.h"
#include "MEcmtMenubar.h"
#include "EcmtResource.h"
#include "SdkProductInfo.h"

// We are using utilities from wlib to access the registry.
// wlib is not part of the licensee build. Therefore, they 
// don't get the history combo box. Besides, we use the product
// registration id (SDK_PRODUCT_ID) as the key name in the
// registry. It's not in the licensee build, too.
#ifdef HAVE_WLIB
static const Str gRegPath = "Software\\Nokia\\" SDK_PRODUCT_ID;
static const Str gRegLoadHistory = "LoadHistory";
const int KMaxLoadHistory = 20;
#endif // HAVE_WLIB

// ---------------------------------------------------------------------------
// CWinMenubar::LoadDialogProc()
// Dialog Window proc for the Load URL dialog
// ---------------------------------------------------------------------------
BOOL CALLBACK CWinMenubar::LoadDialogProc(HWND hDlg, UINT message, 
                                          WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
    case WM_INITDIALOG:
        {
            SetWindowLong(hDlg, DWL_USER, lParam); // CEcmtMenubar*
            HWND hURL = GetDlgItem(hDlg, IDC_URL);
#ifdef HAVE_WLIB
            HKEY key = REG_OpenKey(HKCU, gRegPath, KEY_READ);
            if (key)
            {
                RegMsz* history = REG_QueryMultiSz(key, gRegLoadHistory);
                if (history)
                {
                    const char* entry = REGMSZ_First(history);
                    TRACE("ECMTMENUBAR: reading URL history...\n");
                    while (entry)
                    {
                        TRACE1("    %s\n",entry);
                        // Cannot use ComboBox_AddString because it may call
                        // SendMessageW, while we need SendMessageA
                        SendMessageA(hURL, CB_ADDSTRING, 0, (LPARAM)entry);
                        entry = REGMSZ_Next(history, entry);
                    }
                    int n = ComboBox_GetCount(hURL);
                    if (n > 0)
                    {
                        TRACE1("ECMTMENUBAR: loaded %d history entries\n",n);
                        ComboBox_SetCurSel(hURL, 0);
                    }
                }
                REG_CloseKey(key);
            }
#endif // HAVE_WLIB

            // Disable OK button if the URL field is empty
            HWND hOK = GetDlgItem(hDlg, IDOK);
            EnableWindow(hOK, GetWindowTextLength(hURL) > 0);
        }
        return TRUE;
        
    case WM_COMMAND:
        switch (wParam)
        {
        case MAKELONG(IDC_URL,CBN_SELCHANGE):
            // Will update the OK button after the edit field gets updated
            PostMessageA(hDlg, message, MAKELONG(IDC_URL,EN_CHANGE), lParam);
            return TRUE;

        case MAKELONG(IDC_URL,CBN_EDITCHANGE):
        case MAKELONG(IDC_URL,EN_CHANGE):
            {
                // Disable OK button if the URL is empty
                HWND hURL = GetDlgItem(hDlg, IDC_URL);
                HWND hOK = GetDlgItem(hDlg, IDOK);
                EnableWindow(hOK, GetWindowTextLength(hURL) > 0);
            }
            return TRUE;
        case IDC_BROWSE:
            {
                CWinMenubar* This = (CWinMenubar*)GetWindowLong(hDlg,DWL_USER);
                HWND hURL = GetDlgItem(hDlg, IDC_URL);
                TCHAR* fname = This->SelectFile(hURL);
                if (fname)
                {
					// Opening a local file. Add local file cheme to the beginning 
					// of url, so later on a decision can be made: whether open a 
					// web address using browser or local file using the SDK launcher.
                    TCHAR* url=NULL;					
					const char* cheme = LOCAL_FILE_SCHEME;
					
					int length = strlen(cheme) + wcslen(fname)+1;

					url = (TCHAR*)Alloc(length*sizeof(TCHAR));
					TCHAR* tempurl = url; 	
					if(url)
					{
						// Add cheme
						while(*cheme) {
							*tempurl = *cheme;
							cheme++;
							tempurl++;
						}
				
						// Add pathS
						wcscat(tempurl,fname);					
						tempurl=NULL;
                    
                        HWND hOK = GetDlgItem(hDlg, IDOK);
                        EnableWindow(hOK, TRUE);
                        SetWindowText(hURL, url);
                        SetFocus(hURL);
                        Free(url);
                    }
                    Free(fname);
                }
            }
            return TRUE;

        case IDOK:
            {
                CWinMenubar* This = (CWinMenubar*)GetWindowLong(hDlg,DWL_USER);
                HWND hURL = GetDlgItem(hDlg, IDC_URL);
                int len = GetWindowTextLength(hURL);
                if (This && len > 0)
                {
                    TCHAR* url = (TCHAR*)Alloc((len+1)*sizeof(TCHAR));
                    if (url)
                    {						
                        len = GetWindowText(hURL, url, len+1);

						// Check if url contains "local://" at the beginning.
						const char* cheme = LOCAL_FILE_SCHEME;
						// Boolean indicating does url contain local file cheme
						BOOL match = TRUE;

						for(int k=0;k<strlen(cheme);k++){
							TCHAR c_cheme=cheme[k];
							TCHAR c_url=url[k];

							if(c_url != c_cheme) {
								match = FALSE;
								break;
							}
						}
						
						// Launch file with the SDK launcher local cheme was found.
						// otherwice launch url in browser.
						if(match==TRUE)
						{							
							TCHAR* fname=url;
							fname += strlen(cheme);
							This->iEcmtMenubar->LoadFile(
								This->iDefaultBrowser, fname);
							fname=NULL;
						}
						else
						{//Else launch file in browser:
							This->iEcmtMenubar->LaunchBrowser(
							This->iDefaultBrowser, url);
#ifdef HAVE_WLIB
							// Update the URL history. First, remove duplicates.
							TRACE("ECMTMENUBAR: updating URL history...\n");
							int i, n = ComboBox_GetCount(hURL);
							int bufsize = 0;
							TCHAR* buf = NULL;
							for (i=n-1; i>=0; i--)
							{
								len = ComboBox_GetLBTextLen(hURL,i);
								if (len >= bufsize)
								{
									Free(buf);
									bufsize = len+1;
									buf = (TCHAR*)Alloc(bufsize*sizeof(TCHAR));
									if (!buf) break;
								}
								buf[0] = 0;
								ComboBox_GetLBText(hURL, i, buf);
								if (_tcscmp(buf, url) == 0)
								{
									TRACE1("ECMTMENUBAR: duplicate at %d\n",i);
									ComboBox_DeleteString(hURL, i);
								}
							}

							// Insert new item, recalculate number of items
							ComboBox_InsertString(hURL, 0, url);
							n = ComboBox_GetCount(hURL);
							Free(buf);

							// Delete extra items from the bottom
							int extra = n - KMaxLoadHistory;
							for (i=0; i<extra; i++)
							{
								ComboBox_DeleteString(hURL, n-i-1);
							}

							// Save the history
							RegMsz * msz = REGMSZ_Create();
							if (msz)
							{
								TRACE("ECMTMENUBAR: collecting history\n");
								char* buf2 = NULL;
								bufsize = 0;
								n = ComboBox_GetCount(hURL);
								for (i=0; i<n; i++)
								{
									len = ComboBox_GetLBTextLen(hURL,i);
									if (len >= bufsize)
									{
										Free(buf2);
										bufsize = len+1;
										buf2 = (char*)Alloc(bufsize);
										if (!buf2) break;
									}
									buf2[0] = 0;
									SendMessageA(hURL,CB_GETLBTEXT,i,(LPARAM)buf2);
									TRACE1("    %s\n",buf2);
									if (buf2[0]) REGMSZ_Add(msz, buf2);
								}

								HKEY key = REG_OpenCreateKey(HKCU, gRegPath);
								if (key)
								{
									TRACE("ECMTMENUBAR: saving history\n");
									REG_SetMsz(key, gRegLoadHistory, msz);
									REG_CloseKey(key);
								}
								REGMSZ_Delete(msz);
								Free(buf2);
							}

	#endif // HAVE_WLIB
						}
						// And finally free memory
						Free(url);
                    }
                }
            }
            // Fall through
        case IDCANCEL:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            return FALSE;
        }

    default:
        return FALSE;
    }
}
