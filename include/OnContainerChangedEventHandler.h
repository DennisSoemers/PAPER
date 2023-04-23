#pragma once

#include <RE/Skyrim.h>

namespace OnContainerChangedEvents {
#pragma warning(push)
#pragma warning(disable : 4251)

    // Retrieve manual offset for edge cases CLIB-NG does not account for yet
    // Thanks to Nightfallstorm!
    template <class T>
    T GetManualRelocateMemberVariable(void* object, REL::VariantOffset offset) {
        return *(reinterpret_cast<T*>((uintptr_t)object + offset.offset()));
    }

    /** 
     * Data for an event to be processed.
     */
    struct ItemEvent {
        ItemEvent(RE::FormID otherContainer, RE::FormID baseObj, std::int32_t itemCount)
            : otherContainer(otherContainer), baseObj(baseObj), itemCount(itemCount) {}
        ItemEvent() = delete;

        RE::FormID otherContainer;
        RE::FormID baseObj;
        std::int32_t itemCount;
    };

    /**
     * Our singleton event handler for new variants of OnContainerChanged events.
     */
    class __declspec(dllexport) OnContainerChangedEventHandler : public RE::BSTEventSink<RE::TESContainerChangedEvent> {

    public:

       /**
        * Overridden from RE::BSTEventSink. Lets us process events received from the game engine.
        */
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESContainerChangedEvent* a_event,
                                                      RE::BSTEventSource<RE::TESContainerChangedEvent>* a_eventSource);

        /**
         * Get the singleton instance of the <code>OnContainerChangedEventHandler</code>.
         */
        [[nodiscard]] static OnContainerChangedEventHandler& GetSingleton() noexcept;

        /**
         * The serialization handler for reverting game state.
         */
        static void OnRevert(SKSE::SerializationInterface*);

        /**
         * The serialization handler for saving data to the cosave.
         */
        static void OnGameSaved(SKSE::SerializationInterface* serde);

        /**
         * The serialization handler for loading data from a cosave.
         */
        static void OnGameLoaded(SKSE::SerializationInterface* serde);


        /**
         * Does an item with the given form ID pass the given inventory event filter lists?
         */
        static bool ItemPassesInventoryFilterLists(const RE::FormID itemID,
                                                   const RE::SkyrimVM::InventoryEventFilterLists* filterLists);

        void SendItemAddedEvents();
        void SendItemRemovedEvents();

        /** Map of batched item-added events, to be processed */
        std::unordered_map<RE::FormID, std::vector<ItemEvent>> batchedItemAddedEventsMap;
        /** Map of batched item-removed events, to be processed */
        std::unordered_map<RE::FormID, std::vector<ItemEvent>> batchedItemRemovedEventsMap;
        /** Mutex for access to map with item-added events to be processed */
        std::mutex batchedItemAddedEventsMapMutex;
        /** Mutex for access to map with item-removed events to be processed */
        std::mutex batchedItemRemovedEventsMapMutex;
        /** Did we already queue up a task to process item-added events? */
        bool haveQueuedUpTaskAddedEvents = false;
        /** Did we already queue up a task to process item-removed events? */
        bool haveQueuedUpTaskRemovedEvents = false;

    private:
        OnContainerChangedEventHandler() = default;
        OnContainerChangedEventHandler(const OnContainerChangedEventHandler&) = delete;
        OnContainerChangedEventHandler(OnContainerChangedEventHandler&&) = delete;
        ~OnContainerChangedEventHandler() = default;

        OnContainerChangedEventHandler& operator=(const OnContainerChangedEventHandler&) = delete;
        OnContainerChangedEventHandler& operator=(OnContainerChangedEventHandler&&) = delete;

    };

    struct ItemEventsFilter : RE::SkyrimVM::ISendEventFilter {
        ItemEventsFilter(const std::vector<RE::TESForm*> baseItems) : baseItems(baseItems){};
        ItemEventsFilter() = delete;

        const std::vector<RE::TESForm*> baseItems;

        virtual bool matchesFilter(RE::VMHandle handle) override {
            auto vm = RE::SkyrimVM::GetSingleton();

            //const auto& constInventoryEventFilterMapLock =
            //    GetManualRelocateMemberVariable<RE::BSSpinLock>(vm, REL::VariantOffset(0x8940, 0x8940, 0x8960));

            //auto& inventoryEventFilterMapLock = const_cast<RE::BSSpinLock&>(constInventoryEventFilterMapLock);
            //RE::BSSpinLockGuard locker(inventoryEventFilterMapLock);

            const auto& inventoryEventFilterMap =
                GetManualRelocateMemberVariable<RE::BSTHashMap<RE::VMHandle, RE::SkyrimVM::InventoryEventFilterLists*>>(
                    vm, REL::VariantOffset(0x8948, 0x8948, 0x8968));

            RE::SkyrimVM::InventoryEventFilterLists* filterLists = nullptr;
            auto it = inventoryEventFilterMap.find(handle);
            if (it != inventoryEventFilterMap.end()) {
                filterLists = it->second;
            }

            if (filterLists) {
                // Have filters, so need at least one of our items to match
                for (auto baseObj : baseItems) {
                    if (OnContainerChangedEventHandler::ItemPassesInventoryFilterLists(baseObj->formID, filterLists)) {
                        return true;
                    }
                }
            } else {
                // No filters, so anything matches
                return true;
            }

            // Have filters but none matched, so return false
            return false;
        }
    };
#pragma warning(pop)
}  // namespace OnContainerChangedEvents
