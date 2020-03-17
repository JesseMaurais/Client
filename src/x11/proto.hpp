#ifndef xproto_hpp
#define xproto_hpp

#include "x11.hpp"

namespace x11
{

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

	using PolyPoint = Request
	<
		X_PolyPoint, xPolyPointReq, sz_xPolyPointReq, 
	>;

	using PolyLine = Request
	<
		X_PolyLine, xPolyLineReq, sz_xPolyLineReq
	>;

	using PolySegment = Request
	<
		X_PolySegment, xPolySegmentReq, sz_xPolySegmentReq
	>;

	using PolyRectangle = Request
	<
		X_PolyRectangle, xPolyRectangleReq, sz_xPolyRectangleReq
	>;

	using PolyArc = Request
	<
		X_PolyArc, xPolyArcReq, sz_xPolyArcReq
	>;

	using PolyFill = Request
	<
		X_FillPoly, xFillPolyReq, sz_xFillPolyReq
	>;

	using PolyFillRectangle = Request
	<
		X_PolyFillRectangle, xPolyFillRectangleReq, sz_xPolyFillRectangleReq
	>;

	using PolyFillArc = Request
	<
		X_PolyFillArc, xPolyFillArcReq, sz_xPolyFillArcReq
	>;

	using PutImage = Request
	<
		X_PutImage, xPutImageReq, sz_xPutImageReq
	>;

	using GetImage = Reply
	<
		X_GetImage, xGetImageReply, sz_xGetImageReply
	>;

	using PolyText8 = Request
	<
		X_PolyText8, xPolyText8Req, sz_xPolyText8Req
	>;

	using PolyText16 = Request
	<
		X_PolyText16, xPolyText16Req, sz_xPolyText16Req
	>;

	using ImageText8 = Request
	<
		X_ImageText8, xImageText8Req, sz_xImageText8Req
	>;

	using ImageText16 = Request
	<
		X_ImageText16, xImageText16Req, sz_xImageText16Req
	>;

	using CreateColormap = Request
	<
		X_CreateColormap, xCreateColormapReq, sz_xCreateColormapReq
	>;

	using FreeColormap = Request
	<
		X_FreeColormap
	>;

	using CopyColormapAndFree = Request
	<
		X_CopyColormapAndFree, xCopyColormapAndFreeReq, sz_xCopyColormapAndFreeReq
	>;

	using InstallColormap = Request
	<
		X_InstallColormap, xInstallColormapReq, sz_xInstallColormapReq
	>;

	using UninstallColormap = Request
	<
		X_UninstallColormap, xUninstallColormapReq, sz_xUninstallColormapReq
	>;

	using ListInstalledColormap = Request
	<
		X_ListInstalledColormap, xListInstalledColormapReq, sz_xListInstalledColormapReq
	>;

	using AllocColor = Reply
	<
		X_AllocColor, xAllocColorReply, sz_xAllocColorReply
	>;

	using AllocNamedColor = Reply
	<
		X_AllocNamedColor, xAllocNamedColorReply, sz_xAllocNamedColorReply
	>;

	using AllocColorCells = Reply
	<
		X_AllocColorCells, xAllocColorCellsReply, sz_xAllocColorCellsReply
	>;

	using AllocColorPlanes = Reply
	<
		X_AllocColorPlanes, xAllocColorPlanesReply, sz_xAllocColorPlanesReply
	>;

	using FreeColors = Request
	<
		X_FreeColors, xFreeColorsReq, sz_xFreeColorsReq
	>;

	using StoreColors = Request
	<
		X_StoreColors, xStoreColorsReq, sz_xStoreColorsReq
	>;

	using StoreNamedColors = Request
	<
		X_StoreNamedColors, xStoreNamedColorsReq, sz_xStoreNamedColorsReq
	>;

	using QueryColors = Request
	<
		X_QueryColors, xQueryColorsReq, sz_xQueryColorsReq, xQueryColorsReply, sz_xQueryColorsReply
	>;

	using LookupColor = Request
	<
		X_LookupColor, xLookupColorsReq, sz_xLookupColorsReq, xLookupColorReply, sz_xLookupColorReply
	>;

	using CreateCursor = Request
	<
		X_CreateCursor, xCreateCursorReq, sz_xCreateCursorReq
	>;

	using CreateCursor = Request
	<
		X_CreateGlyphCursor, xCreateGlyphCursorReq, sz_xCreateGlyphCursorReq
	>;

	using FreeCursor = Request
	<
		X_FreeCursor
	>;

	using RecolorCursor = Request
	<
		X_RecolorCursor, xRecolorCursorReq, sz_xRecolorCursorReq
	>;

	using QueryBestSize = Request
	<
		X_QueryBestSize, xQueryBestSizeReq, sz_xQueryBestSizeReq, xQueryBestSizeReply, sz_xQueryBestSizeReply
	>;

	using QueryExtension = Request
	<
		X_QueryExtension, xQueryExtensionReq, sz_xQueryExtensionReq, xQueryExtensionReply, sz_xQueryExtensionReply
	>;

	using ListExtensions = Reply
	<
		X_ListExtensions, xListExtensionsReply, sz_xListExtensionsReply
	>;

	using ChangeKeyboardMapping = Request
	<
		X_ChangeKeyboardMapping, xChangeKeyboardMappingReq, sz_xChangeKeyboardMappingReq
	>;

	using GetKeyboardMapping = Request
	<
		X_GetKeyboardMapping, xGetKeyboardMappingReq, sz_xGetKeyboardMappingReq, xGetKeyboardMappingReply, sz_GetKeyboardMappingReply
	>;

	using ChangeKeyboardControl = Request
	<
		X_ChangeKeyboardControl, xChangeKeyboardControlReq, sz_xChangeKeyboardControlReq
	>;

	using GetKeyboardControl = Reply
	<
		X_GetKeyboardControl, xGetKeyboardControlReply, sz_GetKeyboardControlReply
	>;

	using Bell = Request
	<
		X_Bell, xBellReq, sz_xBellReq
	>;

	using ChangePointerControl = Request
	<
		X_ChangePointerControl, xChangePointerControlReq, sz_xChangePointerControlReq
	>;

	using GetPointerControl = Reply
	<
		X_GetPointerControl, xGetPointerControlReply, sz_GetPoniterControlReply
	>;

	using SetScreenSaver = Request
	<
		X_SetScreenSaver, xSetScreenSaverReq, sz_xSetScreenSaverReq
	>;

	using GetScreenSaver = Reply
	<
		X_GetScreenSaver, xGetScreenSaverReply, sz_xGetScreenSaverReply
	>;

	using ChangeHosts = Request
	<
		X_ChangeHosts, xChangeHostsReq, sz_xChangeHostsReq
	>;

	using ListHosts = Reply
	<
		X_ListHosts, xListHostsReply, sz_xListHostsReply
	>;

	using SetAccessControl = Request
	<
		X_SetAccessControl, xSetAccessControlRep, sz_xSetAccessControlRep
	>;

	using SetCloseDownMode = Request
	<
		X_SetCloseDownMode, xSetCloseDownModeRep, sz_xSetCloseDownModeRep
	>;

	using KillClient = Request
	<
		X_KillClient
	>;

	using RotateProperties = Request
	<
		X_RotateProperties, xRotatePropertiesReq, sz_xRotatePropertiesReq
	>;

	using ForceScreenSaver = Request
	<
		X_ForceScreenSaver, xForceScreenSaverReq, sz_xForceScreenSaverReq
	>;

	using SetPointerMapping = Request
	<
		X_SetPointerMapping, xSetPointerMappingReq, sz_xSetPointerMappingReq, xSetPointerMappingReply, sz_xSetPointerMappingReply
	>;

	using GetPointerMapping = Reply
	<
		X_GetPointerMapping, xGetPointerMappingReply, sz_xGetPointerMappingReply
	>;

	using SetModifierMapping = Request
	<
		X_SetModifierMapping, xSetModifierMappingReq, sz_xSetModifierMappingReq, xSetModifierMappingReply, sz_xSetModifierMappingReply
	>;

	using GetModifierMapping = Reply
	<
		X_GetModifierMapping, xGetModifierMappingReply, sz_xGetModifierMappingReply
	>;

	using NoOperation = Request
	<
		X_NoOperation
	>;
}

#endif // file
