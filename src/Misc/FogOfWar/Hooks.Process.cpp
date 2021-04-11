#include "FogOfWar.h"

// ; process cell
// ;//4ACBC2 = MapClass_UpdateFogOnMap, 7
// ;//4A9D74 = MapClass_RevealFogShroud_RegisterCell, A
// 4ACE3C = MapClass_TryReshroudCell_SetCopyFlag, 6
// 4A9CA0 = MapClass_RevealFogShroud, 7
// 486BF0 = CleanFog, 9
// 486A70 = CellClass_FogCell, 5
// ;//457AA0 = BuildingClass_FreezeInFog, 5
// 440B8D = BuildingClass_Put_CheckFog, 6
// 486C50 = CellClass_ClearFoggedObjects, 6
// 
// 70076E = TechnoClass_GetCursorOverCell_OverFog, 5
// 6D3470 = TacticalClass_DrawFoggedObject, 8
// 51F97C = InfantryClass_MouseOverCell_OverFog, 5

DEFINE_HOOK(4ACE3C, MapClass_TryReshroudCell_SetCopyFlag, 6)
{
	GET(CellClass*, pCell, EAX);

	auto oldfield = pCell->CopyFlags;
	pCell->CopyFlags = oldfield & 0xFFFFFFEF;
	auto nIndex = TacticalClass::Instance->GetOcclusion(pCell->MapCoords, false);
	if (((oldfield & 0x10) != 0 || pCell->Visibility != nIndex) && nIndex >= 0 && pCell->Visibility >= -1)
	{
		pCell->CopyFlags |= 8u;
		pCell->Visibility = nIndex;
	}
	TacticalClass::Instance->RegisterCellAsVisible(pCell);

	return 0x4ACE57;
}

DEFINE_HOOK(4A9CA0, MapClass_RevealFogShroud, 7)
{
	// GET(MapClass*, pMap, ECX);
	auto const pMap = MapClass::Global();
	GET_STACK(CellStruct*, pCell, 0x4);
	GET_STACK(HouseClass*, dwUnk, 0x8);
	// GET_STACK(bool, bUnk, 0xC);

	R->EAX(FogOfWar::MapClass_RevealFogShroud(pMap, pCell, dwUnk));

	return 0x4A9DC6;
}

DEFINE_HOOK(486BF0, CellClass_CleanFog, 9)
{
	GET(CellClass*, pCell_, ECX);

	auto pLocation = pCell_->MapCoords;
	for (int i = 1; i < 15; i += 2)
	{
		auto pCell = MapClass::Global()->TryGetCellAt(pLocation);
		if (pCell->Level >= i - 2 && pCell->Level <= i)
		{
			pCell->Flags &= 0xFFBFFFFF;
			pCell->ClearFoggedObjects();
			++pLocation.X;
			++pLocation.Y;
		}
	}

	return 0x486C4C;
}

// NOT IMPLEMENTED YET!
DEFINE_HOOK(486A70, CellClass_FogCell, 5)
{
	GET(CellClass*, pCell, ECX);

	// rewritten me pls!

	UNREFERENCED_PARAMETER(pCell);

	return 0;
	// return 0x486BE6;
}

DEFINE_HOOK(440B8D, BuildingClass_Put_CheckFog, 6)
{
	GET(BuildingClass*, pBuilding, ESI);

	if (pBuilding->Is_Fogged())
	{
		auto pCell = pBuilding->GetCell();

		// Several extra works to be done here
		UNREFERENCED_PARAMETER(pCell);
	}

	return 0x440C08;
}

// TO BE IMPLEMENTED!
DEFINE_HOOK(486C50, CellClass_ClearFoggedObjects, 6)
{
	GET(CellClass*, pCell, ECX);

	UNREFERENCED_PARAMETER(pCell);

	return 0;
	// return 0x486D8A;
}

// TO BE IMPLEMENTED!
DEFINE_HOOK(70076E, TechnoClass_GetCursorOverCell_OverFog, 5)
{

	return 0;
	// return 0x700800;
}

// TO BE IMPLEMENTED!
DEFINE_HOOK(6D3470, TacticalClass_DrawFoggedObject, 8)
{
	// GET(TacticalClass*, pTactical, ECX);
	auto const pTactical = TacticalClass::Global();
	GET_STACK(RectangleStruct*, pRect1, 0x4);
	GET_STACK(RectangleStruct*, pRect2, 0x8);
	GET_STACK(bool, bUkn, 0xC);

	UNREFERENCED_PARAMETER(pTactical);
	UNREFERENCED_PARAMETER(pRect1);
	UNREFERENCED_PARAMETER(pRect2);
	UNREFERENCED_PARAMETER(bUkn);

	// Draw them

	return 0;
	// return 0x6D3650;
}

// TO BE IMPLEMENTED!
DEFINE_HOOK(51F97C, InfantryClass_MouseOverCell_OverFog, 5)
{
	GET(CellClass*, pCell, EAX);

	UNREFERENCED_PARAMETER(pCell);

	return 0;
}