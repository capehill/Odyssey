/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2008 Google Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef AccessibilityObjectWrapper_h
#define AccessibilityObjectWrapper_h

/* Debug output to serial handled via D(bug("....."));
*  See Base/debug.h for details.
*  D(x)    - to disable debug
*  D(x) x  - to enable debug
*/
#define D(x)

namespace WebCore {

#if HAVE(ACCESSIBILITY)
    class AccessibilityObject;
    class AccessibilityObjectWrapper : public RefCounted<AccessibilityObjectWrapper> {
    public:
        virtual ~AccessibilityObjectWrapper() {}
        virtual void detach() = 0;
        bool attached() const { return m_object; }
        AccessibilityObject* accessibilityObject() const { return m_object; }

    protected:
        AccessibilityObjectWrapper(AccessibilityObject* obj)
            : m_object(obj)
        {
            // FIXME: Remove this once our immediate subclass no longer uses COM.
            
	    D(bug("Disabled addressOfCount()=0 in AccessibilityObjectWrapper... is it even used?"));
	    //*addressOfCount() = 0;
        }
        AccessibilityObjectWrapper() : m_object(0) { }

        AccessibilityObject* m_object;
    };
#endif
} // namespace WebCore

#endif
