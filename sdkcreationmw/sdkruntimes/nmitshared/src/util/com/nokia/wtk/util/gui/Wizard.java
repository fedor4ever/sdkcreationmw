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




package com.nokia.wtk.util.gui;

/* com.nokia.wtk.app */
//import com.nokia.wtk.app.LookFactory;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.ArrayUtils;

/* javax.swing */
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import javax.swing.BoxLayout;
import javax.swing.Icon;
import javax.swing.SwingConstants;

/* java.awt.event */
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/* javax.swing.event */
import javax.swing.event.EventListenerList;

/* java.awt */
import java.awt.CardLayout;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Component;
import java.awt.BorderLayout;
import java.awt.Window;

/* java.util */
import java.util.EventListener;

/**
 * Util to create Wizard's outer framework and screens within the wizard. Provides
 * wizard commands {BACK, NEXT, FINISH, CANCEL} for navigation. This class participates
 * in the Mediator DP with its role as concrete Mediator.
 *
 */
public class Wizard extends JDialog implements WizardMediator {

  /**
   * Constructs the wizard with the given title, screens and container
   *
   * @param parent Container of this wizard
   * @param screens screens displayed in the wizard
   * @param wizTitle Title of the Wizard
   */
  public Wizard(Frame parent, WizardScreen screens[], String wizTitle) {
    super(parent, wizTitle, true);

    //
    // Create the card layout
    //
    this.layout = new CardLayout();
    this.screenPanel = new JPanel(this.layout);

    //
    // Add screens
    //

    int scrCount = (screens == null) ? 0 : screens.length;
    for (int i = 0; i < scrCount; i++) {
      addScreen(screens[i]);
    }

    //
    // Build Wizard Panel
    //
    getContentPane().setLayout(new BorderLayout());
    getContentPane().add(buildWizPanel());

    // Handle window events
    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        //TODO: Ditach HelpSystem

        fireWizardEvent(WizardCommands.CLOSE);
        dispose();
      }
    });
  }

  //======================================================================
  //   PUBLIC WIZARD UTILS
  //======================================================================

  /**
   * Adds a listener to the list that's notified about the Wizard events.
   * @param l     the <code>WizardListener</code>
   */
  public void addWizardListener(WizardListener l) {
      if (this.listenerList == null) {
          this.listenerList = new EventListenerList();
      }
      this.listenerList.add(WizardListener.class, l);
  }

  /**
   * Removes a listener from the list that's notified about the Wizard events.
   * @param l     the <code>WizardListener</code>
   */
  public void removeWizardListener(WizardListener l) {
      if (this.listenerList != null) {
          this.listenerList.remove(WizardListener.class, l);
      }
  }

  /**
   * Adds new screen to the wizard, dynamically.
   */
  public void addScreen(WizardScreen screen) {
    if (screen == null) { return; }

    if (this.screens == null) {
      this.screens = new WizardScreen[1];
      this.screens[0] = screen;
    }else {
      this.screens = (WizardScreen[]) ArrayUtils.add(this.screens, screen);
    }

    this.screenPanel.add(screen, String.valueOf(this.screens.length - 1));
  }

  /**
   * Gets the current screen. Returns <code>null</code> if no current screen
   * available.
   */
   public WizardScreen getCurrentScreen() {
     try {
       return this.screens[this.currScrIndex];
     } catch (NullPointerException e){
       e.printStackTrace();
       return null;
     }
   }

  /**
   * Starts the wizard
   *
   * @Exception throws IllegalStateException, if no screens available to start.
   */
  public void startWizard() {

    if (this.wizFinished) {
      throw new IllegalStateException("Wizard Finished. Construct new wizard to start again.");
    }

    int scrCount = (screens == null) ? 0 : screens.length;
    if (scrCount == 0) {
      throw new IllegalStateException("No screens available to start the Wizard");
    }
    // TODO: Attach HelpSystem

    //
    // Set First screen properties
    //
    this.currScrIndex = 0;
    setCurrScreenProps();

    // Display the Wizard
    setResizable(false);
    pack();
    setLocation();
    setVisible(true);
  }

  /**
   * Enables or disables the current screen wizard command.
   * @param wizCommands command to be set; should be one of
   *                      WizardCommands.{BACK,NEXT,FINISH,CANCEL}
   * @param value boolean value of the command
   *              true to enable
   *              false to disable
   * @return true if value is set
   *         false unable to set OR invalid command argument
   */
  public boolean setWizardCommand(int command, boolean value) {

    switch (command) {
      case WizardCommands.BACK:
      {
        //
        // Disable BACK for the first screen
        //
        if (this.currScrIndex == 0) {
          this.back.setEnabled(false);
        }else {
          this.back.setEnabled(value);
        }
        return true;
      }
      case WizardCommands.NEXT:
      {
        this.next.setEnabled(value);
        return true;
      }
      case WizardCommands.FINISH:
      {
        this.finish.setEnabled(value);
        return true;
      }
      case WizardCommands.CANCEL:
      {
        this.cancel.setEnabled(value);
        return true;
      }
    }

    return false;
  }

  /**
   * Gets the total number of screens in the Wizard
   */
   public int getScreenCount() {
     return (this.screens == null) ? 0 : this.screens.length;
   }

  //======================================================================
  //   WizardMediator IMPLEMENTATION
  //======================================================================

  /**
   * The wizard display item(WizardCommand) changed. The display item that is
   * changed will invoke this method as part of Mediator DP.
   *
   * @param displayItem The changed display item.
   */
  public void displayItemChanged(DisplayItem widget) {
    if (screens == null) { return; }

    // For navigation command
    if (widget instanceof ButtonDisplayItem) {
      ButtonDisplayItem button = (ButtonDisplayItem)widget;

      switch (button.getCommand()) {

        case WizardCommands.BACK:
        {
          // Is ready to show previous screen ?
          if (!this.screens[this.currScrIndex].goBack()) { return; }

          fireWizardEvent(WizardCommands.BACK);
          // Is this first screen ?
          if (this.currScrIndex == 0) { return; }

          this.currScrIndex--;
          this.layout.previous(this.screenPanel);
          setCurrScreenProps();
          break;
        }

        case WizardCommands.NEXT:
        {
          // Is ready to show next screen ?
          if (!this.screens[this.currScrIndex].goNext()) { return; }

          fireWizardEvent(WizardCommands.NEXT);
          // Is this last screen ?
          if (this.currScrIndex == (this.screens.length - 1)) { return; }

          this.currScrIndex++;
          this.layout.next(this.screenPanel);
          setCurrScreenProps();
          break;

        }

        case WizardCommands.FINISH:
        {

          // Is ready to Finish the Wizard ?
          if (!this.screens[this.currScrIndex].finishWizard()) { return; }

          fireWizardEvent(WizardCommands.FINISH);
          // close the wizard
          dispose();
          break;
        }

        case WizardCommands.CANCEL:
        {
          // Is ready to Cancel the Wizard ?
          if (!this.screens[this.currScrIndex].cancelWizard()) { return; }

          fireWizardEvent(WizardCommands.CANCEL);
          // close the wizard
          dispose();
          break;
        }

      } // end of switch
    } // end of if
  }

  //======================================================================
  //   PRIVATE UTIL METHODS
  //======================================================================

  /**
   * Notify all listeners that have registered interest for
   * notification on this event type.
   *
   * param event The Wizard command event.Possible values are
   *             WizardCommands{FINISH, BACK, NEXT, CANCEL, CLOSE}
   */
  private void fireWizardEvent(int event) {
      if (this.listenerList != null) {
          // Following is guaranteed to return a non-null array:
          Object[] listeners = this.listenerList.getListenerList();
          // Process the listeners first to last, notifying
          // those that are interested in this event
          for (int i = 0; i < listeners.length; i += 2) {
              if (listeners[i] == WizardListener.class) {
                  ((WizardListener)listeners[i+1]).wizardEvent(event);
              }
          }
      }
  }

  /**
   * Sets the wizard with the current screen properties. Updates WizardCommands and
   * Wizard title, description.
   */
  private void setCurrScreenProps() {
    if (this.screens == null || this.screens.length == 0) {
      return;
    }

    WizardScreen currScr = this.screens[this.currScrIndex];
    this.currScrTitle.setText(currScr.getTitle());
    this.currScrDesc.setText(currScr.getDescription());

    //
    // Disable BACK for the first screen
    //
    if (this.currScrIndex == 0) {
      this.back.setEnabled(false);
    }else {
      this.back.setEnabled(currScr.isCommandEnabled(WizardCommands.BACK));
    }

    //
    // Do not disable NEXT for the last screen,
    // more screens can be added dynamically
    //
    this.next.setEnabled(currScr.isCommandEnabled(WizardCommands.NEXT));
    this.finish.setEnabled(currScr.isCommandEnabled(WizardCommands.FINISH));
    this.cancel.setEnabled(currScr.isCommandEnabled(WizardCommands.CANCEL));
  }

  /**
   * Builds the wizard panel. Command buttons at bottom; Description at top & screen
   * panel in center.
   */
  private Component buildWizPanel() {

    // Box with command buttons
    Box commandBox = new Box(BoxLayout.X_AXIS);
    this.back = new ButtonDisplayItem(this, WizardCommands.BACK,
                                      getCommandLabel(WizardCommands.BACK),
                                      getIcon("wizard.command.back.icon"));
    this.next = new ButtonDisplayItem(this, WizardCommands.NEXT,
                                      getCommandLabel(WizardCommands.NEXT),
                                      getIcon("wizard.command.next.icon"));
    this.next.setHorizontalTextPosition(SwingConstants.LEFT);
    this.finish = new ButtonDisplayItem(this, WizardCommands.FINISH,
                                        getCommandLabel(WizardCommands.FINISH),
                                        null);
    this.cancel = new ButtonDisplayItem(this, WizardCommands.CANCEL,
                                        getCommandLabel(WizardCommands.CANCEL),
                                        null);

    commandBox.add(Box.createRigidArea(FIRST_BUTTON_AT));
    commandBox.add(this.back);
    commandBox.add(this.next);
    commandBox.add(Box.createRigidArea(COMPONENT_GAP));
    commandBox.add(this.finish);
    commandBox.add(Box.createRigidArea(COMPONENT_GAP));
    commandBox.add(this.cancel);
    commandBox.add(Box.createRigidArea(COMPONENT_GAP));

    JPanel commandPanel = new JPanel(new BorderLayout());
    commandPanel.add(buildLine(), BorderLayout.NORTH);
    commandPanel.add(commandBox, BorderLayout.CENTER);

    JPanel wizPanel = new JPanel(new BorderLayout());

    wizPanel.add(this.screenPanel, BorderLayout.CENTER);
    wizPanel.add(buildHeaderPanel(), BorderLayout.NORTH);
    wizPanel.add(getPadComp(), BorderLayout.WEST);
    wizPanel.add(getPadComp(), BorderLayout.EAST);
    wizPanel.add(commandPanel, BorderLayout.SOUTH);
    wizPanel.setPreferredSize(DEFAULT_DIALOG_SIZE);

    return wizPanel;
  }

 /**
  * Returns the framework for import wizard. Puts main & sub title at the top.
  */
  private JPanel buildHeaderPanel() {

    JPanel headerPanel = new JPanel(new BorderLayout());

    // East west pads
    headerPanel.add(getPadComp(), BorderLayout.WEST);
    headerPanel.add(getPadComp(), BorderLayout.EAST);

    // Labels
    this.currScrTitle = new JLabel();
    this.currScrDesc = new JLabel();

    this.currScrTitle.setFont(new java.awt.Font("sansserif", java.awt.Font.BOLD, 11));//;LookFactory.getStandardBoldFont());
    this.currScrTitle.setAlignmentX(Component.LEFT_ALIGNMENT);

    this.currScrDesc.setFont(new java.awt.Font("sansserif", java.awt.Font.PLAIN, 11));//LookFactory.getStandardPlainFont());
    this.currScrDesc.setAlignmentX(Component.LEFT_ALIGNMENT);

    Box b = new Box(BoxLayout.Y_AXIS);
    b.add(this.currScrTitle);
    // Add invisible component to make label allign left
    b.add(Box.createRigidArea(new Dimension(0,0)));
    b.add(this.currScrDesc);

    headerPanel.add(b, BorderLayout.CENTER);
    headerPanel.add(buildLine(), BorderLayout.SOUTH);

    return headerPanel;
  }

 /**
  * Returns the line component
  */
  protected static Component buildLine () {
    JPanel line = new JPanel();
    line.setBorder( new ThinBevelBorder());//LookFactory.thinLoweredBorder());
    line.setPreferredSize(LINE_DIMENSION);
    line.setMaximumSize(LINE_DIMENSION);

    return line;
  }

 /**
  * Returns the padding component
  */
  private static Component getPadComp () {
    return Box.createRigidArea(DEFAULT_PADDING_SIZE);
  }


  /**
   * Sets the Dialog location to the centre of the frame
   */
  private void setLocation() {
    Window parent = getOwner();

    // Gets parent centre & Adjust dialog location
    java.awt.Point p = parent.getLocation();
    int x = (int)p.getX() + (parent.getWidth()/2 - getWidth()/2);
    int y = (int)p.getY() + (parent.getHeight()/2 - getHeight()/2);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    setLocation(x, y);
  }

  /**
    * Get a resource icon value from the 'properties' file.
    * @param key the name of the resource
    */
  private final static ImageIcon getIcon(String key) {
      //URL url = Resources.getResource(Wizard.class, null, key);
      //return (url != null) ? new ImageIcon(url) : null;
      return null;
  }

  /**
   * Gets the Label associated with particalar command.
   */
  private static String getCommandLabel(int command) {
    switch (command) {
      case WizardCommands.NEXT:
        return "Next";
      case WizardCommands.BACK:
        return "Back";
      case WizardCommands.CANCEL:
        return "Cancel";
      case WizardCommands.FINISH:
        return "Finish";
      default:
        return null;
    }
  }

  /**
   * Disposes the Wizard and clears the references.
   */
  public void dispose() {
    super.dispose();

    this.wizFinished = true;

    // Eat the GUI components
    this.screens = null;
    this.currScrTitle = null;
    this.currScrDesc = null;
    this.layout = null;
    this.screenPanel = null;

    // Eat Wizard Navigation commands
    this.back = this.next = this.finish = this.cancel = null;

    // Eat the Listener List
    this.listenerList = null;
  }

  //======================================================================
  //   INNER CLASSES
  //======================================================================

  /**
   * Provides a default screen implementation for the WizardCommands. Wizard
   * Screen consists of screen descriptions and wizard commands properties.
   * Screens to be added to the Wizard should extend this class and override
   * the WizardCommands methods apporpriately.
   */
  public
  static
  abstract
  class      WizardScreen
  extends    JPanel
  implements WizardCommands {

    protected String screenTitle = null;
    protected String screenDesc = null; //Decsription

    // Commands enable or disable ...
    private boolean backEnabled = true;
    private boolean nextEnabled = true;
    private boolean finishEnabled = false;
    private boolean cancelEnabled = true;

    /**
     * Constructs WizardScreen with the Title & Description
     */
    public WizardScreen(String screenTitle, String screenDecsription) {
      this.screenTitle = screenTitle;
      this.screenDesc = screenDecsription;
    }

    //======================================================================
    //   SET AND GET METHODS
    //======================================================================

    /**
     * Sets the screen commands value
     * @param wizCommands command to be set; should be one of
     *                      WizardCommands.{BACK,NEXT,FINISH,CANCEL}
     * @param value boolean value of the command
     *              true to enable
     *              false to disable
     * @return true if value is set
     *         false unable to set OR invalid command argument
     */
    public boolean setCommandEnabled(int wizCommand, boolean value) {

      switch (wizCommand) {
        case WizardCommands.BACK:
        {
          this.backEnabled = value;
          return true;
        }
        case WizardCommands.NEXT:
        {
          this.nextEnabled = value;
          return true;
        }
        case WizardCommands.FINISH:
        {
          this.finishEnabled = value;
          return true;
        }
        case WizardCommands.CANCEL:
        {
          this.cancelEnabled = value;
          return true;
        }
      }

      return false;
    }

    /**
     * Gets the screen commands value
     * @param wizCommands command to be set; should be one of
     *                      WizardCommands.{BACK,NEXT,FINISH,CANCEL}
     * @return value boolean value of the command
     */
    public boolean isCommandEnabled(int wizCommand) {

      switch (wizCommand) {

        case WizardCommands.BACK:
          return this.backEnabled;

        case WizardCommands.NEXT:
          return this.nextEnabled;

        case WizardCommands.FINISH:
          return this.finishEnabled;

        case WizardCommands.CANCEL:
          return this.cancelEnabled;

        default:
          return false;
      }
    }

    //======================================================================
    //   METHODS TO BE OVERIDDEN BY THE DERIVED CLASS
    //======================================================================

    /**
     * Verifies screen inputs upon selecting the BACK command in the wizard.
     * Returns     <code>true</code> for displaying the previous screen,
     *             else returns <code>false</code>.
     * Screens should overide this implementation to handle BACK command selection.
     */
    public boolean goBack()
    {
      return false;
    }

    /**
     * Verifies screen inputs upon selecting the NEXT command in the wizard.
     * Returns     <code>true</code> for displaying the next screen,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle NEXT command selection.
     */
    public boolean goNext() {
      return true;
    }

    /**
     * Verifies screen inputs upon selecting the FINISH command in the wizard.
     * Returns     <code>true</code> to finish the wizard,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle FINISH command selection.
     */
    public boolean finishWizard() {
      return false;
    }

    /**
     * Verifies screen inputs upon selecting the CANCEL command in the wizard.
     * Returns     <code>true</code> to cancel the wizard,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle CANCEL command selection.
     */
    public boolean cancelWizard() {
      return true;
    }

    //======================================================================
    //   PRIVATE METHODS
    //======================================================================

    /**
     * Return screens title
     */
    private String getTitle() {
      return this.screenTitle;
    }

    /**
     * Returns Screens Description
     */
    private String getDescription() {
      return this.screenDesc;
    }

  } // End of WizardScreen

  /**
   * Provides Buttons for Wizard funtionality. These
   * buttons act as concrete colleague in Mediator DP.
   *
   */
  private
  static
  class      ButtonDisplayItem
  extends    JButton
  implements DisplayItem,
             ActionListener {

    WizardMediator mediator = null;
    int command = -1;

   /**
    * Build the button
    */
    ButtonDisplayItem(WizardMediator med, int command, String label, Icon icon) {
      super(label, icon);
      if (label != null)
           super.setMnemonic(label.charAt(0));
      this.mediator = med;
      this.command = command;
      addActionListener(this);
    }

   /**
    * Implentaion for ActionListener. Informs Mediator about the
    * button selection/desection.
    */
    public void actionPerformed(ActionEvent ae) {
      changed();
    }

   /**
    * Informs Mediator about the change in this button
    */
    public void changed() {
      mediator.displayItemChanged(this);
    }

    /**
     * Gets the command associated with this button
     */
    public int getCommand() {
      return this.command;
    }
  } // End of ButtonDisplayItem

  /**
   * An interface that abstracts the properties of displayable
   * Wizard items. This interface participates in the Mediator design
   * pattern with its role as Colleague.
   *
   */
  interface DisplayItem {
    /**
     * Wizard display item is changed; Informs Mediator about the change.
     */
    public void changed();
  } // End of DisplayItem

  /**
   * An interface that abstracts the properties of navigable Wizard
   * commands. Screens in wizard should provide this inerface implementation to
   * handle wizard command selection.
   */
  public interface WizardCommands {

    // Wizard Commands
    public final static int NEXT = 1;
    public final static int BACK = 2;
    public final static int CANCEL = 3;
    public final static int FINISH = 4;
    public final static int CLOSE = 5;

    /**
     * Verifies screen inputs upon selecting the BACK command in the wizard.
     * Returns     <code>true</code> for displaying the previous screen,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle BACK command selection.
     */
    public boolean goBack();

    /**
     * Verifies screen inputs upon selecting the NEXT command in the wizard.
     * Returns     <code>true</code> for displaying the next screen,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle NEXT command selection.
     */
    public boolean goNext();

    /**
     * Verifies screen inputs upon selecting the FINISH command in the wizard.
     * Returns     <code>true</code> to finish the wizard,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle FINISH command selection.
     */
    public boolean finishWizard();

    /**
     * Verifies screen inputs upon selecting the CANCEL command in the wizard.
     * Returns     <code>true</code> to cancel the wizard,
     *             else returns <code>false</code>.
     * Screens should provide this implementation to handle CANCEL command selection.
     */
    public boolean cancelWizard();

  } // End of WizardCommands

  /**
   * A listener for Wizard events.
   */
  public interface WizardListener extends EventListener {

    /**
     * Called when WizardCommands.{NEXT, BACK, FINISH, CANCEL, CLOSE} are
     * selected upon the Wizard.
     *
     * param wizCommand    the wizard command. Possible values are
     *                  <ul>
     *                    <li> {@link Wizard.WizardCommands#NEXT}
     *                    <li> {@link Wizard.WizardCommands#BACK}
     *                    <li> {@link Wizard.WizardCommands#FINISH}
     *                    <li> {@link Wizard.WizardCommands#CANCEL}
     *                    <li> {@link Wizard.WizardCommands#CLOSE}
     *                  </ul>
     */
    public void wizardEvent(int wizCommand);

  } // End of WizardListener

  //
  // Wizard Fields
  //
  private WizardScreen[] screens = null;
  private JLabel currScrTitle = null; //currentScreenTitle
  private JLabel currScrDesc = null;  //currentScreenDescription
  private short currScrIndex = 0;     //current screen index
  private CardLayout layout = null;
  private JPanel screenPanel = null;  //wizard screens container
  private EventListenerList listenerList;

  // Wixzard Navigation commands
  private ButtonDisplayItem back = null;
  private ButtonDisplayItem next = null;
  private ButtonDisplayItem finish = null;
  private ButtonDisplayItem cancel = null;

  // Wizard flags
  private boolean wizFinished = false;

  // Wizard GUI constants
  private static final int DIALOG_WIDTH = 450;
  private static final int DIALOG_HEIGHT = 400;
  private static final Dimension DEFAULT_DIALOG_SIZE = new Dimension(DIALOG_WIDTH,
	                                                             DIALOG_HEIGHT);
  private static final int LINE_WIDTH = DIALOG_WIDTH;
  private static final int LINE_HEIGHT = 2;
  private static final Dimension LINE_DIMENSION = new Dimension(LINE_WIDTH, LINE_HEIGHT);
  private static final int COMMAND_PANEL_HEIGHT = DIALOG_WIDTH - DIALOG_HEIGHT;
  private static final int QUARTER_DIALOG_WIDTH = (int)(DIALOG_WIDTH * 0.25);

  // TODO: If dialog width decreases, last command button not displayed fully...
  // fix: find total command buttons width & compute FIRST_BUTTON_AT with respect to DIALOG_WIDTH.
  private static final Dimension FIRST_BUTTON_AT = new Dimension(QUARTER_DIALOG_WIDTH,
                                                                 COMMAND_PANEL_HEIGHT);
  private static final int GAP = 5;//LookFactory.getGap();//5;
  private static final Dimension COMPONENT_GAP = new Dimension(GAP,COMMAND_PANEL_HEIGHT);
  private static final int PADD_WIDTH = 20;
  private static final int PADD_HEIGHT = COMMAND_PANEL_HEIGHT;//50;
  private static final Dimension DEFAULT_PADDING_SIZE = new Dimension(PADD_WIDTH, PADD_HEIGHT);

  /**
   * Unit testing...
   */
/*
// UNIT TESTING BEGINS HERE

  public static void main(String args[]) {
     try {
      javax.swing.UIManager.setLookAndFeel(
            javax.swing.UIManager.getSystemLookAndFeelClassName());
    }catch(Exception e) {}


    // First screen: prompts for a URL

    class Screen1 extends WizardScreen {
      javax.swing.JTextField textField = null;
      Screen1(String screenTitle, String screenDescription, String msg) {
        super(screenTitle, screenDescription);
        setLayout(new java.awt.FlowLayout());

        add(new JButton(msg));
        add(new JLabel("Enter URL Location:"));
        textField = new javax.swing.JTextField(15);
        add(textField);
      }

      public boolean goBack() {
        return true;
      }
      public boolean goNext() {
        if (textField.getText().equals("")) {
          javax.swing.JOptionPane.showMessageDialog(new Frame(), "Enter the URL location and press next.");
          return false;
        }
        return true;
      }
      public boolean finishWizard() {
        return false;
      }
    }

    // Second screen: allows adding a screen dynamically

    class Screen2 extends WizardScreen {

      Screen2(String screenTitle, String screenDescription) {
        super(screenTitle, screenDescription);
        setLayout(new java.awt.FlowLayout());
        add(new JLabel("Selecting Next adds new screen to the wizard."));
        add(new javax.swing.JTextField("Screen3"));
        add(new JLabel("Selecting Finish closes the wizard."));

        //Set Finish command enable
        super.setCommandEnabled(WizardCommands.FINISH, true);
      }

      public boolean goBack() {
        return true;
      }
      public boolean goNext() {
        // Add more screens dynamically

        WizardScreen newScreen =  new Screen1("New Screen", "ADDED SCREEN",
		"Screen " + (testWizard.screens.length + 1)) ;
        testWizard.addScreen(newScreen);

        return true;
      }
      public boolean finishWizard() {
        javax.swing.JOptionPane.showMessageDialog(new Frame(), "Operation Successful.");
        return true;
      }
      public boolean cancelWizard() {
        return true;
      }
    }

    WizardScreen scr[] = {new Screen1("Screen1", "Go Next", "Screen1"),
                          new Screen1("Screen2", "Go Next", "Screen2"),
                          new Screen2("Screen3", "Adds New Screen")};
    //scr = null; Should throw exception
    Frame parent = new Frame();
    parent.setLocation(300, 300);

    testWizard = new Wizard(parent, scr, "Wizard Title");
    testWizard.startWizard();
  }
    private static Wizard testWizard = null;
*/

// UNIT TESTING ENDS HERE

} //End of Wizard


/**
 * Defines an interface for communicating with Wizard display items.
 * This interface participates in the Mediator design pattern with its
 * role as Mediator.
 *
 */
  interface WizardMediator {

    /**
     * The wizard display item is changed. The display item that is
     * changed will invoke this method as part of Mediator DP.
     *
     * @param displayItem The changed display item.
     */
    public void displayItemChanged(Wizard.DisplayItem item);
  } // End of WizardMediator

