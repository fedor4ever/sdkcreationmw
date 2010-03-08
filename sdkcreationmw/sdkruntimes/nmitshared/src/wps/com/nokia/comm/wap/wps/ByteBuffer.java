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



package com.nokia.comm.wap.wps;

public class ByteBuffer {
    public byte[] buf;
    public int len;
    public int maxLen;

    public boolean set(byte[] b) {
	if(b == null) {
	    buf = new byte[0];
	    len = 0;
	    return true;
	}

	if(maxLen == 0 || b.length <= maxLen) {
	    buf = b;
	    len = b.length;
	    return true;
	}

	buf = new byte[maxLen];
	System.arraycopy(b, 0, buf, 0, maxLen);
	len = maxLen;
	return false;
    }

    public String toString() {
	StringBuffer sb = new StringBuffer();
	sb.append("[ByteBuffer: ");
	sb.append("(buf:" + ByteUtilities.toString(buf) + ")");
	sb.append("(len:" + len + ")");
	sb.append("(max len:" + maxLen + ")");
	sb.append("]");

	return sb.toString();
    }
}
