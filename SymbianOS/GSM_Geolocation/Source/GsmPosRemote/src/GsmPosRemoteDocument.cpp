/*
========================================================================
 Name        : GsmPosRemoteDocument.cpp
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
// [[[ begin generated region: do not modify [Generated User Includes]
#include "GsmPosRemoteDocument.h"
#include "GsmPosRemoteAppUi.h"
// ]]] end generated region [Generated User Includes]

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CGsmPosRemoteDocument::CGsmPosRemoteDocument( CEikApplication& anApplication )
	: CAknDocument( anApplication )
	{
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CGsmPosRemoteDocument::ConstructL()
	{
	}
	
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CGsmPosRemoteDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CGsmPosRemoteDocument
 */
CGsmPosRemoteDocument* CGsmPosRemoteDocument::NewL( CEikApplication& aApp )
	{
	CGsmPosRemoteDocument* self = new ( ELeave ) CGsmPosRemoteDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */	
CEikAppUi* CGsmPosRemoteDocument::CreateAppUiL()
	{
	return new ( ELeave ) CGsmPosRemoteAppUi;
	}
				
