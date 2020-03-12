// A cache wrapper class useful for inserting caches around rule list lookups

#ifndef RULE_LOOKUP_CACHE_H_
#define RULE_LOOKUP_CACHE_H_

extern struct UnitItemInfo;
extern struct Rule;

#include <memory>
#include <vector>
#include <utility>
#include "lrucache.hpp"

template <typename T>
class RuleLookupCache {
	std::unique_ptr<cache::lru_cache<DWORD, std::pair<std::string, T>>> cache;
	
	protected:
	const std::vector<Rule*> &RuleList;
	virtual T make_cached_T(UnitItemInfo *uInfo, const string &name) = 0;

	public:
	RuleLookupCache(const std::vector<Rule*> &rule_list) 
		: RuleList(rule_list), cache(new cache::lru_cache<DWORD, std::pair<std::string, T>>(50)) {}

	void ResetCache() {
		cache.reset(new cache::lru_cache<DWORD, std::pair<std::string, T>>(50));
	}
	
	// TODO: UnitItemInfo should probably be const, but call to Evaluate needs non-const
	T Get(UnitItemInfo *uInfo, const std::string &name) {
		// set to true to print when mismatch occurs (will never use cached name in this case)
		constexpr bool cache_debug = false;
		static DWORD last_printed_guid = 0; // to prevent excessive printing
		// TODO: report error if cache never initialized (should be done in onGameJoin)
		DWORD guid = uInfo->item->dwUnitId; // global unique identifier
		//const string name_cpy(name); // used to store the original item name which is edited in place
		std::string orig_cached_name; // the cached unmodified item name
		T cached_T; // the cached item name after rules applied
		bool cache_hit = false;
		// First check if the name exists in the cache
		if (cache && cache->exists(guid)) {
			orig_cached_name.assign(cache->get(guid).first);
			if (orig_cached_name == name) {
				//cached_name.assign(cache->get(guid).second);
				cached_T = cache->get(guid).second;
				cache_hit = true; // needed because empty string is also a valid item name
			} else {
				// This print can tell you if a GUID ever changes for an item. Problem is that it will also
				// print whenever you ID an item, make a runeword, personalize an item, etc.
				// I suggest we leave it on for awhile to make sure GUIDs are never changing on us. -ybd
				PrintText(1, "Detected change in unmodified item name. Cached: %s Actual: %s", orig_cached_name.c_str(), name.c_str());
			}
			// cache_hit is false if the unmodified item name has changed from cached version
		}
		if (!cache_hit || cache_debug) {
			cached_T = make_cached_T(uInfo, name);
			if (cache && !cache_hit) {
				std::pair<std::string, T> pair_to_cache(name, cached_T);
				cache->put(guid, pair_to_cache);
				//PrintText(1, "Adding key value pair %u, %s to cache.", guid, name.c_str());
			}
			// Debug stuff below doesn't work if T is not a string.
			//else if (cached_name != name) {
				// only runs if item_name_debug is on
			//	if (guid != last_printed_guid) {
			//		PrintText(1, "Mismatch in modified item name! Cached: %s Actual: %s", cached_name.c_str(), name.c_str());
			//		last_printed_guid = guid;
			//	}
			//}
		} else {
			return cached_T;
		}
			
		}
};

#endif // RULE_LOOKUP_CACHE_H_
