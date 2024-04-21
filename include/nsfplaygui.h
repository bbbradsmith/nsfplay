#pragma once

// NSFPlay gui library public interface

// void* is used to pass/return a WXWidget handle

#include "nsfplaycore.h"

extern "C"
{

// opens an about window (modal)
void* nsfplay_gui_about(void * parent);

// opens a control window for the core
// - returns a window handle as a void* (modeless)
// - core_lock/unlock will be used to mutex all acces to the core state, if not NULL
void* nsfplay_gui_core(NSFCore* core, void* parent, void (*core_lock)(), void (*core_unlock)());

// opens a settings window for the core
void* nsfplay_gui_settings(NSFCore* core, void* parent, bool modal, void (*core_lock)(), void (*core_unlock)());

// TODO other panels? channels? etc.?

} // extern "C"
