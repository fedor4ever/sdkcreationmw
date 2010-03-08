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
 * $Id: XRpcBasicData.java,v 1.5 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBasicData.java
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

/**
 * Base class for XRPC primitive data types
 *
 */

public
abstract
class       XRpcBasicData
extends     XRpcBasicDataElement
implements  XRpcData {

    /**
     * Write this element and its data into an XML stream.
     */
    public void write(XRpcWriter writer) {
        String string = getAsText();
        if (string == null) {
            writeEmptyTag(writer);
        } else {
            openTag(writer);
            writer.print(XRpcUtils.xmlize(string));
            closeTag(writer);
            writer.println();
        }
    }

    /**
     * Converts this object into a string. This method returns the
     * text representation of the value of this data element.
     */
    public String toString() {
        return getAsText();
    }

    /**
     * Converts text string into internal representation.
     * Throws an exception if convertion fails
     */
    abstract public void setAsText(String s) throws Exception;
}
