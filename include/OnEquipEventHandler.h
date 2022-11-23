#pragma once

#include <RE/Skyrim.h>

namespace OnEquipEvents {
#pragma warning(push)
#pragma warning(disable : 4251)

    /**
     * Our singleton event handler for new variants of OnEquip events.
     */
    class __declspec(dllexport) OnEquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent> {

    public:

       /**
        * Overridden from RE::BSTEventSink. Lets us process events received from the game engine.
        */
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event,
                                                      RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource);

        /**
         * Get the singleton instance of the <code>OnEquipEventHandler</code>.
         */
        [[nodiscard]] static OnEquipEventHandler& GetSingleton() noexcept;

    private:
        OnEquipEventHandler() = default;
        OnEquipEventHandler(const OnEquipEventHandler&) = delete;
        OnEquipEventHandler(OnEquipEventHandler&&) = delete;
        ~OnEquipEventHandler() = default;

        OnEquipEventHandler& operator=(const OnEquipEventHandler&) = delete;
        OnEquipEventHandler& operator=(OnEquipEventHandler&&) = delete;

    };
#pragma warning(pop)
}  // namespace OnEquipEvents
