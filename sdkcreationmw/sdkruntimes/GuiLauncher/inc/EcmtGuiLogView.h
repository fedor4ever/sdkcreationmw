/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View class for LogContainer.
*
*/


#ifndef __ECMTGUI_LOGVIEW_H__
#define __ECMTGUI_LOGVIEW_H__

#include <aknlists.h>
#include <aknview.h>

class CEcmtGuiLogContainer;


/**
 *  View class for the LogContainer.
 *
 *  @since S60 v3.1
 */
class CEcmtGuiLogView : public CAknView
	{

public:
 
    /**
     * Two-phased constructor.
     */
    static CEcmtGuiLogView* NewL();

public: //New functions
    
    /**
     * Getter for log container reference.
     *
     * @return A reference to CEcmtGuiLogContainer. 
     */
    CEcmtGuiLogContainer& LogContainer();

    /**
     * Loads log message from resource and adds it to the listbox. 
     *
     * @param aMsgResId A resource id of the message.
     */
    void LogMessageL( TInt aMsgResId );
    
    /**
     * Adds log message to the listbox.
     *
     * @param aMsg Log message string.
     */
    void LogMessageL( const TDesC& aMsg );

    /**
     * Clears the log.
     */
    void ClearLogL();
    
    /**
     * Setter for iConnected.
     *
     * @param aConnected A new value for iConnected.
     */
    void SetConnected( TBool aConnected );
    
    /**
     * Getter for Ecmt Server connectivity state.
     *
     * @return Boolean representing is Ecmt Server in connected state.
     */
    TBool IsConnected();
      
    /**
     * Called by HandleResourceChangeL() from CEcmtGuiAppUi when layout 
     * is changed.
     *
     * @param aType Type of resources that have changed
     */
    void HandleSizeChange( TInt aType );

public: // From MEikAutoMenuObserver

		/**
     * From MEikAutoMenuObserver.
     * Dynamically initialises a menu pane.
     *
     * @param aResourceId Menu pane resource id.
     * @param aMenuPane The in-memory representation of the menu pane.
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

private: //From CAknView.

    /**
     * From CAknView.
     * Returns views id.
     *
     * @return Id for this view.
     */
    TUid Id() const;

    /**
     * From CAknView.
     * Handles commands
     *
     * @param aCommand A command to be handled.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CAknView.
     * Activates view
     *
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid aCustomMessageId,
                      const TDesC8& aCustomMessage );

    /**
     * From CAknView.
     * Deactivates view.
     */
    void DoDeactivate();

private:

		/**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CEcmtGuiLogView();
    
    /**
     * Destructor.
     */
    ~CEcmtGuiLogView();

private:

		/**
     * Container.
     * Own.
     */
    CEcmtGuiLogContainer* iContainer;
    
    /**
     * List for log messages. Container modifies this.
     * Own.
     */
    CDesCArrayFlat* iMessageList;
    
    /**
     * Boolean indicating is Ecmt Server in connected state.
     */
    TBool iConnected;


		};

#endif // __ECMTGUI_LOGVIEW_H__
