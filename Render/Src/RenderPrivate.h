/*=============================================================================
	RenderPrivate.h: Rendering package private header.
	Copyright 1997 Epic MegaGames, Inc.
=============================================================================*/

/*----------------------------------------------------------------------------
	API.
----------------------------------------------------------------------------*/

#ifndef RENDER_API
	#define RENDER_API DLL_IMPORT
#endif

/*------------------------------------------------------------------------------------
	Dependencies.
------------------------------------------------------------------------------------*/

#include "Engine.h"
#include "UnRender.h"
#include "Amd3d.h"

/*------------------------------------------------------------------------------------
	Render package private.
------------------------------------------------------------------------------------*/

#include "UnSpan.h"

#define LINE_NEAR_CLIP_Z   1.0
#define MAKELABEL(A,B,C,D) A##B##C##D

struct FBspDrawList
{
	INT_UNREAL_32S 			iNode;
	INT_UNREAL_32S				iSurf;
	INT_UNREAL_32S				iZone;
	INT_UNREAL_32S				Key;
	DWORD			PolyFlags;
	FSpanBuffer		Span;
	AZoneInfo*		Zone;
	FBspDrawList*	Next;
	FBspDrawList*	SurfNext;
	FActorLink*		Volumetrics;
	FSavedPoly*		Polys;
	FActorLink*		SurfLights;
};

//
// Class encapsulating the dynamic lighting subsystem.
//
class RENDER_API FLightManagerBase
{
public:
	virtual void Init()=0;
	virtual void Exit()=0;
	virtual DWORD SetupForActor( FSceneNode* Frame, AActor* Actor, struct FVolActorLink* LeafLights, FActorLink* Volumetrics )=0;
	virtual void SetupForSurf( FSceneNode* Frame, FCoords& FacetCoords, FBspDrawList* Draw, FTextureInfo*& LightMap, FTextureInfo*& FogMap, FTextureInfo* BumpMap, UBOOL Merged )=0;
	virtual void FinishSurf()=0;
	virtual void FinishActor()=0;
	virtual FPlane Light( FTransSample& Point, DWORD ExtraFlags )=0;
	virtual FPlane Fog( FTransSample& Point, DWORD ExtraFlags )=0;
};

/*------------------------------------------------------------------------------------
	Links.
------------------------------------------------------------------------------------*/

//
// Linked list of actors with volumetric flag.
//
struct FVolActorLink
{
	// Variables.
	FVector			Location;
	AActor*			Actor;
	FVolActorLink*	Next;
	UBOOL			Volumetric;

	// Functions.
	FVolActorLink( FCoords& Coords, AActor* InActor, FVolActorLink* InNext, UBOOL InVolumetric )
	:	Location	( InActor->Location.TransformPointBy( Coords ) )
	,	Actor		( InActor )
	,	Next		( InNext )
	,	Volumetric	( InVolumetric )
	{}
	FVolActorLink( FVolActorLink& Other, FVolActorLink* InNext )
	:	Location	( Other.Location )
	,	Actor		( Other.Actor )
	,	Volumetric	( Other.Volumetric )
	,	Next		( InNext )
	{}
};

/*------------------------------------------------------------------------------------
	Dynamic Bsp contents.
------------------------------------------------------------------------------------*/

struct FDynamicItem
{
	// Variables.
	FDynamicItem*	FilterNext;
	FLOAT			Z;

	// Functions.
	FDynamicItem() {}
	FDynamicItem( INT_UNREAL_32S iNode );
	virtual void Filter( UViewport* Viewport, FSceneNode* Frame, INT_UNREAL_32S iNode, INT_UNREAL_32S Outside ) {}
	virtual void PreRender( UViewport* Viewport, FSceneNode* Frame, FSpanBuffer* SpanBuffer, INT_UNREAL_32S iNode, FVolActorLink* Volumetrics ) {}
};

struct FDynamicSprite : public FDynamicItem
{
	// Variables.
	FSpanBuffer*	SpanBuffer;
	FDynamicSprite*	RenderNext;
	FTransform		ProxyVerts[4];
	AActor*			Actor;
	INT_UNREAL_32S				X1, Y1;
	INT_UNREAL_32S				X2, Y2;
	FLOAT 			ScreenX, ScreenY;
	FLOAT			Persp;
	FActorLink*		Volumetrics;
	FVolActorLink*	LeafLights;

	// Functions.
	FDynamicSprite( FSceneNode* Frame, INT_UNREAL_32S iNode, AActor* Actor );
	FDynamicSprite( AActor* InActor ) : Actor( InActor ), SpanBuffer( NULL ), Volumetrics( NULL ), LeafLights( NULL ) {}
	UBOOL Setup( FSceneNode* Frame );
};

struct FDynamicChunk : public FDynamicItem
{
	// Variables.
	FRasterPoly*	Raster;
	FDynamicSprite* Sprite;

	// Functions.
	FDynamicChunk( INT_UNREAL_32S iNode, FDynamicSprite* InSprite, FRasterPoly* InRaster );
	void Filter( UViewport* Viewport, FSceneNode* Frame, INT_UNREAL_32S iNode, INT_UNREAL_32S Outside );
};

struct FDynamicFinalChunk : public FDynamicItem
{
	// Variables.
	FRasterPoly*	Raster;
	FDynamicSprite* Sprite;

	// Functions.
	FDynamicFinalChunk( INT_UNREAL_32S iNode, FDynamicSprite* InSprite, FRasterPoly* InRaster, INT_UNREAL_32S IsBack );
	void PreRender( UViewport* Viewport,  FSceneNode* Frame, FSpanBuffer* SpanBuffer, INT_UNREAL_32S iNode, FVolActorLink* Volumetrics );
};

struct FDynamicLight : public FDynamicItem
{
	// Variables.
	AActor*			Actor;
	UBOOL			IsVol;
	UBOOL			HitLeaf;

	// Functions.
	FDynamicLight( INT_UNREAL_32S iNode, AActor* Actor, UBOOL IsVol, UBOOL InHitLeaf );
	void Filter( UViewport* Viewport, FSceneNode* Frame, INT_UNREAL_32S iNode, INT_UNREAL_32S Outside );
};

/*------------------------------------------------------------------------------------
	Globals.
------------------------------------------------------------------------------------*/

extern FLightManagerBase* GLightManager;

/*------------------------------------------------------------------------------------
	Debugging stats.
------------------------------------------------------------------------------------*/

//
// General-purpose statistics:
//
#if STATS
	// Macro to execute an optional statistics-related command.
	#define STAT(cmd) cmd

	// All stats.
	struct FRenderStats
	{
		// Misc.
		INT_UNREAL_32S ExtraTime;

		// MeshStats.
		INT_UNREAL_32S MeshTime;
		INT_UNREAL_32S MeshGetFrameTime, MeshProcessTime, MeshLightSetupTime, MeshLightTime, MeshSubTime, MeshClipTime, MeshTmapTime;
		INT_UNREAL_32S MeshCount, MeshPolyCount, MeshSubCount, MeshVertLightCount, MeshLightCount, MeshVtricCount;

		// ActorStats.

		// FilterStats.
		INT_UNREAL_32S FilterTime;

		// RejectStats.

		// SpanStats.

		// ZoneStats.

		// OcclusionStats.
		INT_UNREAL_32S OcclusionTime, ClipTime, RasterTime, SpanTime;
		INT_UNREAL_32S NodesDone, NodesTotal;
		INT_UNREAL_32S NumRasterPolys, NumRasterBoxReject;
		INT_UNREAL_32S NumTransform, NumClip;
		INT_UNREAL_32S BoxTime, BoxChecks, BoxBacks, BoxIn, BoxOutOfPyramid, BoxSpanOccluded;
		INT_UNREAL_32S NumPoints;

		// GameStats.

		// IllumStats.
		INT_UNREAL_32S IllumTime;

		// PolyVStats.
		INT_UNREAL_32S PolyVTime;

		// Actor drawing stats:
		INT_UNREAL_32S NumSprites;			// Number of sprites filtered.
		INT_UNREAL_32S NumChunks;			// Number of final chunks filtered.
		INT_UNREAL_32S NumFinalChunks;		// Number of final chunks.
		INT_UNREAL_32S NumMovingLights;    // Number of moving lights.
		INT_UNREAL_32S ChunksDrawn;		// Chunks drawn.

		// Texture subdivision stats
		INT_UNREAL_32S DynLightActors;		// Number of actors shining dynamic light.

		// Span buffer:
		INT_UNREAL_32S SpanTotalChurn;		// Total spans added.
		INT_UNREAL_32S SpanRejig;			// Number of span index that had to be reallocated during merging.

		// Clipping:
		INT_UNREAL_32S ClipAccept;			// Polygons accepted by clipper.
		INT_UNREAL_32S ClipOutcodeReject;	// Polygons outcode-rejected by clipped.
		INT_UNREAL_32S ClipNil;			// Polygons clipped into oblivion.

		// Memory:
		INT_UNREAL_32S GMem;				// Bytes used in global memory pool.
		INT_UNREAL_32S GDynMem;			// Bytes used in dynamics memory pool.

		// Zone rendering:
		INT_UNREAL_32S CurZone;			// Current zone the player is in.
		INT_UNREAL_32S NumZones;			// Total zones in world.
		INT_UNREAL_32S VisibleZones;		// Zones actually processed.
		INT_UNREAL_32S MaskRejectZones;	// Zones that were mask rejected.

		// Illumination cache:
		INT_UNREAL_32S PalCycles;			// Time spent in palette regeneration.

		// Lighting:
		INT_UNREAL_32S Lightage,LightMem,MeshPtsGen,MeshesGen,VolLightActors;

		// Textures:
		INT_UNREAL_32S UniqueTextures,UniqueTextureMem,CodePatches;

		// Extra:
		INT_UNREAL_32S Extra1,Extra2,Extra3,Extra4;

		// Routine timings:
		INT_UNREAL_32S GetValidRangeCycles;
		INT_UNREAL_32S BoxIsVisibleCycles;
		INT_UNREAL_32S CopyFromRasterUpdateCycles;
		INT_UNREAL_32S CopyFromRasterCycles;
		INT_UNREAL_32S CopyIndexFromCycles;
		INT_UNREAL_32S MergeWithCycles;
		INT_UNREAL_32S CalcRectFromCycles;
		INT_UNREAL_32S CalcLatticeFromCycles;
		INT_UNREAL_32S GenerateCycles;
		INT_UNREAL_32S CalcLatticeCycles;
		INT_UNREAL_32S RasterSetupCycles;
		INT_UNREAL_32S RasterGenerateCycles;
		INT_UNREAL_32S TransformCycles;
		INT_UNREAL_32S ClipCycles;
		INT_UNREAL_32S AsmCycles;
	};
	extern FRenderStats GStat;
#else
	#define STAT(x) /* Do nothing */
#endif // STATS

/*------------------------------------------------------------------------------------
	Random numbers.
------------------------------------------------------------------------------------*/

// Random number subsystem.
// Tracks a list of set random numbers.
class FGlobalRandomsBase
{
public:
	// Functions.
	virtual void Init()=0; // Initialize subsystem.
	virtual void Exit()=0; // Shut down subsystem.
	virtual void Tick(FLOAT TimeSeconds)=0; // Mark one unit of passing time.

	// Inlines.
	FLOAT RandomBase( int i ) {return RandomBases[i & RAND_MASK]; }
	FLOAT Random(     int i ) {return Randoms    [i & RAND_MASK]; }

protected:
	// Constants.
	enum {RAND_CYCLE = 16       }; // Number of ticks for a complete cycle of Randoms.
	enum {N_RANDS    = 256      }; // Number of random numbers tracked, guaranteed power of two.
	enum {RAND_MASK  = N_RANDS-1}; // Mask so that (i&RAND_MASK) is a valid index into Randoms.

	// Variables.
	static FLOAT RandomBases	[N_RANDS]; // Per-tick discontinuous random numbers.
	static FLOAT Randoms		[N_RANDS]; // Per-tick continuous random numbers.
};
extern FGlobalRandomsBase *GRandoms;

/*------------------------------------------------------------------------------------
	Fast approximate math code.
------------------------------------------------------------------------------------*/

#define APPROX_MAN_BITS 10		/* Number of bits of approximate square root mantissa, <=23 */
#define APPROX_EXP_BITS 9		/* Number of bits in IEEE exponent */

extern FLOAT SqrtManTbl[2<<APPROX_MAN_BITS];
extern FLOAT DivSqrtManTbl[1<<APPROX_MAN_BITS],DivManTbl[1<<APPROX_MAN_BITS];
extern FLOAT DivSqrtExpTbl[1<<APPROX_EXP_BITS],DivExpTbl[1<<APPROX_EXP_BITS];

//
// Macro to look up from a power table.
//
#if ASM
#define POWER_ASM(ManTbl,ExpTbl)\
	__asm\
	{\
		/* Here we use the identity sqrt(a*b) = sqrt(a)*sqrt(b) to perform\
		** an approximate floating point square root by using a lookup table\
		** for the mantissa (a) and the exponent (b), taking advantage of the\
		** ieee floating point format.\
		*/\
		__asm mov  eax,[F]									/* get float as int                   */\
		__asm shr  eax,(32-APPROX_EXP_BITS)-APPROX_MAN_BITS	/* want APPROX_MAN_BITS mantissa bits */\
		__asm mov  ebx,[F]									/* get float as int                   */\
		__asm shr  ebx,32-APPROX_EXP_BITS					/* want APPROX_EXP_BITS exponent bits */\
		__asm and  eax,(1<<APPROX_MAN_BITS)-1				/* keep lowest 9 mantissa bits        */\
		__asm fld  DWORD PTR ManTbl[eax*4]					/* get mantissa lookup                */\
		__asm fmul DWORD PTR ExpTbl[ebx*4]					/* multiply by exponent lookup        */\
		__asm fstp [F]										/* store result                       */\
	}\
	return F;
//
// Fast floating point power routines.
// Pretty accurate to the first 10 bits.
// About 12 cycles on the Pentium.
//
inline FLOAT DivSqrtApprox(FLOAT F) {POWER_ASM(DivSqrtManTbl,DivSqrtExpTbl);}
inline FLOAT DivApprox    (FLOAT F) {POWER_ASM(DivManTbl,    DivExpTbl    );}
inline FLOAT SqrtApprox   (FLOAT F)
{
	__asm
	{
		mov  eax,[F]                        // get float as int.
		shr  eax,(23 - APPROX_MAN_BITS) - 2 // shift away unused low mantissa.
		mov  ebx,[F]						// get float as int.
		and  eax, ((1 << (APPROX_MAN_BITS+1) )-1) << 2 // 2 to avoid "[eax*4]".
		and  ebx, 0x7F000000				// 7 bit exp., wipe low bit+sign.
		shr  ebx, 1							// exponent/2.
		mov  eax,DWORD PTR SqrtManTbl [eax]	// index hi bit is exp. low bit.
		add  eax,ebx						// recombine with exponent.
		mov  [F],eax						// store.
	}
	return F;								// compiles to fld [F].
}
#else
inline FLOAT DivSqrtApprox(FLOAT F) {return 1.0/appSqrt(F);}
inline FLOAT DivApprox    (FLOAT F) {return 1.0/F;}
inline FLOAT SqrtApprox   (FLOAT F) {return appSqrt(F);}
#endif

/*------------------------------------------------------------------------------------
	URender.
------------------------------------------------------------------------------------*/

//
// Software rendering subsystem.
//
class RENDER_API URender : public URenderBase
{
	DECLARE_CLASS_WITHOUT_CONSTRUCT(URender,URenderBase,CLASS_Config)

	// Friends.
	friend class  FGlobalSpanTextureMapper;
	friend struct FDynamicItem;
	friend struct FDynamicSprite;
	friend struct FDynamicChunk;
	friend struct FDynamicFinalChunk;
	friend struct FDynamicLight;
	friend class  FLightManager;
	friend void RenderSubsurface
	(
		UViewport*		Viewport,
		FSceneNode*	Frame,
		UTexture*		Texture,
		FSpanBuffer*	Span,
		FTransTexture*	Pts,
		DWORD			PolyFlags,
		INT_UNREAL_32S				SubCount
	);

	// obsolete!!
	enum EDrawRaster
	{
		DRAWRASTER_Flat				= 0,	// Flat shaded
		DRAWRASTER_Normal			= 1,	// Normal texture mapped
		DRAWRASTER_Masked			= 2,	// Masked texture mapped
		DRAWRASTER_Blended			= 3,	// Blended texture mapped
		DRAWRASTER_Fire				= 4,	// Fire table texture mapped
		DRAWRASTER_MAX				= 5,	// First invalid entry
	};

	// Constructor.
	URender();
	static void InternalClassInitializer( UClass* Class );

	// UObject interface.
	void Destroy();

	// URenderBase interface.
	void Init( UEngine* InEngine );
	UBOOL Exec( const char* Cmd, FOutputDevice* Out=GSystem );
	void PreRender( FSceneNode* Frame );
	void PostRender( FSceneNode* Frame );
	void DrawWorld( FSceneNode* Frame );
	UBOOL Deproject( FSceneNode* Frame, INT_UNREAL_32S ScreenX, INT_UNREAL_32S ScreenY, FVector& V );
	UBOOL Project( FSceneNode* Frame, const FVector& V, FLOAT& ScreenX, FLOAT& ScreenY, FLOAT* Scale );
	void DrawActor( FSceneNode* Frame, AActor* Actor );
	void GetVisibleSurfs( UViewport* Viewport, TArray<INT_UNREAL_32S>& iSurfs );
	void OccludeBsp( FSceneNode* Frame );
	void SetupDynamics( FSceneNode* Frame, AActor* Exclude );
	UBOOL BoundVisible( FSceneNode* Frame, FBox* Bound, FSpanBuffer* SpanBuffer, FScreenBounds& Results );
	void GlobalLighting( UBOOL Realtime, AActor* Owner, FLOAT& Brightness, FPlane& Color );
	FSceneNode* CreateMasterFrame( UViewport* Viewport, FVector Location, FRotator Rotation, FScreenBounds* Bounds );
	FSceneNode* CreateChildFrame( FSceneNode* Frame, FSpanBuffer* Span, ULevel* Level, INT_UNREAL_32S iSurf, INT_UNREAL_32S iZone, FLOAT Mirror, const FPlane& NearClip, const FCoords& Coords, FScreenBounds* Bounds );
	void Draw2DClippedLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2 );
	void Draw3DLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector OrigP, FVector OrigQ );
	void DrawCircle( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector& Location, FLOAT Radius );
	void DrawBox( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector Min, FVector Max );

	// Dynamics cache.
	FVolActorLink* FirstVolumetric;

	// Scene frames.
	enum {MAX_FRAME_RECURSION=4};

	// Dynamic lighting.
	enum {MAX_DYN_LIGHT_SURFS=2048};
	enum {MAX_DYN_LIGHT_LEAVES=1024};
	static INT_UNREAL_32S				NumDynLightSurfs;
	static INT_UNREAL_32S				NumDynLightLeaves;
	static INT_UNREAL_32S				MaxSurfLights;
	static INT_UNREAL_32S				MaxLeafLights;
	static INT_UNREAL_32S				DynLightSurfs[MAX_DYN_LIGHT_SURFS];
	static INT_UNREAL_32S				DynLightLeaves[MAX_DYN_LIGHT_LEAVES];
	static FActorLink**		SurfLights;
	static FVolActorLink**	LeafLights;

	// Variables.
	UBOOL					Toggle;
	UBOOL					LeakCheck;

	// Timing.
	DOUBLE					LastEndTime;
	DOUBLE					ThisStartTime;
	DOUBLE					ThisEndTime;
	DWORD					NodesDraw;
	DWORD					PolysDraw;

	// Which stats to display.
	UBOOL FpsStats;
	UBOOL GlobalStats;
	UBOOL MeshStats;
	UBOOL ActorStats;
	UBOOL FilterStats;
	UBOOL RejectStats;
	UBOOL SpanStats;
	UBOOL ZoneStats;
	UBOOL LightStats;
	UBOOL OcclusionStats;
	UBOOL GameStats;
	UBOOL SoftStats;
	UBOOL CacheStats;
	UBOOL PolyVStats;
	UBOOL PolyCStats;
	UBOOL IllumStats;
	UBOOL HardwareStats;
	UBOOL Extra6Stats;
	UBOOL Extra7Stats;
	UBOOL Extra8Stats;

	// OccludeBsp dynamics.
	enum {MAX_NODES  = 65536};
	enum {MAX_POINTS = 128000};
	static struct FDynamicsCache
	{
		FDynamicItem* Dynamics[2];
	}* DynamicsCache;
	static struct FStampedPoint
	{
		FTransform* Point;
		DWORD		Stamp;
	}* PointCache;
	static FMemStack VectorMem;
	static DWORD Stamp;
	INT_UNREAL_32S						NumPostDynamics;
	FDynamicsCache**		PostDynamics;
	FDynamicItem*& Dynamic( INT_UNREAL_32S iNode, INT_UNREAL_32S i )
	{
		return DynamicsCache[iNode].Dynamics[i];
	}

	// Implementation.
	void OccludeFrame( FSceneNode* Frame );
	void DrawFrame( FSceneNode* Frame );
	void LeafVolumetricLighting( FSceneNode* Frame, UModel* Model, INT_UNREAL_32S iLeaf );
	INT_UNREAL_32S ClipBspSurf( INT_UNREAL_32S iNode, FTransform**& OutPts );
	INT_UNREAL_32S AMD3DClipBspSurf( INT_UNREAL_32S iNode, FTransform**& OutPts );
	INT_UNREAL_32S ClipTexPoints( FSceneNode* Frame, FTransTexture* InPts, FTransTexture* OutPts, INT_UNREAL_32S Num0 );
	void DrawActorSprite( FSceneNode* Frame, FDynamicSprite* Sprite );
	void DrawMesh( FSceneNode* Frame, AActor* Owner, FSpanBuffer* SpanBuffer, AZoneInfo* Zone, const FCoords& Coords, FVolActorLink* LeafLights, FActorLink* Volumetrics, DWORD PolyFlags );
	void AMD3DDrawMesh( FSceneNode* Frame, AActor* Owner, FSpanBuffer* SpanBuffer, AZoneInfo* Zone, const FCoords& Coords, FVolActorLink* LeafLights, FActorLink* Volumetrics, DWORD PolyFlags );
	void ShowStat( FSceneNode* Frame, INT_UNREAL_32S& StatYL, const char* Fmt, ... );
	void DrawStats( FSceneNode* Frame );
};
extern RENDER_API URender* GRender;

/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
