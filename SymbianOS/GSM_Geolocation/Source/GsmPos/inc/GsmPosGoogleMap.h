/*
 ============================================================================
 Name		: GsmPosGoogleMap.h
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosGoogleMap declaration
 ============================================================================
 */

#ifndef GSMPOSGOOGLEMAP_H
#define GSMPOSGOOGLEMAP_H

#include <e32std.h>
#include <e32base.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <map>
using namespace std;

#include "GsmPosWebClient.h"
#include "GsmPosImageFormatDecoder.h"
#include "GsmPosEngine.h"
#include "GsmPosMapObserver.h"

//Static Maps V2 API
const TInt KGoogleMapsMaxUrlLength = 2048 - 128;
const TInt KGoogleMapsMaxZoom = 21;
const TInt KGoogleMapsMinZoom = 0;


class CGsmPosGoogleMap : 	public CBase,
							public MGsmPosWebClientObserver
	{
	public:
		static CGsmPosGoogleMap* NewL(CGsmPosEngine& aEngine);
		~CGsmPosGoogleMap();
        
        TBool GetMapForCellIdL(TBool aMarkCurrentPosition, const CTelephony::TNetworkInfoV1 aNetworkInfo, TSize aSize, TDisplayMode aDisplayMode, MGsmPosMapObserver* aObserver);
        TBool GetMapForCellIdAsPolygonL(TBool aMarkCurrentPosition, TBool aCoverage, const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, TSize aSize, TDisplayMode aDisplayMode, MGsmPosMapObserver* aObserver);
        TBool GetMapAgainL();
    
		TBool ZoomLevelIncrease();
		TBool ZoomLevelDecrease();
        
        //TODO - redukcja do potrzebnych metod
    	//MGsmPosWebClientObserver
    	void ClientOpenSessionFailedL(CGsmPosWebClient& aEngine, TInt aError );
    	void ClientConnectingL(CGsmPosWebClient& aEngine);
    	void ClientHeaderReceivedL(CGsmPosWebClient& aEngine, const TDesC& aHeaderData);
    	void ClientBodyReceivedL(CGsmPosWebClient& aEngine, const TDesC8& aBodyData);
    	void ClientConnectionCanceledL(CGsmPosWebClient& aEngine);
    	void ClientResponseCompleteL(CGsmPosWebClient& aEngine);
    	void ClientTransactionSucceededL(CGsmPosWebClient& aEngine);
    	void ClientTransactionFailedL(CGsmPosWebClient& aEngine);
    	void ClientUnknownEventL(CGsmPosWebClient& aEngine, TInt aStatus );
    	void ClientRunErrorL(CGsmPosWebClient& aEngine, TInt aError );
    	TBool ClientGetCredentialsL(CGsmPosWebClient& aEngine, const TUriC8& aUri, const TDesC8& aRealm, TDes& aUsername, TDes& aPassword);
        
	private:
		CGsmPosGoogleMap(CGsmPosEngine& aEngine);
		void ConstructL();
	
		void GoogleMapsIssueHttpRequestL();
		TBool GoogleMapsUrlCreate(TSize aSize, TInt aZoom, TBool aSensor);
		TBool GoogleMapsUrlAddCenter(TReal64 aLongitude, TReal64 aLatitude);
		TBool GoogleMapsUrlAddMarker(TReal64 aLongitude, TReal64 aLatitude, TUint32 aColor, const TDesC8& aSize, const char aSymbol);
		TBool GoogleMapsUrlAddMarkers(const vector<TPosition>& aCoordinates, TUint32 aColor, const TDesC8& aSize, const char aSymbol);
		TBool GoogleMapsUrlAddPolyline(const vector<TPosition>& aCoordinates, TUint aWeight, TUint32 aColor, TBool aFill, TUint32 aColorFill);
		TBool GoogleMapsUrlEncodePolyline(const vector<TPosition>& aCoordinates);
		TBool GoogleMapsUrlEncodeSingleNumber(TReal64 aNumber);
		TBool GoogleMapsUrlSetZoomLevel(TUint aZoomLevel);
		
	private:
		//engine
		CGsmPosEngine& iEngine;
		
		//obsluga HTTP
		CGsmPosWebClient* iWebClient;
		
		//Google maps API
		RBuf8 iGoogleMapsUrl;
		
		//TODO - mechanizm cache? (pamietanie historii pobran)
		TFileName iFileName;
		RFs iFileServer;
		RFile iFile;
		
		CGsmPosImageFormatDecoder* iImageFormatDecoder;
		
		TSize iGoogleMapsSize;
		TDisplayMode iGoogleMapsDisplayMode;
		MGsmPosMapObserver* iGoogleMapsObserver;
		TInt iGoogleMapsZoomLevel;		
	};

#endif // GSMPOSGOOGLEMAP_H
