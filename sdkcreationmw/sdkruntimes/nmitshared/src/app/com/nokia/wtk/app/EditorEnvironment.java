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

/**
 * Interface moderating between the editors and the UI.
 */
public interface EditorEnvironment {

    /**
     * Create an editor, initialized with file, and
     * add it to this editing environment.
     * @param File to open
     * @param mime type of content
     * @return Editor object
     */
    public Saveable createEditor(String filename, String mimeType);

    /**
     * Create an editor, initialized with template contents, and
     * add it to this editing environment.
     * @param mime type of content
     * @param File to use as template
     * @return Editor object or null if can't determine editor type to use
     */
    public Saveable createEditorWithTemplate(String mimeType, String template);

    /**
     * Create an editor, initialized with file, and
     * add it to this editing environment.
     * @param mime type of content
     * @param content the content to put in
     * @return Editor object if can't determine editor type to use
     */
    public Saveable createEditorWithContent(String mimeType, String content);

    /* 
     * Add a file to the list of recently used files (for menu building).
     * @param file name to be added for display
     */
    public void addRecentFile(String newFile);

    /* 
     * Set a file to be the initial file for the editor of this content type.
     * @param ctype content type for which this will be a template
     * @param file name to be registered as a template
     */
    public void setTemplate(String ctype, String path);
}
