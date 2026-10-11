/*
 ============================================================================
 Name		: CGsmPosMap.cpp
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCGsmPosMap implementation
 ============================================================================
 */

#include "GsmPosMap.h"
#include "GsmPosLogger.h"

#include <aknnotewrappers.h> 

CGsmPosMap::CGsmPosMap(CGsmPosEngine& aEngine)
: iEngine(aEngine), iScaleX(1.0), iScaleY(1.0)
	{
	}

CGsmPosMap::~CGsmPosMap()
	{
	}

CGsmPosMap* CGsmPosMap::NewL(CGsmPosEngine& aEngine)
	{
	CGsmPosMap* self = new (ELeave) CGsmPosMap(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosMap::ConstructL()
	{

	}

//TODO - dodaj podzialke w metrach !?
//TODO - zapamietanie intersekcji aby bezposrednio pobrac google map !?
//TODO - zmien kolejnosc rysowania tak aby fill nie nadpisywa³ punktow(?)
//TODO - gdzie logika wyboru z DB? (wspolna dla google map)
//TODO - gdzie logika kolorowania? (wspolna dla google map)
//TODO - czy nie lepiej przeniesc logike do engine? TAK - inersection z convex hull bedzie trzeba tez wykreslic na Google Mapie
//TODO - kolorystyka wspolna z google maps (przenies do engine kolory)
TBool CGsmPosMap::GetMapForCellIdAsPolygonL(TBool aMarkCurrentPosition, TBool aFill, TBool aCoverage,
											const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo,
											TSize aSize, 
											TDisplayMode aDisplayMode,
											MGsmPosMapObserver* aObserver)
	{
	LOG_FUNCTION_LEAVE;
	
	//TODO - przemysl role sygnalizowania bledu pozycji GPS w tej metodzie (bo mozliwe wielokrotne wywolania automatyczne)
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

	// create a bitmap to be used off-screen
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(aSize, aDisplayMode));

	// create an off-screen device and context
	CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(bmp);
	CleanupStack::PushL(device);
	CGraphicsContext* gc = NULL;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	
	//dopasuj skalowanie do wymiarow ekranu (skala powinna byc policzona dla wszystkich aNetworkInfo)
	vector<TPosition> points;
	vector<TPosition> rect;
	for (TInt i = 0; i < aNetworkInfo.size(); i++)
		{
		//pobierz z bazy danych wszystkie lokalizacje dla wybranej CellId
		vector<TPosition> coords;
		TInt err = iEngine.Database()->GetAllCoordinates(aNetworkInfo[i], coords);
		if (err != KErrNone)
			{
			LOG_ERROR(_L("CGsmPosMap::GetMapForCellIdAsPolygonL(), database error = %d"), err);
			
			User::Leave(err);
			}
		
		//dodaj poprzednie skrajne punkty
		if (!points.empty())
			{
			coords.insert(coords.end(), points.begin(), points.end());
			points.clear();
			}
		
		iEngine.GeomComputeBoundingRect(coords, rect, points);
		}
	
	SetScreenPointCoordsScale(aSize, rect);

	vector<const vector<TPosition>*> convex_hulls;
	
	//wykreslaj dane dla kazdej z CellId
	
	if (aCoverage)
		{
		//wypisz polygon do LOG (aby mozna bylo przygotowac plik KML do GoogleEarth z kszaltem komorki
		// KML header
		LOG(_L("kml;<?xml version=\"1.0\" encoding=\"UTF-8\"?><kml xmlns=\"http://www.opengis.net/kml/2.2\">"));
		LOG(_L("kml;<Document><Style id=\"cell_style\"><LineStyle><width>1.5</width></LineStyle>"));
		LOG(_L("kml;<PolyStyle><color>7dff0000</color></PolyStyle></Style>")); //TODO - format ABGR, kolor operatora?
		}
	
	for (TInt i = 0; i < aNetworkInfo.size(); i++)
		{
		const CTelephony::TNetworkInfoV1 info = aNetworkInfo[i];
		
		//pobierz z bazy danych wszystkie lokalizacje dla wybranej CellId
		vector<TPosition> coords;
		TInt err = iEngine.Database()->GetAllCoordinates(info, coords);
		if (err != KErrNone)
			{
			LOG_ERROR(_L("CGsmPosMap::GetMapForCellIdAsPolygonL(), database error = %d"), err);
			
			User::Leave(err);
			}

		//get unique color for each network operator
		TRgb color = iEngine.GetColorForCellId(info);
		
		//create points
		if (!aCoverage)
			DrawPoints(gc, coords, color);
		
		//create polygons
		//TODO auto_ptr/symbian cleanupdelete etc.
		vector<TPosition>* polygon = new (ELeave) vector<TPosition>();
		iEngine.GeomComputeConvexHullL(coords, *polygon);
		if (!polygon->empty())
			{
			DrawPolygonL(gc, *polygon, EFalse, color);

			if (aCoverage)
				{
				//wypisz cell id przy losowo wybranej pozycji
				TBuf<16> cellid;
				cellid.Format(_L("%d"), aNetworkInfo.at(i).iCellId);
				DrawTextL(gc, cellid,polygon->at(Math::Random() % polygon->size()), color, EFalse);
				
				//wypisz polygon do LOG (aby mozna bylo przygotowac plik KML do GoogleEarth z kszaltem komorki
				// KML body
				//TODO - usun "kml;" (to tylko znacznik w LOG) 
				LOG(_L("kml;<Placemark><name>%S_%S</name>"),&aNetworkInfo.at(i).iShortName, &cellid);
				LOG(_L("kml;<styleUrl>#cell_style</styleUrl><Polygon><extrude>1</extrude><altitudeMode>relativeToGround</altitudeMode>"));
				LOG(_L("kml;<outerBoundaryIs><LinearRing><coordinates>"));
				for (TInt i = 0; i < polygon->size(); i++)
					{
					//TODO - zamien altitude (00000001) aby nie nakladaldaly sie obszary na siebie
					LOG(_L("kml;%f,%f,00000001"), polygon->at(i).Longitude(), polygon->at(i).Latitude());
					}
			    LOG(_L("kml;</coordinates></LinearRing></outerBoundaryIs></Polygon></Placemark>"));
				}
			}
	
		//przechowaj do obliczenia intersekcji
		convex_hulls.push_back(polygon);		//transfer ownership
		}
	
	if (aCoverage)
		{
		//c.d. wypisz polygon do LOG (aby mozna bylo przygotowac plik KML do GoogleEarth z kszaltem komorki
		//KML footer
		LOG(_L("kml;</Document></kml>"));
		
		//dodaj stacje BTS w promieniu 35km od aktualnej pozycji GPS
		vector<TPosition>* bts = iEngine.GpsMonitor()->ReadLandmarkL(TPosition(locality, TTime(0)), 35000);
		if (bts)
			{
			for (TInt i = 0; i < bts->size(); i++)
				DrawTextL(gc, _L("BTS"), bts->at(i), TRgb(0x80,0x80,0x80), EFalse);
			delete bts;
			}
		}
	else
		{
		//create & fill intersections
		vector<vector<TPosition>*> polygons;
		TPosition* position = iEngine.GeomComputeIntersectionL(convex_hulls, &polygons);
		for (TInt i = 0; i < polygons.size(); i++)
			{
			//DrawPoints(gc, *(polygons[i]), KRgbRed);	//FIXME
			DrawPolygonL(gc, *(polygons[i]), aFill, KRgbRed);
			}
		
		//oznacz centroid - wyliczona przez aplikacje pozycje
		if (position)
			{
			//oznacz tekstem 'APP' wlasna wyliczona pozycje na mapie...
			DrawTextL(gc, _L("APP"), *position, TRgb(0xA0, 0x0, 0x0), EFalse);
			
			//...a na ekranie pokaz dokladnosc pozycjonowania
			if (aObserver)
				aObserver->MapPositionMarked(TPosition(locality, TTime(0)), *position);

			delete position;
			position = NULL;
			}

		//cleanup
		for (TInt i = 0; i < polygons.size(); i++)
			{
			delete polygons[i];
			}
		}

	//oznacz pozycje GPS na ekranie
	if (aMarkCurrentPosition)
		{
		DrawTextL(gc, _L("GPS"), TPosition(locality, TTime(0)), TRgb(0x33, 0xCC, 0xFF), ETrue);
		
		//TODO - nie ma sensu wykreslac okregu niedokladnosci chyba...
		}
	
	//show on screen
	aObserver->MapCreated(bmp);				//transfer ownership

	//TODO STL elegancki algorithm etc?
	//cleanup
	for (TInt i = 0; i < convex_hulls.size(); i++)
		{
		delete convex_hulls[i];
		}

	//cleanup
	CleanupStack::PopAndDestroy(gc); 
	CleanupStack::PopAndDestroy(device);
	CleanupStack::Pop(bmp);					//ownership already transferred
	
	return ETrue;
	}

void CGsmPosMap::SetScreenPointCoordsScale(TSize aSize, const vector<TPosition>& aRectangle)
	{
	LOG_FUNCTION;

	//zachowaj wymiary ekranu pomniejszone o marginesy
	iSize = aSize - TSize(2 * KPixelMargin, 2 * KPixelMargin);
	
	TPosition uppper_left = aRectangle[0];
	TPosition bottom_right = aRectangle[2];
	iScaleX = iSize.iWidth / (bottom_right.Longitude() - uppper_left.Longitude());
	iScaleY = iSize.iHeight/ (bottom_right.Latitude() - uppper_left.Latitude());
	
	//zachowaj poczatek ukl. wspolrzednych
	iPointZeroZero = uppper_left;
	
	//zachowaj aspect ratio
	if (iScaleX > iScaleY)
		iScaleX = iScaleY;
	else
		iScaleY = iScaleX;
			
	//TODO - offset tj. logiczne (0,0) aby wycentrowac ekran ?
	}

TPoint CGsmPosMap::GetScreenPointCoords(const TPosition& aPosition)
	{
	TReal64 x = aPosition.Longitude() - iPointZeroZero.Longitude();
	TReal64 y = aPosition.Latitude() - iPointZeroZero.Latitude();
	
	//orientacja ukl. wspolrzednych: pkt(0,0) w lewym dolnym rogu
	return TPoint(KPixelMargin + (TInt) (iScaleX * x), iSize.iHeight - ((TInt) (iScaleY * y)));
	}

void CGsmPosMap::DrawPoints(CGraphicsContext* aGc, const vector<TPosition>& aCoords, TRgb aColor)
	{
	aGc->SetPenColor(aColor);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	
	//TODO co jesli punkty z roznych CellId sie nakladaja (ignorowac, polygon da poprawny widok)
	aGc->SetPenSize(TSize(3,3));
	
	for (TInt i = 0; i < aCoords.size(); i++)
		{
		TPoint point = GetScreenPointCoords(aCoords[i]);
		aGc->Plot(point);
		}
	}

void CGsmPosMap::DrawPolygonL(CGraphicsContext* aGc, const vector<TPosition>& aPolygon, TBool aFill, TRgb aColor)
	{
	aGc->SetPenColor(aColor);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	
	//aGc->SetPenSize(TSize(6,6));	//rysuj tylko wierzcholki
	
	aGc->SetPenSize(TSize(1,1));
	
	CArrayFixFlat<TPoint>* points = new (ELeave)CArrayFixFlat<TPoint>(1);
	CleanupStack::PushL(points);
	points->SetReserveL(aPolygon.size());
	
	for (TInt i = 0; i < aPolygon.size(); i++)
		{
		/* rysuj tylko wierzcholki
		TPoint point = GetScreenPointCoords((*aPolygon)[i]);
		aGc->Plot(point);
		*/
		
		points->AppendL(GetScreenPointCoords(aPolygon[i]));
		}
	
	if (aFill)
		{
		aGc->SetBrushColor(aColor);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		User::LeaveIfError(aGc->DrawPolygon(points));
		}
	else
		{
		aGc->DrawPolyLine(points);
		}
	
	CleanupStack::PopAndDestroy(points);
	}

void CGsmPosMap::DrawTextL(CGraphicsContext* aGc, const TDesC& aText, const TPosition& aPosition, TRgb aColor, TBool aBold)
	{
	const CFont* font = aBold ? CEikonEnv::Static()->NormalFont() : CEikonEnv::Static()->DenseFont();
	aGc->UseFont(font);

	aGc->SetPenColor(aColor);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	
	TPoint point = GetScreenPointCoords(aPosition);

	aGc->SetPenSize(TSize(3,3));
	aGc->Plot(point);

	aGc->SetPenSize(TSize(1,1));
	aGc->DrawText(aText, point);
	
	aGc->DiscardFont();
	}
