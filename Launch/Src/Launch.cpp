/*=============================================================================
	Launch.cpp: Unreal launcher.
	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

#include "LaunchPrivate.h"
#include "Window.h"
#include "Res\LaunchRes.h"
#pragma warning( disable: 4355 )

extern CORE_API FGlobalPlatform GTempPlatform;
extern "C" {HINSTANCE hInstance;}
extern "C" {char GPackage[64]="Launch";}

/*-----------------------------------------------------------------------------
	WinMain.
-----------------------------------------------------------------------------*/

//
// Main window entry point.
//
INT_UNREAL_32S WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char* InCmdLine, INT_UNREAL_32S nCmdShow )
{
	// Remember instance.
	GIsStarted = 1;
	hInstance = hInInstance;

	// Set package name.
	appStrcpy( GPackage, appPackage() );

	// Init mode.
	GIsServer = 1;
	GIsClient = !ParseParam(appCmdLine(),"SERVER") && !ParseParam(appCmdLine(),"MAKE");
	GIsEditor = ParseParam(appCmdLine(),"EDITOR") || ParseParam(appCmdLine(),"MAKE");

	// Init windowing.
	appChdir( appBaseDir() );
	InitWindowing();

	// Init log window.
	GExecHook = GThisExecHook;
	GLog = new WLog( "" );
	GLog->OpenWindow( NULL, 1 );

	// Begin.
#ifndef _DEBUG
	try
	{
#endif
		// Start main loop.
		GIsGuarded=1;
		GSystem = &GTempPlatform;
		UEngine* Engine = InitEngine();
		if( !GIsRequestingExit )
			MainLoop( Engine );
		ExitEngine( Engine );
		GIsGuarded=0;
#ifndef _DEBUG
	}
	catch( ... )
	{
		// Crashed.
		try {HandleError();} catch( ... ) {}
	}
#endif

	// Shut down.
	GExecHook=NULL;
	if( GLog )
		delete GLog;
	appExit();
	GIsStarted = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
