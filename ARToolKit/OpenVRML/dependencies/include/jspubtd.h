/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU Public License (the "GPL"), in which case the
 * provisions of the GPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only
 * under the terms of the GPL and not to allow others to use your
 * version of this file under the NPL, indicate your decision by
 * deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL.  If you do not delete
 * the provisions above, a recipient may use your version of this
 * file under either the NPL or the GPL.
 */

#ifndef jspubtd_h___
#define jspubtd_h___
/*
 * JS public API typedefs.
 */
#include "jstypes.h"
#include "jscompat.h"

JS_BEGIN_EXTERN_C

/* Scalar typedefs. */
typedef uint16    jschar;
typedef int32     jsint;
typedef uint32    jsuint;
typedef float64   jsdouble;
typedef jsword    jsval;
typedef jsword    jsid;
typedef int32     jsrefcount;   /* PRInt32 if JS_THREADSAFE, see jslock.h */

typedef enum JSVersion {
    JSVERSION_1_0     = 100,
    JSVERSION_1_1     = 110,
    JSVERSION_1_2     = 120,
    JSVERSION_1_3     = 130,
    JSVERSION_1_4     = 140,
    JSVERSION_1_5     = 150,
    JSVERSION_DEFAULT = 0,
    JSVERSION_UNKNOWN = -1
} JSVersion;

#define JSVERSION_IS_ECMA(version) \
    ((version) == JSVERSION_DEFAULT || (version) >= JSVERSION_1_3)

/* Result of typeof operator enumeration. */
typedef enum JSType {
    JSTYPE_VOID,                /* undefined */
    JSTYPE_OBJECT,              /* object */
    JSTYPE_FUNCTION,            /* function */
    JSTYPE_STRING,              /* string */
    JSTYPE_NUMBER,              /* number */
    JSTYPE_BOOLEAN,             /* boolean */
    JSTYPE_LIMIT
} JSType;

/* JSObjectOps.checkAccess mode enumeration. */
typedef enum JSAccessMode {
    JSACC_PROTO,                /* XXXbe redundant w.r.t. id */
    JSACC_PARENT,               /* XXXbe redundant w.r.t. id */
    JSACC_IMPORT,               /* import foo.bar */
    JSACC_WATCH,                /* a watchpoint on object foo for id 'bar' */
    JSACC_READ,                 /* a "get" of foo.bar */
    JSACC_WRITE,                /* a "set" of foo.bar = baz */
    JSACC_LIMIT
} JSAccessMode;

/*
 * This enum type is used to control the behavior of a JSObject property
 * iterator function that has type JSNewEnumerate.
 */
typedef enum JSIterateOp {
    JSENUMERATE_INIT,       /* Create new iterator state */
    JSENUMERATE_NEXT,       /* Iterate once */
    JSENUMERATE_DESTROY     /* Destroy iterator state */
} JSIterateOp;

/* Struct typedefs. */
typedef struct JSClass           JSClass;
typedef struct JSConstDoubleSpec JSConstDoubleSpec;
typedef struct JSContext         JSContext;
typedef struct JSErrorReport     JSErrorReport;
typedef struct JSFunction        JSFunction;
typedef struct JSFunctionSpec    JSFunctionSpec;
typedef struct JSIdArray         JSIdArray;
typedef struct JSProperty        JSProperty;
typedef struct JSPropertySpec    JSPropertySpec;
typedef struct JSObject          JSObject;
typedef struct JSObjectMap       JSObjectMap;
typedef struct JSObjectOps       JSObjectOps;
typedef struct JSRuntime         JSRuntime;
typedef struct JSRuntime         JSTaskState;	/* XXX deprecated name */
typedef struct JSScript          JSScript;
typedef struct JSString          JSString;
typedef struct JSXDRState	 JSXDRState;
typedef struct JSExceptionState  JSExceptionState;
typedef struct JSLocaleCallbacks JSLocaleCallbacks;

/* JSClass (and JSObjectOps where appropriate) function pointer typedefs. */

/*
 * Add, delete, get or set a property named by id in obj.  Note the jsval id
 * type -- id may be a string (Unicode property identifier) or an int (element
 * index).  The *vp out parameter, on success, is the new property value after
 * an add, get, or set.  After a successful delete, *vp is JSVAL_FALSE iff
 * obj[id] can't be deleted (because it's permanent).
 */
typedef JSBool
(* JS_DLL_CALLBACK JSPropertyOp)(JSContext *cx, JSObject *obj, jsval id,
                                 jsval *vp);

/*
 * This function type is used for callbacks that enumerate the properties of
 * a JSObject.  The behavior depends on the value of enum_op:
 *
 *  JSENUMERATE_INIT
 *    A new, opaque iterator state should be allocated and stored in *statep.
 *    (You can use PRIVATE_TO_JSVAL() to tag the pointer to be stored).
 *
 *    The number of properties that will be enumerated should be returned as
 *    an integer jsval in *idp, if idp is non-null, and provided the number of
 *    enumerable properties is known.  If idp is non-null and the number of
 *    enumerable properties can't be computed in advance, *idp should be set
 *    to JSVAL_ZERO.
 *
 *  JSENUMERATE_NEXT
 *    A previously allocated opaque iterator state is passed in via statep.
 *    Return the next jsid in the iteration using *idp.  The opaque iterator
 *    state pointed at by statep is destroyed and *statep is set to JSVAL_NULL
 *    if there are no properties left to enumerate.
 *
 *  JSENUMERATE_DESTROY
 *    Destroy the opaque iterator state previously allocated in *statep by a
 *    call to this function when enum_op was JSENUMERATE_INIT.
 *
 * The return value is used to indicate success, with a value of JS_FALSE
 * indicating failure.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSNewEnumerateOp)(JSContext *cx, JSObject *obj,
                                     JSIterateOp enum_op,
                                     jsval *statep, jsid *idp);

/*
 * The old-style JSClass.enumerate op should define all lazy properties not
 * yet reflected in obj.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSEnumerateOp)(JSContext *cx, JSObject *obj);

/*
 * Resolve a lazy property named by id in obj by defining it directly in obj.
 * Lazy properties are those reflected from some peer native property space
 * (e.g., the DOM attributes for a given node reflected as obj) on demand.
 *
 * JS looks for a property in an object, and if not found, tries to resolve
 * the given id.  If resolve succeeds, the engine looks again in case resolve
 * defined obj[id].  If no such property exists directly in obj, the process
 * is repeated with obj's prototype, etc.
 *
 * NB: JSNewResolveOp provides a cheaper way to resolve lazy properties.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSResolveOp)(JSContext *cx, JSObject *obj, jsval id);

/*
 * Like JSResolveOp, but flags provide contextual information as follows:
 *
 *  JSRESOLVE_QUALIFIED   a qualified property id: obj.id or obj[id], not id
 *  JSRESOLVE_ASSIGNING   obj[id] is on the left-hand side of an assignment
 *
 * The *objp out parameter, on success, should be null to indicate that id
 * was not resolved; and non-null, referring to obj or one of its prototypes,
 * if id was resolved.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSNewResolveOp)(JSContext *cx, JSObject *obj, jsval id,
                                   uintN flags, JSObject **objp);

/*
 * Convert obj to the given type, returning true with the resulting value in
 * *vp on success, and returning false on error or exception.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSConvertOp)(JSContext *cx, JSObject *obj, JSType type,
                                jsval *vp);

/*
 * Finalize obj, which the garbage collector has determined to be unreachable
 * from other live objects or from GC roots.  Obviously, finalizers must never
 * store a reference to obj.
 */
typedef void
(* JS_DLL_CALLBACK JSFinalizeOp)(JSContext *cx, JSObject *obj);

/*
 * Used by JS_AddExternalStringFinalizer and JS_RemoveExternalStringFinalizer
 * to extend and reduce the set of string types finalized by the GC.
 */
typedef void
(* JS_DLL_CALLBACK JSStringFinalizeOp)(JSContext *cx, JSString *str);

/*
 * The signature for JSClass.getObjectOps, used by JS_NewObject's internals
 * to discover the set of high-level object operations to use for new objects
 * of the given class.  All native objects have a JSClass, which is stored as
 * a private (int-tagged) pointer in obj->slots[JSSLOT_CLASS].  In contrast,
 * all native and host objects have a JSObjectMap at obj->map, which may be
 * shared among a number of objects, and which contains the JSObjectOps *ops
 * pointer used to dispatch object operations from API calls.
 *
 * Thus JSClass (which pre-dates JSObjectOps in the API) provides a low-level
 * interface to class-specific code and data, while JSObjectOps allows for a
 * higher level of operation, which does not use the object's class except to
 * find the class's JSObjectOps struct, by calling clasp->getObjectOps.
 *
 * If this seems backwards, that's because it is!  API compatibility requires
 * a JSClass *clasp parameter to JS_NewObject, etc.  Most host objects do not
 * need to implement the larger JSObjectOps, and can share the common JSScope
 * code and data used by the native (js_ObjectOps, see jsobj.c) ops.
 */
typedef JSObjectOps *
(* JS_DLL_CALLBACK JSGetObjectOps)(JSContext *cx, JSClass *clasp);

/*
 * JSClass.checkAccess type: check whether obj[id] may be accessed per mode,
 * returning false on error/exception, true on success with obj[id]'s last-got
 * value in *vp, and its attributes in *attrsp.  As for JSPropertyOp above, id
 * is either a string or an int jsval.
 *
 * See JSCheckAccessIdOp, below, for the JSObjectOps counterpart, which takes
 * a jsid (a tagged int or aligned, unique identifier pointer) rather than a
 * jsval.  The native js_ObjectOps.checkAccess simply forwards to the object's
 * clasp->checkAccess, so that both JSClass and JSObjectOps implementors may
 * specialize access checks.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSCheckAccessOp)(JSContext *cx, JSObject *obj, jsval id,
                                    JSAccessMode mode, jsval *vp);

/*
 * Encode or decode an object, given an XDR state record representing external
 * data.  See jsxdrapi.h.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSXDRObjectOp)(JSXDRState *xdr, JSObject **objp);

/*
 * Check whether v is an instance of obj.  Return false on error or exception,
 * true on success with JS_TRUE in *bp if v is an instance of obj, JS_FALSE in
 * *bp otherwise.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSHasInstanceOp)(JSContext *cx, JSObject *obj, jsval v,
                                    JSBool *bp);

/*
 * Function type for JSClass.mark and JSObjectOps.mark, called from the GC to
 * scan live GC-things reachable from obj's private data structure.  For each
 * such thing, a mark implementation must call
 *
 *    JS_MarkGCThing(cx, thing, name, arg);
 *
 * The trailing name and arg parameters are used for GC_MARK_DEBUG-mode heap
 * dumping and ref-path tracing.  The mark function should pass a (typically
 * literal) string naming the private data member for name, and it must pass
 * the opaque arg parameter through from its caller.
 *
 * For the JSObjectOps.mark hook, the return value is the number of slots at
 * obj->slots to scan.  For JSClass.mark, the return value is ignored.
 *
 * NB: JSMarkOp implementations cannot allocate new GC-things (JS_NewObject
 * called from a mark function will fail silently, e.g.).
 */
typedef uint32
(* JS_DLL_CALLBACK JSMarkOp)(JSContext *cx, JSObject *obj, void *arg);

/* JSObjectOps function pointer typedefs. */

/*
 * Create a new subclass of JSObjectMap (see jsobj.h), with the nrefs and ops
 * members initialized from the same-named parameters, and with the nslots and
 * freeslot members initialized according to ops and clasp.  Return null on
 * error, non-null on success.
 *
 * JSObjectMaps are reference-counted by generic code in the engine.  Usually,
 * the nrefs parameter to JSObjectOps.newObjectMap will be 1, to count the ref
 * returned to the caller on success.  After a successful construction, some
 * number of js_HoldObjectMap and js_DropObjectMap calls ensue.  When nrefs
 * reaches 0 due to a js_DropObjectMap call, JSObjectOps.destroyObjectMap will
 * be called to dispose of the map.
 */
typedef JSObjectMap *
(* JS_DLL_CALLBACK JSNewObjectMapOp)(JSContext *cx, jsrefcount nrefs,
                                     JSObjectOps *ops, JSClass *clasp,
                                     JSObject *obj);

/*
 * Generic type for an infallible JSObjectMap operation, used currently by
 * JSObjectOps.destroyObjectMap.
 */
typedef void
(* JS_DLL_CALLBACK JSObjectMapOp)(JSContext *cx, JSObjectMap *map);

/*
 * Look for id in obj and its prototype chain, returning false on error or
 * exception, true on success.  On success, return null in *propp if id was
 * not found.  If id was found, return the first object searching from obj
 * along its prototype chain in which id names a direct property in *objp, and
 * return a non-null, opaque property pointer in *propp.
 *
 * If JSLookupPropOp succeeds and returns with *propp non-null, that pointer
 * may be passed as the prop parameter to a JSAttributesOp, as a short-cut
 * that bypasses id re-lookup.  In any case, a non-null *propp result after a
 * successful lookup must be dropped via JSObjectOps.dropProperty.
 *
 * NB: successful return with non-null *propp means the implementation may
 * have locked *objp and added a reference count associated with *propp, so
 * callers should not risk deadlock by nesting or interleaving other lookups
 * or any obj-bearing ops before dropping *propp.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSLookupPropOp)(JSContext *cx, JSObject *obj, jsid id,
                                   JSObject **objp, JSProperty **propp
#if defined JS_THREADSAFE && defined DEBUG
                                 , const char *file, uintN line
#endif
                                  );

/*
 * Define obj[id], a direct property of obj named id, having the given initial
 * value, with the specified getter, setter, and attributes.  If the propp out
 * param is non-null, *propp on successful return contains an opaque property
 * pointer usable as a speedup hint with JSAttributesOp.  But note that propp
 * may be null, indicating that the caller is not interested in recovering an
 * opaque pointer to the newly-defined property.
 *
 * If propp is non-null and JSDefinePropOp succeeds, its caller must be sure
 * to drop *propp using JSObjectOps.dropProperty in short order, just as with
 * JSLookupPropOp.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSDefinePropOp)(JSContext *cx, JSObject *obj,
                                   jsid id, jsval value,
                                   JSPropertyOp getter, JSPropertyOp setter,
                                   uintN attrs, JSProperty **propp);

/*
 * Get, set, or delete obj[id], returning false on error or exception, true
 * on success.  If getting or setting, the new value is returned in *vp on
 * success.  If deleting without error, *vp will be JSVAL_FALSE if obj[id] is
 * permanent, and JSVAL_TRUE if id named a direct property of obj that was in
 * fact deleted, or if id names no direct property of obj (id could name a
 * prototype property, or no property in obj or its prototype chain).
 */
typedef JSBool
(* JS_DLL_CALLBACK JSPropertyIdOp)(JSContext *cx, JSObject *obj, jsid id,
                                   jsval *vp);

/*
 * Get or set attributes of the property obj[id].  Return false on error or
 * exception, true with current attributes in *attrsp.  If prop is non-null,
 * it must come from the *propp out parameter of a prior JSDefinePropOp or
 * JSLookupPropOp call.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSAttributesOp)(JSContext *cx, JSObject *obj, jsid id,
                                   JSProperty *prop, uintN *attrsp);

/*
 * JSObjectOps.checkAccess type: check whether obj[id] may be accessed per
 * mode, returning false on error/exception, true on success with obj[id]'s
 * last-got value in *vp, and its attributes in *attrsp.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSCheckAccessIdOp)(JSContext *cx, JSObject *obj, jsid id,
                                      JSAccessMode mode, jsval *vp,
                                      uintN *attrsp);

/*
 * A generic type for functions mapping an object to another object, or null
 * if an error or exception was thrown on cx.  Used by JSObjectOps.thisObject
 * at present.
 */
typedef JSObject *
(* JS_DLL_CALLBACK JSObjectOp)(JSContext *cx, JSObject *obj);

/*
 * A generic type for functions taking a context, object, and property, with
 * no return value.  Used by JSObjectOps.dropProperty currently (see above,
 * JSDefinePropOp and JSLookupPropOp, for the object-locking protocol in which
 * dropProperty participates).
 */
typedef void
(* JS_DLL_CALLBACK JSPropertyRefOp)(JSContext *cx, JSObject *obj,
                                    JSProperty *prop);

/*
 * Function type for JSObjectOps.setProto and JSObjectOps.setParent.  These
 * hooks must check for cycles without deadlocking, and otherwise take special
 * steps.  See jsobj.c, js_SetProtoOrParent, for an example.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSSetObjectSlotOp)(JSContext *cx, JSObject *obj,
                                      uint32 slot, JSObject *pobj);

/*
 * Get and set a required slot, one that should already have been allocated.
 * These operations are infallible, so required slots must be pre-allocated,
 * or implementations must suppress out-of-memory errors.  The native ops
 * (js_ObjectOps, see jsobj.c) access slots reserved by including a call to
 * the JSCLASS_HAS_RESERVED_SLOTS(n) macro in the JSClass.flags initializer.
 *
 * NB: the slot parameter is a zero-based index into obj->slots[], unlike the
 * index parameter to the JS_GetReservedSlot and JS_SetReservedSlot API entry
 * points, which is a zero-based index into the JSCLASS_RESERVED_SLOTS(clasp)
 * reserved slots that come after the initial well-known slots: proto, parent,
 * class, and optionally, the private data slot.
 */
typedef jsval
(* JS_DLL_CALLBACK JSGetRequiredSlotOp)(JSContext *cx, JSObject *obj,
                                        uint32 slot);

typedef void
(* JS_DLL_CALLBACK JSSetRequiredSlotOp)(JSContext *cx, JSObject *obj,
                                        uint32 slot, jsval v);

/* Typedef for native functions called by the JS VM. */

typedef JSBool
(* JS_DLL_CALLBACK JSNative)(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);

/* Callbacks and their arguments. */

typedef enum JSGCStatus {
    JSGC_BEGIN,
    JSGC_END,
    JSGC_MARK_END,
    JSGC_FINALIZE_END
} JSGCStatus;

typedef JSBool
(* JS_DLL_CALLBACK JSGCCallback)(JSContext *cx, JSGCStatus status);

typedef JSBool
(* JS_DLL_CALLBACK JSBranchCallback)(JSContext *cx, JSScript *script);

typedef void
(* JS_DLL_CALLBACK JSErrorReporter)(JSContext *cx, const char *message,
                                    JSErrorReport *report);

typedef struct JSErrorFormatString {
    const char *format;
    uintN argCount;
} JSErrorFormatString;

typedef const JSErrorFormatString *
(* JS_DLL_CALLBACK JSErrorCallback)(void *userRef, const char *locale,
			            const uintN errorNumber);

#ifdef va_start
#define JS_ARGUMENT_FORMATTER_DEFINED 1

typedef JSBool
(* JS_DLL_CALLBACK JSArgumentFormatter)(JSContext *cx, const char *format,
                                        JSBool fromJS, jsval **vpp,
                                        va_list *app);
#endif

typedef JSBool 
(* JS_DLL_CALLBACK JSLocaleToUpperCase)(JSContext *cx, JSString *src,
                                        jsval *rval);

typedef JSBool
(* JS_DLL_CALLBACK JSLocaleToLowerCase)(JSContext *cx, JSString *src,
                                        jsval *rval);

typedef JSBool
(* JS_DLL_CALLBACK JSLocaleCompare)(JSContext *cx,
                                    JSString *src1, JSString *src2,
                                    jsval *rval);

/*
 * Security protocol types.
 */
typedef struct JSPrincipals JSPrincipals;

/*
 * XDR-encode or -decode a principals instance, based on whether xdr->mode is
 * JSXDR_ENCODE, in which case *principalsp should be encoded; or JSXDR_DECODE,
 * in which case implementations must return a held (via JSPRINCIPALS_HOLD),
 * non-null *principalsp out parameter.  Return true on success, false on any
 * error, which the implementation must have reported.
 */
typedef JSBool
(* JS_DLL_CALLBACK JSPrincipalsTranscoder)(JSXDRState *xdr,
                                           JSPrincipals **principalsp);

JS_END_EXTERN_C

#endif /* jspubtd_h___ */