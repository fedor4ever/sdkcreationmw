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
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import javax.swing.Box;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.ArrayUtils;
import com.nokia.wtk.util.FileFilter;
import com.nokia.wtk.util.MessageWriters;

/**
 * A class for filing services. Every application will have one instance
 * of this class, which it can configure for its own purposes. All other
 * elements of the application can then use these filing services without
 * having to know anything about the application they are in, or be worried
 * about partitioning, or user-interaction.
 *
 */

public class DefaultFilingManager implements FilingManager {

    private static javax.swing.filechooser.FileFilter defaultSaveFileFilter =
        null;

    // Resource Keys
    final private static String SAVE_TITLE = "optionPane.save.title";
    final private static String SAVE_MESSAGE = "optionPane.save.message";
    final private static String FILE_EXISTS_MESSAGE =
        "dialog.fileExists.prompt";
    final private static String FILE_EXISTS_TITLE = "dialog.fileExists.title";
    final private static String FILE_CANT_WRITE_MESSAGE =
        "dialog.cantWrite.prompt";
    final private static String FILE_CANT_WRITE_TITLE = 
        "dialog.cantWrite.title";
    final private static String FILE_NOT_EXIST_MESSAGE =
        "dialog.fileNotExist.prompt";
    final private static String FILE_NOT_EXIST_TITLE =
        "dialog.fileNotExist.title";
    final private static String FILE_CANT_SAVE_MESSAGE =
        "dialog.cantSave.prompt";
    final private static String FILE_CANT_SAVE_TITLE = 
        "dialog.cantSave.title";

    static {
        AppEnvironment.addResources(
            "resources/DefaultFilingManager.properties",
            DefaultFilingManager.class);
    }

    private File currentDirectory = null;
    private File currentSaveDirectory = null;

    /**
     * This class steals initial focus from the combo box. The problem with
     * the combo box is that ot swallows the Escape keystroke, i.e. Escape
     * does not work as expected (i.e. does not close the dialog) if combo
     * box has the focus. Also in most cases it's far more useful to let
     * the JList have the focus (i.e. the file list).
     */
    private static class FocusSelector implements HierarchyListener {
        private boolean wasShowing = false;
        private JFileChooser chooser;
        FocusSelector(JFileChooser ch) {
            chooser = ch;
        }
        public void hierarchyChanged(HierarchyEvent e) {
            boolean showing = chooser.isShowing();
            if (showing && !wasShowing) {
                final JComponent focus = findInitialFocus(chooser);
                if (focus != null) {
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            focus.requestFocus();
                        }
                    });
                }
            }
            wasShowing = showing;
        }

        /**
         * Finds the first JList in the hierarhy
         */
        private static JComponent findInitialFocus(JComponent jc) {
            int n = jc.getComponentCount();
            for (int i = 0; i < n; i++) {
                Component c = jc.getComponent(i);
                if (c instanceof JList) {
                    return (JComponent) c;
                }
                if (c instanceof JComponent) {
                    JComponent found = findInitialFocus((JComponent) c);
                    if (found != null) {
                        return found;
                    }
                }
            }
            return null;
        }
    }

    public synchronized void createDefaultSaveFileFilter() {
        if (defaultSaveFileFilter == null) {
            defaultSaveFileFilter = getSaveAsChooser().getAcceptAllFileFilter();
        }
    }

    private static void setupFileChooser(JFileChooser chooser) {
        chooser.addHierarchyListener(new FocusSelector(chooser));
    }

    private JFileChooser getSaveAsChooser() {
        JFileChooser chooser = new JFileChooser();
        setupFileChooser(chooser);
        return chooser;
    }

    private JFileChooser getOpenChooser(
        javax.swing.filechooser.FileFilter[] fs) {
        JFileChooser chooser = new JFileChooser();
        chooser.resetChoosableFileFilters();
        if (fs != null) {
            for (int i = 0; i < fs.length; i++) {
                chooser.addChoosableFileFilter(fs[i]);
            }
            chooser.setFileFilter(fs[0]);
        }
        chooser.setFileHidingEnabled(false);
        setupFileChooser(chooser);
        return chooser;
    }

    private javax.swing.filechooser.FileFilter getDefaultSaveFileFilter() {
        if (defaultSaveFileFilter == null)
            createDefaultSaveFileFilter();
        return defaultSaveFileFilter;
    }

    /**
     * Sets the current directory for the chooser to open on.
     * @param f If pointing to a directory, opens to that directory, if
     *   pointing to a file opens to the containing directory.
     */
    public void setCurrentDirectory(File f) {
        if (f.isFile())
            currentDirectory = f.getParentFile();
        else if (f.isDirectory())
            currentDirectory = f;
        // NOTE: if f is not a valid file, current directory remains unchanged
    }

    /**
     * Returns the current directory for the chooser.
     * @return a File object pointing to a directory. Never returns null.
     *   If no file has been set, point to default user directory.
     */
    public File getCurrentDirectory() {
        // never return NULL or non-existent dir
        if (currentDirectory == null || !currentDirectory.exists()) {
            currentDirectory = new File(System.getProperty("user.dir"));
        }
        return currentDirectory;
    }

    /*
     * Display warning to user.
     * @param parent The parent component of the dialog
     * @param message The message to write.
     */
    private void handleSavingError(Component parent, String message) {
        JOptionPane.showMessageDialog(
            parent,
            AppEnvironment.getString(FILE_CANT_SAVE_MESSAGE) + message,
            AppEnvironment.getString(FILE_CANT_SAVE_TITLE),
            JOptionPane.WARNING_MESSAGE);
        // MessageWriters.ErrorMsg.println(message);
    }

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
        javax.swing.filechooser.FileFilter defaultFilter,
        javax.swing.filechooser.FileFilter[] filterAlternatives) {
        return doSaveAsFileInternal(
            saveableComponent,
            defaultFilter,
            filterAlternatives);
    }

    /**
     * Asks file name from user and saves it. Returns true when successfull,
     * false if not. Saveable will have setSourceFile() called with the
     * right file when saved. All exceptions will go to the MessageLog
     * under Error, and the user will be dialoged.  Also, if user changes the
     * character encoding menu setting, the SaveableComponent's setCharEncoding() will be called.
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
    private boolean doSaveAsFileInternal(
        Saveable saveableComponent,
        javax.swing.filechooser.FileFilter defaultFilter,
        javax.swing.filechooser.FileFilter[] filterAlternatives) {

        Component parent = null;
        ApplicationWindow appWin =
            AppEnvironment.getAppEnvironment().getMainWindow();
        if (appWin != null) {
            parent = appWin.getFrame();
        }
        if (saveableComponent instanceof Component) {
            parent = (Component) saveableComponent;
        }

        File file = getSaveAsFile(parent, saveableComponent.getSourceFile(), defaultFilter, filterAlternatives);

        if (file == null) { // Nothing selected
            return false;
        }

        File prev = saveableComponent.getSourceFile();
        saveableComponent.setSourceFile(file);

        try {
            saveableComponent.save();
        } catch (IOException ex) {
            handleSavingError(parent, ex.getMessage());
            saveableComponent.setSourceFile(prev);
            return false;
        }

        return true;
    }

    /**
     * Asks file name from user and returns it.
     *
     * @param defaultFile file to suggest to dialog to use.
     * @param defaultFilter the default file filter to present to the user.
     * @param filterAlternatives all possible file filters that should be set
     *   in the dialog. If defaultFilter is null, the first file filter from this
     *   list will be chosen. If both parameters are null, the All Files filter
     *   will be chosen.
     * @return true when successfully svaed, false if not
     */
    public File getSaveAsFile(
        Component parent,
        File defaultFile,
        javax.swing.filechooser.FileFilter defaultFilter,
        javax.swing.filechooser.FileFilter[] filterAlternatives) {

        if (currentSaveDirectory == null) {
            currentSaveDirectory = getCurrentDirectory();
        }

        JFileChooser chooser = getSaveAsChooser();
        chooser.setDialogType(JFileChooser.SAVE_DIALOG);

        // Suggest current file name as a default
        chooser.setSelectedFile(defaultFile);
        if (defaultFile != null) {
            currentSaveDirectory = chooser.getCurrentDirectory();
        } else {
            chooser.setCurrentDirectory(currentSaveDirectory);
        }

        chooser.rescanCurrentDirectory();
        chooser.setFileHidingEnabled(false);

        int n = 0;
        javax.swing.filechooser.FileFilter filter = defaultFilter;
        javax.swing.filechooser.FileFilter[] filters = filterAlternatives;
        javax.swing.filechooser.FileFilter defFilter = filter;

        // Add file filter associated with the content type
        if (!ArrayUtils.contains(filters, filter)) {
            chooser.addChoosableFileFilter(filter);
            n++;
        }

        // Add compatible file filters
        if (filters != null) {
            for (int i = 0; i < filters.length; i++) {
                if (filters[i] != null) {
                    if (defFilter == null)
                        defFilter = filters[i];
                    chooser.addChoosableFileFilter(filters[i]);
                    n++;
                }
            }
        }

        // Set the default file filter that we have chosen
        if (defFilter != null) {
            chooser.setFileFilter(defFilter);
        }

        // Set default "accept all" file filter if we didn't get anything
        // based on the MIME type
        if (n == 0) {
            chooser.addChoosableFileFilter(getDefaultSaveFileFilter());
        }

        File file = null;
        chooser.setDialogTitle("Save As");
        int retval = chooser.showSaveDialog(parent);
        if (retval == JFileChooser.APPROVE_OPTION) {
            File choice = chooser.getSelectedFile();
            if (choice != null) {
                File dir = choice.getParentFile();
                currentSaveDirectory = dir;
                if (chooser.getFileFilter() instanceof FileFilter) {
                    // Replace unsupported extension with a supported one
                    FileFilter fileFilter;
                    String ext = FileFilter.getFileExtension(choice);
                    fileFilter = (FileFilter) chooser.getFileFilter();
                    if (!fileFilter.isSupportedExtension(ext)) {
                        ext = fileFilter.getExtension();
                        if (ext != null) {
                            String fileName = choice.getName();
                            int dot = fileName.lastIndexOf('.');
                            if (dot > 0) {
                                // Replace extension
                                fileName = fileName.substring(0, dot + 1) + ext;
                            } else {
                                // Add extension
                                fileName += "." + ext;
                            }
                            choice = new File(dir, fileName);
                        }
                    }
                }

                file = choice;
            }
        }

        if (file != null && file.exists()) { // Confirm save:
            int s =
                JOptionPane.showConfirmDialog(
                    parent,
                    AppEnvironment.getString(FILE_EXISTS_MESSAGE),
                    AppEnvironment.getString(FILE_EXISTS_TITLE),
                    JOptionPane.YES_NO_OPTION);
            if (s != JOptionPane.YES_OPTION) {
                return null;
            }
        }

        return file;
    }

    /**
     * Saves a Saveable.
     *
     * All excpetions will go to the MessageLog
     * under Error, and the user will be dialoged.
     * @param saveableComponent The Saveable to save. If no source file has
     * been set, the user will be asked using doSaveAsFile. Saveable will
     * then have setSourceFile() called with the right file when saved.
     * If saveableComponent is instanceof
     * Component, it will be given to the dialogs as parent.
     * @param defaultFilter the default file filter to present to the user.
     * To be used in case no source file has been set and the user needs
     * to be asked.
     * @param filterAlternatives all possible file filters that should be set
     * in the dialog. If defaultFilter is null, the first file filter from this
     * list will be chosen. If both parameters are null, the All Files filter
     * will be chosen. To be used in case no source file has been set and the user
     * needs to be asked about file filters.
     * @return true when successfully saved, false if not
     *
     */
    public boolean doSaveFile(
        Saveable saveableComponent,
        javax.swing.filechooser.FileFilter defaultFilter,
        javax.swing.filechooser.FileFilter[] filterAlternatives) {
        boolean status = true;
        File file = saveableComponent.getSourceFile();
        Component parent = null;
        ApplicationWindow appWin =
            AppEnvironment.getAppEnvironment().getMainWindow();
        if (appWin != null) {
            parent = appWin.getFrame();
        }
        if (saveableComponent instanceof Component) {
            parent = (Component) saveableComponent;
        }
        if (file == null) {
            status =
                doSaveAsFile(
                    saveableComponent,
                    defaultFilter,
                    filterAlternatives);
        } else if (!file.exists()) {
            status = 
               doSaveAsFile(saveableComponent, defaultFilter, filterAlternatives);
        } else if (!file.canWrite()) {
            JOptionPane.showMessageDialog(
                parent,
                AppEnvironment.getString(FILE_CANT_WRITE_MESSAGE),
                AppEnvironment.getString(FILE_CANT_WRITE_TITLE),
                JOptionPane.WARNING_MESSAGE);
            status =
                doSaveAsFile(
                    saveableComponent,
                    defaultFilter,
                    filterAlternatives);
        } else {
            try {
                saveableComponent.save();
            } catch (IOException ex) {
                handleSavingError(parent, ex.getMessage());
                status = false;

            }
        }
        return status;
    }

    /**
    * Will display a dialog for selecting a file in the
    * currentDirectory. Will return a File if anything usable and
    * exisiting selected, null otherwise.
    * @param parent The component the chooser should have as parent. If null,
    * the parent will be the appComponent.
    * @param fs the set of file filters for the chooser. If null the default
    * file chooser filters will be set.
    * @param title the title the dialog will have. If null, the default
    * 'Open' will be set.
    */
    public File doOpenFile(
        Component parent,
        javax.swing.filechooser.FileFilter[] fs,
        String title) {
        return doOpenFileInternal(parent, fs, title, null);
    }

    /**
     * Will display a dialogue for selecting a text file in the
     * currentDirectory.  If saveableComponent is not null, 
     * the dialog wil include a menu accessory to select a character encoding
     * to use when selecting the file. Will return a File if anything usable and
     * exisiting selected, null otherwise.  Will call setCharEncoding() on the
     * Saveable to indicate how to decode the file.
     *
     * @param parent The component the chooser should have as parent. If null,
     *  the parent will be the appComponent.
     *
     * @param fs the set of file filters for the chooser. If null the default
     *   file chooser filters will be set.
     *
     * @param title the title the dialog will have. If null, the default
     *   'Open' will be set.
     *
     * @param saveableComponent the Saveable. If the saveableComponent is
     *   instanceof Component, it will be given to the dialogs as parent.
     *   If the user selects a character encoding, saveableComponent.setCharEncoding() 
     *   will be called with the selected encoding.
     */
    private File doOpenFileInternal(
        Component parent,
        javax.swing.filechooser.FileFilter[] fs,
        String title,
        Saveable saveableComponent) {
        Component pt = parent;
        if (pt == null) {
            ApplicationWindow appWin =
                AppEnvironment.getAppEnvironment().getMainWindow();
            if (appWin != null) {
                pt = appWin.getFrame();
            }
        }
        JFileChooser chooser = getOpenChooser(fs);
        chooser.setDialogType(JFileChooser.OPEN_DIALOG);
        chooser.setCurrentDirectory(getCurrentDirectory());
        chooser.rescanCurrentDirectory();
        chooser.setDialogTitle(title == null ? "Open" : title);
        if (saveableComponent != null) {
            chooser.setAccessory(new EncodingMenu(saveableComponent));
        }
        int retval = chooser.showOpenDialog(pt);
        if (retval == JFileChooser.APPROVE_OPTION) {
            File choice = chooser.getSelectedFile();
            if ((choice != null) && choice.exists()) {
                currentDirectory = choice.getParentFile();
                return choice;
            } else if ((choice != null) && !choice.exists()) {
                // Show warning if not found
                JOptionPane.showMessageDialog(
                    pt,
                    AppEnvironment.getString(FILE_NOT_EXIST_MESSAGE),
                    AppEnvironment.getString(FILE_NOT_EXIST_TITLE),
                    JOptionPane.WARNING_MESSAGE);
            }
        }
        return null;
    }

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
        String title) {
        Component pt = parent;
        if (pt == null) {
            ApplicationWindow appWin =
                AppEnvironment.getAppEnvironment().getMainWindow();
            if (appWin != null) {
                pt = appWin.getFrame();
            }
        }
        JFileChooser chooser = getOpenChooser(fs);
        chooser.setDialogType(JFileChooser.OPEN_DIALOG);
        chooser.setMultiSelectionEnabled(true);
        chooser.setCurrentDirectory(getCurrentDirectory());
        chooser.rescanCurrentDirectory();
        chooser.setDialogTitle(title == null ? "Open" : title);
        int retval = chooser.showOpenDialog(pt);
        File[] choices = new File[] {};
        if (retval == JFileChooser.APPROVE_OPTION) {
            choices = chooser.getSelectedFiles();
            if (choices.length > 0) {
                File f = choices[0];
                if (f.exists()) {
                    currentDirectory = f.getParentFile();
                }
            }
        }
        return choices;
    }
}
