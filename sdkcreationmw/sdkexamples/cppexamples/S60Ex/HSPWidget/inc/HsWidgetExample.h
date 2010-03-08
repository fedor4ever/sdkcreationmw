/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __HSWIDGETEXAMPLE_H__
#define __HSWIDGETEXAMPLE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <string>
#include <hsdataobserver.h>


// CLASS DECLARATION
namespace Hs 
    {
    class HsWidgetPublisher;
    }

/**
 *  CHsWidgetExample
 * 
 */
class CHsWidgetExample : 
	public CBase,
	public Hs::IHsDataObserver
	{
public:

	/**
	 * Intended to handle an event that occured on a widget.
	 * This would include EActivate, EDeactivate, ESuspend, EReusume.
	 *
	 * @param aWidgetName Name of the widget that event is envoked for.
	 * @param aEvent Event type that has taken place.
	 */
	virtual void handleEvent( std::string aWidgetName, 
			IHsDataObserver::EEvent aEvent);
	
	/**
	 * Intended to handle an event that occured on a widget's item.
	 * This would include ESelection.
	 *
	 * @param aWidgetName Name of the widget that event is envoked for.
	 * @param aWidgetItemName Name of the item that event is envoked for.
	 * @param aEvent Event type that has taken place
	 */    
	virtual void handleItemEvent( std::string aWidgetName,
			std::string aWidgetItemName,
			IHsDataObserver::EItemEvent aEvent);
	    
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CHsWidgetExample();

	/**
	 * Two-phased constructor.
	 */
	static CHsWidgetExample* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CHsWidgetExample* NewLC();

	/**
	 * Register widget to HS so it can be added as content.
	 * New template is created and is keept by HSApi.
	 */	
	void RegisterWidget();
	
	/**
	 * Publish widget with actual seted items' values to HS.
	 */	
	void PublishWidget();
	
	/**
	 * Change items' values.
	 */
	void ChangeWidgetValuesL();
	
	/**
	 * Remove widget's template from HSApi.
	 * It is now not avaliable for HS and it doesn't support events. 
	 */
	void RemoveWidget();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CHsWidgetExample();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
	/**
	 * Conversion method. 
	 * @param aText string to convert
	 * @return converted string
	 */
	std::string ToString(const TDesC& aText);

private:
	Hs::HsWidgetPublisher* iHsWidgetPublisher;
	
	std::string templateType;
	std::string widgetName;
	std::string widgetId;
	
	TInt iCounter;
	};

#endif // __HSWIDGETEXAMPLE_H__
