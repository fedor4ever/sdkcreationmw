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




package com.nokia.wtk.app;

import javax.swing.Action;

/** Every element in our tabbed panes that wants to support the Edit menu has
 * to implement this interface. Whenever a tab is selected that supports this
 * interface, the Edit menu will be instantiated with the objects called from
 * this interface.
 **/
public interface EditMenuSupport {

    /** This call will return an action suitable to hook up to a menu for doing
     * a 'cut' in the document. The semantics of the returned object should be
     * comparable to DefaultEditorKit.CutAction. However, this call does not
     * require that exact return type so as to allow media editors not derived
     * from DefaultEditorKit to also implement this interface.
     */
    public Action getCutAction();

    /**
     * This call will return an action suitable to hook up to a menu for doing
     * a 'cut' in the document. The semantics of the returned object should be
     * comparable to DefaultEditorKit.CopyAction. However, this call does not
     * require that exact return type so as to allow media editors not derived
     * from DefaultEditorKit to also implement this interface.
     */
    public Action getCopyAction();

    /**
     * This call will return an action suitable to hook up to a menu for doing
     * a 'paste' in the document. The semantics of the returned object should be
     * comparable to DefaultEditorKit.PasteAction. However, this call does not
     * require that exact return type so as to allow media editors not derived
     * from DefaultEditorKit to also implement this interface.
     */
    public Action getPasteAction();

    /**
     * This call will return an action suitable to hook up to a menu for doing
     * an 'undo'.
     */
    public Action getUndoAction();

    /**
     * This call will return an action suitable to hook up to a menu for doing
     * a 'redo'.
     */
    public Action getRedoAction();

    /**
     * This call will return an action suitable to hook up to a menu for doing
     * a 'find'.
     */
    public Action getFindAction();
}
