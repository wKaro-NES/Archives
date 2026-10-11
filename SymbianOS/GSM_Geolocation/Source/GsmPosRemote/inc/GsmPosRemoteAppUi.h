/*
========================================================================
 Name        : GsmPosRemoteAppUi.h
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSREMOTEAPPUI_H
#define GSMPOSREMOTEAPPUI_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknappui.h>
#include <eikmenub.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Forward Declarations]
class CGsmPosRemoteContainer;
// ]]] end generated region [Generated Forward Declarations]

class CGsmPosRemoteEngine;

/**
 * @class	CGsmPosRemoteAppUi GsmPosRemoteAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        container management and the default menu, control pane, and status pane.
 */
class CGsmPosRemoteAppUi : public CAknAppUi
	{
public: 
	// constructor and destructor
	CGsmPosRemoteAppUi();
	void ConstructL();
	virtual ~CGsmPosRemoteAppUi();

public: 
	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );	
	
private:
	void InitializeContainersL();
	// [[[ begin generated region: do not modify [Generated Methods]
public: 
	static TInt RunConfQuery1L( const TDesC* aOverrideText = NULL );
	// ]]] end generated region [Generated Methods]
	
	// [[[ begin generated region: do not modify [Generated Instance Variables]
private: 
	CGsmPosRemoteContainer* iGsmPosRemoteContainer;
	// ]]] end generated region [Generated Instance Variables]
	
	
	// [[[ begin [Overridden Methods]
protected: 
	// ]]] end [Overridden Methods]
	
	
	// [[[ begin [User Handlers]
protected: 
	TBool HandlePingMenuItemSelectedL( TInt aCommand );
	// ]]] end [User Handlers]
	
	CGsmPosRemoteEngine* iEngine;
	};

#endif // GSMPOSREMOTEAPPUI_H			
