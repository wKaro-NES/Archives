/*
 ============================================================================
 Name		: GsmPosImageFormatDecoder.cpp
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosImageFormatDecoder implementation
 ============================================================================
 */

#include "GsmPosImageFormatDecoder.h"
#include "GsmPosLogger.h"

CGsmPosImageFormatDecoder::CGsmPosImageFormatDecoder() :
	CActive(EPriorityStandard) // Standard priority
	{
	}

CGsmPosImageFormatDecoder* CGsmPosImageFormatDecoder::NewL()
	{
	CGsmPosImageFormatDecoder* self = new (ELeave) CGsmPosImageFormatDecoder();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosImageFormatDecoder::ConstructL()
	{
	CActiveScheduler::Add(this);
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	}

CGsmPosImageFormatDecoder::~CGsmPosImageFormatDecoder()
	{
	Cancel();
	
	delete iDecoder;
	delete iActiveWait;
	}

void CGsmPosImageFormatDecoder::DoCancel()
	{
	iDecoder->Cancel();
	}

CFbsBitmap* CGsmPosImageFormatDecoder::CreateNativeBitmapL(RFs& aFs, const TDesC& aFileName, TSize aSize, TDisplayMode aDisplayMode)
	{
	LOG_FUNCTION_LEAVE;
	
	iDecoder = CImageDecoder::FileNewL(aFs, aFileName);
	
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	//nie mozna polegac na FrameInfo() - size 0x0
	/*
	TFrameInfo frame = iDecoder->FrameInfo();
	User::LeaveIfError(bitmap->Create(frame.iFrameSizeInPixels, frame.iFrameDisplayMode));
	*/
	User::LeaveIfError(bitmap->Create(aSize, aDisplayMode));
	CleanupStack::Pop(bitmap);
	
	iDecoder->Convert(&iStatus, *bitmap);
	SetActive();
	
	//start waiting...
	iActiveWait->Start();
	
	//...finished, check results
	if(iStatus != KErrNone)
		{
		LOG_ERROR(_L("CGsmPosImageFormatDecoder::CreateNativeBitmapL(), error %d"), iStatus.Int());
		
		delete bitmap;
		bitmap = NULL;
		}
	
	delete iDecoder;
	iDecoder = NULL;
	
	return bitmap;
	}

void CGsmPosImageFormatDecoder::RunL()
	{
	LOG_FUNCTION_LEAVE;
	
	iActiveWait->AsyncStop();
	}

TInt CGsmPosImageFormatDecoder::RunError(TInt aError)
	{
	//TODO remove
	return aError;
	}
