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



package com.nokia.wtk.bookmarks;

import java.util.*;
import java.io.*;
import java.net.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.tree.*;

import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;

import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.IndentWriter;


/**
 * Package-private class with various static utility methods relating to
 * bookmarks. Set the <code>bookmarkLoader</code>  before using static utils
 * of this class.
 *
 * @see BookmarkView
 */
class Bookmarks implements MessageWriters, Resource {

    static {
        AppEnvironment.addResources("resources/Bookmarks.properties",
                                    Bookmarks.class);
    }


    private final static String bookmarkFolderElement = "bookmark-folder";
    private final static String bookmarkElement = "bookmark";
    private final static String bookmarkNameAttribute = "name";
    private final static String bookmarkLocationAttribute = "href";
    private final static String bookmarkDescriptionAttribute = "descr";

    private final static Icon LEAF_ICON = getIcon("bookmark.leaf.icon");
    private final static Icon SAMPLES_LEAF_ICON = getIcon("bookmark.samples.leaf.icon");
    private final static Icon OPEN_BRANCH_ICON = getIcon("bookmark.branch-open.icon");
    private final static Icon CLOSED_BRANCH_ICON = getIcon("bookmark.branch-closed.icon");

    private static final String BOOKMARKS_XMLFILENAME = "bookmarks.xml";
    private static final String SAMPLEBOOKMARKS_XMLFILENAME = "sampleBookmarks.xml";

    private static DefaultTreeModel model = null;
    private static DefaultTreeModel sampleModel =null;
    private static BookmarkLoader bookmarkLoader;

    /**
     * Sets the loader.
     * @param loader the bookmark loader
     */
    public static void setBookmarkLoader(BookmarkLoader loader) {
      bookmarkLoader = loader;
    }

    public static Icon getLeafIcon() {
        return LEAF_ICON;
    }

    public static Icon getSamplesLeafIcon() {
        return SAMPLES_LEAF_ICON;
    }
    private static String getString(String key) {
        return AppEnvironment.getString(key);
    }
    private static ImageIcon getIcon(String key) {
        return AppEnvironment.getIcon(key);
    }

    public static Icon getBranchIcon(boolean open) {
        return (open) ? OPEN_BRANCH_ICON : CLOSED_BRANCH_ICON;
    }

    /**
     * No instantation for this.
     */
    Bookmarks() throws InstantiationException {
      throw new InstantiationException("Bookmarks is an static util.");
    }

    /**
     * Get the file used for saving and restoring bookmarks based on the model.
     * @param tmodel a Treemodel, either corresponding to the global static variables
     * model or sampleModel. This call will not work if the supplied tmodel
     * is null.
     */
    private static File getBookmarksFile(TreeModel tmodel) {
        if (tmodel == model){
            return getUserBookmarksFile();
        } else if (tmodel == sampleModel){
            return getSampleBookmarksFile();
        }
        return null;
    }

    /**
     * Get the file used for saving and restoring user bookmarks.
     */
    private static File getUserBookmarksFile() {
        File bookmarksFile = new File(System.getProperty("user.home")
                                   + File.separator
                                   + BOOKMARKS_XMLFILENAME);
        return bookmarksFile;
    }
    /**
     * Get the file used for saving and restoring sample bookmarks.
     */
    private static File getSampleBookmarksFile() {
        File bookmarksFile = new File(System.getProperty("user.dir")
                                   + File.separator
                                   + "samples"
                                   + File.separator
                                   + SAMPLEBOOKMARKS_XMLFILENAME);
        return bookmarksFile;
    }


    /**
     * Expands the URL of a bookmark relative to the file path.
     * This method will be called when restoring bookmarks.
     * @param href the bookmark URL string
     * @f file name.
     * @return the expanded bookmark URL
     */
    private static String expandBookmarkURL(String href, File fi) {

        if (href != null) {
            if (href.startsWith("/")) {
                String filepath = href;
                String reference = null;
                int lastHashPos = filepath.lastIndexOf('#');
                if (lastHashPos >= 0) {
                    // Temporarily save and remove any card reference:
                    reference = filepath.substring(lastHashPos);
                    filepath = filepath.substring(0, lastHashPos);
                }
                File f = new File(fi.getParent() + File.separator + filepath);
//System.out.println("expandBookmarkURL: filepath=\"" + filepath + "\", f=\"" + f + "\"");
                if ((f != null) && f.exists()) {
                    try {
                        filepath = f.toURL().toExternalForm();
                        if (reference != null) {
                            // Reattach the card reference:
                            filepath += reference;
                        }
                        href = filepath;
                    }
                    catch (MalformedURLException ex) {}
//                } else {
//System.out.println(" *** NON-EXISTENT");
                }
            }
        }
        return href;
    }

    /**
     * Compresses the URL of a bookmark relative to the bookmarks file path.
     * This method will be called when saving bookmarks.
     * @param href the bookmark URL string
     * @return the expanded bookmark URL
     */
    private static String compressBookmarkURL(String href, TreeModel tmodel) {
        if (href != null) {
            try {
                String base = getBookmarksFile(tmodel).toURL().toExternalForm();
                // Remove the filename from the end of the string to get the parent directory:
                base = base.substring(0, base.length()-getBookmarksFile(tmodel).getName().length());

                if (href.startsWith(base)) {
                    // Prefix length reduced by 1 to preserve leading "/":
                    href = href.substring(base.length()-1);
                }
            }
            catch (MalformedURLException ex) {}
        }
        return href;
    }

    /**
     * @return either the model in whoch the node has a path to the root.
     * or null.
     */
    private static DefaultTreeModel getModelForNode(TreeNode node) {
        TreeNode test = node, root = null;
        for (; test != null; test = test.getParent()) {
            root = test;
        }
        try {
          if (root == getUserModel().getRoot()) {
              return getUserModel();
          }  else if (root == getSampleModel().getRoot()) {
              return getSampleModel();
          }
        } catch (SamplesNotFoundException e) {
          //Ignore
        }
        return null;
    }
    /**
     * Adds a new bookmark.
     */
    static MutableTreeNode addBookmark(MutableTreeNode parent, Bookmark bookmark) {
        DefaultMutableTreeNode thing = new DefaultMutableTreeNode(bookmark);
        thing.setAllowsChildren(false);
        addNew(parent, thing);

        return thing;
    }

    /**
     * Adds a new bookmarks folder.
     * @param parent the parent to add the node to. If null, the root of
     * the user model will be used.
     */
    static MutableTreeNode addFolder(MutableTreeNode parent, String title) {

        DefaultMutableTreeNode thing = new DefaultMutableTreeNode(title);
        thing.setAllowsChildren(true);
        addNew(parent, thing);
        return thing;
    }

    private static synchronized void addNew(MutableTreeNode parent,
                                        MutableTreeNode child){
        DefaultTreeModel tmodel;
        if (parent == null) {
            tmodel = getUserModel();
            tmodel.insertNodeInto(child, (MutableTreeNode) tmodel.getRoot(), 0);
            return;
        } else {
            tmodel = getModelForNode(parent);
        }

        if (parent.getAllowsChildren()) {
            tmodel.insertNodeInto(child, parent, 0);
        } else {
            MutableTreeNode grandma = (MutableTreeNode)parent.getParent();
            tmodel.insertNodeInto(child, grandma, grandma.getIndex(parent)+1);
        }
    }

    static synchronized TreeNode[] getPathToRoot(TreeNode node) {
        TreeNode[] ret = null;
        ret = model.getPathToRoot(node);
        if (ret == null) {
            ret = sampleModel.getPathToRoot(node);
        }
        return ret;
    }

    static synchronized DefaultTreeModel getUserModel() {
        if (model ==null){
            loadUserBookmarks(null);
        }
        return model;
    }

    /**
     * Gets the samples model.
     * @throws <code>SamplesNotFoundException</code> if sample bookmark file not
     * found.
     */
    static synchronized DefaultTreeModel getSampleModel() throws
                                                       SamplesNotFoundException{
        if (sampleModel == null){
            loadSampleBookmarks(null);
        }
        return sampleModel;
    }

    static synchronized void setUserModel(DefaultTreeModel newModel) {
        model = newModel;
    }
    static synchronized void setSampleModel(DefaultTreeModel newModel){
        sampleModel = newModel;
    }

    static synchronized void remove(MutableTreeNode node) {
        TreeNode parentNode = node.getParent();
        DefaultTreeModel tmodel = getModelForNode(node);
        if (parentNode != null){
            try{
                tmodel.removeNodeFromParent(node);
            } catch(Exception e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Saves bookmarks as XML.
     * @param tree the tree (samples or user bookmarks) to save
     * @param sampleModel the sample bookmarks tree model
     */
    static synchronized void saveBookmarks(TreeModel tmodel) {
        try {
            saveBookmarks(new FileOutputStream(getBookmarksFile(tmodel)), tmodel);
        }
        catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Saves bookmarks as XML.
     * @param model the bookmarks tree model
     */
    static void saveBookmarks(OutputStream out, TreeModel model) {
        try {
            IndentWriter w = new IndentWriter(new OutputStreamWriter(out), 0);
            w.println("<?xml version=\"1.0\" standalone=\"yes\"?>");
            w.println("<!-- " + AppEnvironment.getTitle() + " Bookmarks -->");
            w.println("<!-- This is a generated file!  Do not edit. -->");
            w.println("<!-- Last Modified: " + new Date().toString() + " -->");
            w.println();
            if(model!=null){
                writeNode(model, (TreeNode)model.getRoot(), w);
            } else {
                System.out.println("the model does not exist");
            }
            w.println();
        } catch (IOException ex) {
        }
    }

    /**
     * Write a bookmark tree node as XML.
     * @param node the bookmark tree node to print
     * @param w describes how the children should be printed
     */
    private static void writeNode(TreeModel tmodel,
                                TreeNode node, IndentWriter w) throws IOException {
        w.write("<");
        String nodeName = node.getClass().getName();
        if (node instanceof DefaultMutableTreeNode) {
            Object userObject = ((DefaultMutableTreeNode)node).getUserObject();
            if (userObject instanceof Bookmarks.Bookmark) {
                Bookmarks.Bookmark bookmark = (Bookmarks.Bookmark)userObject;
                nodeName = bookmarkElement;
                w.write(nodeName);
                w.write(' ');
                w.write(bookmarkNameAttribute);
                w.write("=\"");
                w.write(encodeEntities(bookmark.getName()));
                w.write("\" ");
                w.write(bookmarkLocationAttribute);
                w.write("=\"");
                w.write(encodeEntities(compressBookmarkURL(bookmark.getLocation(), tmodel)));
                w.write("\" ");
                w.write(bookmarkDescriptionAttribute);
                w.write("=\"");
                w.write(encodeEntities(bookmark.getComment()));
                w.write('"');
            } else {
                String name = "";
                if (userObject instanceof String) {
                    name = (String)userObject;
                }
                nodeName = bookmarkFolderElement;
                w.write(nodeName);
                w.write(' ');
                w.write(bookmarkNameAttribute);
                w.write("=\"");
                w.write(encodeEntities(name));
                w.write('"');
            }
        } else {
            w.write(nodeName);
        }

        if (node.getChildCount() > 0) {
            w.println(">");
            w.indent();
            Enumeration e = node.children();
            while (e.hasMoreElements()) {
                writeNode(tmodel, (TreeNode)e.nextElement(), w);
            }
            w.unindent();
            w.println("</" + nodeName + ">");
        } else {
            w.println("/>");
        }
    }

    /**
     * Convert a string to XML-compliant entity-encoded version.
     * @param s a string to encode
     */
    private static String encodeEntities(String s) {
        StringBuffer sbuf = new StringBuffer();
        if ((s != null) && (s.length() > 0)) {
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '&') {                 // Ampersand
                    sbuf.append("&amp;");
                } else if (c == '<') {          // Less than
                    sbuf.append("&lt;");
                } else if (c == '>') {          // Greater than
                    sbuf.append("&gt;");
                } else if (c == '\"') {         // Double quote
                    sbuf.append("&quot;");
                } else if (c == '\'') {         // Single quote
                    sbuf.append("&apos;");
                } else if ((((int)c & 0xFFFF) < 128) && !Character.isISOControl(c)) {
                    sbuf.append(c);     // Normal 8-bit printable character
                } else if (((int)c & 0xFFFF) < 256) { // 8-bit Unicode (US-ASCII)
                    sbuf.append("&#x");
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                    sbuf.append(";");
                } else {                            // 16-bit Unicode
                    sbuf.append("&#x");
                    sbuf.append(Integer.toString(((c >> 12) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  8) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                    sbuf.append(";");
                }
            }
        }
        return sbuf.toString();
    }


    /**
     * Load user bookmarks from XML file, will also set the internal user model
     * @param in The InputStream to load from. If this is null the default file
     * will be used based on the paramater fileName
     * @param modelType the file type name, either userBookmark or SampleBookmark
     * @return the user bookmarks tree model
     */
    static synchronized TreeModel loadUserBookmarks(InputStream is) {
        DefaultTreeModel t = null;
        try {
        t = loadBookmarks(is, true);
        } catch (SamplesNotFoundException e) {
          //Ignore
        }
        if (t != null) {
            model = t;
        }
        return t;
    }
    /**
     * Load sample bookmarks from XML file, will also set the internal sampleModel
     * @param in The InputStream to load from. If this is null the default file
     * will be used based on the paramater fileName
     * @param modelType the file type name, either userBookmark or SampleBookmark
     * @return sample bookmarks tree model.
     * @throws <code>SamplesNotFoundException</code> if sample bookmark file not
     * found.
     */
    static synchronized TreeModel loadSampleBookmarks(InputStream is)
                                  throws SamplesNotFoundException {
        DefaultTreeModel t = loadBookmarks(is, false);
        if (t != null) {
            sampleModel = t;
        }
        return t;
    }

    /**
     * Load bookmarks from XML file.
     * @param in The InputStream to load from. If this is null the default file
     * will be used based on the paramater fileName
     * @param userModel do we want to load userBookmark (if false, sampleBookmarks)
     * @return the user bookmarks tree model or sample bookmarks tree model.
     * @throws <code>SamplesNotFoundException</code> if sample bookmark file not
     * found.
     */
    private static synchronized DefaultTreeModel loadBookmarks(
                                                   InputStream is,
                                                   final boolean userModel)
                                                   throws
                                                   SamplesNotFoundException {
        final DefaultMutableTreeNode root = new DefaultMutableTreeNode();
        InputStream in = is;
        final File fs;

        if (userModel) {
            root.setUserObject(getString(USER_BOOKMARKS_NAME));
            fs = getUserBookmarksFile();
        } else {
            root.setUserObject(getString(SAMPLES_DIRECTORY_NAME));
            fs = getSampleBookmarksFile();
            if (!fs.exists()) {
              throw new SamplesNotFoundException();
            }
        }
        root.setAllowsChildren(true);

        try {
            if (in == null) {
                in = new FileInputStream(fs);
            }
            XMLReader parser = SAXParserFactory.newInstance().newSAXParser().getXMLReader();
            DefaultHandler docHandler = new DefaultHandler() {
                // Track element nodes as document is parsed.
                private DefaultMutableTreeNode currentNode = root;
                private boolean isRootNode = true;

                public void startElement(String uri, String localName, String qname,
                   Attributes attrs) throws SAXException {
                    if (isRootNode) {
                        // Root Element:
                        isRootNode = false;
                    } else if (bookmarkFolderElement.equals(qname)) {
                        // Folder Element:
                        DefaultMutableTreeNode folder = new DefaultMutableTreeNode();
                        folder.setAllowsChildren(true);
                        folder.setUserObject(attrs.getValue(bookmarkNameAttribute));
                        currentNode.add(folder);
                        currentNode = folder;
                    } else if (bookmarkElement.equals(qname)) {
                        // Bookmark Element:
                        DefaultMutableTreeNode bookmark = new DefaultMutableTreeNode();
                        bookmark.setAllowsChildren(false);
                        String name = attrs.getValue(bookmarkNameAttribute);
                        String href = attrs.getValue(bookmarkLocationAttribute);
                        //insert the install directory in front of relative sample directory.
                        if(href.indexOf(":")<0){
                            href = "file://" + System.getProperty("user.dir") + "/" + href;
                            href = href.replace('\\','/');
                        }
                        href = expandBookmarkURL(href, fs);
                        String descr = attrs.getValue(bookmarkDescriptionAttribute);
                        bookmark.setUserObject(
                            new Bookmarks.Bookmark(((name != null) ? name : ""),
                                                    ((href != null) ? href : ""),
                                                    ((descr != null) ? descr : ""),
                                                    bookmarkLoader, userModel));
                        currentNode.add(bookmark);
                    }
                }

                public void endElement(String uri, String localName, String qname) throws SAXException {
                    if (bookmarkFolderElement.equals(qname) && (currentNode.getParent() != null)) {
                        currentNode = (DefaultMutableTreeNode)currentNode.getParent();
                    }
                }

                // treat validation errors as fatal
                public void error (SAXParseException err) throws SAXParseException {
                    throw err;
                }

                // dump warnings too
                public void warning (SAXParseException err) throws SAXParseException {
                    throw err;
                }
            };
            parser.setContentHandler(docHandler);
            parser.setErrorHandler(docHandler);
            parser.parse(new InputSource(in));
        } catch (IOException ex) {
            // Ignore
        } catch (SAXParseException ex) {
            MessageWriters.DetailMsg.println("*** Bookmarks.loadBookmarks: ParseException: "
                                           + "line=" + ex.getLineNumber()
                                           + ", uri=" + ex.getSystemId()
                                           + ":\n    " + ex.getMessage());
        } catch (SAXException ex) {
            // Ignore
        } catch (FactoryConfigurationError ex) {
            MessageWriters.DetailMsg.println("*** " + ex);
        } catch (ParserConfigurationException ex) {
            MessageWriters.DetailMsg.println("*** " + ex);
        }
        return new DefaultTreeModel(root);
    }


    /**
     * Inner class: A container for bookmark data.
     */
    static class Bookmark implements Serializable {
        private String name;
        private String location;
        private String comment;
        private Action action = null;
        private boolean isUserBookmark = true;

        /**
         * Creates a Bookmark instance.
         * @param name the identifying label for the bookmark
         * @param location the URL of the bookmark
         * @param comment the descriptive text string for the bookmark
         */
        Bookmark(String name, String location, String comment,
                    BookmarkLoader bl, boolean userBookmark) {
            set(name, location, comment);
            isUserBookmark = userBookmark;
        }

        public boolean isUserBookmark() {
            return isUserBookmark;
        }

        /**
         * Adds a new entry
         * @param name the bookmark's name
         * @param location the bookmark's location
         * @return <code>true</code> if name was valid
         */
        public void set(String name, String location, String comment) {
            this.name = name;
            this.location = location;
            /* Set file:// to file:// "//" supported by all emulators */
            if (location.startsWith("file://")) {
              if(!location.startsWith("file://")) {
                this.location = "file://" + location.substring(7);
              }
            }

            this.comment = comment;
            if (action != null) {
                setUpAction();
            }
        }

        /**
         * Returns the specified name
         */
        public String getName() {
            return name;
        }

        /**
         * Returns the specified location
         */
        public String getLocation() {
            return location;
        }

        /**
         * Returns the specified comment
         */
        public String getComment() {
            return comment;
        }

        public String toString() {
            return getName();
        }

        /**
         * Returns action which connects to the bookmark's location
         */
        public Action getAction() {
            if (action == null) {
                action = new BookmarkAction();
                setUpAction();
            }
            return action;
        }

        /**
         * Checks whether string is null or empty or consists of spaces
         */
        private static boolean isEmpty(String s) {
            return (s == null || s.length() == 0 || s.trim().length() == 0);
        }

        /**
         * Sets up the action attributes, such as name and enabled state
         */
        private void setUpAction() {
            action.putValue(Action.NAME, name);
            action.putValue(Action.SHORT_DESCRIPTION, comment);

            // disable bookmarks with invalid URL
            try {
                boolean enable = false;
                if (bookmarkLoader.isSchemaRequest(location)) {
                    URL url = new URL(location);
                    // disable "Go" for rudimentary URLs like "http://"
                    enable = !(isEmpty(url.getFile()) && isEmpty(url.getHost()));
                }
                action.setEnabled(enable);
            } catch (Exception x) {
                // malformed URL
                action.setEnabled(false);
            }
        }

        private class BookmarkAction extends AbstractAction implements Runnable {
            BookmarkAction() {
                super(Bookmark.this.name);
            }
            public void actionPerformed(ActionEvent e) {
                SwingUtilities.invokeLater(this);
            }

            public void run() {
                bookmarkLoader.loadContent(getLocation());
            }
        }
    }

    static class BookmarkCellRenderer extends DefaultTreeCellRenderer {
        private static final String EXPANDED_PROPERTY = "bookmarks.show-location";
        private static Property expandedProp;

        static {
            PropertySet ps = AppEnvironment.getUserProperties();
            expandedProp = ps.getProperty(EXPANDED_PROPERTY);
            if (expandedProp == null) {
                expandedProp = ps.addProperty(EXPANDED_PROPERTY,
                                              Boolean.class,
                                              Boolean.TRUE,
                                              true);
            }
        }

        final private JPanel fullThing_ = new JPanel(new BorderLayout());
        final private JLabel nameLabel_ = new JLabel();
        final private JLabel locationLabel_ = new JLabel();

        private Icon rootIcon;

        public BookmarkCellRenderer() {
            JPanel labels = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 0));
            labels.add(nameLabel_);
            nameLabel_.setFont(LookFactory.getStandardBoldFont());
            labels.add(locationLabel_);
            labels.setOpaque(false);
            fullThing_.add(labels, BorderLayout.CENTER);
            locationLabel_.setForeground(Color.gray);
            locationLabel_.setFont(LookFactory.getStandardCodeFont());
            nameLabel_.setText("");
            locationLabel_.setText("");
            rootIcon = this.getClosedIcon();
        }

        public Component getTreeCellRendererComponent(JTree tree,
                    Object value, boolean selected, boolean expanded,
                    boolean leaf, int row, boolean hasFocus) {
            tree.setRowHeight(-1);
            super.getTreeCellRendererComponent(tree, value, selected, expanded,
                            leaf, row, hasFocus);
            Object val = ((DefaultMutableTreeNode) value).getUserObject();

            if (!selected) {
                fullThing_.setBackground(super.getBackgroundNonSelectionColor());
                nameLabel_.setForeground(super.getTextNonSelectionColor());
            } else {
                fullThing_.setBackground(super.getBackgroundSelectionColor());
                nameLabel_.setForeground(super.getTextSelectionColor());
            }
            if (val instanceof Bookmarks.Bookmark) {
                Bookmarks.Bookmark bk = (Bookmarks.Bookmark)val;
                this.setText("");
                fullThing_.add(this, BorderLayout.WEST);
                String txt = bk.getComment();
                nameLabel_.setText(bk.getName());
                if (getLocationExpanded()) {
                    locationLabel_.setText(bk.getLocation());
                } else {
                    locationLabel_.setText("");
                    if (txt == null || "".equals(txt)) {
                        txt = bk.getLocation();
                    } else {
                        txt = bk.getLocation() + "     " + bk.getComment();
                    }
                }
                fullThing_.setToolTipText(txt);
                return fullThing_;
            } else {
                if (row == 0) {
                    this.setIcon(this.getRootIcon());
                } else if (((DefaultMutableTreeNode) value).getChildCount() == 0) {
                    this.setIcon(this.getClosedIcon());
                }
                setToolTipText(null);
            }
            return this;
        }

        /* Note that changing this value will <b>not</b> force a redraw
         * on the tree - a change will only become apparent after redrawing
         * the tree.
         */
        public static boolean getLocationExpanded() {
            return ((Boolean)expandedProp.getValue()).booleanValue();
        }
        public static Property getExpandedProperty() {
            return expandedProp;
        }
        public void setRootIcon(Icon rI) {
            rootIcon = rI;
        }
        public Icon getRootIcon() {
            return rootIcon;
        }
    }
}

