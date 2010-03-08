/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * $Id: XRpcWriter.java,v 1.2 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcWriter.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.xml.rpc;

/* java.io */
import java.io.PrintWriter;

/**
 * A writer interface for producing nicely formatted XML.
 *
 */
public interface XRpcWriter {

    public Object getLock();
    public PrintWriter getPrintWriter();

    public void print(char c);
    public void print(String s);

    public void println();
    public void indent();
    public void unindent();

    public void flush();
    public void close();
}
