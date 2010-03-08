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



package com.nokia.wtk.util;

/**
 * String buffer that keeps track of
 * tab level for XML readability.
 *
 * Note: This is not a derived class because 
 * StringBuffer is defined as final.
 */
public class TabbedStringBuffer {

    // Current tab level
    int mTabs = 0;

    // Our 'real' String buffer
    StringBuffer mSb;

    // String used as tab sequence
    private String mTabString = "  ";

    /**
     * Null Constructor
     */
    public TabbedStringBuffer() {
        mSb = new StringBuffer();
    }

    /**
     * Constructor
     * @param String initial string
     */
    public TabbedStringBuffer(String s) {
        this();
        mSb.append(s);
    }

    /**
     * Constructor
     * @param int initial buffer length
     */
    public TabbedStringBuffer(int len) {
        mSb = new StringBuffer(len);
    }

    /**
     * Append with tab
     */
    public void append(String s) {
        for (int i = 0; i < mTabs; i++) {
            mSb.append(mTabString);
        }
        mSb.append(s);
    }

    /**
     * Append with no tab
     */
    public void appendNoTab(String s) {
        mSb.append(s);
    }

    /** Increment tab stop */
    public void tab() { mTabs++; }

    /** Decrement tab stop */
    public void untab() { mTabs--; }

    /**
     * Set string used as tab
     */
    public void setTabString(String t) { 
        if (t == null) t = "";
        mTabString = t; 
    }

    /** 
     * return String version of buffer 
     */
    public String toString() { return mSb.toString(); }
}
