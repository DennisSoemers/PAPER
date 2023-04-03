#pragma once

#include <RE/Skyrim.h>

namespace OnContainerChangedEvents {
#pragma warning(push)
#pragma warning(disable : 4251)

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
        bool ItemPassesInventoryFilterLists(const RE::FormID itemID,
                                            const RE::SkyrimVM::InventoryEventFilterLists* filterLists) const;

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
#pragma warning(pop)
}  // namespace OnContainerChangedEvents
