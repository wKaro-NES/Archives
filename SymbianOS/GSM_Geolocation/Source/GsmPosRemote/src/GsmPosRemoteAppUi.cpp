/*
========================================================================
 Name        : GsmPosRemoteAppUi.cpp
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
// [[[ begin generated region: do not modify [Generated System Includes]
#include <eikmenub.h>
#include <akncontext.h>
#include <akntitle.h>
#include <aknquerydialog.h>
#include <GsmPosRemote.rsg>
// ]]] end generated region [Generated System Includes]

// [[[ begin generated region: do not modify [Generated User Includes]
#include "GsmPosRemoteAppUi.h"
#include "GsmPosRemote.hrh"
#include "GsmPosRemoteContainer.hrh"
#include "GsmPosRemoteContainer.h"
// ]]] end generated region [Generated User Includes]

// [[[ begin generated region: do not modify [Generated Constants]
// ]]] end generated region [Generated Constants]

#include "GsmPosRemoteEngine.h"

/**
 * Construct the CGsmPosRemoteAppUi instance
 */ 
CGsmPosRemoteAppUi::CGsmPosRemoteAppUi()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	iGsmPosRemoteContainer = NULL;
	// ]]] end generated region [Generated Contents]
	
	}

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CGsmPosRemoteAppUi::~CGsmPosRemoteAppUi()
	{
	
	delete iEngine;
	
	// [[[ begin generated region: do not modify [Generated Contents]
	if ( iGsmPosRemoteContainer != NULL )
		{
		RemoveFromStack( iGsmPosRemoteContainer );
		delete iGsmPosRemoteContainer;
		iGsmPosRemoteContainer = NULL;
		}
	// ]]] end generated region [Generated Contents]
	
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here. 
 */ 
void CGsmPosRemoteAppUi::ConstructL()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	
	BaseConstructL( EAknEnableSkin  | 
					 EAknEnableMSK ); 
	InitializeContainersL();
	// ]]] end generated region [Generated Contents]
	
	iEngine = CGsmPosRemoteEngine::NewL(*iGsmPosRemoteContainer);
	}

// [[[ begin generated function: do not modify
void CGsmPosRemoteAppUi::InitializeContainersL()
	{
	iGsmPosRemoteContainer = CGsmPosRemoteContainer::NewL( ClientRect(), NULL, this );
	iGsmPosRemoteContainer->SetMopParent( this );
	AddToStackL( iGsmPosRemoteContainer );
	}
// ]]] end generated function

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CGsmPosRemoteAppUi::HandleCommandL( TInt aCommand )
	{
	//TODO refactor after removing UI designer code
	if ( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit )
		{
		if (RunConfQuery1L() == EAknSoftkeyYes)
			{
			Exit();
			}
		else
			{
			return;
			}
		}
	
	// [[[ begin generated region: do not modify [Generated Code]
	TBool commandHandled = EFalse;
	switch ( aCommand )
		{ // code to dispatch to the AppUi's menu and CBA commands is generated here
		case EGsmPosRemoteApplicationPingMenuItemCommand:
			commandHandled = HandlePingMenuItemSelectedL( aCommand );
			break;
		default:
			break;
		}
	
		
	if ( !commandHandled ) 
		{
		if ( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit )
			{
			Exit();
			}
		}
	// ]]] end generated region [Generated Code]
	
	}

/** 
 * Override of the HandleResourceChangeL virtual function
 */
void CGsmPosRemoteAppUi::HandleResourceChangeL( TInt aType )
	{
	CAknAppUi::HandleResourceChangeL( aType );
	// [[[ begin generated region: do not modify [Generated Code]
	// ]]] end generated region [Generated Code]
	
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosRemoteAppUi::HandlePingMenuItemSelectedL( TInt aCommand )
	{
	// TODO: implement selected event handler
	
	if (iEngine)
		{
		iEngine->PublishOwnCellIdL();
		}
	
	return ETrue;
	}
				
// [[[ begin generated function: do not modify
/**
 * Show the popup dialog for confQuery1
 * @param aOverrideText optional override text
 * @return EAknSoftkeyYes (left soft key id) or 0
 */
TInt CGsmPosRemoteAppUi::RunConfQuery1L( const TDesC* aOverrideText )
	{
	
	CAknQueryDialog* queryDialog = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );	
	
	if ( aOverrideText != NULL )
		{
		queryDialog->SetPromptL( *aOverrideText );
		}
	return queryDialog->ExecuteLD( R_APPLICATION_CONF_QUERY1 );
	}
// ]]] end generated function

