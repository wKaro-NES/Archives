/*
========================================================================
 Name        : GsmPosRemoteContainer.h
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSREMOTECONTAINER_H
#define GSMPOSREMOTECONTAINER_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <coecntrl.h>		
// ]]] end generated region [Generated Includes]


// [[[ begin [Event Handler Includes]
// ]]] end [Event Handler Includes]

// [[[ begin generated region: do not modify [Generated Forward Declarations]
class MEikCommandObserver;		
class CEikLabel;
// ]]] end generated region [Generated Forward Declarations]

/**
 * Container class for GsmPosRemoteContainer
 * 
 * @class	CGsmPosRemoteContainer GsmPosRemoteContainer.h
 */
class CGsmPosRemoteContainer : public CCoeControl
	{
public:
	// constructors and destructor
	CGsmPosRemoteContainer();
	static CGsmPosRemoteContainer* NewL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver );
	static CGsmPosRemoteContainer* NewLC( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver );
	void ConstructL( 
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver );
	virtual ~CGsmPosRemoteContainer();

	// pokaz operatora/local area code/cell id na ekranie
	void UpdateScreen(const TDesC& aOperator, TInt aLac, TInt aCellId);
	
public:
	// from base class CCoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl( TInt aIndex ) const;
	TKeyResponse OfferKeyEventL( 
			const TKeyEvent& aKeyEvent, 
			TEventCode aType );
	void HandleResourceChange( TInt aType );
	
protected:
	// from base class CCoeControl
	void SizeChanged();

private:
	// from base class CCoeControl
	void Draw( const TRect& aRect ) const;

private:
	void InitializeControlsL();
	void LayoutControls();
	CCoeControl* iFocusControl;
	MEikCommandObserver* iCommandObserver;
	// [[[ begin generated region: do not modify [Generated Methods]
public: 
	// ]]] end generated region [Generated Methods]
	
	// [[[ begin generated region: do not modify [Generated Type Declarations]
public: 
	// ]]] end generated region [Generated Type Declarations]
	
	// [[[ begin generated region: do not modify [Generated Instance Variables]
private: 
	CEikLabel* iOperator;
	CEikLabel* iOperator_val;
	CEikLabel* iLac;
	CEikLabel* iLac_val;
	CEikLabel* iCi;
	CEikLabel* iCi_val;
	// ]]] end generated region [Generated Instance Variables]
	
	
	// [[[ begin [Overridden Methods]
protected: 
	// ]]] end [Overridden Methods]
	
	
	// [[[ begin [User Handlers]
protected: 

	// ]]] end [User Handlers]
	
public: 
	enum TControls
		{
		// [[[ begin generated region: do not modify [Generated Contents]
		EOperator,
		EOperator_val,
		ELac,
		ELac_val,
		ECi,
		ECi_val,
		
		// ]]] end generated region [Generated Contents]
		
		// add any user-defined entries here...
		
		ELastControl
		};
	};
				
#endif // GSMPOSREMOTECONTAINER_H
