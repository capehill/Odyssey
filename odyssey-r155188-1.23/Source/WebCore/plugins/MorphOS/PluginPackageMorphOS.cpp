/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "PluginPackage.h"

#include <wtf/text/CString.h>
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include "npruntime_impl.h"
#include "PluginDebug.h"
#include "../../../WebKit/OrigynWebBrowser/Api/MorphOS/gui.h"

/* Debug output to serial handled via D(bug("....."));
*  See Base/debug.h for details.
*  D(x)    - to disable debug
*  D(x) x  - to enable debug
*/
#define D(x)

#define MIN_PLUGIN_VERSION 2

#if 0

#include <ppcinline/macros.h>
#define PLUGIN_BASE_NAME this->m_module

#define NP_Initialize(__p0) \
	LP1(30, short , NP_Initialize, \
		void *, __p0, a0, \
		, PLUGIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define NP_ShutDown(__p0) \
	LP0(36, short , NP_ShutDown, \
		, PLUGIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define NP_GetEntryPoints(__p0) \
	LP1(42, short , NP_GetEntryPoints, \
		void *, __p0, a0, \
		, PLUGIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define NP_GetMIMEDescription() \
	LP0(48, char *, NP_GetMIMEDescription, \
		, PLUGIN_BASE_NAME, 0, 0, 0, 0, 0, 0)
#endif 

#ifdef __amigaos4__
	#define NP_Initialize(__p0) (NPERR_GENERIC_ERROR)
	#define NP_ShutDown(__p0) (NPERR_GENERIC_ERROR)
	#define NP_GetEntryPoints(__p0) (NPERR_GENERIC_ERROR)
	#define NP_GetMIMEDescription() (NPERR_GENERIC_ERROR)
#endif
	
		
namespace WebCore {

bool PluginPackage::fetchInfo()
{
    if (!load())
        return false;

	if (!(&m_pluginFuncs)->getvalue)
        return false;

    char* buffer = 0;
	NPError err = (&m_pluginFuncs)->getvalue(0, NPPVpluginNameString, &buffer);
    if (err == NPERR_NO_ERROR)
        m_name = buffer;

    buffer = 0;
	err = (&m_pluginFuncs)->getvalue(0, NPPVpluginDescriptionString, &buffer);
    if (err == NPERR_NO_ERROR) {
        m_description = buffer;
        determineModuleVersionFromDescription();
    }

	String types = (char *) NP_GetMIMEDescription();
	Vector<String> mimeDescs;
	types.split(";", true, mimeDescs);
	
	for (size_t i = 0; i < mimeDescs.size() && !mimeDescs[i].isEmpty(); i++) {
		Vector<String> mimeData;
		mimeDescs[i].split(":", true, mimeData);
		if (mimeData.size() < 3) {
            continue;
        }

		String description = mimeData[2];
		Vector<String> extensions;
		mimeData[1].split(",", true, extensions);

		//determineQuirks(mimeData[0]);

		D(bug("PluginPackage: Adding mimetype: %s|%s|%s\n", mimeData[0].latin1().data(), extensions[0].latin1().data(), description.latin1().data()));

		m_mimeToExtensions.add(mimeData[0], extensions);
        m_mimeToDescriptions.add(mimeData[0], description);
    }

    return true;
}

bool PluginPackage::load()
{
	char cmodule[512];

    if (m_isLoaded) {
        m_loadCount++;
        return true;
    }

	stccpy(cmodule, m_path.latin1().data(), sizeof(cmodule));

	D(bug("PluginPackage: OpenLibrary(%s)\n", cmodule));

	m_module = (struct Library *) OpenLibrary(cmodule, MIN_PLUGIN_VERSION);

	if(m_module)
	{
		NPError npErr;

	    memset(&m_pluginFuncs, 0, sizeof(m_pluginFuncs));
	    m_pluginFuncs.size = sizeof(m_pluginFuncs);

	    initializeBrowserFuncs();

		npErr = NP_GetEntryPoints(&m_pluginFuncs);

		if(npErr == NPERR_NO_ERROR)
		{
			npErr = NP_Initialize(&m_browserFuncs);

			if(npErr == NPERR_NO_ERROR)
			{
				m_isLoaded = true;
				m_loadCount++;
				return true;
			}
		}
	}
	else
	{
		D(bug("PluginPackageLoading: Opening %s failed\n", cmodule));
		return false;
	}

    unloadWithoutShutdown();
    return false;
}

void PluginPackage::unload()
{
    if (!m_isLoaded)
        return;

    if (--m_loadCount > 0)
        return;

	NP_ShutDown();
	
	unloadWithoutShutdown();
}

uint16_t PluginPackage::NPVersion() const
{
    return NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL;
}


}
