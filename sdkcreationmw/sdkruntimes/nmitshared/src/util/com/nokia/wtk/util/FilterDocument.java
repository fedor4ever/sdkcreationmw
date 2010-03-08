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

/* javax.swing.text */
import javax.swing.text.Element;
import javax.swing.text.Segment;
import javax.swing.text.Document;
import javax.swing.text.Position;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;

/* javax.swing.event */
import javax.swing.event.DocumentListener;
import javax.swing.event.UndoableEditListener;

/**
 * Base class for building document "filters".
 *
 * @see SingleLineDocument
 */
public class FilterDocument implements Document {

    protected Document doc;

    public FilterDocument(Document target) {
        doc = target;
    }

    /**
     * Returns number of characters of content currently
     * in the document.
     *
     * @return number of characters >= 0
     */
    public int getLength() {
        return doc.getLength();
    }

    /**
     * Registers the given observer to begin receiving notifications
     * when changes are made to the document.
     *
     * @param listener the observer to register
     * @see Document#removeDocumentListener
     */
    public void addDocumentListener(DocumentListener listener) {
        doc.addDocumentListener(listener);
    }

    /**
     * Unregisters the given observer from the notification list
     * so it will no longer receive change updates.
     *
     * @param listener the observer to register
     * @see Document#addDocumentListener
     */
    public void removeDocumentListener(DocumentListener listener) {
        doc.removeDocumentListener(listener);
    }

    /**
     * Registers the given observer to begin receiving notifications
     * when undoable edits are made to the document.
     *
     * @param listener the observer to register
     * @see javax.swing.event.UndoableEditEvent
     */
    public void addUndoableEditListener(UndoableEditListener listener) {
        doc.addUndoableEditListener(listener);
    }

    /**
     * Unregisters the given observer from the notification list
     * so it will no longer receive updates.
     *
     * @param listener the observer to register
     * @see javax.swing.event.UndoableEditEvent
     */
    public void removeUndoableEditListener(UndoableEditListener listener) {
        doc.removeUndoableEditListener(listener);
    }

    /**
     * Gets properties associated with the document.  Allows one to
     * store things like the document title, author, etc.
     *
     * @param key a non-null property
     * @return the properties
     */
    public Object getProperty(Object key) {
        return doc.getProperty(key);
    }

    /**
     * Puts a new property on the list.
     *
     * @param key the non-null property key
     * @param value the property value
     */
    public void putProperty(Object key, Object value) {
        doc.putProperty(key, value);
    }

    /**
     * Removes a portion of the content of the document.
     * This will cause a DocumentEvent of type
     * DocumentEvent.EventType.REMOVE to be sent to the
     * registered DocumentListeners, unless an exception
     * is thrown.  The notification will be sent to the
     * listeners by calling the removeUpdate method on the
     * DocumentListeners.
     * <p>
     * To ensure reasonable behavior in the face
     * of concurrency, the event is dispatched after the
     * mutation has occurred. This means that by the time a
     * notification of removal is dispatched, the document
     * has already been updated and any marks created by
     * createPosition have already changed.
     * For a removal, the end of the removal range is collapsed
     * down to the start of the range, and any marks in the removal
     * range are collapsed down to the start of the range.
     * <p align=center><img src="doc-files/Document-remove.gif">
     * <p>
     * If the Document structure changed as result of the removal,
     * the details of what Elements were inserted and removed in
     * response to the change will also be contained in the generated
     * DocumentEvent. It is up to the implementation of a Document
     * to decide how the structure should change in response to a
     * remove.
     * <p>
     * If the Document supports undo/redo, an UndoableEditEvent will
     * also be generated.
     *
     * @param offs  the offset from the begining >= 0
     * @param len   the number of characters to remove >= 0
     * @exception BadLocationException  some portion of the removal range
     *   was not a valid part of the document.  The location in the exception
     *   is the first bad position encountered.
     * @see javax.swing.event.DocumentEvent
     * @see javax.swing.event.DocumentListener
     * @see javax.swing.event.UndoableEditEvent
     * @see javax.swing.event.UndoableEditListener
     */
    public void remove(int offs, int len) throws BadLocationException {
        doc.remove(offs, len);
    }

    /**
     * Inserts a string of content.  This will cause a DocumentEvent
     * of type DocumentEvent.EventType.INSERT to be sent to the
     * registered DocumentListers, unless an exception is thrown.
     * The DocumentEvent will be delivered by calling the
     * insertUpdate method on the DocumentListener.
     * The offset and length of the generated DocumentEvent
     * will indicate what change was actually made to the Document.
     * <p align=center><img src="doc-files/Document-insert.gif">
     * <p>
     * If the Document structure changed as result of the insertion,
     * the details of what Elements were inserted and removed in
     * response to the change will also be contained in the generated
     * DocumentEvent.  It is up to the implementation of a Document
     * to decide how the structure should change in response to an
     * insertion.
     * <p>
     * If the Document supports undo/redo, an UndoableEditEvent will
     * also be generated.
     *
     * @param offset  the offset into the document to insert the content >= 0.
     *    All positions that track change at or after the given location
     *    will move.
     * @param str    the string to insert
     * @param a      the attributes to associate with the inserted
     *   content.  This may be null if there are no attributes.
     * @exception BadLocationException  the given insert position is not a valid
     * position within the document
     * @see javax.swing.event.DocumentEvent
     * @see javax.swing.event.DocumentListener
     * @see javax.swing.event.UndoableEditEvent
     * @see javax.swing.event.UndoableEditListener
     */
    public void insertString(int offset, String str, AttributeSet a)
    throws BadLocationException {
        doc.insertString(offset, str, a);
    }

    /**
     * Fetches the text contained within the given portion
     * of the document.
     *
     * @param offset  the offset into the document representing the desired
     *   start of the text >= 0
     * @param length  the length of the desired string >= 0
     * @return the text, in a String of length >= 0
     * @exception BadLocationException  some portion of the given range
     *   was not a valid part of the document.  The location in the exception
     *   is the first bad position encountered.
     */
    public String getText(int offset, int length) throws BadLocationException {
        return doc.getText(offset, length);
    }

    /**
     * Fetches the text contained within the given portion
     * of the document.
     *
     * @param offset  the offset into the document representing the desired
     *   start of the text >= 0
     * @param length  the length of the desired string >= 0
     * @param txt the Segment object to return the text in
     *
     * @exception BadLocationException  Some portion of the given range
     *   was not a valid part of the document.  The location in the exception
     *   is the first bad position encountered.
     */
    public void getText(int offset, int length, Segment txt)
    throws BadLocationException {
        doc.getText(offset, length, txt);
    }

    /**
     * Returns a position that represents the start of the document.  The
     * position returned can be counted on to track change and stay
     * located at the beginning of the document.
     *
     * @return the position
     */
    public Position getStartPosition() {
        return doc.getStartPosition();
    }

    /**
     * Returns a position that represents the end of the document.  The
     * position returned can be counted on to track change and stay
     * located at the end of the document.
     *
     * @return the position
     */
    public Position getEndPosition() {
        return doc.getEndPosition();
    }

    /**
     * This method allows an application to mark a place in
     * a sequence of character content. This mark can then be
     * used to tracks change as insertions and removals are made
     * in the content. The policy is that insertions always
     * occur prior to the current position (the most common case)
     * unless the insertion location is zero, in which case the
     * insertion is forced to a position that follows the
     * original position.
     *
     * @param offs  the offset from the start of the document >= 0
     * @return the position
     * @exception BadLocationException  if the given position does not
     *   represent a valid location in the associated document
     */
    public Position createPosition(int offs) throws BadLocationException {
        return doc.createPosition(offs);
    }

    /**
     * Returns all of the root elements that are defined.
     * <p>
     * Typically there will be only one document structure, but the interface
     * supports building an arbitrary number of structural projections over the
     * text data. The document can have multiple root elements to support
     * multiple document structures.  Some examples might be:
     * </p>
     * <ul>
     * <li>Text direction.
     * <li>Lexical token streams.
     * <li>Parse trees.
     * <li>Conversions to formats other than the native format.
     * <li>Modification specifications.
     * <li>Annotations.
     * </ul>
     *
     * @return the root element
     */
    public Element[] getRootElements() {
        return doc.getRootElements();
    }

    /**
     * Returns the root element that views should be based upon,
     * unless some other mechanism for assigning views to element
     * structures is provided.
     *
     * @return the root element
     */
    public Element getDefaultRootElement() {
        return doc.getDefaultRootElement();
    }

    /**
     * This allows the model to be safely rendered in the presence
     * of currency, if the model supports being updated asynchronously.
     * The given runnable will be executed in a way that allows it
     * to safely read the model with no changes while the runnable
     * is being executed.  The runnable itself may <em>not</em>
     * make any mutations.
     *
     * @param r a Runnable used to render the model
     */
    public void render(Runnable r) {
        doc.render(r);
    }
}
