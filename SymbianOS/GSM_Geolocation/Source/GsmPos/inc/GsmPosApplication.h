/*
========================================================================
 Name        : GsmPosApplication.h
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSAPPLICATION_H
#define GSMPOSAPPLICATION_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknapp.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Constants]
const TUid KUidGsmPosApplication = { 0xE8385A69 };
// ]]] end generated region [Generated Constants]

/**
 *
 * @class	CGsmPosApplication GsmPosApplication.h
 * @brief	A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CGsmPosApplication : public CAknApplication
	{
private:
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	
	};
			
#endif // GSMPOSAPPLICATION_H		
