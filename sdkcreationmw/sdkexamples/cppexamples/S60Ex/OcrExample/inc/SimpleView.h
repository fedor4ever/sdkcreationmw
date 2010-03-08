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


#ifndef OCR_EXAMPLE_SIMPLE_VIEW
#define OCR_EXAMPLE_SIMPLE_VIEW

#include <aknview.h>
#include <aknutils.h>

/**
 * namespace contains various utility classes
 */
namespace NOCRUtils
{

/**
 * CSimpleView is a template class for creating a simple view.
 * CSimpleView is a template class, which implements a simple view
 * for Avkon View-switching architecture. The view is called 'simple', because
 * it assumes that view offers only Ok- or Back-option to return to previous
 * view and no Options-menu at all.
 *
 * Container class is required to fulfil following criteria:
 * - container has to offer static NewL(const TRect& aRect, D& aData)
 * - container does not activate itself during the execution of NewL(). This
 *   view-class will do that when view is activated.
 * - container has to do all its activation steps in ActivateL() i.e.
 *   the container should override CCoeControl::ActivateL(), if needed.
 *   Note, container has to call baseclass' ActivateL() also in such case!
 *
 * Container is created everytime, when the view is activated, and it is
 * destroyed, when the view is deactivated. Container should store its 
 * possible state-info into data-object.
 *
 * @param T container class used by this view
 * @param D data-object used by container class
 */
template <class T, class D>
class CSimpleView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * Symbian two-phased constructor
        * @param aViewResource resource id of the view
        * @param aId unique id of the view
        * @param aData data-object for container (ownership not taken)
        */
        static CSimpleView* NewL(TInt aViewResource, TUid aId, D& aData);
        /** Destructor. */
        virtual ~CSimpleView();

        /** Handle screen size change */
        void HandleSizeChange();

    private: // Functions from base classes

        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const { return iUid; }

        /**
        * From CAknView, handle commands
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Activate this view
        * @param aPrevViewId
        * @param aCustomMessageId
        * @param aCustomMessage
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /** From CAknView Deactivate this view */
        void DoDeactivate();

    private:
        /**
        * C++ constructor
        * @param aId unique id of the view
        * @param aData data-object for container
        */
        CSimpleView(TUid aId, D& aData);

        /**
        * Symbian 2nd phase constructor
        * @param aViewResource resource id of the view
        */
        void ConstructL(TInt aViewResource) { BaseConstructL(aViewResource); }

    private:
        /** id of this view */
        TUid        iUid;

        /** id of the previous view */
        TVwsViewId  iPrevView;

        /** container object */
        T*          iContainer;

        /** container data object */
        D&          iData;
    };

/** ctor */
template <class T, class D>
CSimpleView<T, D>::CSimpleView(TUid aId, D& aData)
    : iUid(aId)
    , iContainer(0)
    , iData(aData)
    {
    }

/** NewL */
template <class T, class D>
CSimpleView<T, D>* CSimpleView<T, D>::NewL(TInt aViewResource, TUid aId, D& aData)
    {
    CSimpleView<T,D>* self = new (ELeave) CSimpleView<T,D>(aId, aData);
    self->ConstructL(aViewResource);
    return self;
    }

/** dtor */
template <class T, class D>
CSimpleView<T, D>::~CSimpleView()
    {
    if (iContainer)
	    {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
	    }
    }

/** DoActivateL */
template <class T, class D>
void CSimpleView<T, D>::DoActivateL(
                                    const TVwsViewId& aPrevViewId,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/ )
    {
    iPrevView = aPrevViewId;

    if (!iContainer)
        {
        iContainer = T::NewL(ClientRect(), iData);
        iContainer->SetMopParent( this );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->ActivateL();
        }
    }

/** DoDeactivate */
template <class T, class D>
void CSimpleView<T, D>::DoDeactivate()
    {
    if (iContainer)
	    {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
	    }
    }

/** HandleSizeChange */
template <class T, class D>
void CSimpleView<T, D>::HandleSizeChange()
    {
    if (iContainer)
	    {
		TRect mainPaneRect;
     	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
		iContainer->SetRect(mainPaneRect);
        }
    }

/** HandleCommandL */
template <class T, class D>
void CSimpleView<T, D>::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
	    {
        case EAknSoftkeyOk:
        case EAknSoftkeyBack:
		    {
            AppUi()->ActivateLocalViewL( iPrevView.iViewUid );
            break;
		    }

        default:
		    {
            AppUi()->HandleCommandL( aCommand );
            break;
		    }
 	    }
    }

} // namespace

#endif
