/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weaponammo.inc
 *  Type:          Core
 *  Description:   API for all weaponammo-related functions.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * @section Grenade Pack plugin information.
 */
#define GRENADE_PACK_FILENAME "grenadepack.smx"
#define GRENADE_PACK_CVAR_LIMIT "gp_limit"
/**
 * @endsection
 */

/**
 * @section Defines for ZMarket grenade limits, supporting the Grenade Pack plugin.
 */
#define WEAPONAMMO_HEGRENADE_LIMIT      1
#define WEAPONAMMO_FLASHBANG_LIMIT      2
#define WEAPONAMMO_SMOKEGRENADE_LIMIT   1
#define WEAPONAMMO_INCENDIARY_LIMIT		1
#define WEAPONAMMO_MOLOTOV_LIMIT		1
#define WEAPONAMMO_DECOY_LIMIT			1
/**
 * @endsection
 */

/**
 * Variable to store if Grenade Pack is loaded and running.
 */
new bool:g_bGrenadePack;

/**
 * @section Variables to store ammo offset values.
 */
new g_iToolsClip1;
new g_iToolsClip2;
new g_iToolsAmmo;
/**
 * @endsection
 */

/**
 * Grenade slots.
 */
enum WeaponAmmoGrenadeType
{
    GrenadeType_Invalid             = -1,   /** Invalid grenade slot. */
    GrenadeType_HEGrenade           = 11,   /** HEGrenade slot. */
    GrenadeType_Flashbang           = 12,   /** Flashbang slot. */
    GrenadeType_Smokegrenade        = 13,   /** Smokegrenade slot. */
    GrenadeType_HEGrenadeCSGO       = 14,   /** HEGrenade slot. CSGO ONLY */
    GrenadeType_FlashbangCSGO       = 15,   /** Flashbang slot. CSGO ONLY */
    GrenadeType_SmokegrenadeCSGO    = 16,   /** Smokegrenade slot. CSGO ONLY */
    GrenadeType_Incendiary          = 17,	/** Inc Grenade and Molotov slot. */
    GrenadeType_Decoy               = 18,	/** Decoy slot. */
}

/**
 * All plugins have finished loading.
 */
WeaponAmmoOnAllPluginsLoaded()
{
    // Check if "Grenade Pack" is loaded
    g_bGrenadePack = ZMarketIsGPLoaded();
}

/**
 * Find ammo-reserve-specific offsets here.
 */
WeaponAmmoOnOffsetsFound()
{
    // If offset "m_iClip1" can't be found, then stop the plugin.
    g_iToolsClip1 = FindSendPropInfo("CBaseCombatWeapon", "m_iClip1");
    if (g_iToolsClip1 == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBaseCombatWeapon::m_iClip1\" was not found.");
    }

    // If offset "m_iClip2" can't be found, then stop the plugin.
    g_iToolsClip2 = FindSendPropInfo("CBaseCombatWeapon", "m_iClip2");
    if (g_iToolsClip2 == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBaseCombatWeapon::m_iClip2\" was not found.");
    }

    // If offset "m_iAmmo" can't be found, then stop the plugin.
    g_iToolsAmmo = FindSendPropInfo("CBasePlayer", "m_iAmmo");
    if (g_iToolsAmmo == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBasePlayer::m_iAmmo\" was not found.");
    }
}

/**
 * Set clip/reserve ammo on a weapon.
 *
 * @param weapon    The weapon index.
 * @param clip      True sets clip ammo, false sets reserve.
 * @param value     The amount of ammo to set to.
 * @param add       (Optional) If true, the value is added to the weapon's current ammo count.
 */
stock WeaponAmmoSetAmmo(weapon, bool:clip, value, bool:add = false)
{
    // Set variable to offset we are changing.
    new ammooffset = clip ? g_iToolsClip1 : g_iToolsClip2;

    // Initialize variable (value is 0)
    new ammovalue;

    // If we are adding, then update variable with current ammo value.
    if (add)
    {
        ammovalue = WeaponAmmoGetAmmo(weapon, clip);
    }

    // Return ammo offset value.
    SetEntData(weapon, ammooffset, ammovalue + value, _, true);
}

/**
 * Get clip/reserve ammo on a weapon.
 *
 * @param weapon    The weapon index.
 * @param clip      True gets clip ammo, false gets reserve.
 */
stock WeaponAmmoGetAmmo(weapon, bool:clip)
{
    // Set variable to offset we are changing.
    new ammooffset = clip ? g_iToolsClip1 : g_iToolsClip2;

    // Return ammo offset value.
    return GetEntData(weapon, ammooffset);
}

/**
 * Set the count of any grenade-type a client has.
 *
 * @param client    The client index.
 * @param slot      The type of
 * @param value     The amount of ammo to set to.
 * @param add       (Optional) If true, the value is added to the grenades' current ammo count.
 */
stock WeaponAmmoSetGrenadeCount(client, WeaponAmmoGrenadeType:type, value, bool:add)
{
    // Initialize variable (value is 0)
    new ammovalue;

    // If we are adding, then update variable with current ammo value.
    if (add)
    {
        ammovalue = WeaponAmmoGetGrenadeCount(client, type);
    }

    SetEntData(client, g_iToolsAmmo + (_:type * 4), ammovalue + value, _, true);
}

/**
 * Get the count of any grenade-type a client has.
 *
 * @param client    The client index.
 * @param slot      The type of
 */
stock WeaponAmmoGetGrenadeCount(client, WeaponAmmoGrenadeType:type)
{
    return GetEntData(client, g_iToolsAmmo + (_:type * 4));
}

/**
 * Takes a weapon entity and returns an entry in enum WeaponAmmoGrenadeType.
 *
 * @param weaponentity  The weapon entity to find entry for.
 * @return              An entry in WeaponAmmoGrenadeType.
 */
stock WeaponAmmoGrenadeType:WeaponAmmoEntityToGrenadeType(const String:weaponentity[])
{
    if (StrEqual(weaponentity, "weapon_hegrenade"))
    {
        if (g_Game == Game_CSGO)
            return GrenadeType_HEGrenadeCSGO;
        else
            return GrenadeType_HEGrenade;
    }
    else if (StrEqual(weaponentity, "weapon_flashbang"))
    {
        if (g_Game == Game_CSGO)
            return GrenadeType_FlashbangCSGO;
        else
            return GrenadeType_Flashbang;
    }
    else if (StrEqual(weaponentity, "weapon_smokegrenade"))
    {
        if (g_Game == Game_CSGO)
            return GrenadeType_SmokegrenadeCSGO;
        else
            return GrenadeType_Smokegrenade;
    }
    else if (StrEqual(weaponentity, "weapon_incgrenade"))
    {
        return GrenadeType_Incendiary;
    }
	else if (StrEqual(weaponentity, "weapon_molotov"))
    {
        return GrenadeType_Incendiary;
    }
	else if (StrEqual(weaponentity, "weapon_decoy"))
    {
        return GrenadeType_Decoy;
    }

    return GrenadeType_Invalid;
}

/**
 * Returns the max amount of this type of grenades the client is allowed to carry.
 *
 * @param weaponentity  The weapon entity to get the limit for.
 * @return              The grenade limit, -1 if an unhandled grenade entity was given.
 */
 stock WeaponAmmoGetGrenadeLimit(WeaponAmmoGrenadeType:grenadetype)
 {
    switch(grenadetype)
    {
        case GrenadeType_HEGrenade, GrenadeType_HEGrenadeCSGO:
        {
            // Attempt to find a cvar provided by an outside plugin.
            new Handle:gplimit = FindConVar(GRENADE_PACK_CVAR_LIMIT);

            // If Grenade Pack is loaded and the cvar was found, then get the value of the outside cvar, if not return CS:S default.
            return (g_bGrenadePack && gplimit != INVALID_HANDLE) ? GetConVarInt(gplimit) : WEAPONAMMO_HEGRENADE_LIMIT;
        }
        case GrenadeType_Flashbang, GrenadeType_FlashbangCSGO:
        {
            return WEAPONAMMO_FLASHBANG_LIMIT;
        }
        case GrenadeType_Smokegrenade, GrenadeType_SmokegrenadeCSGO:
        {
            return WEAPONAMMO_SMOKEGRENADE_LIMIT;
        }
        case GrenadeType_Incendiary:
        {
            return WEAPONAMMO_INCENDIARY_LIMIT;
        }
		case GrenadeType_Decoy:
        {
            return WEAPONAMMO_DECOY_LIMIT;
        }
    }

    return -1;
}

/**
 * Returns true if plugin "Grenade Pack" is found and running.
 */
stock bool:ZMarketIsGPLoaded()
{
    new Handle:hPlugins = GetPluginIterator();
    new Handle:hPlugin;
    decl String:strPlugin[PLATFORM_MAX_PATH];

    while (MorePlugins(hPlugins))
    {
        // Read the next plugin.
        hPlugin = ReadPlugin(hPlugins);

        GetPluginFilename(hPlugin, strPlugin, sizeof(strPlugin));
        if (!StrEqual(strPlugin, GRENADE_PACK_FILENAME, false))
        {
            continue;
        }

        // Plugin was found, now return true only if it is running.
        return (GetPluginStatus(hPlugin) == Plugin_Running);
    }

    // Done iterating, close handle.
    CloseHandle(hPlugins);

    // Plugin wasn't found.
    return false;
}