/*
 * nsfplay.h written by Mitsutaka Okazaki 2004.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. 
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software 
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not 
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"


class CnsfplayApp : public CWinApp
{
protected:
    HACCEL m_hAccel;
    BOOL m_bAccel;
public:
	CnsfplayApp();

public:
	virtual BOOL InitInstance();
    
    void SetAccel(BOOL bEnable);
    BOOL GetAccel() const;

	DECLARE_MESSAGE_MAP()
    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
};

extern CnsfplayApp theApp;
