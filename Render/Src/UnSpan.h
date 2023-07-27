/*=============================================================================
	UnSpan.h: Span buffering functions and structures
	Copyright 1995 Epic MegaGames, Inc.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*------------------------------------------------------------------------------------
	General span buffer related classes.
------------------------------------------------------------------------------------*/

//
// A span buffer linked-list entry representing a free (undrawn) 
// portion of a scanline. 
//
class FSpan
{
public:
	// Variables.
	INT_UNREAL_32S Start, End;
	FSpan* Next;

	// Constructors.
	FSpan()
	{}
	FSpan( INT_UNREAL_32S InStart, INT_UNREAL_32S InEnd )
	:	Start		(InStart)
	,	End			(InEnd)
	{}
};

//
// A raster span.
//
struct FRasterSpan
{
	INT_UNREAL_32S X[2];
};

//
// A raster polygon.
//
class FRasterPoly
{
public:
	INT_UNREAL_32S	StartY;
	INT_UNREAL_32S EndY;
	FRasterSpan Lines[];
};

//
// A span buffer, which represents all free (undrawn) scanlines on
// the screen.
//
class RENDER_API FSpanBuffer
{
public:
	INT_UNREAL_32S			StartY;		// Starting Y value.
	INT_UNREAL_32S			EndY;		// Last Y value + 1.
	INT_UNREAL_32S			ValidLines;	// Number of lines at beginning (for screen).
	FSpan**		Index;		// Contains (EndY-StartY) units pointing to first span or NULL.
	FMemStack*	Mem;		// Memory pool everything is stored in.
	FMemMark	Mark;		// Top of memory pool marker.

	// Constructors.
	FSpanBuffer()
	{}
	FSpanBuffer( const FSpanBuffer& Source, FMemStack& InMem )
	:	StartY		(Source.StartY)
	,	EndY		(Source.EndY)
	,	ValidLines	(Source.ValidLines)
	,	Index		(new(InMem,EndY-StartY)FSpan*)
	,	Mem			(&InMem)
	,	Mark		(InMem)
	{
		for( int i=0; i<EndY-StartY; i++ )
		{
			FSpan** PrevLink = &Index[i];
			for( FSpan* Other=Source.Index[i]; Other; Other=Other->Next )
			{
				*PrevLink = new( *Mem, 1, 4 )FSpan( Other->Start, Other->End );
				PrevLink  = &(*PrevLink)->Next;
			}
			*PrevLink = NULL;
		}
	}

	// Allocation.
	void AllocIndex( INT_UNREAL_32S AllocStartY, INT_UNREAL_32S AllocEndY, FMemStack* Mem );
	void AllocIndexForScreen( INT_UNREAL_32S SXR, INT_UNREAL_32S SYR, FMemStack* Mem );
	void Release();
	void GetValidRange( SWORD* ValidStartY, SWORD* ValidEndY );

	// Merge/copy/alter operations.
	void CopyIndexFrom( const FSpanBuffer& Source, FMemStack* Mem );
	void MergeWith( const FSpanBuffer& Other );

	// Grabbing and updating from rasterizations.
	INT_UNREAL_32S CopyFromRaster( FSpanBuffer& ScreenSpanBuffer, INT_UNREAL_32S RasterStartY, INT_UNREAL_32S RasterEndY, FRasterSpan* Raster );
	INT_UNREAL_32S CopyFromRasterUpdate( FSpanBuffer& ScreenSpanBuffer, INT_UNREAL_32S RasterStartY, INT_UNREAL_32S RasterEndY, FRasterSpan* Raster );

	// Occlusion.
	INT_UNREAL_32S BoxIsVisible( INT_UNREAL_32S X1, INT_UNREAL_32S Y1, INT_UNREAL_32S X2, INT_UNREAL_32S Y2 );

	// Debugging.
	void AssertEmpty( char* Name );
	void AssertNotEmpty( char* Name );
	void AssertValid( char* Name );
	void AssertGoodEnough( char* Name );
};

/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
