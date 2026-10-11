/*
========================================================================
 Name        : GsmPosDocument.h
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSDOCUMENT_H
#define GSMPOSDOCUMENT_H

#include <akndoc.h>
		
class CEikAppUi;

/**
* @class	CGsmPosDocument GsmPosDocument.h
* @brief	A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CGsmPosDocument : public CAknDocument
	{
public: 
	// constructor
	static CGsmPosDocument* NewL( CEikApplication& aApp );

private: 
	// constructors
	CGsmPosDocument( CEikApplication& aApp );
	void ConstructL();
	
public: 
	// from base class CEikDocument
	CEikAppUi* CreateAppUiL();
	};
#endif // GSMPOSDOCUMENT_H
