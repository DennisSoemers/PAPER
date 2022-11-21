#include <SKSE/SKSE.h>
#include <OnEquipEventHandler.h>

using namespace RE;
using namespace OnEquipEvents;
using namespace SKSE;

static BSFixedString OnSpellEquippedEventName = "OnSpellEquipped";
static BSFixedString OnSpellUnequippedEventName = "OnSpellUnequipped";
static BSFixedString OnShoutEquippedEventName = "OnShoutEquipped";
static BSFixedString OnShoutUnequippedEventName = "OnShoutUnequipped";

OnEquipEventHandler& OnEquipEventHandler::GetSingleton() noexcept {
    static OnEquipEventHandler instance;
    return instance;
}

RE::BSEventNotifyControl OnEquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event,
                                                           RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {

    const auto actor = a_event->actor.get();
    if (actor) {
        auto actorFormType = actor->GetFormType();
        auto vm = RE::SkyrimVM::GetSingleton();

        if (vm) {
            const auto handle = vm->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actorFormType), actor);

            if (handle && handle != vm->handlePolicy.EmptyHandle()) {
                const auto equippedForm = RE::TESForm::LookupByID(a_event->baseObject);
                auto equippedFormType = equippedForm->GetFormType();

                if (equippedFormType == RE::FormType::Spell) {
                    const auto spell = equippedForm->As<RE::SpellItem>();
                    auto eventArgs = RE::MakeFunctionArguments((SpellItem*)spell, (TESObjectREFR*)actor);

                    if (a_event->equipped) {
                        // Send OnSpellEquipped event
                        RE::SkyrimVM::GetSingleton()->SendAndRelayEvent(handle, &OnSpellEquippedEventName, eventArgs,
                                                                        nullptr);
                    } else {
                        // Send OnSpellUnequipped event
                        RE::SkyrimVM::GetSingleton()->SendAndRelayEvent(handle, &OnSpellUnequippedEventName, eventArgs,
                                                                        nullptr);
                    }
                } else if (equippedFormType == RE::FormType::Shout) {
                    const auto shout = equippedForm->As<RE::TESShout>();
                    auto eventArgs = RE::MakeFunctionArguments((TESShout*)shout, (TESObjectREFR*)actor);

                    if (a_event->equipped) {
                        // Send OnShoutEquipped event
                        RE::SkyrimVM::GetSingleton()->SendAndRelayEvent(handle, &OnShoutEquippedEventName, eventArgs,
                                                                        nullptr);
                    } else {
                        // Send OnShoutUnequipped event
                        RE::SkyrimVM::GetSingleton()->SendAndRelayEvent(handle, &OnShoutUnequippedEventName, eventArgs,
                                                                        nullptr);
                    }
                }
            }
        }
    }

    // Let other code process the same event next
    return RE::BSEventNotifyControl::kContinue;
}
