/*
========================================================================
 Name        : GsmPosAppUi.h
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSAPPUI_H
#define GSMPOSAPPUI_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknviewappui.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Forward Declarations]
class CGsmPosContainerView;
// ]]] end generated region [Generated Forward Declarations]

#include "GsmPosEngine.h"

/**
 * @class	CGsmPosAppUi GsmPosAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        view management and the default menu, control pane, and status pane.
 */
class CGsmPosAppUi : public CAknViewAppUi
	{
public: 
	// constructor and destructor
	CGsmPosAppUi();
	virtual ~CGsmPosAppUi();
	void ConstructL();

public:
	// from CCoeAppUi
	TKeyResponse HandleKeyEventL(
				const TKeyEvent& aKeyEvent,
				TEventCode aType );

	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );

	// from CAknAppUi
	void HandleViewDeactivation( 
			const TVwsViewId& aViewIdToBeDeactivated, 
			const TVwsViewId& aNewlyActivatedViewId );

	CGsmPosEngine* Engine();
	
private:
	void InitializeContainersL();
	// [[[ begin generated region: do not modify [Generated Methods]
public: 
	// ]]] end generated region [Generated Methods]
	
	// [[[ begin generated region: do not modify [Generated Instance Variables]
private: 
	CGsmPosContainerView* iGsmPosContainerView;
	// ]]] end generated region [Generated Instance Variables]
	
	
	// [[[ begin [User Handlers]
protected: 
	// ]]] end [User Handlers]
	
private:
	CGsmPosEngine* iEngine;
	
	};

#endif // GSMPOSAPPUI_H			
