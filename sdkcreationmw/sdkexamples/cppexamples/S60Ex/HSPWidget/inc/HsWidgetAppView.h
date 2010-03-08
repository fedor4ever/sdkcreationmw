/*
 ============================================================================
 Name		: HsWidgetAppView.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __HSWIDGETAPPVIEW_H__
#define __HSWIDGETAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CHsWidgetAppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CHsWidgetAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CHsWidgetAppView.
	 */
	static CHsWidgetAppView* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CHsWidgetAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CHsWidgetAppView.
	 */
	static CHsWidgetAppView* NewLC(const TRect& aRect);

	/**
	 * ~CHsWidgetAppView
	 * Virtual Destructor.
	 */
	virtual ~CHsWidgetAppView();

public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CHsWidgetAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CHsWidgetAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CHsWidgetAppView.
	 * C++ default constructor.
	 */
	CHsWidgetAppView();

	};

#endif // __HSWIDGETAPPVIEW_H__
// End of File
