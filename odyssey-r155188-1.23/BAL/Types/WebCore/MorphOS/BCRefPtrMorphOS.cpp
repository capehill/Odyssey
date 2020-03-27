/*
 * Copyright (C) 2009 Martin Robinson
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "GRefPtr.h"

#include <glib.h>

/* Debug output to serial handled via D(bug("....."));
*  See Base/debug.h for details.
*  D(x)    - to disable debug
*  D(x) x  - to enable debug
*/
#define D(x)

namespace WTF {

template <> GHashTable* refPlatformPtr(GHashTable* ptr)
{
    if (ptr)
        g_hash_table_ref(ptr);
    return ptr;
}

template <> void derefPlatformPtr(GHashTable* ptr)
{
    g_hash_table_unref(ptr);
}

#if GLIB_CHECK_VERSION(2, 24, 0)
template <> GVariant* refPlatformPtr(GVariant* ptr)
{
    if (ptr)
        g_variant_ref(ptr);
    return ptr;
}

template <> void derefPlatformPtr(GVariant* ptr)
{
    g_variant_unref(ptr);
}
ll
#else
// We do this so that we can avoid including the glib.h header in GRefPtr.h.
typedef struct _GVariant {
    bool fake;
} GVariant; 

template <> GVariant* refPlatformPtr(GVariant* ptr)
{
    D(bug("refPlatformPtr for GVariant called\n"));
    return ptr;
}

template <> void derefPlatformPtr(GVariant* ptr)
{
    D(bug("derefPlatformPtr for GVariantcalled\n"));
}

#endif

} // namespace WTF
