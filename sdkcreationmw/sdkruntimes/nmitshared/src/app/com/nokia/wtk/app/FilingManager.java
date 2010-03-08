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

/* java.io */
import java.io.File;

/* java.awt */
import java.awt.Component;

/* javax.swing */
import javax.swing.filechooser.FileFilter;

/**
 * An interface for filing services. Part of the AppEnvironment services.
 */
public interface FilingManager {

    /**
     * Sets the current directory for the chooser to open on.
     * @param f If pointing to a directory, opens to that directory, if
     *   pointing to a file opens to the containing directory.
     */
    public void setCurrentDirectory(File f);

    /**
     * Returns the current directory for the chooser.
     * @return a File object pointing to a directory. Never returns null.
     *   If no file has been set, point to default user directory.
     */
    public File getCurrentDirectory();

    /**
     * Asks file name from user and saves it. Returns true when successfull,
     * false if not. Saveable will have setSourceFile() called with the
     * right file when saved. All exceptions will go to the MessageLog
     * under Error, and the user will be dialoged.
     *
     * @param saveableComponent the Saveable. If the saveableComponent is
     *   instanceof Component, it will be given to the dialogs as parent.
     * @param defaultFilter the default file filter to present to the user.
     * @param filterAlternatives all possible file filters that should be set
     *   in the dialog. If defaultFilter is null, the first file filter from this
     *   list will be chosen. If both parameters are null, the All Files filter
     *   will be chosen.
     * @return true when successfully svaed, false if not
     */
    public boolean doSaveAsFile(
        Saveable saveableComponent,
        FileFilter defaultFilter,
        FileFilter[] filterAlternatives);

    /**
     * Asks file name from user and returns it, or null if none selected or
     * if operation is cancelled.  All exceptions will go to the MessageLog
     * under Error, and the user will be dialoged.  Intended to be used for
     * saving auxiliary files not directly controlled by the Saveable interface.
     *
     * @param parent the parent Component.  Will be given to the dialogs as parent.
     * @param defaultFile File to initialize the dialog entry box with.
     * @param fs the set of file filters for the chooser. If null the default
     *   file chooser filters will be set.
     * @param title the title the dialog will have. If null, the default
     *   'Open' will be set.
     */
    public File getSaveAsFile(
        Component parent,
        File defaultFile,
        javax.swing.filechooser.FileFilter defaultFilter,
        javax.swing.filechooser.FileFilter[] filterAlternatives);

    /**
     * Saves a Saveable.
     *
     * All excpetions will go to the MessageLog
     * under Error, and the user will be dialoged.
     *
     * @param saveableComponent The Saveable to save. If no source file has
     *   been set, the user will be asked using doSaveAsFile. Saveable will
     *   then have setSourceFile() called with the right file when saved.
     *   If saveableComponent is instanceof
     *   Component, it will be given to the dialogs as parent.
     * @param defaultFilter the default file filter to present to the user.
     *   To be used in case no source file has been set and the user needs
     *   to be asked.
     * @param filterAlternatives all possible file filters that should be set
     *   in the dialog. If defaultFilter is null, the first file filter from this
     *   list will be chosen. If both parameters are null, the All Files filter
     *   will be chosen. To be used in case no source file has been set and the user
     *   needs to be asked about file filters.
     * @return true when successfully saved, false if not
     *
     */
    public boolean doSaveFile(
        Saveable saveableComponent,
        FileFilter defaultFilter,
        FileFilter[] filterAlternatives);

    /**
     * Will display a dialogue for selecting a file in the
     * currentDirectory. Will return a File if anything usable and
     * exisiting selected, null otherwise.
     *
     * @param parent The component the chooser should have as parent. If null,
     *   the parent will be the appComponent.
     * @param fs the set of file filters for the chooser. If null the default
     *   file chooser filters will be set.
     * @param title the title the dialog will have. If null, the default
     *   'Open' will be set.
     */
    public File doOpenFile(
        Component parent,
        javax.swing.filechooser.FileFilter[] fs,
        String title);

    /**
     * Will display a dialogue for selecting a list of files in the
     * currentDirectory. Will return a File array if anything usable and
     * exisiting selected, otherwise, a zero-length array.
     * This is the preferred method over 'doOpenFile', since it is more
     * convenient for the user.
     *
     * @param parent The component the chooser should have as parent. If null,
     *  the parent will be the appComponent.
     * @param fs the set of file filters for the chooser. If null the default
     *  file chooser filters will be set.
     * @param title the title the dialog will have. If null, the default
     *  'Open' will be set.
     */
    public File[] doOpenFiles(
        Component parent,
        javax.swing.filechooser.FileFilter[] fs,
        String title);
}
