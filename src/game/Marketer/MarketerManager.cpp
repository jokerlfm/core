#include "MarketerManager.h"
#include "AuctionHouseMgr.h"

MarketerManager::MarketerManager()
{
	sellingIndex = 0;
	selling = false;
	buyerCheckDelay = 0;
	sellerCheckDelay = 0;
	auctionHouseIDSet.clear();
	vendorUnlimitItemSet.clear();
	sellingItemIDMap.clear();
	sellableItemIDMap.clear();
	exceptionEntrySet.clear();
}

MarketerManager* MarketerManager::instance()
{
	static MarketerManager instance;
	return &instance;
}

void MarketerManager::InitializeManager()
{
	sLog.outBasic("Initialize marketer manager");

	buyerCheckDelay = TimeConstants::HOUR * TimeConstants::IN_MILLISECONDS;
	sellerCheckDelay = 2 * TimeConstants::HOUR * TimeConstants::IN_MILLISECONDS;

	auctionHouseIDSet.clear();
	auctionHouseIDSet.insert(1);
	auctionHouseIDSet.insert(6);
	auctionHouseIDSet.insert(7);

	vendorUnlimitItemSet.clear();
	QueryResult* vendorItemQR = WorldDatabase.Query("SELECT distinct item FROM npc_vendor where maxcount = 0");
	if (vendorItemQR)
	{
		do
		{
			Field* fields = vendorItemQR->Fetch();
			uint32 eachItemEntry = fields[0].GetUInt32();
			vendorUnlimitItemSet.insert(eachItemEntry);
		} while (vendorItemQR->NextRow());
		delete vendorItemQR;
	}
	exceptionEntrySet.clear();
	exceptionEntrySet.insert(24358);
	exceptionEntrySet.insert(5558);
	exceptionEntrySet.insert(20370);
	exceptionEntrySet.insert(20372);

	sellingItemIDMap.clear();
	sellableItemIDMap.clear();
	for (uint32 id = 0; id < sItemStorage.GetMaxEntry(); id++)
	{
		if (exceptionEntrySet.find(id) != exceptionEntrySet.end())
		{
			continue;
		}
		ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(id);
		if (!proto)
		{
			continue;
		}
		if (proto->ItemLevel < 1)
		{
			continue;
		}
		if (proto->Quality < 1)
		{
			continue;
		}
		if (proto->Quality > 4)
		{
			continue;
		}
		if (proto->Bonding != ItemBondingType::NO_BIND && proto->Bonding != ItemBondingType::BIND_WHEN_EQUIPPED && proto->Bonding != ItemBondingType::BIND_WHEN_USE)
		{
			continue;
		}
		if (proto->SellPrice < 1000 && proto->BuyPrice < 1000)
		{
			continue;
		}
		if (vendorUnlimitItemSet.find(proto->ItemId) != vendorUnlimitItemSet.end())
		{
			continue;
		}
		bool sellThis = false;
		switch (proto->Class)
		{
		case ItemClass::ITEM_CLASS_CONSUMABLE:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_CONTAINER:
		{
			if (proto->Quality >= 2)
			{
				sellThis = true;
			}
			break;
		}
		case ItemClass::ITEM_CLASS_WEAPON:
		{
			if (proto->Quality >= 2)
			{
				sellThis = true;
			}
			break;
		}
		case ItemClass::ITEM_CLASS_GEM:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_ARMOR:
		{
			if (proto->Quality >= 2)
			{
				sellThis = true;
			}
			break;
		}
		case ItemClass::ITEM_CLASS_REAGENT:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_PROJECTILE:
		{
			break;
		}
		case ItemClass::ITEM_CLASS_TRADE_GOODS:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_GENERIC:
		{
			break;
		}
		case ItemClass::ITEM_CLASS_RECIPE:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_MONEY:
		{
			break;
		}
		case ItemClass::ITEM_CLASS_QUIVER:
		{
			if (proto->Quality >= 2)
			{
				sellThis = true;
			}
			break;
		}
		case ItemClass::ITEM_CLASS_QUEST:
		{
			sellThis = true;
			break;
		}
		case ItemClass::ITEM_CLASS_KEY:
		{
			break;
		}
		case ItemClass::ITEM_CLASS_PERMANENT:
		{
			break;
		}
		case ItemClass::ITEM_CLASS_JUNK:
		{
			if (proto->Quality > 0)
			{
				sellThis = true;
			}
			break;
		}
		default:
		{
			break;
		}
		}
		if (sellThis)
		{
			sellableItemIDMap[sellableItemIDMap.size()] = proto->ItemId;
		}
	}
	sellingIndex = 0;
	selling = false;
}

void MarketerManager::Clean()
{
	sLog.outBasic("Ready to clean marketer");

	for (std::set<uint32>::iterator ahIDIT = auctionHouseIDSet.begin(); ahIDIT != auctionHouseIDSet.end(); ahIDIT++)
	{
		uint32 ahID = *ahIDIT;
		AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(*ahIDIT);
		AuctionHouseObject* aho = sAuctionMgr.GetAuctionsMap(ahEntry);
		if (!aho)
		{
			sLog.outError("AuctionHouseObject is null");
			return;
		}
		std::set<uint32> auctionIDSet;
		auctionIDSet.clear();
		std::map<uint32, AuctionEntry*>* aem = aho->GetAuctions();
		for (std::map<uint32, AuctionEntry*>::iterator aeIT = aem->begin(); aeIT != aem->end(); aeIT++)
		{
			auctionIDSet.insert(aeIT->first);
		}
		for (std::set<uint32>::iterator auctionIDIT = auctionIDSet.begin(); auctionIDIT != auctionIDSet.end(); auctionIDIT++)
		{
			AuctionEntry* eachAE = aho->GetAuction(*auctionIDIT);
			if (eachAE)
			{
				if (eachAE->owner == 0)
				{
					eachAE->DeleteFromDB();
					sAuctionMgr.RemoveAItem(eachAE->itemGuidLow);
					aho->RemoveAuction(eachAE);
					delete eachAE;
					eachAE = nullptr;
					sLog.outBasic("Auction %d removed for auctionhouse %d", eachAE->itemTemplate, ahID);
				}
			}
		}
	}

	sLog.outBasic("Marketer cleaned");
}

bool MarketerManager::UpdateMarketer(uint32 pmDiff)
{
	if (!sMarketerConfig.Enable)
	{
		return false;
	}
	sellerCheckDelay -= pmDiff;
	if (sellerCheckDelay < 0)
	{
		UpdateSeller();
	}
	buyerCheckDelay -= pmDiff;
	if (buyerCheckDelay < 0)
	{
		UpdateBuyer();
	}

	return true;
}

bool MarketerManager::UpdateSeller()
{
	if (sellingItemIDMap.empty())
	{
		sellingIndex = 0;
		while (sellingItemIDMap.size() < 100)
		{
			uint32 toSellItemID = urand(0, sellableItemIDMap.size() - 1);
			toSellItemID = sellableItemIDMap[toSellItemID];
			if (sellingItemIDMap.find(toSellItemID) == sellingItemIDMap.end())
			{
				sellingItemIDMap[sellingItemIDMap.size()] = toSellItemID;
			}
		}
	}
	if (sellingIndex < sellingItemIDMap.size())
	{
		int itemEntry = sellingItemIDMap[sellingIndex];
		const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itemEntry);
		if (proto)
		{
			for (std::set<uint32>::iterator ahIDIT = auctionHouseIDSet.begin(); ahIDIT != auctionHouseIDSet.end(); ahIDIT++)
			{
				uint32 ahID = *ahIDIT;
				AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(*ahIDIT);
				AuctionHouseObject* aho = sAuctionMgr.GetAuctionsMap(ahEntry);
				if (!aho)
				{
					sLog.outError("AuctionHouseObject is null");
					return false;
				}
				uint32 stackCount = urand(1, proto->Stackable);
				uint32 priceMultiple = urand(10, 15);
				Item* item = Item::CreateItem(proto->ItemId, stackCount, NULL);
				if (item)
				{
					if (uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemEntry))
					{
						item->SetItemRandomProperties(randomPropertyId);
					}
					uint32 finalPrice = 0;
					finalPrice = proto->SellPrice * stackCount * priceMultiple;
					if (finalPrice == 0)
					{
						finalPrice = proto->BuyPrice * stackCount * priceMultiple / 4;
					}
					if (finalPrice == 0)
					{
						break;
					}
					if (finalPrice > 100)
					{
						uint32 dep = sAuctionMgr.GetAuctionDeposit(ahEntry, 2 * TimeConstants::HOUR, item);

						AuctionEntry* auctionEntry = new AuctionEntry;
						auctionEntry->Id = sObjectMgr.GenerateAuctionID();
						auctionEntry->auctionHouseEntry = ahEntry;
						auctionEntry->itemGuidLow = item->GetGUIDLow();
						auctionEntry->itemTemplate = item->GetEntry();
						auctionEntry->owner = 0;
						auctionEntry->startbid = finalPrice / 2;
						auctionEntry->buyout = finalPrice;
						auctionEntry->bidder = 0;
						auctionEntry->bid = 0;
						auctionEntry->deposit = dep;
						auctionEntry->depositTime = time(nullptr);
						auctionEntry->expireTime = (time_t)(2 * TimeConstants::HOUR) + time(nullptr);
						item->SaveToDB();
						sAuctionMgr.AddAItem(item);
						aho->AddAuction(auctionEntry);
						auctionEntry->SaveToDB();

						sLog.outBasic("Auction %s added for auctionhouse %d", proto->Name1, ahID);
					}
				}
			}
		}
		sellingIndex++;
		sellerCheckDelay = 1 * IN_MILLISECONDS;
	}
	else
	{
		sellingIndex = 0;
		sellingItemIDMap.clear();
		sellerCheckDelay = 125 * TimeConstants::MINUTE * IN_MILLISECONDS;
	}

	return false;
}

bool MarketerManager::UpdateBuyer()
{
	buyerCheckDelay = HOUR * IN_MILLISECONDS;

	sLog.outBasic("Ready to update marketer buyer");

	std::set<uint32> toBuyAuctionIDSet;
	for (std::set<uint32>::iterator ahIDIT = auctionHouseIDSet.begin(); ahIDIT != auctionHouseIDSet.end(); ahIDIT++)
	{
		uint32 ahID = *ahIDIT;
		AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(*ahIDIT);
		AuctionHouseObject* aho = sAuctionMgr.GetAuctionsMap(ahEntry);
		if (!aho)
		{
			sLog.outError("AuctionHouseObject is null");
			return false;
		}
		toBuyAuctionIDSet.clear();
		std::map<uint32, AuctionEntry*>* aem = aho->GetAuctions();
		for (std::map<uint32, AuctionEntry*>::iterator aeIT = aem->begin(); aeIT != aem->end(); aeIT++)
		{
			Item* checkItem = sAuctionMgr.GetAItem(aeIT->second->itemGuidLow);
			if (!checkItem)
			{
				continue;
			}
			if (aeIT->second->owner == 0)
			{
				continue;
			}
			const ItemPrototype* destIT = sObjectMgr.GetItemPrototype(aeIT->second->itemTemplate);
			if (!destIT)
			{
				continue;
			}
			if (destIT->Quality < 1)
			{
				continue;
			}
			if (destIT->Quality > 4)
			{
				continue;
			}
			uint32 basePrice = destIT->SellPrice;
			if (basePrice == 0)
			{
				basePrice = destIT->BuyPrice / 4;
			}
			if (basePrice == 0)
			{
				continue;
			}
			float buyRate = sMarketerConfig.BuyRate;
			if (vendorUnlimitItemSet.find(aeIT->second->itemTemplate) != vendorUnlimitItemSet.end())
			{
				buyRate = buyRate / 2.0f;
			}
			float priceRate = (float)aeIT->second->buyout / (float)basePrice;
			buyRate = buyRate / priceRate;
			float buyPower = frand(0.0f, 10000.0f);
			if (buyPower < buyRate)
			{
				toBuyAuctionIDSet.insert(aeIT->first);
			}
		}

		for (std::set<uint32>::iterator toBuyIT = toBuyAuctionIDSet.begin(); toBuyIT != toBuyAuctionIDSet.end(); toBuyIT++)
		{
			AuctionEntry* destAE = aho->GetAuction(*toBuyIT);
			if (destAE)
			{
				destAE->bid = destAE->buyout;

				sAuctionMgr.SendAuctionSuccessfulMail(destAE);
				sAuctionMgr.SendAuctionWonMail(destAE);
				sAuctionMgr.RemoveAItem(destAE->itemGuidLow);
				aho->RemoveAuction(destAE);
				destAE->DeleteFromDB();
				delete destAE;
				sLog.outBasic("Auction %d was bought by marketer buyer", *toBuyIT);
			}
		}
	}

	sLog.outBasic("Marketer buyer updated");
	return true;
}