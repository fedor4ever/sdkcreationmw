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

import java.util.Iterator;

/**
 * An iterator that does not support remove operation.
 *
 */
public class UnmodifiableIterator implements Iterator {

    /**
     * The target <code>Iterator</code>.
     */
    private Iterator target;

    /**
     * Creates <code>UnmodifiableIterator</code>.
     */
    public UnmodifiableIterator(Iterator target) {
        this.target = target;
    }

    /**
     * Returns <tt>true</tt> if the iteration has more elements.
     */
    public boolean hasNext() {
        return target.hasNext();
    }

    /**
     * Returns the next element in the interation.
     */
    public Object next() {
        return target.next();
    }

    /**
     * Removes from the underlying collection the last element returned
     * by the iterator (optional operation). This implementation always
     * throws <code>UnsupportedOperationException</code>.
     *
     * @exception UnsupportedOperationException because this operation is not
     *            supported by this class.
     */
    public void remove() {
        throw new UnsupportedOperationException();
    }
}
