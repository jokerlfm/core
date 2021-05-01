#ifndef MARKETER_MANAGER_H
#define MARKETER_MANAGER_H

#ifndef MARKETER_MANAGER_UPDATE_GAP
#define MARKETER_MANAGER_UPDATE_GAP 500
#endif

#include <string>
#include "Log.h"
#include "AuctionHouse/AuctionHouseMgr.h"
#include "Item.h"

#include "MarketerConfig.h"

class MarketerManager
{
	MarketerManager();
	MarketerManager(MarketerManager const&) = delete;
	MarketerManager& operator=(MarketerManager const&) = delete;
	~MarketerManager() = default;

public:
	void InitializeManager();
	void Clean();
	bool UpdateMarketer(uint32 pmDiff);
	static MarketerManager* instance();

private:
	bool UpdateSeller();
	bool UpdateBuyer();

public:
	std::unordered_set<uint32> vendorUnlimitItemSet;

	int32 buyerCheckDelay;
	int32 sellerCheckDelay;

private:
	std::unordered_set<uint32> exceptionEntrySet;
	std::unordered_map<uint32, uint32> sellableItemIDMap;
	bool selling;
	uint32 sellingIndex;
	std::unordered_map<uint32, uint32> sellingItemIDMap;
	Player* pMarketer;
	std::set<uint32> auctionHouseIDSet;
};

#define sMarketerManager MarketerManager::instance()

#endif