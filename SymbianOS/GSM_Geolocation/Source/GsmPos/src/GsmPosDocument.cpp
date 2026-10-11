/*
========================================================================
 Name        : GsmPosDocument.cpp
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
// [[[ begin generated region: do not modify [Generated User Includes]
#include "GsmPosDocument.h"
#include "GsmPosAppUi.h"
// ]]] end generated region [Generated User Includes]

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CGsmPosDocument::CGsmPosDocument( CEikApplication& anApplication )
	: CAknDocument( anApplication )
	{
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CGsmPosDocument::ConstructL()
	{
	}
	
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CGsmPosDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CGsmPosDocument
 */
CGsmPosDocument* CGsmPosDocument::NewL( CEikApplication& aApp )
	{
	CGsmPosDocument* self = new ( ELeave ) CGsmPosDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */	
CEikAppUi* CGsmPosDocument::CreateAppUiL()
	{
	return new ( ELeave ) CGsmPosAppUi;
	}
				
