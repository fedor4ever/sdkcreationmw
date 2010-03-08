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




package com.nokia.wtk.diagnostics.gui.traffic;

import javax.swing.tree.TreeModel;

import com.nokia.wtk.diagnostics.views.support.ContentNode;

/**
 * An interface to capture the commonality of getting information from
 * wtkdve to show in trees. 
 */
interface ContentModel {
    
    /**
     * Appends an element and fires the corresponding change notification
     * in the tree.
     * Convenience call since all the views do a lot of adding.
     * @param elem the source element
     * (Either <code>DeviceContentRequest</code> or 
     * <code>DeviceContentReceived</code>)
     */
    void add(ContentNode node);
    
    /**
     * Gets the number of contained elements.
     * @return the number of contained elements.
     */
    int getSize();
    
    /**
     * Inserts an element and fires the corresponding change notification in
     * the tree models
     * @param elem the source element
     * @param index the index of the element to insert
     */
    void insert(ContentNode node, int index);
    
    /**
     * Removes an element from the model and propagates it to the tree
     * @param index the index of the element to remove
     */
    void remove(ContentNode node);

    /**
     * Removes an element from the model and propagates it to the tree
     * @param index the index of the element to remove
     * @return the user object of the removed node or <code>null</code> 
     * if non-existent.
     */    
    Object remove(int index);
    
    /**
     * Replace a previous node with a new node. Will update viewing in filtered
     * tree. If oldNode is not found, nothing happens.
     * @param oldNode the node to replace
     * @param node the new node to replace oldNode with.
     */    
    void replace(ContentNode oldNode, ContentNode node);
    
    /**
     * Replace a previous node with a new node. Will update viewing in filtered
     * tree. If oldNode is not found, nothing happens.
     * @param pos the location to replace
     * @param node the new node to replace oldNode with.
     */    
    void replace(int pos, ContentNode node);
    
    /**
     * Removes all entries from the model and propagates it to the tree.
     * Will clear root node from the tree and allow currently visible to
     * be garbage collected.
     */    
    void removeAll();
    
    TreeModel getTreeModel();
}
