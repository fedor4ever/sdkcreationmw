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


package com.nokia.wtk.diagnostics.gui;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.File;
import java.util.Properties;
import java.util.Vector;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;
import javax.swing.OverlayLayout;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationWindow;
import com.nokia.wtk.app.FilingManager;
import com.nokia.wtk.bookmarks.BookmarkApp;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.Slider;
import com.nokia.wtk.diagnostics.views.TexturedArea;
import com.nokia.wtk.diagnostics.views.ViewPanel;
import com.nokia.wtk.diagnostics.views.DummyView;
import com.nokia.wtk.diagnostics.views.ViewsManager;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;
import com.nokia.wtk.diagnostics.views.support.ContentDelayedHeartbeat;
import com.nokia.wtk.diagnostics.views.support.InvokeLaterQueueable;
import com.nokia.wtk.diagnostics.views.support.ErrorLogDialog;
import com.nokia.wtk.diagnostics.views.support.Misc;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.gui.MenuButton;
import com.nokia.wtk.util.gui.RolloverJButton;

/**
 * A class for managing and showing the diagnostic part of the browser.
 * Will assemble itself with full browser buttons, location entry component,
 * menu bar, and tabbed content panes.
 */
public class MainWindow
    extends JPanel
    implements MessageWriters, Resource, CommandNames {

    private class BigSmallFlexChangeListener
        implements PropertyChangeListener {

        final private ViewsManager.ViewState state_;

        BigSmallFlexChangeListener(ViewsManager.ViewState state) {
            state_ = state;
        }

        public void propertyChange(PropertyChangeEvent e) {
            try {
                boolean state = ((Boolean) e.getNewValue()).booleanValue();
                state_.setFlexible(state);
                MainWindow.this.resizeWinToComponent_.queue();
            } catch (ClassCastException ex) {
                System.err.println(
                    "MainWindow class cast except: state was not boolean");
            }
        }
    }

    private abstract class Property extends PropertySet.BasicProperty {
        private PropertyChangeSupport propChange = null;

        Property(String path, Class type, boolean archive) {
            super(path, type, archive);
        }

        /**
         * Adds a PropertyChangeListener to the listener list.
         * @param l The PropertyChangeListener to be added
         */
        public void addPropertyChangeListener(PropertyChangeListener l) {
            if (propChange == null)
                createPropertyChangeSupport();
            propChange.addPropertyChangeListener(getName(), l);
        }

        /**
         * Creates PropertyChangeSupport
         */
        private synchronized void createPropertyChangeSupport() {
            if (propChange == null) {
                propChange = new PropertyChangeSupport(this);
            }
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(boolean oldValue, boolean newValue) {
            if (oldValue != newValue) {
                firePropertyChange(
                    Utils.getBoolean(oldValue),
                    Utils.getBoolean(newValue));
            }
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(Object oldValue, Object newValue) {
            if (propChange != null) {
                // create our own property change event to make
                // this property the source of the event
                propChange.firePropertyChange(
                    new PropertyChangeEvent(
                        this,
                        getName(),
                        oldValue,
                        newValue));
            }
        }

        /**
         * Removes a PropertyChangeListener from the listener list.
         * @param l The PropertyChangeListener to be removed
         */
        public void removePropertyChangeListener(PropertyChangeListener l) {
            if (propChange != null) {
                propChange.removePropertyChangeListener(getName(), l);
            }
        }
    }

    /**
     * This factory will take a class name and be ready to produce all
     * elements that go into making a pod. As such it concentrates Pod making
     * into one place, and is the link between a class name and the resources
     * in the properties file wehere we set all this.
     */
    private class PropertyLinkedPanelsFactory
        implements AbstractViewFactoryInterface {

        private String id;
        private AbstractViewFactoryInterface factory;
        private ViewAndControlsInterface viewAndControls;
        private PropertySet properties;

        PropertyLinkedPanelsFactory(String s) {
            id = s;
        }

        public String getKey() {
            return id;
        }

        public PropertySet getViewProperties() {
            if (properties == null) {
                getRealViewFactory();
                if (factory != null) {
                    properties = factory.getViewProperties();
                } else {
                    properties = new PropertySet();
                }
            }
            return properties;
        }

        public boolean getDefaultViewOpen() {
            return AppEnvironment.getBoolean(id + ".open.default", false);
        }

        public int getDefaultViewWidth() {
          return AppEnvironment.getInt(id + ".width.default");
        }

        public int getDefaultViewHeight() {
          return AppEnvironment.getInt(id + ".height.default");
        }

        public String getViewTitle() {
            return AppEnvironment.getString(id + ".title.short");
        }

        public String getViewDescription() {
            return AppEnvironment.getString(id + ".title.long");
        }

        public ViewAndControlsInterface getPodComponent() {
            getRealViewFactory();
            if (viewAndControls == null) {
                if (factory != null) {
                    try {
                        viewAndControls = factory.getPodComponent();
                    } catch (Throwable x) {
                        ErrorMsg.println("Cannot instantiate "+id+" view: "+x);
                        ErrorMsg.printStackTrace(x);
                    }
                }
                if (viewAndControls == null) {
                    String text = "Cannot instantiate " + id + " view";
                    viewAndControls = new DummyView(text);
                }
            }
            return viewAndControls;
        }

        public void bindToCommunicator(InterViewCommunicator comm) {
            AbstractViewFactoryInterface f = getRealViewFactory();
            if (f != null) f.bindToCommunicator(comm);
        }

        public void unbindFromCommunicator(InterViewCommunicator comm) {
            AbstractViewFactoryInterface  f = getRealViewFactory();
            if (f != null) f.unbindFromCommunicator(comm);
        }

        AbstractViewFactoryInterface getRealViewFactory() {
            if (factory == null && viewAndControls == null) {
                String className = AppEnvironment.getString(id + ".class");
                Object o = instantiateFromClassName(commands_,className,id);
                if (o instanceof AbstractViewFactoryInterface) {
                    factory = (AbstractViewFactoryInterface)o;
                } else if (o instanceof ViewAndControlsInterface) {
                    viewAndControls = (ViewAndControlsInterface)o;
                }
            }
            return factory;
        }

        public JComponent getSmallPodComponent() {
            AbstractViewFactoryInterface f = getRealViewFactory();
            if (f != null) {
                return f.getSmallPodComponent();
            } else {
                String className = AppEnvironment.getString(id + ".small.class");
                JComponent c =
                    (JComponent) instantiateFromClassName(commands_,className,id);
                Misc.setOpaque(c, false);
                return c;
            }
        }
    }

    /**
     * Returns the {@link getInterViewCommunicator} shared bwtween the views.
     * @return the instance of {@link getInterViewCommunicator}
     */
    private InterViewCommunicator getInterViewCommunicator() {
        if (communicator_ == null) {
            communicator_ = new InterViewCommunicator();
        }
        return communicator_;
    }

    /**
     * Instantiate an object from a classname, trying the following
     * instantiation methods in order <br>
     *   The sequence:<ul>
     *   <li>View(Browser)</li>
     *   <li>View(Browser, Commands)</li>
     *   <li>View()</li>
     *   <li>static View.getInstance</li>
     *   </ul>
     * The return type is the very generic Object, so the calling call
     * should know what to expect. (Which it will since it supplies the class
     * name).
     * @param className the name of the class to instantiate
     * @return the object of class className or the error parameter
     */
    private static Object instantiateFromClassName(
        Commands commands,
        String className,
        String id) {

        Object view;
        try {
            if (verbose)
                debugTrace("Instantiating " + className);

            Class klass = Class.forName(className);
            try {
                Constructor constructor =
                    klass.getConstructor(new Class[] { Browser.class });
                view =
                    constructor.newInstance(
                        new Object[] { commands.getBrowser()});
            } catch (InvocationTargetException y) {
                throw y.getTargetException();
            } catch (Exception x1) {
                try {
                    Constructor constructor =
                        klass.getConstructor(
                            new Class[] { Browser.class, Commands.class });
                    view =
                        constructor.newInstance(
                            new Object[] {
                                commands.getBrowser(),
                                commands });
                } catch (InvocationTargetException y) {
                    throw y.getTargetException();
                } catch (Exception x2) {
                    try {
                        view =
                            (ViewAndControlsInterface) klass.newInstance();
                    } catch (ExceptionInInitializerError y) {
                        throw y.getException();
                    } catch (Exception x3) {
                        Method getInstance =
                            klass.getMethod("getInstance", (Class[])null);
                        try {
                            view = (ViewAndControlsInterface)
                                getInstance.invoke(null, (Object[])null);
                        } catch (InvocationTargetException y) {
                            throw y.getTargetException();
                        }
                    }
                }
            }
        } catch (Throwable x) {
            ErrorMsg.println("Cannot instantiate " + id + " view: " + x);
            ErrorMsg.printStackTrace(x);
            return null;
        }
        return view;
    }

    static {
        AppEnvironment.addResources(
            "resources/MainWindow.properties",
            MainWindow.class);
    }

    /** Resource key for bookmarks window title */
    private static final String BOOKMARKS_TITLE = "bookmarksview.title.prefix";

    private ViewsManager podManager;
    private PropertySet properties;

    /** Resource keys used in resource file lookups */
    private static final String toolkitViewsKey = "toolkit.views";

    /**
     * Prints debug trace to console and DebugMsg writer is verbose flag is set
     */
    private static void debugTrace(String s) {
        if (verbose) {
            DebugMsg.println(s);
        }
    }

    private ApplicationWindow appWindow_;
    private BookmarkApp bookmarkApp_;
    private Commands commands_;
    private InterViewCommunicator communicator_;

    //new
    private ComponentAdapter compSizeListener_ = null;

    protected ComponentListener getCompSizeListener() {
        if (compSizeListener_ == null) {
            compSizeListener_ = new ComponentAdapter() {

              public void componentResized(ComponentEvent e) {
                resizeWinToComponent_.queue();
          }
        };
      }
      return compSizeListener_;
    }

    public Browser getBrowser() {
        return commands_.getBrowser();
    }
    private PropertyChangeListener dragListener_;

    private ContentDelayedHeartbeat heartbeat_;
    private Vector viewStates_ = new Vector();

    private Property homePageProp;
    final private InvokeLaterQueueable resizeWinToComponent_ =
        new InvokeLaterQueueable() {
        public void doRun() {
          ApplicationWindow appWindow = getApplicationWindow();
          if (appWindow != null) {
            appWindow.getFrame().pack();
          }
        }
    };

    private PropertyChangeListener viewChangeListener_;

    /**
     * Creates a MainWindow, if already created, or will create a new
     * one with parameters supplied.
     * @param browser the browser instance to use as a model
     * @param ps the propertySet containing information about previous
     * incarnations of the MainWindow
     */
    public MainWindow(Browser browser, PropertySet ps) {
        ErrorLogDialog.startErrorDialogs();

        homePageProp =
            new Property(
                ToolkitPreferences.HOME_PAGE_PROPERTY,
                String.class,
                true) {
            Object value = null;

            public Object getValue() {
                return value;
            }
            public void setValue(Object value) {
                Object oldVal = this.value;
                this.value = value;
                super.firePropertyChange(oldVal, value);
            }
        };

        properties = ps;

        // Add the home page property so it available for the constructor of
        // goHomeAction
        properties.addProperty(homePageProp, true);

        String[] commandNames =
            {
                exitAction,
                reloadDeckAction,
                goPreviousAction,
                goHomeAction,
                refreshAction,
                stopAction,
                gotoURLAction,
                preferencesAction,
                showHelpAction,
                searchHelpCommand,
                };
        commands_ = new Commands(commandNames, browser, ps);

        setLayout(new BorderLayout());

        // Initializes Pods
        podManager = new ViewsManager();
        podManager.getComponent().setOpaque(false);

        // Initializes Location Entry Field
        // This has to be done after commands_ have been created, because it
        // needs the "Go" action
        JComponent toolPane = createToolPane(podManager);
        int contentPaneHeight = (int) podManager.getComponent().getPreferredSize().getHeight();

        add(toolPane, BorderLayout.NORTH);
        add(podManager.getComponent(), BorderLayout.CENTER);
        setBorder(BorderFactory.createLoweredBevelBorder());

        appWindow_ = new ApplicationWindow();
        appWindow_.setApplicationComponent(this);

        // undo the damage done by ApplicationWindow which sets the preferred
        // size of the content pane to the hardcoded value 580x400
        ((JComponent)getParent()).setPreferredSize(null);

        appWindow_.getFrame().addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                boolean sizeUpdated = false;
                Component c = e.getComponent();
                Dimension size = c.getSize();
                Dimension min = c.getMinimumSize();
                if (size.width < min.width || size.height < min.height) {
                    size.width = Math.max(size.width, min.width);
                    size.height = Math.max(size.height, min.height);
                    sizeUpdated = true;
                }

                boolean haveExpandedView = false;
                ViewPanel[] vp = podManager.getViewPanels();
                int n = vp.length;
                for (int i=0; i<n; i++) {
                    if (vp[i].isVisible() && vp[i].isBigPanelShowing()) {
                        haveExpandedView = true;
                        break;
                    }
                }

                if (!haveExpandedView) {
                    Dimension pref = c.getPreferredSize();
                    if (size.height != pref.height) {
                        size.height = pref.height;
                        sizeUpdated = true;
                    }
                }
                if (sizeUpdated) {
                    e.getComponent().setSize(size);
                }
            }
        });

        /* Set the title of bookmarks window */
        appWindow_
            .getFrame()
            .addPropertyChangeListener("title", new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent p) {
                String title = (String) p.getNewValue();
                if (title != null) {
                    getBookmarkApp().setTitle(
                        AppEnvironment.getString(BOOKMARKS_TITLE) + title);
                }
            }
        });

        FilingManager fm;
        if ((fm = AppEnvironment.getAppEnvironment().getFilingManager())
            != null) {
            fm.setCurrentDirectory(
                new File(
                    System.getProperty("user.dir")
                        + File.separator
                        + "samples"));
        }

        // Set default width
        if (ps.getProperty("size") == null) {
            appWindow_.setWindowSize(new Dimension(415,390));
        }

        ps.addProperties(appWindow_.getProperties(), true);
        appWindow_.setWindowTitle(
            AppEnvironment.getString(AppEnvironment.appTitle));
    }

    private void addKeyedMenuItem(JPopupMenu pop, String key) {
        JMenuItem mi = pop.add(new Commands.EmptyAction());
        mi.putClientProperty(EMSMenuListener.NAME_PROPERTY_KEY, key);
    }

    /**
     * Adds a view to the diagnostics main window
     * @param viewFactory the view factory interface
     */
    public void addView(AbstractViewFactoryInterface viewFactory) {

        ViewPanel bSPanel = new ViewPanel(viewFactory);
        PropertySet podProps = bSPanel.getProperties();
        ViewsManager.ViewState state = podManager.addComponent(bSPanel);
        viewStates_.add(state);
        podProps.addPropertyChangeListener(
            ViewPanel.Open_Property,
            new BigSmallFlexChangeListener(state));
        podProps.addPropertyChangeListener(
            ViewPanel.Visible_Property,
            getViewChangeListener());
        podProps.addPropertyChangeListener(
            ViewPanel.Size_Property,
            getViewChangeListener());
        viewFactory.bindToCommunicator(getInterViewCommunicator());
        String viewKey = viewFactory.getKey();
        if (viewKey != null) {
            if (properties.getProperty(viewKey + "." +
                ViewPanel.Visible_Property) == null) {
                bSPanel.setVisible(true);
            }
            if (properties.getProperty(viewKey + "." +
                ViewPanel.Open_Property) == null) {
                bSPanel.setBigPanelShowing(viewFactory.getDefaultViewOpen());
            }
            properties.addPropertySet(viewKey, podProps, true);
        }
        state.setFlexible(
            podProps.getBooleanValue(ViewPanel.Open_Property, false));
        bSPanel.getSlider().addPropertyChangeListener(
            Slider.DRAGGING_PROPERTY,
            getDragListener());
    }

    /**
     * Creates views from the property file in the old diagnostics format
     * @param p the properties
     */
    public void addViewsFromProperties(Properties p) {
        // added to be able to use AppEnvironment services
        AppEnvironment.addResources(p, MainWindow.class);
        String[] views = StringUtils.tokenize(p.getProperty(toolkitViewsKey)," ");
        for (int i = 0; i < views.length; i++) {
            addView(new PropertyLinkedPanelsFactory(views[i]));
        }
        resizeWinToComponent_.queue();

    }

    /**
     * Fill the tool pane with visible toolbar components.
     * @param podManager the ViewsManager object from which to take the pods
     * visible/invisible menu.
     */
    private JComponent createToolPane(ViewsManager podManager) {

        BrowserComponentsFactory bcf = null;
        JPanel ret = new JPanel(new BorderLayout());
        JComponent tmp;

        if (AppEnvironment.getBoolean("mainWindow.toolbar.visible", true)) {
            JComponent toolPane = new JPanel();

            tmp = makeSausagePane();
            if (tmp != null) {
                JComponent tmp2 = tmp;
                tmp = new JPanel();

                OverlayLayout card = new OverlayLayout(toolPane);
                toolPane.setLayout(card);
                toolPane.add(tmp);
                toolPane.add(tmp2);

            } else {
                tmp = toolPane;
            }
            ret.setBackground(Color.white);

            tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
            tmp.add(getBookmarkApp().getBookmarksMenuComponent());

            if (AppEnvironment
                .getBoolean(
                    "mainWindow.toolbar.browserbuttons.visible",
                    true)) {
                bcf =
                    new BrowserComponentsFactory(
                        commands_.getBrowser(),
                        commands_,
                        properties);
                tmp.add(bcf.getBrowserButtons());
            }
            tmp.add(Box.createHorizontalGlue());
            tmp.add(makeMenuButton(podManager));

            int leftMargin = AppEnvironment.getInt(TOOLBAR_LEFT_MARGIN);
            int rightMargin = AppEnvironment.getInt(TOOLBAR_RIGHT_MARGIN);
            tmp.setBorder(
                BorderFactory.createEmptyBorder(0, leftMargin, 0, rightMargin));
            ret.add(toolPane, BorderLayout.CENTER);
            Misc.setOpaque(toolPane, false);
        }

        if (AppEnvironment
            .getBoolean("mainWindow.urlEntrybar.visible", true)) {
            if (bcf == null) {
                bcf =
                    new BrowserComponentsFactory(
                        commands_.getBrowser(),
                        commands_,
                        properties);
            }
            tmp = bcf.getLocationEntryBar(commands_);
            ret.add(tmp, BorderLayout.NORTH);
        }

        ret.setOpaque(true);
        return ret;
    }

    /**
     * Provide access to superclass method from non-derived classes.
     * @see JComponent#firePropertyChange(String, Object, Object)
     */
    protected final void firePropertyChange(
        String propertyName,
        Object oldValue,
        Object newValue) {
        super.firePropertyChange(propertyName, oldValue, newValue);
    }

    public ApplicationWindow getApplicationWindow() {
        return appWindow_;
    }

    BookmarkApp getBookmarkApp() {
        if (bookmarkApp_ == null) {
            bookmarkApp_ = BookmarkApp.getBookmarkApp(commands_);
        }
        return bookmarkApp_;
    }

    private PropertyChangeListener getDragListener() {
        if (dragListener_ == null) {
            dragListener_ = new PropertyChangeListener() {

                public void propertyChange(PropertyChangeEvent pe) {
                    Slider sl = (Slider) pe.getSource();
                    boolean newVal =
                        ((Boolean) pe.getNewValue()).booleanValue();
                    if (newVal) {
                      sl.getComponent().addComponentListener(getCompSizeListener());
                    } else {
                      sl.getComponent().removeComponentListener(getCompSizeListener());
                    }
                }
            };
        }
        return dragListener_;
    }

    /**
     * Gets the main Window Preferences.
     * @return the set of preferences
     */
    public PropertySet getPreferences() {
        return properties;
    }

   // we prefer to retain our width
    public Dimension getPreferredSize() {
        Dimension prefSize = super.getPreferredSize();
        int width = getWidth();
        if (width != 0) prefSize.width = width;
        return prefSize;
    }

    private PropertyChangeListener getViewChangeListener() {
        if (viewChangeListener_ == null) {
            viewChangeListener_ = new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent e) {
                    MainWindow.this.resizeWinToComponent_.queue();
                }
            };
        }
        return viewChangeListener_;

    }

    /**
     * Make the MenuButton with all the Save/Edit/ViewPanel (in)visible
     * options
     * @param podManager the ViewsManager to get the menu item for pod
     * visibility from
     * @return MenuButton a menu button, Rollover transformed, to use in
     * a toolbar
     */
    // Need never be focussable. Ever. Trust me. If the user tabs
    // to this menu, nothing that could enable a single element on
    // this button will be enabled. It is basically a menubar in
    // the wrong place. (Somehow this should be giving me pause...)
    private MenuButton makeMenuButton(ViewsManager podManager) {
        MenuButton ret =
            new MenuButton(AppEnvironment.getString(TEXT_MENU_NAME));
        JPopupMenu pop = ret.getPopupMenu();
        addKeyedMenuItem(pop, SaveableMenuListener.SAVE_KEY);
        pop.add(new JSeparator());
        addKeyedMenuItem(pop, EMSMenuListener.CUT_KEY);
        addKeyedMenuItem(pop, EMSMenuListener.COPY_KEY);
        addKeyedMenuItem(pop, EMSMenuListener.PASTE_KEY);
        pop.add(new JSeparator());
        addKeyedMenuItem(pop, EMSMenuListener.FIND_KEY);
        pop.add(new JSeparator());
        pop.add(podManager.getPodContainerMenuItem());

        pop.addPopupMenuListener(new EMSMenuListener(pop));
        pop.addPopupMenuListener(new SaveableMenuListener(pop));
        ret.setOpaque(false);
        ret.setFocusable(false);
        RolloverJButton.transformToRollover(ret);
        return ret;
    }

    private JComponent makeSausagePane() {
        String key;
        JComponent ret;
        Icon cap;

        key = AppEnvironment.getString(TOOLBAR_TILE);
        if ((key != null) && (key.length() > 0)) {
            ret = new TexturedArea(AppEnvironment.getIcon(TOOLBAR_TILE));
            ret.setLayout(new BorderLayout());
        } else {
            return null;
        }

        key = AppEnvironment.getString(TOOLBAR_LEFT_CAP);
        if ((key != null) && (key.length() > 0)) {
            cap = AppEnvironment.getIcon(TOOLBAR_LEFT_CAP);
            ret.add(new JLabel(cap), BorderLayout.WEST);
        }

        key = AppEnvironment.getString(TOOLBAR_RIGHT_CAP);
        if ((key != null) && (key.length() > 0)) {
            cap = AppEnvironment.getIcon(TOOLBAR_RIGHT_CAP);
            ret.add(new JLabel(cap), BorderLayout.EAST);
        }
        return ret;
    }
}
