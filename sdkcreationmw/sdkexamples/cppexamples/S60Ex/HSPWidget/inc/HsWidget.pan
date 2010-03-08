/*
 ============================================================================
 Name		: HsWidget.pan
 Author	  : 
 Copyright   : Your copyright notice
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __HSWIDGET_PAN__
#define __HSWIDGET_PAN__

/** HsWidget application panic codes */
enum THsWidgetPanics
	{
	EHsWidgetUi = 1
	// add further panics here
	};

inline void Panic(THsWidgetPanics aReason)
	{
	_LIT(applicationName, "HsWidget");
	User::Panic(applicationName, aReason);
	}

#endif // __HSWIDGET_PAN__
