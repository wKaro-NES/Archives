/*
 ============================================================================
 Name		: GsmPosImageFormatDecoder.h
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosImageFormatDecoder declaration
 ============================================================================
 */

#ifndef GSMPOSIMAGEFORMATDECODER_H
#define GSMPOSIMAGEFORMATDECODER_H

#include <e32base.h>
#include <e32std.h>	
#include <ImageConversion.h>

class CGsmPosImageFormatDecoder : public CActive
	{
	public:
		static CGsmPosImageFormatDecoder* NewL();
		~CGsmPosImageFormatDecoder();
		
		CFbsBitmap* CreateNativeBitmapL(RFs& aFs, const TDesC& aFileName, TSize aSize, TDisplayMode aDisplayMode);
		
	private:
		CGsmPosImageFormatDecoder();
		void ConstructL();

		// From CActive
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();
	
	private:
		CActiveSchedulerWait* iActiveWait;
		CImageDecoder* iDecoder;
	};

#endif // GSMPOSIMAGEFORMATDECODER_H
