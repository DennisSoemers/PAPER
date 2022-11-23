#pragma once

#include <RE/Skyrim.h>

namespace OnHitEvents {
#pragma warning(push)
#pragma warning(disable : 4251)

    /** 
     * Data about recently-processed OnHit events, so we can avoid 
     * spamming multiple events for a single hit in a short timespan.
     */
    struct RecentHitEventData {

        RecentHitEventData(RE::TESObjectREFR* target, RE::TESObjectREFR* cause,
                           float applicationRuntime)
            : target(target), cause(cause), applicationRuntime(applicationRuntime) {}

        /** The target that was hit */
        RE::TESObjectREFR* target;
        /** The cause / aggressor of the hit */
        RE::TESObjectREFR* cause;
        /** Runtime of the Skyrim application at the time the event was received */
        float applicationRuntime;

    };

    /**
     * Our singleton event handler for new variants of OnHit events.
     */
    class __declspec(dllexport) OnHitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {

    public:

       /**
        * Overridden from RE::BSTEventSink. Lets us process events received from the game engine.
        */
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event,
                                                      RE::BSTEventSource<RE::TESHitEvent>* a_eventSource);

        /**
         * Get the singleton instance of the <code>OnHitEventHandler</code>.
         */
        [[nodiscard]] static OnHitEventHandler& GetSingleton() noexcept;

    private:
        OnHitEventHandler() = default;
        OnHitEventHandler(const OnHitEventHandler&) = delete;
        OnHitEventHandler(OnHitEventHandler&&) = delete;
        ~OnHitEventHandler() = default;

        OnHitEventHandler& operator=(const OnHitEventHandler&) = delete;
        OnHitEventHandler& operator=(OnHitEventHandler&&) = delete;

        /** Keep track of recently-processed hit events here. */
        std::vector<RecentHitEventData> recentHits;
    };
#pragma warning(pop)
}  // namespace OnHitEvents
