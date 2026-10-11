/*
 ============================================================================
 Name		: GsmPosGoogleMap.cpp
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosGoogleMap implementation
 ============================================================================
 */

#include "GsmPosGoogleMap.h"
#include "GsmPosLogger.h"

#include <aknnotewrappers.h> 

CGsmPosGoogleMap::CGsmPosGoogleMap(CGsmPosEngine& aEngine)
: iEngine(aEngine)
	{
	iGoogleMapsZoomLevel = 15;
	}

CGsmPosGoogleMap::~CGsmPosGoogleMap()
	{
	LOG_FUNCTION;
	
	delete iImageFormatDecoder;
	delete iWebClient;
	
	iGoogleMapsUrl.Close();
	
	iFile.Close();
	iFileServer.Close();
	}

CGsmPosGoogleMap* CGsmPosGoogleMap::NewL(CGsmPosEngine& aEngine)
	{
	CGsmPosGoogleMap* self = new (ELeave) CGsmPosGoogleMap(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosGoogleMap::ConstructL()
	{
	LOG_FUNCTION_LEAVE;

	iImageFormatDecoder = CGsmPosImageFormatDecoder::NewL();
	
	iWebClient = CGsmPosWebClient::NewL(*this);
	
	iGoogleMapsUrl.CreateL(KGoogleMapsMaxUrlLength);
	
	User::LeaveIfError(iFileServer.Connect());
	}

void CGsmPosGoogleMap::ClientOpenSessionFailedL(CGsmPosWebClient& aEngine, TInt aError )
	{
	LOG_ERROR(_L("CGsmPosGoogleMap::ClientOpenSessionFailedL(,%d)"), aError);

	}
void CGsmPosGoogleMap::ClientConnectingL(CGsmPosWebClient& aEngine)
	{
	LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientHeaderReceivedL(CGsmPosWebClient& aEngine, const TDesC& aHeaderData)
	{
	//LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientBodyReceivedL(CGsmPosWebClient& aEngine, const TDesC8& aBodyData)
	{
	//LOG_FUNCTION_LEAVE;

	iFile.Write(aBodyData);
	}

void CGsmPosGoogleMap::ClientConnectionCanceledL(CGsmPosWebClient& aEngine)
	{
	LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientResponseCompleteL(CGsmPosWebClient& aEngine)
	{
	LOG_FUNCTION_LEAVE;

	iFile.Close();
	
	if (iImageFormatDecoder)
		{
		CFbsBitmap* bmp =  iImageFormatDecoder->CreateNativeBitmapL(iFileServer, iFileName, iGoogleMapsSize, iGoogleMapsDisplayMode);
		
		//update screen
		if (iGoogleMapsObserver)
			{
			iGoogleMapsObserver->MapCreated(bmp);
			}
		//TODO - memory leak
		//delete bmp;
		}
	}

void CGsmPosGoogleMap::ClientTransactionSucceededL(CGsmPosWebClient& aEngine)
	{
	LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientTransactionFailedL(CGsmPosWebClient& aEngine)
	{
	LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientUnknownEventL(CGsmPosWebClient& aEngine, TInt aStatus )
	{
	LOG_FUNCTION_LEAVE;

	}

void CGsmPosGoogleMap::ClientRunErrorL(CGsmPosWebClient& aEngine, TInt aError )
	{
	LOG_ERROR(_L("CGsmPosGoogleMap::ClientRunErrorL(, %d)"), aError);

	//update screen
	if (iGoogleMapsObserver)
		{
		iGoogleMapsObserver->MapCreated(NULL);
		}
	}

TBool CGsmPosGoogleMap::ClientGetCredentialsL(CGsmPosWebClient& aEngine, const TUriC8& aUri, const TDesC8& aRealm, TDes& aUsername, TDes& aPassword)
	{
	LOG_FUNCTION_LEAVE;

	//TODO - jak dotychczas google maps api nie wymaga autoryzacji
	return EFalse;
	}

//TODO - Refactor:
//		a) otwarcie pliku i HTTP GET w odzielnej metodzie (tak aby mozna bylo doczepic inne dane do Url'a)
//		b) AddMarker/AddCenter
//		c) do klasy GoogleMapsApi wraz z WebClientem
//		d) kolory itp (enumy?)
TBool CGsmPosGoogleMap::GetMapForCellIdL(TBool aMarkCurrentPosition, const CTelephony::TNetworkInfoV1 aNetworkInfo, TSize aSize, TDisplayMode aDisplayMode, MGsmPosMapObserver* aObserver)
	{
	LOG_FUNCTION_LEAVE;
	
	if (!iWebClient->IsRunning())
		{
		TLocality locality;
		TBool gps_valid = iEngine.Position(locality);
		if (!gps_valid)
			{
			LOG(_L("use cached position"));
			
			//TODO - usun docelowo notke
			//TODO - teksty z RSC, UI poza ta klasa
			CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote;
			note->ExecuteLD(_L("cached GPS data"));		
			}
		//FIXME:	//iLatitude = 61.493500f;	iLongitude = 23.822267f;
		//TODO zoom z UI, sensor z UI?
		if (GoogleMapsUrlCreate(aSize, iGoogleMapsZoomLevel, ETrue))
			{
			if (!aMarkCurrentPosition || GoogleMapsUrlAddMarker(locality.Longitude(), locality.Latitude(), 0x33CCFF, _L8("mid"), 'G'))
				{
				//TODO - refactor
				iGoogleMapsSize = aSize;
				iGoogleMapsDisplayMode = aDisplayMode;
				iGoogleMapsObserver = aObserver;
						
				//pobierz z bazy danych wszystkie lokalizacje dla wybranej CellId
				vector<TPosition> coords;
				TInt err = iEngine.Database()->GetAllCoordinates(aNetworkInfo, coords);
				if (err != KErrNone)
					{
					LOG_ERROR(_L("CGsmPosGoogleMap::GetMapForCellIdL(), database error = %d"), err);
					
					return EFalse;
					}

				if (GoogleMapsUrlAddMarkers(coords, iEngine.GetColorForCellId24bit(aNetworkInfo), _L8("tiny"), 'o'))
					{
					GoogleMapsIssueHttpRequestL();
					
					//...a na ekranie pokaz pasek wait
					iGoogleMapsObserver->MapDownloadStarted();
					return ETrue;
					}
				}
			}
		}
	
	return EFalse;
	}

TBool CGsmPosGoogleMap::GetMapAgainL()
	{
	LOG_FUNCTION_LEAVE;
	
	if (!iWebClient->IsRunning())
		{
			if (GoogleMapsUrlSetZoomLevel(iGoogleMapsZoomLevel))
				{
				//...a na ekranie pokaz pasek wait
				iGoogleMapsObserver->MapDownloadStarted();
				
				GoogleMapsIssueHttpRequestL();
				return ETrue;
				}
		}
	
	return EFalse;
	}

TBool CGsmPosGoogleMap::GetMapForCellIdAsPolygonL(TBool aMarkCurrentPosition, TBool aCoverage, const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, TSize aSize, TDisplayMode aDisplayMode, MGsmPosMapObserver* aObserver)
	{
	LOG_FUNCTION_LEAVE;
	
	if (!iWebClient->IsRunning())
		{
		TLocality locality;
		TBool gps_valid = iEngine.Position(locality);
		if (!gps_valid)
			{
			LOG(_L("use cached position"));
			
			//TODO - usun docelowo notke
			//TODO - teksty z RSC, UI poza ta klasa
			CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote;
			note->ExecuteLD(_L("cached GPS data"));		
			}
		//TODO zoom z UI, sensor z UI?
		if (GoogleMapsUrlCreate(aSize, iGoogleMapsZoomLevel, ETrue))
			{
			TBool success = ETrue;
			if (aMarkCurrentPosition)
				{
				//oznacz symbolem 'G' pozycje GPS wraz z okregiem pokazujacym niedokladnosc lokalizacji
				
				//TODO - kolory wspolne dla local map
				//Uwaga! uzyta "GPS cached position" aby moc demonstrowac w pomieszczeniach zamknietych
				TReal x = locality.Longitude();
				TReal y = locality.Latitude();
				if (success = GoogleMapsUrlAddMarker(x, y, 0x33CCFF, _L8("mid"), 'G'))					//24bit color for markers
					{
					//zamien metry na stopnie 
					//TODO - profesjonalny algorytm, datum ? - a przynajmniej omowic w dokumencie...
					//TODO - uzyj LBS math function (patrz: CGsmPosEngine::GeomComputeCentroidL())... a to zostaw do dokumentu?
					TReal rx = locality.HorizontalAccuracy() / (40074784.2129548 / 360.0);	//obwod Ziemii rownikowy
					TReal ry = locality.HorizontalAccuracy() / (39940952.3658944 / 180.0);	//obwod Ziemii biegunowy
					vector<TPosition>* circle = iEngine.GeomComputeGeoCircleL(x, y, rx, ry);
					success = GoogleMapsUrlAddPolyline(*circle, 2, 0x33CCFFFF, ETrue, 0x33CCFF30);	//z zaznaczona granica okregu!
					delete circle;
					}
				
				//centruj mape na pozycji GPS
				if (aCoverage)
					{
					if (success)
						success = GoogleMapsUrlAddCenter(x, y);
					}				
				}
			if (success)
				{
				//TODO - refactor
				iGoogleMapsSize = aSize;
				iGoogleMapsDisplayMode = aDisplayMode;
				iGoogleMapsObserver = aObserver;
			
				TInt url_checkpoint = iGoogleMapsUrl.Length();
				vector<const vector<TPosition>*> convex_hulls;
				
				for (TInt i = 0; i < aNetworkInfo.size(); i++)
					{
					const CTelephony::TNetworkInfoV1 info = aNetworkInfo[i];
					
					//pobierz z bazy danych wszystkie lokalizacje dla wybranej CellId
					vector<TPosition> coords;
					TInt err = iEngine.Database()->GetAllCoordinates(info, coords);
					if (err != KErrNone)
						{
						LOG_ERROR(_L("CGsmPosGoogleMap::GetMapForCellIdAsPolygonL(), database error = %d"), err);
						
						return EFalse;
						}
					
					//create polygons
					//TODO auto_ptr/symbian cleanupdelete etc.
					vector<TPosition>* polygon = new (ELeave) vector<TPosition>();
					iEngine.GeomComputeConvexHullL(coords, *polygon);
				
					//przechowaj do obliczenia intersekcji
					convex_hulls.push_back(polygon);		//transfer ownership
					
					//pozostaw wspolny kolor gdyz wielokrotne przecinanie tych samych obszarow zwiekszy intensywnosc koloru dobrze oddajac koncepcje intersekcji
					//(LocalMap - to widok "naukowy", GoogleMap - to widok "komercyjny"
					if (success)
						{
						if (aCoverage)
							{
							//dla mapy pokrycia kolory wg operatora
							TUint32 color = iEngine.GetColorForCellId24bit(info) << 8;
							success = GoogleMapsUrlAddPolyline(*polygon, 2, color | 0xa0, ETrue, color | 0x40);
							}
						else
							{
							//wyliczona pozycja - kolory czerwieni
							success = GoogleMapsUrlAddPolyline(*polygon, 2, 0xFF000000, ETrue, 0xFF000040);
							}
						}
					if (!success)
						break;
					}
				
				if (!success)
					{
					//odrzuc polygon dla kazdej CellId - pozostaw miejsce dla intersection
					iGoogleMapsUrl.Delete(url_checkpoint, iGoogleMapsUrl.Length() - url_checkpoint);
					success = ETrue;
					}
				
				if (aCoverage)
					{
					//dodaj stacje BTS w promieniu 35km od aktualnej pozycji GPS...
					vector<TPosition>* bts = iEngine.GpsMonitor()->ReadLandmarkL(TPosition(locality, TTime(0)), 35000);
					if (bts)
						{
						if (!bts->empty())
							success = GoogleMapsUrlAddMarkers(*bts, 0x808080, _L8("mid"), 'B');
						delete bts;
						}
					
					if (success)
						{
						//...a na ekranie pokaz pasek wait
						iGoogleMapsObserver->MapDownloadStarted();
						}
					}
				else
					{
					//create & fill intersections and mark computed position...
					vector<vector<TPosition>*> polygons;
					TPosition* position = iEngine.GeomComputeIntersectionL(convex_hulls, &polygons);
					if (position)
						{
						//oznacz symbolem 'A' wlasna wyliczona pozycje na mapie...
						success = GoogleMapsUrlAddMarker(position->Longitude(), position->Latitude(), 0xFF0000, _L8("mid"), 'A');	//24bit color for markers
						
						//...a na ekranie pokaz dokladnosc pozycjonowania...
						iGoogleMapsObserver->MapPositionMarked(TPosition(locality, TTime(0)), *position);
						
						//...i centruj mape na wyliczonej lokalizacji
						if (success)
							{
							success = GoogleMapsUrlAddCenter(position->Longitude(), position->Latitude());
							}
						
						delete position;
						}
					if (success)
						{
						for (TInt i = 0; i < polygons.size(); i++)
							{
							success = GoogleMapsUrlAddPolyline(*(polygons[i]), 2, 0xFF0000a0, ETrue, 0xFF000080);
							if (!success)
								break;
							}
						}
					//cleanup
					for (TInt i = 0; i < polygons.size(); i++)
						{
						delete polygons[i];
						}
					}

				//TODO STL elegancki algorithm etc?
				//cleanup
				for (TInt i = 0; i < convex_hulls.size(); i++)
					{
					delete convex_hulls[i];
					}

				if (success)
					GoogleMapsIssueHttpRequestL();
				
				return success;
				}
			}
		}
	
	return EFalse;
	}

void CGsmPosGoogleMap::GoogleMapsIssueHttpRequestL()
	{
	LOG_FUNCTION_LEAVE;
	
	//TODO - lokalizacja na obrazki: karta pamieci albo nadpisuj ten sam plik
	_LIT(KFilePath, "c:\\logs\\gsmpos\\");
	iFile.Close();
	TInt err = iFile.Temp(iFileServer, KFilePath, iFileName, EFileWrite);
	if (err)
		{
		LOG_ERROR(_L("can't create file err = %d"), err);
		
		User::Leave(err);
		}
	
	LOG_MULTILINE(iGoogleMapsUrl);
	
	iWebClient->IssueHTTPGetL(iGoogleMapsUrl);	
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlCreate(TSize aSize, TInt aZoom, TBool aSensor)
	{
	LOG_FUNCTION;

	const TInt KMaxImageSizeWidth = 640;
	const TInt KMaxImageSizeHeight = 640;

	//API v.2.0 nie wymaga API key
	//_LIT8(KApiKey, "ABQIAAAAoEv781OJhKsRC8aU_NHyJxSv3lFoAAQ4BHrF-oNImXumNc_uGxRb1pE_BJOEMqWel4GXPo_l3OMFcg");
	//_LIT8(KUrlFormat, "http://maps.google.com/maps/api/staticmap?format=png&zoom=%u&size=%ux%u&sensor=%S&key=%S");
	_LIT8(KUrlFormat, "http://maps.google.com/maps/api/staticmap?format=png&zoom=%u&size=%ux%u&sensor=%S");
	_LIT8(KTrue, "true");
	_LIT8(KFalse,"false");
	
	if (aZoom > KGoogleMapsMaxZoom)
		aZoom = KGoogleMapsMaxZoom;
	else if (aZoom < KGoogleMapsMinZoom)
			aZoom = KGoogleMapsMinZoom;
	
	//iGoogleMapsUrl.Format(KUrlFormat, aZoom, aSize.iWidth, aSize.iHeight, aSensor ? &KTrue() : &KFalse(), &KApiKey());
	iGoogleMapsUrl.Format(KUrlFormat, aZoom, aSize.iWidth, aSize.iHeight, aSensor ? &KTrue() : &KFalse());
	
	//LOG_DUMP(iGoogleMapsUrl);
	
	if (aSize.iHeight > KMaxImageSizeHeight || aSize.iWidth > KMaxImageSizeWidth)
		{
		return EFalse;
		}
	
	return ETrue;
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlAddCenter(TReal64 aLongitude, TReal64 aLatitude)
	{
	LOG_FUNCTION;
	
	_LIT8(KUrlFormat, "center=%f,%f&");
	
	TBuf8<64> buf;
	buf.Format(KUrlFormat, aLatitude, aLongitude);

	TBool success = EFalse;
	if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
		{
		TInt query_start = iGoogleMapsUrl.Locate(TChar('?'));
		if (query_start != KErrNotFound)
			{
			iGoogleMapsUrl.Insert(query_start + 1,buf);
			success = ETrue;
			}
		}
	
	//LOG_DUMP(iGoogleMapsUrl);
	
	return success;
	}

//TODO enum dla rozmiaru, i koloru?
TBool CGsmPosGoogleMap::GoogleMapsUrlAddMarker(TReal64 aLongitude, TReal64 aLatitude, TUint32 aColor, const TDesC8& aSize, const char aSymbol)
	{
	LOG_FUNCTION;
	
	//color = {black, brown, green, purple, yellow, blue, gray, orange, red, white}
	//TODO opcjonalne - jesli pominiesz bedzie "normal" najwiekszy
	//color 24bit
	//symbol = {tiny, mid, small}
	_LIT8(KUrlFormat, "&markers=color:0x%06x|size:%S|label:%c|%f,%f");
	
	TBuf8<128> buf;
	buf.Format(KUrlFormat, aColor, &aSize, aSymbol, aLatitude, aLongitude);

	TBool success = EFalse;
	if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
		{
		iGoogleMapsUrl.Append(buf);
		success = ETrue;
		}
	
	//LOG_DUMP(iGoogleMapsUrl);
	
	return success;
	
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlAddMarkers(const vector<TPosition>& aCoordinates, TUint32 aColor, const TDesC8& aSize, const char aSymbol)
	{
	LOG_FUNCTION;
	
	//Note that transparencies are supported in paths, though they are not supported for markers.
	//color = {black, brown, green, purple, yellow, blue, gray, orange, red, white}
	//color 24bit
	//TODO opcjonalne - jesli pominiesz bedzie "normal" najwiekszy
	//symbol = {tiny, mid, small}
	_LIT8(KUrlFormat, "&markers=color:0x%06x|size:%S|label:%c");
	_LIT8(KUrlFormatCoordinates, "|%f,%f");
	
	TBool success = EFalse;
	
	if (!aCoordinates.empty())
		{
		TBuf8<128> buf;
		buf.Format(KUrlFormat, aColor, &aSize, aSymbol);
		
		if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
			{
			iGoogleMapsUrl.Append(buf);
			success = ETrue;
			}
		else
			return EFalse;
		
		vector<TPosition>::const_iterator iter;
		for (iter = aCoordinates.begin(); iter != aCoordinates.end(); iter++)
			{
			buf.Format(KUrlFormatCoordinates, (*iter).Latitude(), (*iter).Longitude());
			if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
				{
				iGoogleMapsUrl.Append(buf);
				}
			else
				{
				success = EFalse;
				break;
				}
			}
		
		//LOG_DUMP(iGoogleMapsUrl);
		}
	
	return success;
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlAddPolyline(const vector<TPosition>& aCoordinates, TUint aWeight, TUint32 aColor, TBool aFill, TUint32 aColorFill)
	{
	LOG_FUNCTION;

	//Color: When a 32-bit hex value is specified, the last two characters specify the 8-bit alpha transparency value.
	//		This value varies between 00  (completely transparent) and FF (completely opaque).
	_LIT8(KUrlFormat, "&path=weight:%u|color:0x%08x");
	_LIT8(KUrlFormatFill, "&path=weight:%u|color:0x%08x|fillcolor:0x%08x");
	_LIT8(KUrlFormatCoordinates, "|%f,%f");
	_LIT8(KUrlFormatEncoding, "|enc:");
	
	TBool success = EFalse;
	
	if (!aCoordinates.empty())
		{
		TBuf8<128> buf;
		if (aFill)
			{
			buf.Format(KUrlFormatFill, aWeight, aColor, aColorFill);
			}
		else
			{
			buf.Format(KUrlFormat, aWeight, aColor);
			}
	
		//TODO - refaktor, wielokrotnie duplikowany kod
		//TODO - KGoogleMapsMaxUrlLength jest chyba za duzy
		if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
			{
			iGoogleMapsUrl.Append(buf);
			success = ETrue;
			}
		else
			return EFalse;
		
		TBool enable_polyline_encoding = ETrue;						//TODO debug option - application settings?						
		if (enable_polyline_encoding)
			{
			//koduj wspolrzedne aby nie przekroczyc limitu dlugosci GET URL
			if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
				{
				iGoogleMapsUrl.Append(KUrlFormatEncoding);
				success = GoogleMapsUrlEncodePolyline(aCoordinates);
				}
			}
		else
			{
			//koduj w sposob czytelny dla czlowieka - ulatwienie dla debug
			vector<TPosition>::const_iterator iter;
			for (iter = aCoordinates.begin(); iter != aCoordinates.end(); iter++)
				{
				buf.Format(KUrlFormatCoordinates, (*iter).Latitude(), (*iter).Longitude());
				if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
					{
					iGoogleMapsUrl.Append(buf);
					}
				else
					{
					success = EFalse;
					break;
					}
				}
			}
		
		//LOG_DUMP(iGoogleMapsUrl);
		}
	
	//TODO FIXME - remove this, test only
	/*
	{
		LOG(_L("GoogleMapsUrlEncodePolyline() TEST"));
		
		TInt save = iGoogleMapsUrl.Length();
		
		vector<TPosition> vect;
		vect.push_back(TPosition(TLocality(TCoordinate(38.5, -120.2), 0), TTime(0)));
		vect.push_back(TPosition(TLocality(TCoordinate(40.7, -120.95), 0), TTime(0)));
		vect.push_back(TPosition(TLocality(TCoordinate(43.252, -126.453), 0), TTime(0)));
		TBool success = GoogleMapsUrlEncodePolyline(vect);		//_p~iF~ps|U_ulLnnqC_mqNvxq`@
		
		LOG_DUMP(iGoogleMapsUrl.Mid(save));
		iGoogleMapsUrl.SetLength(save);		//obetnij dodane dane
	
		vect.clear();
		vect.push_back(TPosition(TLocality(TCoordinate(52.13096,21.06597), 0), TTime(0)));
		success = GoogleMapsUrlEncodePolyline(vect);			//oyd}Himq_C
		
		LOG_DUMP(iGoogleMapsUrl.Mid(save));
		iGoogleMapsUrl.SetLength(save);		//obetnij dodane dane

		vect.clear();
		vect.push_back(TPosition(TLocality(TCoordinate(-0.0001,-0.00042), 0), TTime(0)));	//RrA
		vect.push_back(TPosition(TLocality(TCoordinate(0.0,-0.00026), 0), TTime(0)));	//S_@
		vect.push_back(TPosition(TLocality(TCoordinate(-0.000001,-0.00025), 0), TTime(0)));	//?p@		//that was source of bug!
		
		success = GoogleMapsUrlEncodePolyline(vect);			//RrAS_@?A
		
		LOG_DUMP(iGoogleMapsUrl.Mid(save));
		iGoogleMapsUrl.SetLength(save);		//obetnij dodane dane
		
	}
	*/
	//\FIXME
	
	return success;
	}

//TODO zoomlevel?
TBool CGsmPosGoogleMap::GoogleMapsUrlEncodePolyline(const vector<TPosition>& aCoordinates)
	{
	LOG_FUNCTION;
	
	/*
	Using the enc: prefix within the location declaration of the path. 
	Note that if you supply an encoded polyline path for a map, you do not need to specify the (normally required) center and zoom parameters.
	*/
	
	TBool success = EFalse;
	
	for (TInt i = 0; i < aCoordinates.size(); i++)
		{
		success = GoogleMapsUrlEncodeSingleNumber(i == 0 ? aCoordinates[i].Latitude() : aCoordinates[i].Latitude() - aCoordinates[i - 1].Latitude());
		if (success)
			success = GoogleMapsUrlEncodeSingleNumber(i == 0 ? aCoordinates[i].Longitude() : aCoordinates[i].Longitude() - aCoordinates[i - 1].Longitude());
		if (!success)
			return EFalse;
		}
	
	return success;
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlEncodeSingleNumber(TReal64 aNumber)
	{
	/*
	Encoding Latitudes and Longitudes
	The encoding process converts a binary value into a series of character codes for ASCII characters using the familiar base64 encoding scheme:
	 to ensure proper display of these characters, encoded values are summed with 63 (the ASCII character '?') before converting them into ASCII.
	The algorithm also checks for additional character codes for a given point by checking the least significant bit of each byte group;
	 if this bit is set to 1, the point is not yet fully formed and additional data must follow.
	Additionally, to conserve space, points only include the offset from the previous point (except of course for the first point). 
	All points are encoded in Base64 as signed integers, as latitudes and longitudes are signed values. 
	The encoding format within a polyline needs to represent two coordinates representing latitude and longitude to a reasonable precision. 
	Given a maximum longitude of +/- 180 degrees to a precision of 5 decimal places (180.00000 to -180.00000), 
	this results in the need for a 32 bit signed binary integer value.
	*/
	
	TBuf<16> buf;
	
	//Algorithm:
	//Take the initial signed value, then...
	//Take the decimal value and multiply it by 1e5, rounding the result
	//Convert the decimal value to binary. Note that a negative value must be calculated using its two's complement by inverting the binary value and adding one to the result:
	TReal number = 0.0;
	Math::Round(number, aNumber, 5);
	TInt32 num = TInt32 (number * 100000);
	
	//Left-shift the binary value one bit:
	num = num << 1;
	
	//If the original decimal value is negative, invert this encoding:
	if (number < 0)
		num ^= 0xFFFFFFFF;
	
	//Break the binary value out into 5-bit chunks (starting from the right hand side):
	//Place the 5-bit chunks into reverse order:
	for (TInt i = 0;i < 6; i++)
		{
		TBool finish = EFalse;
		TUint8 chunk = (TUint8) ((num >> (i * 5))  & 0x1F);
		
		//OR each value with 0x20 if another bit chunk follows:
		if (i < 5 && (((num >> ((i + 1) * 5))) != 0))
			chunk |= 0x20;
		else
			finish = ETrue;
		
		//Convert each value to decimal:
		//Add 63 to each value:
		TChar chr(chunk + 63);
		
		//Convert each value to its ASCII equivalent:
		#ifdef __WINS__
		buf.Append(chr);
		#else
		//Uwaga! z zakresu 0x3F - 0x7E (czyli mozliwych znakow po jako wyjscie w/w algorytmu) encodowania wymaga pare znakow (blad na device)
		if (chr == TChar('`'))
			buf.Append(_L("%60"));
		else if (chr == TChar('?'))
			buf.Append(_L("%3F"));
		else if (chr == TChar('|'))
			buf.Append(_L("%7C"));
		else if (chr == TChar('\\'))
			buf.Append(_L("%5C"));
		else
			buf.Append(chr);
		#endif

		/* 
		//Note that the backslash is interpreted as an escape character within string literals. 
		//Any output of this utility should convert backslash characters to double-backslashes within string literals.
		if (chr == TChar('\\'))
			buf.Append(chr);
		*/

		if (finish)
			break;
		}
	
	if (iGoogleMapsUrl.Length() + buf.Length() <= KGoogleMapsMaxUrlLength)
		{
		iGoogleMapsUrl.Append(buf);
		return ETrue;
		}

	return EFalse;
	}

TBool CGsmPosGoogleMap::GoogleMapsUrlSetZoomLevel(TUint aZoomLevel)
	{
	LOG_FUNCTION;
	
	_LIT8(KZoom, "zoom=");

	if (aZoomLevel > KGoogleMapsMaxZoom)
		aZoomLevel = KGoogleMapsMaxZoom;
	else if (aZoomLevel < KGoogleMapsMinZoom)
			aZoomLevel = KGoogleMapsMinZoom;
	
	TBool success = EFalse;
	
	TBuf8<16> buf;
	buf.Num(aZoomLevel);

	if (iGoogleMapsUrl.Length() + buf.MaxLength() <= KGoogleMapsMaxUrlLength)
		{
		TInt start = iGoogleMapsUrl.FindF(KZoom);
		if (start >= 0)
			{
			TInt end = start + iGoogleMapsUrl.Mid(start).Locate(TChar('&'));
			if (end > start)
				{
				start += KZoom().Length();
				iGoogleMapsUrl.Replace(start, end-start, buf);
				success = ETrue;
				}
			}
		}
	
	//LOG_DUMP(iGoogleMapsUrl);
	
	return success; 
	}

TBool CGsmPosGoogleMap::ZoomLevelIncrease()
	{
	if (iGoogleMapsZoomLevel + 1 <= KGoogleMapsMaxZoom)
		{
		iGoogleMapsZoomLevel++;
		return ETrue;
		}
	return EFalse;
	}

TBool CGsmPosGoogleMap::ZoomLevelDecrease()
	{
	if (iGoogleMapsZoomLevel - 1 >= KGoogleMapsMinZoom)
		{
		iGoogleMapsZoomLevel--;
		return ETrue;
		}
	return EFalse;
	}

