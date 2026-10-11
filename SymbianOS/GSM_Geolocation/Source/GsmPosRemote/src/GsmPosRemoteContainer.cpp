/*
========================================================================
 Name        : GsmPosRemoteContainer.cpp
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
// [[[ begin generated region: do not modify [Generated System Includes]
#include <barsread.h>
#include <stringloader.h>
#include <eiklabel.h>
#include <eikenv.h>
		
#include <eikmenub.h>
		
#include <eikcmobs.h>
#include <eikappui.h>
#include <GsmPosRemote.rsg>
// ]]] end generated region [Generated System Includes]

// [[[ begin generated region: do not modify [Generated User Includes]
#include "GsmPosRemoteContainer.h"
#include "GsmPosRemoteAppUi.h"
#include "GsmPosRemote.hrh"
#include "GsmPosRemoteContainer.hrh"
// ]]] end generated region [Generated User Includes]

// [[[ begin generated region: do not modify [Generated Constants]
// ]]] end generated region [Generated Constants]

/**
 * First phase of Symbian two-phase construction. Should not 
 * contain any code that could leave.
 */
CGsmPosRemoteContainer::CGsmPosRemoteContainer()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	iOperator = NULL;
	iOperator_val = NULL;
	iLac = NULL;
	iLac_val = NULL;
	iCi = NULL;
	iCi_val = NULL;
	// ]]] end generated region [Generated Contents]
	
	}
/** 
 * Destroy child controls.
 */
CGsmPosRemoteContainer::~CGsmPosRemoteContainer()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	delete iOperator;
	iOperator = NULL;
	delete iOperator_val;
	iOperator_val = NULL;
	delete iLac;
	iLac = NULL;
	delete iLac_val;
	iLac_val = NULL;
	delete iCi;
	iCi = NULL;
	delete iCi_val;
	iCi_val = NULL;
	// ]]] end generated region [Generated Contents]
	
	}
				
/**
 * Construct the control (first phase).
 *  Creates an instance and initializes it.
 *  Instance is not left on cleanup stack.
 * @param aRect bounding rectangle
 * @param aParent owning parent, or NULL
 * @param aCommandObserver command observer
 * @return initialized instance of CGsmPosRemoteContainer
 */
CGsmPosRemoteContainer* CGsmPosRemoteContainer::NewL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver )
	{
	CGsmPosRemoteContainer* self = CGsmPosRemoteContainer::NewLC( 
			aRect, 
			aParent, 
			aCommandObserver );
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Construct the control (first phase).
 *  Creates an instance and initializes it.
 *  Instance is left on cleanup stack.
 * @param aRect The rectangle for this window
 * @param aParent owning parent, or NULL
 * @param aCommandObserver command observer
 * @return new instance of CGsmPosRemoteContainer
 */
CGsmPosRemoteContainer* CGsmPosRemoteContainer::NewLC( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver )
	{
	CGsmPosRemoteContainer* self = new ( ELeave ) CGsmPosRemoteContainer();
	CleanupStack::PushL( self );
	self->ConstructL( aRect, aParent, aCommandObserver );
	return self;
	}
			
/**
 * Construct the control (second phase).
 *  Creates a window to contain the controls and activates it.
 * @param aRect bounding rectangle
 * @param aCommandObserver command observer
 * @param aParent owning parent, or NULL
 */ 
void CGsmPosRemoteContainer::ConstructL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver )
	{
	if ( aParent == NULL )
	    {
		CreateWindowL();
	    }
	else
	    {
	    SetContainerWindowL( *aParent );
	    }
	iFocusControl = NULL;
	iCommandObserver = aCommandObserver;
	InitializeControlsL();
	SetRect( aRect );
	ActivateL();
	// [[[ begin generated region: do not modify [Post-ActivateL initializations]
	// ]]] end generated region [Post-ActivateL initializations]
	
	//czesc zmienna innym kolorem
	iOperator_val->OverrideColorL(EColorLabelText, TRgb(0x80, 0x80, 0x80));
	iLac_val->OverrideColorL(EColorLabelText, TRgb(0x80, 0x80, 0x80));
	iCi_val->OverrideColorL(EColorLabelText, TRgb(0x80, 0x80, 0x80));
	}
			
/**
* Return the number of controls in the container (override)
* @return count
*/
TInt CGsmPosRemoteContainer::CountComponentControls() const
	{
	return ( int ) ELastControl;
	}
				
/**
* Get the control with the given index (override)
* @param aIndex Control index [0...n) (limited by #CountComponentControls)
* @return Pointer to control
*/
CCoeControl* CGsmPosRemoteContainer::ComponentControl( TInt aIndex ) const
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	switch ( aIndex )
		{
		case EOperator:
			return iOperator;
		case EOperator_val:
			return iOperator_val;
		case ELac:
			return iLac;
		case ELac_val:
			return iLac_val;
		case ECi:
			return iCi;
		case ECi_val:
			return iCi_val;
		}
	// ]]] end generated region [Generated Contents]
	
	// handle any user controls here...
	
	return NULL;
	}
				
/**
 *	Handle resizing of the container. This implementation will lay out
 *  full-sized controls like list boxes for any screen size, and will layout
 *  labels, editors, etc. to the size they were given in the UI designer.
 *  This code will need to be modified to adjust arbitrary controls to
 *  any screen size.
 */				
void CGsmPosRemoteContainer::SizeChanged()
	{
	CCoeControl::SizeChanged();
	LayoutControls();
	// [[[ begin generated region: do not modify [Generated Contents]
			
	// ]]] end generated region [Generated Contents]
	
	}
				
// [[[ begin generated function: do not modify
/**
 * Layout components as specified in the UI Designer
 */
void CGsmPosRemoteContainer::LayoutControls()
	{
	iOperator->SetExtent( TPoint( 5, 7 ), TSize( 148, 27 ) );
	iOperator_val->SetExtent( TPoint( 50, 35 ), TSize( 179, 27 ) );
	iLac->SetExtent( TPoint( 5, 67 ), TSize( 227, 27 ) );
	iLac_val->SetExtent( TPoint( 50, 95 ), TSize( 194, 27 ) );
	iCi->SetExtent( TPoint( 5, 131 ), TSize( 208, 27 ) );
	iCi_val->SetExtent( TPoint( 50, 165 ), TSize( 192, 27 ) );
	}
// ]]] end generated function

/**
 *	Handle key events.
 */				
TKeyResponse CGsmPosRemoteContainer::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	// ]]] end generated region [Generated Contents]
	
	if ( iFocusControl != NULL
		&& iFocusControl->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
		{
		return EKeyWasConsumed;
		}
	return CCoeControl::OfferKeyEventL( aKeyEvent, aType );
	}
				
// [[[ begin generated function: do not modify
/**
 *	Initialize each control upon creation.
 */				
void CGsmPosRemoteContainer::InitializeControlsL()
	{
	iOperator = new ( ELeave ) CEikLabel;
	iOperator->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_OPERATOR );
		iOperator->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	iOperator_val = new ( ELeave ) CEikLabel;
	iOperator_val->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_OPERATOR_VAL );
		iOperator_val->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	iLac = new ( ELeave ) CEikLabel;
	iLac->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_LAC );
		iLac->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	iLac_val = new ( ELeave ) CEikLabel;
	iLac_val->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_LAC_VAL );
		iLac_val->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	iCi = new ( ELeave ) CEikLabel;
	iCi->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_CI );
		iCi->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	iCi_val = new ( ELeave ) CEikLabel;
	iCi_val->SetContainerWindowL( *this );
		{
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_REMOTE_CONTAINER_CI_VAL );
		iCi_val->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	
	}
// ]]] end generated function

/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CGsmPosRemoteContainer::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	SetRect( static_cast< CEikAppUi* >( iCoeEnv->AppUi() )->ClientRect() );
	// [[[ begin generated region: do not modify [Generated Contents]
	// ]]] end generated region [Generated Contents]
	
	}
				
/**
 *	Draw container contents.
 */				
void CGsmPosRemoteContainer::Draw( const TRect& aRect ) const
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	CWindowGc& gc = SystemGc();
	gc.Clear( aRect );
	
	// ]]] end generated region [Generated Contents]
	
	}

void CGsmPosRemoteContainer::UpdateScreen(const TDesC& aOperator, TInt aLac, TInt aCellId)
	{
	
	if (iOperator_val)
		iOperator_val->SetTextL(aOperator);
	
	TBuf<32> buf;
	_LIT(KFormat, "%d");
	_LIT(KUnknown, "???");
	
	if (iLac_val)
		{
		aLac < 0 ? buf.Copy(KUnknown) : buf.Format(KFormat(), aLac);
		iLac_val->SetTextL(buf);
		}
	
	if (iCi_val)
		{
		aCellId <0 ? buf.Copy(KUnknown): buf.Format(KFormat(), aCellId);
		iCi_val->SetTextL(buf);
		}
	}
