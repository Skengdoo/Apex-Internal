#include "Engine.h"

class CWeapon
{
private:

public:
	DWORD64 Handle;

	explicit CWeapon(DWORD64 handle)
	{
		Handle = handle;
	}


	int WeaponID()
	{
		if (!Handle)
			return -1;
		return *(int*)(Handle + Offsets::Classes::CWeaponX::m_weaponNameIndex);
	}

	int AmmoInClip()
	{
		if (!Handle)
			return -1;
		return *(int*)(Handle + Offsets::Classes::CPropSurvival::m_ammoInClip);
	}

	void ChangeSkin(int id)
	{
		if (!Handle)
			return;
		*(int*)(Handle + Offsets::Classes::CPropSurvival::m_nSkin) = id;
		*(int*)(Handle + Offsets::Classes::CPropSurvival::m_skinMod) = id;
	}

	std::string GetWeaponName()
	{
		auto weaponid = this->WeaponID();
		if (!weaponid)
			return "";

		if (weaponid == WeaponIDS::ALTERNATOR)
			return "ALTERNATOR";
		else if (weaponid == WeaponIDS::BOCEK)
			return "BOCEK";
		else if (weaponid == WeaponIDS::CAR_MP)
			return "C.A.R.-MP";
		else if (weaponid == WeaponIDS::CHARGE_RIFLE)
			return "CHARGE-RIFLE";
		else if (weaponid == WeaponIDS::DEVOTION)
			return "DEVOTION";
		else if (weaponid == WeaponIDS::EVA_8_AUTO)
			return "EVA-8 AUTO";
		else if (weaponid == WeaponIDS::FLATLINE)
			return "FLATLINE";
		else if (weaponid == WeaponIDS::G7_SCOUT)
			return "G7 SCOUT";
		else if (weaponid == WeaponIDS::HAVOC)
			return "HAVOC";
		else if (weaponid == WeaponIDS::HEMLOK)
			return "HAMLOK";
		else if (weaponid == WeaponIDS::KRABER)
			return "KRABER";
		else if (weaponid == WeaponIDS::LONGBOW)
			return "LONGBOW";
		else if (weaponid == WeaponIDS::L_STAR)
			return "L-STAR";
		else if (weaponid == WeaponIDS::MASTIFF)
			return "MASTIFF";
		else if (weaponid == WeaponIDS::MOSAMBIK)
			return "MOZAMBIQUE";
		else if (weaponid == WeaponIDS::P2020)
			return "P2020";
		else if (weaponid == WeaponIDS::PEACEKEEPER)
			return "PEACEKEEPER";
		else if (weaponid == WeaponIDS::PROWLER)
			return "PROWLER";
		else if (weaponid == WeaponIDS::R301)
			return "R301";
		else if (weaponid == WeaponIDS::R99)
			return "R99";
		else if (weaponid == WeaponIDS::RAMPAGE)
			return "RAMPAGE";
		else if (weaponid == WeaponIDS::RE_45)
			return "RE-45 AUTO";
		else if (weaponid == WeaponIDS::SENTINEL)
			return "SENTINEL";
		else if (weaponid == WeaponIDS::SPITFIRE)
			return "SPITFIRE";
		else if (weaponid == WeaponIDS::TRIPLE_TAKE)
			return "TRIPLE TAKE";
		else if (weaponid == WeaponIDS::VOLT)
			return "VOLT-MP";
		else if (weaponid == WeaponIDS::WINGMAN)
			return "WINGMAN";
		else if (weaponid == WeaponIDS::_30_30)
			return "30-30 REPEATER";
		else if (weaponid == WeaponIDS::MEELE)
			return "MEELE";
		else
			return "UNKNOWN";
	}
};