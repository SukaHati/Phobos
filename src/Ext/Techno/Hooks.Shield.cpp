#include "Body.h"
#include <SpecificStructures.h>
#include <UnitClass.h>
#include <BuildingClass.h>

#include "../TechnoType/Body.h"

// #issue 88 : shield logic
DEFINE_HOOK(701900, TechnoClass_ReceiveDamage_Shield, 6)
{
    GET(TechnoClass*, pThis, ECX);
    LEA_STACK(args_ReceiveDamage*, args, 0x4);
    //GET_STACK(int*, pDamage, 0x4);
    //GET_STACK(WarheadTypeClass*, pWH, 0xC);
    //GET_STACK(HouseClass*, pSourceHouse, -0x1C);
    auto pExt = TechnoExt::ExtMap.Find(pThis);

    if (auto pShieldData = pExt->ShieldData.get())
    {
        auto nDamageLeft = pShieldData->ReceiveDamage(args);

        if (nDamageLeft >= 0)
        {
            *args->Damage = nDamageLeft;
        }
    }

    return 0;
}

DEFINE_HOOK(6FC339, TechnoClass_CanFire_Shield, 6)
{
    GET_STACK(TechnoClass*, pTarget, STACK_OFFS(0x20, -0x4));
    GET(TechnoClass*, pThis, ESI);
    GET(WeaponTypeClass*, pWeapon, EDI);

    if (auto pExt = TechnoExt::ExtMap.Find(pTarget))
    {
        if (auto pShieldData = pExt->ShieldData.get())
        {
            if (!pShieldData->CanBeTargeted(pWeapon, pThis))
            {
                return 0x6FCB7E;
            }
        }
    }
    return 0;
}

//Abandoned because of Ares!!!! - Uranusian
/*
DEFINE_HOOK_AGAIN(6F3725, TechnoClass_WhatWeaponShouldIUse_Shield, 6)
DEFINE_HOOK(6F36F2, TechnoClass_WhatWeaponShouldIUse_Shield, 6)
{
    GET(TechnoClass*, pTarget, EBP);
    if (auto pExt = TechnoExt::ExtMap.Find(pTarget))
    {
        if (auto pShieldData = pExt->ShieldData.get())
        {
            if (pShieldData->GetShieldHP())
            {
                auto pTypeExt = TechnoTypeExt::ExtMap.Find(pTarget->GetTechnoType());

                if (R->Origin() == 0x6F36F2)
                    R->ECX(pTypeExt->Shield_Armor);
                else
                    R->EAX(pTypeExt->Shield_Armor);

                return R->Origin() + 6;
            }
        }
    }
    return 0;
}
*/

DEFINE_HOOK(6F36DB, TechnoClass_WhatWeaponShouldIUse_Shield, 8)
{
    GET(TechnoClass*, pThis, ESI);
    GET(TechnoClass*, pTarget, EBP);
    if (!pTarget) return 0x6F37AD; //Target invalid, skip. (test ebp,ebp  jz loc_6F37AD)
    if (auto pExt = TechnoExt::ExtMap.Find(pTarget))
    {
        if (auto pShieldData = pExt->ShieldData.get())
        {
            if (pShieldData->GetShieldHP())
            {
                auto Primary = pThis->GetWeapon(0);
                if (auto Secondary = pThis->GetWeapon(1))
                {
                    if (!pShieldData->CanBeTargeted(Primary->WeaponType, pThis))
                        return 0x6F3745; //Priamry cannot attack, always use Secondary
                    return 0x6F3754; //Further check in vanilla function
                }
                return 0x6F37AD; //Don't have Secondary, always use Primary
            }
        }
    }
    return 0x6F36E3; //Target doesn't have a shield, back
}

DEFINE_HOOK(6F9E50, TechnoClass_AI_Shield, 5)
{
    GET(TechnoClass*, pThis, ECX);
    auto pExt = TechnoExt::ExtMap.Find(pThis);
    auto pTypeData = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

    if (pTypeData->Shield_Strength)
    {
        if (!pExt->ShieldData)
        {
            pExt->ShieldData = std::make_unique<ShieldTechnoClass>(pThis);
        }

        pExt->ShieldData->AI();
    }
    return 0;
}

DEFINE_HOOK(6F6AC4, TechnoClass_Remove_Shield, 5)
{
    GET(TechnoClass*, pThis, ECX);
    auto pExt = TechnoExt::ExtMap.Find(pThis);

    if (pExt->ShieldData && (pThis->WhatAmI() != AbstractType::Building || !pThis->GetTechnoType()->UndeploysInto))
    {
        pExt->ShieldData = nullptr;
    }

    return 0;
}

DEFINE_HOOK_AGAIN(44A03C, DeploysInto_UndeploysInto_SyncShieldStatus, 6) //BuildingClass_Mi_Selling_SyncShieldStatus
DEFINE_HOOK(739956, DeploysInto_UndeploysInto_SyncShieldStatus, 6) //UnitClass_Deploy_SyncShieldStatus
{
    GET(TechnoClass*, pThis, EBP);
    GET(TechnoClass*, pInto, EBX);
    auto pThisExt = TechnoExt::ExtMap.Find(pThis);
    auto pIntoTypeExt = TechnoTypeExt::ExtMap.Find(pInto->GetTechnoType());

    if (pThisExt->ShieldData && pIntoTypeExt->Shield_Strength)
    {
        ShieldTechnoClass::SyncShieldToAnother(pThis, pInto);
    }

    if (pThis->WhatAmI() == AbstractType::Building && pThisExt->ShieldData)
        pThisExt->ShieldData = nullptr;

    return 0;
}


DEFINE_HOOK(6F65D1, TechnoClass_DrawHealthBar_DrawBuildingShieldBar, 6)
{
    GET(TechnoClass*, pThis, ESI);
    GET(int, iLength, EBX);
    GET_STACK(Point2D*, pLocation, STACK_OFFS(0x4C, -0x4));
    GET_STACK(RectangleStruct*, pBound, STACK_OFFS(0x4C, -0x8));
    auto pExt = TechnoExt::ExtMap.Find(pThis);

    if (pExt->ShieldData)
    {
        pExt->ShieldData->DrawShieldBar(iLength, pLocation, pBound);
    }

    return 0;
}

DEFINE_HOOK(6F683C, TechnoClass_DrawHealthBar_DrawOtherShieldBar, 7)
{
    GET(TechnoClass*, pThis, ESI);
    GET_STACK(Point2D*, pLocation, STACK_OFFS(0x4C, -0x4));
    GET_STACK(RectangleStruct*, pBound, STACK_OFFS(0x4C, -0x8));
    auto pExt = TechnoExt::ExtMap.Find(pThis);

    if (pExt->ShieldData)
    {
        int iLength = pThis->WhatAmI() == AbstractType::Infantry ? 8 : 17;
        pExt->ShieldData->DrawShieldBar(iLength, pLocation, pBound);
    }

    return 0;
}