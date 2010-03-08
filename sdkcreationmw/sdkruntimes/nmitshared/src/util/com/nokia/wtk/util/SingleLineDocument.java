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

/* java.io */
import java.io.IOException;
import java.io.StringReader;
import java.io.BufferedReader;

/* javax.swing.text */
import javax.swing.text.Document;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;

/**
 * A document that does not allow adding multi-line strings to another
 * document. It can be used as a document for <code>JTextField</code>
 * object which allow setting multi-line text or pasting multi-line
 * strings but don't know how to deal with those (the result is really,
 * really weird...)
 *
 */
public class SingleLineDocument extends FilterDocument {

    public SingleLineDocument(Document target) {
        super(target);
    }

    /**
     * Inserts a string of content. If the string contains multiple lines,
     * inserts only the first non-empty line.
     *
     * @param offset  the offset into the document to insert the content >= 0.
     *    All positions that track change at or after the given location
     *    will move.
     * @param str    the string to insert
     * @param a      the attributes to associate with the inserted
     *   content.  This may be null if there are no attributes.
     * @exception BadLocationException  the given insert position is not a valid
     * position within the document
     */
    public void insertString(int offset, String str, AttributeSet a)
    throws BadLocationException {
        if (str != null && str.length() > 0) {
            BufferedReader r = new BufferedReader(new StringReader(str));
            try {
                String line;
                while ((line = r.readLine()) != null) {
                    if (line.length() > 0) {
                        doc.insertString(offset, line, a);
                        return;
                    }
                }
                str = "";
            }  catch (IOException x) {
                x.printStackTrace();   // this should never happen
            }
        }
        doc.insertString(offset, str, a);
    }
}
