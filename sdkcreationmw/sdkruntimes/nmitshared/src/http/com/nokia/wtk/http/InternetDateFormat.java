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



package com.nokia.wtk.http;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.TimeZone;
import java.util.Locale;
import java.util.Calendar;

/**
  * A class that extends SimpleDateFormat by setting default
  * date format to that stated in the HTTP 1.1 specification
  * (RFC 2068). The date format itself is defined in RFC 822.
  */
public class InternetDateFormat extends SimpleDateFormat
{
    // The recommended date format to use in HTTP. Defined in RFC 822.
    static final String defaultFormat = "EEE, dd MMM yyyy HH:mm:ss 'GMT'";

    /**
     * Default Constructor.
     *
     * We force it to be UK to match the GMT zone.
     * Use constructor with Locale to get local format.
     */
    public InternetDateFormat() {
        this(Locale.UK);
    }

    /**
     * Default Constructor.
     * @param loc Locale that date should be formatted for.  
     *    If null, system default.
     */
    public InternetDateFormat(Locale loc) {
	super(defaultFormat, loc);
	TimeZone tz = TimeZone.getTimeZone("GMT");
	Calendar cal;
        if (loc == null) {
            loc = Locale.getDefault();
        }
        cal = Calendar.getInstance(tz, loc);
	setCalendar(cal);
    }

    // DEBUG main
    public static void main(String args[]) {
	DateFormat formatter = new InternetDateFormat();
	Calendar cal = formatter.getCalendar();
	String dateStr = formatter.format(cal.getTime());
	System.out.println("Date is "+dateStr);
    }
}
