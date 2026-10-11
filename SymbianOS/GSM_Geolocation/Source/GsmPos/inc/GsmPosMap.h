/*
 ============================================================================
 Name		: CGsmPosMap.h
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCGsmPosMap declaration
 ============================================================================
 */

#ifndef CGSMPOSMAP_H
#define CGSMPOSMAP_H

#include <vector>
#include <algorithm>
using namespace std;

#include <e32std.h>
#include <e32base.h>

#include <fbs.h>
#include <gdi.h>
#include <bitdev.h>

#include "GsmPosMapObserver.h"
#include "GsmPosEngine.h"

const TInt KPixelMargin = 2; 

class CGsmPosMap : public CBase
	{
	public:
		static CGsmPosMap* NewL(CGsmPosEngine& aEngine);
		~CGsmPosMap();

		//TODO wspolna metoda z CGsmPosGoogleMap - zmien nazwe, refaktoring (wyodrebnienie wspolnego ciala metod)
		TBool GetMapForCellIdAsPolygonL(TBool aMarkCurrentPosition, TBool aFill, TBool aCoverage, const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo,
										TSize aSize, TDisplayMode aDisplayMode,	MGsmPosMapObserver* aObserver);
		
	private:
		CGsmPosMap(CGsmPosEngine& aEngine);
		void ConstructL();
	
		void SetScreenPointCoordsScale(TSize aSize, const vector<TPosition>& aRectangle);
		TPoint GetScreenPointCoords(const TPosition& aPosition);
		
		void DrawPoints(CGraphicsContext* aGc, const vector<TPosition>& aCoords, TRgb aColor);
		void DrawPolygonL(CGraphicsContext* aGc, const vector<TPosition>& aPolygon, TBool aFill, TRgb aColor);
		void DrawTextL(CGraphicsContext* aGc, const TDesC& aText, const TPosition& aPosition, TRgb aColor, TBool aBold);
		
	private:
		CGsmPosEngine& iEngine;
	
		TPosition iPointZeroZero;
		TSize iSize;
		//TODO - refactor, wystarczy iScale
		TReal64 iScaleX, iScaleY;
	};

#endif // CGSMPOSMAP_H
