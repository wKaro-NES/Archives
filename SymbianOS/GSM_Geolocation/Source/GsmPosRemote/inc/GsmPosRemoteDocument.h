/*
========================================================================
 Name        : GsmPosRemoteDocument.h
 Author      : w.karolik
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSREMOTEDOCUMENT_H
#define GSMPOSREMOTEDOCUMENT_H

#include <akndoc.h>
		
class CEikAppUi;

/**
* @class	CGsmPosRemoteDocument GsmPosRemoteDocument.h
* @brief	A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CGsmPosRemoteDocument : public CAknDocument
	{
public: 
	// constructor
	static CGsmPosRemoteDocument* NewL( CEikApplication& aApp );

private: 
	// constructors
	CGsmPosRemoteDocument( CEikApplication& aApp );
	void ConstructL();
	
public: 
	// from base class CEikDocument
	CEikAppUi* CreateAppUiL();
	};
#endif // GSMPOSREMOTEDOCUMENT_H
