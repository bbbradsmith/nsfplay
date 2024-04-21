// nsfplaygui.cpp
//   Contains the public interface.

#include <nsfplaygui.h>

void* nsfplay_gui_about(void * parent)
{
	(void)parent;
	// TODO
	return NULL;
}

void* nsfplay_gui_core(NSFCore* core, void* parent, void (*core_lock)(), void (*core_unlock)())
{
	(void)core;
	(void)parent;
	(void)core_lock;
	(void)core_unlock;
	// TODO
	return NULL;
}

// opens a settings window for the core
void* nsfplay_gui_settings(NSFCore* core, void* parent, bool modal, void (*core_lock)(), void (*core_unlock)())
{
	(void)core;
	(void)parent;
	(void)modal;
	(void)core_lock;
	(void)core_unlock;
	// TODO
	return NULL;
}
