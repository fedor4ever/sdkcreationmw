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

/* com.nokia.wtk.app */
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationWindow;

/* java.awt */
import java.awt.Insets;

/* java.awt.event */
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.gui.MenuButton;
import com.nokia.wtk.util.gui.RolloverJButton;

/* java.awt.event */
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;

/* javax.swing */
import javax.swing.Action;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JComponent;
import javax.swing.SwingUtilities;
import javax.swing.AbstractAction;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;


/**
 * Application class for showing bookmarks in its own window. This is the facade
 * class for tools using bookmarks. The sequence of operations to attach
 * bookmarks to tools are:
 * <p><blockquote><pre>
 *    BookmarkApp bApp = BookmarkApp.getBookmarkApp(bookmarkLoader);
 *    bApp.setTitle("Mices Bookmarks");
 *    JComponent c = bApp.getBookmarksMenuComponent();
 * </blockquote></pre>
 */
public class BookmarkApp {

  /* The shared instance */
  private static BookmarkApp bookApp = null;

  /* The application window */
  private ApplicationWindow appWin = null;

  /* The Bookmark view */
  private BookmarkView bookView = null;

  /** The toolbar component */
  private MenuButton menuButton = null;

  /**
   * Gets the singleton Bookmark Application.
   * @param loader the loader for the bookmarks.
   * <code>loader</code> should not be <code>null</code> upon the first
   * call of this method. Subsequent calls can have <code>null</code> value of
   * <code>loader</code>
   * @return the shared <code>BookmarkApp</code>
   * @throws <code>NullPointerException</code> if <code>loader</code> is
   * <code>null</code> upon the first call of this method.
   */
  public static synchronized BookmarkApp getBookmarkApp(BookmarkLoader loader) {
    if (bookApp == null) {
      bookApp = new BookmarkApp(loader);
    }
    return bookApp;
  }

  /**
   * Creates the bookmakrs menu button for quickly adding a bookmark,
   * editing and loading them.
   * @return the bookmark menu component
   */
  public final JComponent getBookmarksMenuComponent() {
      if (menuButton != null) {
        return menuButton;
      }
      menuButton = new MenuButton(
                      BookmarkView.getResource(Resource.BOOKMARK_BUTTON_LABEL));

      final JMenuItem addMark = new
       JMenuItem(BookmarkView.getResource(Resource.BOOKMARK_MENU_ADD_LABEL));
      final JMenuItem editMarks = new
        JMenuItem(BookmarkView.getResource(Resource.BOOKMARK_MENU_EDIT_LABEL));
      JPopupMenu pop = menuButton.getPopupMenu();
      RolloverJButton.transformToRollover(menuButton);
      menuButton.setMargin(new Insets(0, 0, 0, 0));

      addMark.addActionListener(addMarkAction);
      editMarks.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
              show();
          }
        });

      pop.addPopupMenuListener(new PopupMenuListener() {
          public void popupMenuCanceled(PopupMenuEvent e) {
              ((JPopupMenu) e.getSource()).removeAll();
          }
          public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {
              ((JPopupMenu) e.getSource()).removeAll();
          }
          public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
              JPopupMenu thisPop =  (JPopupMenu) e.getSource();
              thisPop.add(addMark);
              thisPop.add(editMarks);
              thisPop.addSeparator();

              // add items directly to thisPop
              int before = thisPop.getComponentCount();
              buildMenu(thisPop);

              // if there's no bookmarks, add a bogus menu item
              if (thisPop.getComponentCount() == before) {
                  JMenuItem empty = new JMenuItem("(Empty)");
                  empty.setEnabled(false);
                  thisPop.add(empty);
              }
          }
      });
      menuButton.setFont(LookFactory.getStandardPlainFont());
      return menuButton;
   }

  /**
   * Sets the title of the Bookmark application.
   * @param title  the title for the bookmark application.
   */
  public void setTitle(final String title) {
    if (title == null) {
      return;
    }
    try {
      if (SwingUtilities.isEventDispatchThread()) {
        appWin.setWindowTitle(title);
      } else {
        SwingUtilities.invokeLater( new Runnable() {
          public void run() {
            appWin.setWindowTitle(title);
          }
        });
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  /**
   * Creates the appliction window for bookmarks.
   * @param loader the loader for bookmarks app.
   * @throws <code>NullPointerException</code> if <code>loader</code> is
   * <code>null</code> upon the first call of this method.
   */
  private BookmarkApp(BookmarkLoader loader) {
    if (loader == null) {
      throw new NullPointerException("Bookmark loader is null.");
    }
    bookView = new BookmarkView(loader);
    appWin = createWindow();
    appWin.setApplicationComponent(bookView);
    /*@todo: add propertyChangelisteners */
    if (AppEnvironment.getAppEnvironment() != null) {
        AppEnvironment.getAppEnvironment().getUserProperties().addProperties(
            BookmarkView.getProperties(), true);
    }
  }

  /**
   * Shows the bookmark application.
   */
  private void show() {
    appWin.setWindowVisible(true);
  }

  /**
   * Action implementation of "addBookmark" command
   */
  private Action addMarkAction =  new AbstractAction() {
      public void actionPerformed(ActionEvent e) {
        String location = getBookmarkView().getBookmarkLoader().getLocation();
          if (location == null) {
              //ErrorMsg.println("The history stack is empty.");
              //System.out.println("The history stack is empty.");
          } else {
              addBookmark("", location);
          }
      }
  };

  /**
   * Adds a bookmark. This will prompt the user for more information before
   * adding the bookmark, giving the user a chance ot bail out.
   * @param name     the name of the bookmark
   * @param location the url location of the bookmark
   */
  private void addBookmark(String name, String location) {
    getBookmarkView().addBookmark(name, location);
  }

  /**
   * Builds the bookmark menu items.
   * @param jMenu the popup menu where menu items to be added
   */
  private void buildMenu(JPopupMenu jMenu) {
    getBookmarkView().buildMenu(jMenu);
  }

  /**
   * Gets the <code>BookmarkView</code>
   * @return the <code>BookmarkView</code>
   */
  private BookmarkView getBookmarkView() {
    return bookView;
  }

  /**
   * Creates the application window for showing bookmarks.
   * @return the application window for bookmarks.
   */
  private ApplicationWindow createWindow() {
    final ApplicationWindow win = new ApplicationWindow();
    win.setWindowTitle(BookmarkView.getTitle());
    win.getFrame().addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
          win.getFrame().setVisible(false);
      }
    });

    //appWin.setWindowIcon(new ImageIcon(((Frame)top).getIconImage()));
    return win;
  }
}
