#pragma once
#include "../nsfplug_ui/nsfplug_ui.h"
#include "../xgm/version.h"
#include "../xgm/player/nsf/nsf.h"

// structure to provide direct access to in_yansf plugin
// note: only virtual members of classes from ui/npm will be accessible
typedef struct
{
	const char* const version; // should match NSFPLUG_TITLE
	NSFplug_UI_DLL** const ui; // could be NULL if not yet opened
	NSFplug_Model* const npm;
} InYansfDirect;
