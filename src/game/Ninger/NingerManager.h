#ifndef NINGER_MANAGER_H
#define NINGER_MANAGER_H

#define enum_to_string(x) #x

#ifndef NINGER_PASSWORD
# define NINGER_PASSWORD "ninger"
#endif

#ifndef AOE_TARGETS_RANGE
# define AOE_TARGETS_RANGE 5.0f
#endif

#ifndef MID_RANGE
# define MID_RANGE 8.0f
#endif

#ifndef MIN_DISTANCE_GAP
# define MIN_DISTANCE_GAP 0.5f
#endif

#ifndef FOLLOW_MIN_DISTANCE
# define FOLLOW_MIN_DISTANCE 0.5f
#endif

#ifndef FOLLOW_NEAR_DISTANCE
# define FOLLOW_NEAR_DISTANCE 5.0f
#endif

#ifndef FOLLOW_NORMAL_DISTANCE
# define FOLLOW_NORMAL_DISTANCE 9.0f
#endif

#ifndef FOLLOW_FAR_DISTANCE
# define FOLLOW_FAR_DISTANCE 25.0f
#endif

#ifndef FOLLOW_MAX_DISTANCE
# define FOLLOW_MAX_DISTANCE 38.0f
#endif

#ifndef MELEE_MIN_DISTANCE
# define MELEE_MIN_DISTANCE 0.5f
#endif

#ifndef MELEE_MAX_DISTANCE
# define MELEE_MAX_DISTANCE 3.0f
#endif

#ifndef RANGE_MIN_DISTANCE
# define RANGE_MIN_DISTANCE 9.0f
#endif

#ifndef RANGE_DPS_DISTANCE
# define RANGE_DPS_DISTANCE 28.0f
#endif

#ifndef RANGE_HEAL_DISTANCE
# define RANGE_HEAL_DISTANCE 38.0f
#endif

#ifndef DEFAULT_REST_DELAY
# define DEFAULT_REST_DELAY 20000
#endif

#include "NingerEntity.h"
#include "Player.h"

#include <string>
#include <iostream>
#include <sstream>

#include "NingerConfig.h"

class NingerManager
{
	NingerManager();
	NingerManager(NingerManager const&) = delete;
	NingerManager& operator=(NingerManager const&) = delete;
	~NingerManager() = default;

public:
	void InitializeManager();
	void UpdateNingerManager(uint32 pmDiff);
	bool Deleteningers();

	uint32 CheckNingerAccount(std::string pmAccountName);
	bool CreateNingerAccount(std::string pmAccountName);
	uint32 CheckAccountCharacter(uint32 pmAccountID);
	uint32 GetCharacterRace(uint32 pmCharacterID);
	uint32 CreateNingerCharacter(uint32 pmAccountID);
	uint32 CreateNingerCharacter(uint32 pmAccountID, uint32 pmCharacterClass, uint32 pmCharacterRace);
	bool PrepareNinger(Player* pmninger);
	std::unordered_set<uint32> GetUsableEquipSlot(const ItemPrototype* pmIT);
	Player* CheckLogin(uint32 pmAccountID, uint32 pmCharacterID);
	bool LoginNinger(uint32 pmAccountID, uint32 pmCharacterID);
	bool LoginNingers(uint32 pmLevel);
	void LogoutNinger(uint32 pmCharacterID);
	void LogoutNingers();
	void LogoutNingers(uint32 pmLevel);
	void HandlePlayerSay(Player* pmPlayer, std::string pmContent);
	void HandleChatCommand(Player* pmSender, std::string pmCMD, Player* pmReceiver = NULL);
	bool StringEndWith(const std::string& str, const std::string& tail);
	bool StringStartWith(const std::string& str, const std::string& head);
	std::vector<std::string> SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored);
	std::string TrimString(std::string srcStr);
	static NingerManager* instance();
	void HandlePacket(WorldSession* pmSession, WorldPacket pmPacket);
	void WhisperTo(Player* pmTarget, std::string pmContent, Language pmLanguage, Player* pmSender);

	bool InitializeCharacter(Player* pmTargetPlayer, uint32 pmTargetLevel);
    void LearnPlayerTalents(Player* pmTargetPlayer);
	void InitializeEquipments(Player* pmTargetPlayer, bool pmReset);
	uint32 GetUsableArmorSubClass(Player* pmTargetPlayer);
	void TryEquip(Player* pmTargetPlayer, std::unordered_set<uint32> pmClassSet, std::unordered_set<uint32> pmSubClassSet, uint32 pmTargetSlot);
	bool EquipNewItem(Player* pmTargetPlayer, uint32 pmItemEntry, uint8 pmEquipSlot);
	void RandomTeleport(Player* pmTargetPlayer);    

	bool TankThreatOK(Player* pmTankPlayer, Unit* pmVictim);
	bool HasAura(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster = NULL);
	bool MissingAura(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster = NULL);
	uint32 GetAuraDuration(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster = NULL);
	uint32 GetAuraStack(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster = NULL);

public:
	std::unordered_map<uint32, std::unordered_map<uint32, uint32>> allianceRaces;
	std::unordered_map<uint32, std::unordered_map<uint32, uint32>> hordeRaces;
	std::unordered_map<uint32, std::unordered_map<uint32, uint32>> availableRaces;
	std::unordered_map<uint32, std::string> ningerNameMap;

	std::unordered_map<uint8, std::unordered_map<uint8, std::string>> characterTalentTabNameMap;
	std::set<uint32> deleteningerAccountSet;	
	std::unordered_map<std::string, NingerEntity*> ningerEntityMap;

	uint32 nameIndex;

	std::unordered_set<uint32> spellRewardClassQuestIDSet;
	std::unordered_map<uint32, uint32> onlinePlayerIDMap;

	std::unordered_map<uint32, uint32> tamableBeastEntryMap;
	std::unordered_map<std::string, std::set<uint32>> spellNameEntryMap;

	int onlineCheckDelay;
	int offlineCheckDelay;
};

#define sNingerManager NingerManager::instance()

#endif
