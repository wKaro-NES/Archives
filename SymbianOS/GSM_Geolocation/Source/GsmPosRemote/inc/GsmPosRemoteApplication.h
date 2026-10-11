/*
========================================================================
 Name        : GsmPosRemoteApplication.h
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSREMOTEAPPLICATION_H
#define GSMPOSREMOTEAPPLICATION_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknapp.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Constants]
const TUid KUidGsmPosRemoteApplication = { 0xE50E7AC1 };
// ]]] end generated region [Generated Constants]

/**
 *
 * @class	CGsmPosRemoteApplication GsmPosRemoteApplication.h
 * @brief	A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CGsmPosRemoteApplication : public CAknApplication
	{
private:
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	
	};
			
#endif // GSMPOSREMOTEAPPLICATION_H		
