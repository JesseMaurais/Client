#ifndef x11_hpp
#define x11_hp;

#if __has_include <X11/Xproto.h>
#include <X11/Xproto.h>
#else
#error Require the X11 protocol header.
#endif

namespace x11
{
	template 
	<
		CARD8 ReqType, 
		class Req = xReq, 
		CARD16 ReqSize = sz_xReq, 
		class Reply = xGenericReply
		CARD16 ReplySize = sz_xReply
	>
	struct Request : Req
	{
		constexpr auto requestType = ReqType;
		using request = typename Req;
		constexpr auto requestSize = ReqSize;
		using reply = typename Reply;
		constexpr auto replySize = ReplySize;

		Request() : reqType(requestType), length(requestSize) = default;

		static_assert(sizeof(Req) == requestSize);
		static_assert(sizeof(Reply) == replySize);
	};

	template
	<
		CARD8 ReqType,
		class Reply = xGenericReply,
		CARD16 ReplySize = sz_xReply
	>
	using Reply = Request<ReqType, xReq, sz_xReq, Reply, ReplySize>;

	using CreateWindow = Request
	<
		X_CreateWindow, xCreateWidowReq, sz_xCreateWindow
	>;

	using ChangeWindowAttributes = Request
	<
		X_ChangeWindowAttributes, xChangeWindowAttributesReq, sz_xChangeWindowAttributes
	>;

	using GetWindowAttributes = Request
	<
		X_GetWindowAttributes, xGetWindowAttributesReq, sz_xGetWindowAttributes, xGetWindowAttributesReply, sz_xGetWindowAttributesReply
	>;

	using DestroyWindow = Request
	<
		X_DestroyWindow
	>;

	using DestroySubwindows = Request
	<
		X_DestroySubwindows
	>;

	using ChangeSaveSet = Request
	<
		X_ChangeSaveSet
	>;

	using ReparentWindow = Request
	<
		X_ReparentWindow, xReparentWindowReq, sz_xReparentWindowReq
	>;

	using MapWindow = Request
	<
		X_MapWindow
	>;

	using MapSubwindows = Request
	<
		X_MapSubwindows
	>;

	using UnmapWindow = Request
	<
		X_UnmmapWindow
	>;

	using UnmapSubwindows = Request
	<
		X_UnmapSubwindows
	>;

	using ConfigureWindow = Request
	<
		X_ConfigureWindow, xConfigureWindowReq, sz_xConfigureWindowReq
	>;

	using CirculateWindow = Request
	<
		X_CirculateWindow, xCirculateWindowReq, sz_xCirculateWindowReq
	>;

	using GetGeometry = Request
	<
		X_GetGeometry, xGetGeometryReq, sz_xGetGeometry, xGetGeometryReply, sz_GetGeometryReply
	>;

	using QueryTree = Request
	<
		X_QueryTree, xQueryTreeReq, sz_xQueryTreeReq, xQueryTreeReply, sz_QueryTreeReply
	>;

	using InternAtom = Request
	<
		X_InternAtom, xInternAtomReq, sz_xInternAtomReq, xInternAtomReply, sz_xInternAtomReply
	>;

	using GetAtomName = Reply
	<
		X_GetAtomName, xGetAtomNameReply, sz_xGetAtomNameReply
	>;

	using ChangeProperty = Request
	<
		X_ChangeProperty, xChangePropertyReq, sz_xChangePropertyReq
	>;

	using DeleteProperty = Request
	<
		X_DeleteProperty, xDeletePropertyReq, sz_xDeletePropertyReq
	>;

	using GetProperty = Request
	<
		X_GetProperty, xReq, sz_xGetPropertyReq, xGetPropertyReply, sz_xGetPropertyReply
	>;

	using ListProperties = Reply
	<
		X_ListProperties, xListPropertiesReply, sz_xListPropertiesReply
	>;

	using SetSelectionOwner = Request
	<
		X_SetSelectionOwner, xSetSelectionOwnerReq, sz_xSetSelectionOwner
	>;

	using GetSelectionOwner = Reply
	<
		X_GetSelectionOwner, xGetSelectionOwnerReply, sz_xGetSelectionOwnerReply
	>;

	using ConvertSelection = Request
	<
		X_ConvertSelection, xConvertSelectionReq, sz_xConvertSelectionReq
	>;

	using SendEvent = Request
	<
		X_SendEvent, xSendEventReq, sz_xSendEventReq
	>;

	using GrabPointer = Reply
	<
		X_GrabPointer, xGrabPointerReply, sz_xGrabPointerReply
	>;

	using UngrabPointer = Request
	<
		X_UngrabPointer
	>;

	using GrabButton = Request
	<
		X_GrabButton, xGrabButtonReq, sz_xGrabButtonReq
	>;

	using UngrabButton = Request
	<
		X_UngrabButton, xUgrabButtonReq, sz_xUngrabButtonReq
	>;

	using ChangeActivePointerGrab = Request
	<
		X_ChangeActivePointerGrab, xChangeActivePointerGrabReq, sz_xChangeActivePointerGrabReq
	>;

	using GrabKeyboard = Request
	<
		X_GrabKeyboard, xGrabKeyboardReq, sz_xGrabKeyboardReq, xGrabKeyboardReply, sz_xGrabKeyboardReply
	>;

	using UngrabKeyboard = Request
	<
		X_UngrabKeyboard
	>;

	using GrabKey = Request
	<
		X_GrabKey, xGrabKeyReq, sz_xGrabKeyReq, xGrabKeyReply, xGrabKeyReply
	>;

	using UngrabKey = Request
	<
		X_UngrabKey, xUngrabKeyReq, sz_xUngrabKeyReq
	>;

	using AllowEvents = Request
	<
		X_AllowEvents, xAllowEvents, sz_xAllowEvents
	>;

	using GrabServer = Request
	<
		X_GrabServer
	>;

	using UngrabServer = Request
	<
		X_UngrabServer
	>;

	using QueryPointer = Reply
	<
		X_QueryPointer, xQueryPointerReply, sz_xQueryPointerReply
	>;

	using GetMotionEvents = Request
	<
		X_GetMotionEvents, xGetMotionEventsReq, sz_xGetMotionEventsReq, xGetMotionEventsReply, sz_xGetMotionEventsReply
	>;

	using TranslateCoords = Request
	<
		X_TranslateCoords, xTranslateCoordsReq, sz_xTranslateCoordsReq, xTranslateCoordsReply, sz_xTranslateCoordsReply
	>;

	using WarpPointer = Request
	<
		X_WarpPointer, xWarpPointer, sz_xWarpPointer
	>;

	using SetInputFocus = Request
	<
		X_SetInputFocus, xSetInputFocus, sz_xSetInputFocus
	>;

	using GetInputFocus = Reply
	<
		X_GetInputFocus, xGetInputFocusReply, sz_xGetInputFocusReply
	>;

	using QueryKeymap = Reply
	<
		X_QueryKeymap, xQueryKeymapReply, sz_xQueryKeymapReply
	>;

	using OpenFont = Request
	<
		X_OpenFont, xOpenFontReq, sz_xOpenFontReq
	>;

	using CloseFont = Request
	<
		X_CloseFont
	>;

	using QueryFont = Reply
	<
		X_QueryFont, xQueryFontReply, sz_xQueryfontReply
	>;

	using QueryTextExtents = Request
	<
		X_QueryTextExtents, xQueryTextExtentsReq, sz_xQueryTextExtentsReq, xQueryTextExtentsReply, sz_xQueryTextExtentsReply
	>;

	using ListFonts = Request
	<
		X_ListFonts, xListFontsReq, sz_xListFontsReq, xListFontsReply, sz_xListFontsReq
	>;

	using ListFontsWithInfo = Request
	<
		X_ListFontsWithInfo, xListFontsWithInfoReq, sz_xListFontsWithInfoReq, xListFontsWithInfoReply, sz_xListFontsWithInfoReq
	>;

	using SetFontPath = Request
	<
		X_SetFontPath, xSetFontPathReq, sz_xSetFontPathReq
	>;

	using GetFontPath = Reply
	<
		X_GetFontPath, xGetFontPathReply, sz_xGetFontPathReply
	>;

	using CreatePixmap = Request
	<
		X_CreatePixmap, xCreatePixmapReq, sz_xCreatePixmapReq
	>;

	using FreePixmap = Request
	<
		X_FreePixmap
	>;

	using CreateGC = Request
	<
		X_CreateGC, xCreateGCReq, sz_xCreateGCReq
	>;

	using ChangeGC = Request
	<
		X_ChangeGC, xChangeGCReq, sz_xChangeGCReq
	>;

	using CopyGC = Request
	<
		X_CopyGC, xCopyGCReq, sz_xCopyGCReq
	>;

	using SetDashes = Request
	<
		X_SetDashes, xSetDashesReq, sz_xSetDashesReq
	>;

	using SetClipRectangles = Request
	<
		X_SetClipRectangles, xSetClipRectanglesReq, sz_xSetClipRectanglesReq
	>;

	using FreeGC = Request
	<
		X_FreeGC
	>;

	using ClearArea = Request
	<
		X_ClearArea, xCleareAreaReq, sz_xCleareAreaReq
	>;

	using CopyArea = Request
	<
		X_CopyArea, xCopyAreaReq, sz_xCopyAreaReq
	>;

	using CopyPlane = Request
	<
		X_CopyPlane, xCopyPlaneReq, sz_xCopyPlaneReq
	>;
}

#endif // file
