/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


// Includes
#include "SysIni.h"
#include <f32file.h>
#include <stdlib.h>

/****************************************************************************
*	CSysIni()
*****************************************************************************
*
*
*
****************************************************************************/
CSysIni::CSysIni()
:iFil(0,0,0)
	{
	}
/****************************************************************************
*	~CSysIni()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CSysIni::~CSysIni()
	{
	delete iData;
	delete iFiltered;
	iFiltered = 0;
	}
/****************************************************************************
*	CostructL(TDes& filename)
*****************************************************************************
*
*
*
****************************************************************************/
void CSysIni::CostructL(const TDesC& filename)
	{
	// Open a session to fileserver
	RFs fs;
	User::LeaveIfError(fs.Connect());

	// Open file
	RFile f;
    CleanupClosePushL(f);
	User::LeaveIfError(f.Open(fs, filename, EFileRead));
	
	// Allocate memory for the file
	TInt size = 0;
	User::LeaveIfError(f.Size(size));

	iData		= HBufC8::New(size);

	if(iData == 0)
		{
		User::Leave(KErrNoMemory);
		}
	// Read the file
	TPtr8 p(iData->Des());
	User::LeaveIfError(f.Read(p));

	//f.Close();
    CleanupStack::PopAndDestroy();
	fs.Close();

	RemoveComments(p);
	}
/****************************************************************************
*	RemoveComments(TPtr8& src)
*****************************************************************************
*
*	Remove comments from .ini file
*
****************************************************************************/
void CSysIni::RemoveComments(TPtr8& src)
	{
	// Allocate memory for cleaned buffer
	TLex8 lex(src);
	iFiltered = HBufC8::New(src.Size()+1);
	if(iFiltered == 0)
		{
		User::Leave(KErrNoMemory);
		}

	TPtr8 fi(iFiltered->Des());
	fi.SetLength(src.Size()+1);

	int c = 0;
	int i = 0;
	do
		{
		c = lex.Get();
		if(c == ';')
			{
			// When ";" is found, skip rest of the line
			while(c != 0 && c != '\n')
				{
				c = lex.Get();
				}
			}
		// Else: just copy
		fi[i] = (unsigned char)c;
		i++;
		}while(c != 0);
	// Delete old buffer
	delete iData;
	iData = 0;
	iFil.Set(fi);
	}
/****************************************************************************
*	NewL(TDes& filename)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CSysIni* CSysIni::NewL(const TDesC& filename)
	{
	CSysIni* self = new (ELeave)CSysIni;
	CleanupStack::PushL(self);
	self->CostructL(filename);
	CleanupStack::Pop();
	return self;
	}
/****************************************************************************
*	SeekBool(const  char* string)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C bool CSysIni::SeekBool(const  char* string)
	{
	TBuf8<50> p;
	if(SeekField(string,p) == false)
		{
		return false;
		}
	TBuf8<1> b;
	b.Append(_L8("1"));
	p.SetLength(1);
	if(p.Compare(b) == 0)
		{
		return true;
		}
	return false;
	}
/****************************************************************************
*	strlen(const char* s)
*****************************************************************************
*
*
*
****************************************************************************/
inline int strlen(const char* s)
	{
	int len = 0;
	while(*s != 0)
		{
		len++;
		s++;
		}
	return len;
	}
/****************************************************************************
*	SeekField
*****************************************************************************
*
*
*
****************************************************************************/
inline bool CSysIni::SeekField(const  char* string, TDes8& res
							   , bool allow_spaces)
	{
	int len = strlen(string);
	TBuf8<50> seek;
	seek.Append((const unsigned char*)string, len);

	int r = iFil.Find(seek);
	if(r== KErrNotFound)
	{
		// Requested field not found
		return false;
	}
	// Copy field data
	r = r + 1 + len;
	while(r < iFil.Size() && (res.Size() < res.MaxSize()-1))
	{
		// Is it alowed for field data to contain spaces
		if( (!allow_spaces && iFil[r] == ' ') || iFil[r] == '\n' 
			|| iFil[r] == '\r')
		{
			break;
		}
		res.Append(iFil[r]);
		r++;
	}
	res.Append(0);
	return true;
	}
/****************************************************************************
*	SeekStr
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C bool CSysIni::SeekStr(const char* string, TDes8& output)
	{
	return SeekField(string,output, false);
	}
/****************************************************************************
*	SeekLine
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C bool CSysIni::SeekLine(const char* string, TDes8& output)
	{
	return SeekField(string, output, true);
	}
/****************************************************************************
*	SeekInt
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C bool CSysIni::SeekInt(const char* string, int& in)
	{
	// Seek field
	TBuf8<10> i;
	if(SeekStr(string, i) == false)
		{
		return false;
		}
	// Convert to int
	in = atoi((const char*)i.Ptr());
	return true;
	}
/****************************************************************************
*	SeekIntList
*****************************************************************************
*
*
*
****************************************************************************/
static const unsigned char sp[] = {" "};

EXPORT_C bool CSysIni::SeekIntList(const char* string, int* results, int  max_results
						  	, int& actual_results)
	{
	// It is possible that the whole file is in one very long line
	HBufC8* lb = HBufC8::New(iFiltered->Des().Length());
	TPtr8 lbuf(lb->Des());

	// Seek field
	if(SeekLine(string, lbuf) == false)
		{
		delete lb;
		return false;
		}
	// Replace "," with spaces
	TPtrC8 space(sp, 1);
	int pos = lbuf.Find((const unsigned char*)",",1);

	while(pos != KErrNotFound)
		{
		lbuf.Replace(pos, 1, space);
		pos = lbuf.Find((const unsigned char*)",",1);
		}
	// Do actual scanning
	actual_results = 0;
	TLex8 ul(lbuf);
	do
		{
		// Get next integer
		TPtrC8 p = ul.NextToken();
		if(p.Size() <= 0 || actual_results >= max_results)
			{
			break;			// Exit is here
			}
		results[actual_results] = atoi((const char *)p.Ptr());
		actual_results++;

	}while(&ul != 0);		// Infinite loop
	delete lb;
	return true;
	}
/****************************************************************************
*	GetDataLength()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C int CSysIni::GetDataLength()
	{
	return iFiltered->Des().Length();
	}
