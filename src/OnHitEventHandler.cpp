#include <SKSE/SKSE.h>
#include <OnHitEventHandler.h>

using namespace RE;
using namespace OnHitEvents;
using namespace SKSE;

OnHitEventHandler& OnHitEventHandler::GetSingleton() noexcept {
    static OnHitEventHandler instance;
    return instance;
}

RE::BSEventNotifyControl OnHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event,
                                                         RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) {
    const auto target = a_event->target.get();
    if (target) {
        const auto applicationRuntime = RE::GetDurationOfApplicationRunTime();

        bool skipEvent = false;
        bool clearRecentHits = true;
        for (const auto& recentHit : recentHits) {
            if (recentHit.applicationRuntime == applicationRuntime) {
                // We're probably still in the same frame (or close enough), so don't clear data yet
                clearRecentHits = false;

                if (recentHit.cause == a_event->cause.get() && recentHit.target == target) {
                    // Already processed hit for same cause+target too recently, so skip
                    skipEvent = true;
                    break;
                }
            }
        }

        if (clearRecentHits) {
            recentHits.clear();
        }

        if (!skipEvent) {
            // Now the actual processing of the event
            auto targetFormType = target->GetFormType();

            if (targetFormType == RE::FormType::ActorCharacter) {
                auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

                if (vm) {
                    auto policy = vm->GetObjectHandlePolicy();

                    if (policy) {
                        const auto handle =
                            policy->GetHandleForObject(static_cast<RE::VMTypeID>(targetFormType), target);

                        if (handle != policy->EmptyHandle()) {
                            const auto aggressor = a_event->cause.get();
                            const auto source = RE::TESForm::LookupByID(a_event->source);
                            const auto projectile = RE::TESForm::LookupByID<RE::BGSProjectile>(a_event->projectile);

                            const auto powerAttack = a_event->flags.any(RE::TESHitEvent::Flag::kPowerAttack);
                            const auto sneakAttack = a_event->flags.any(RE::TESHitEvent::Flag::kSneakAttack);
                            const auto bashAttack = a_event->flags.any(RE::TESHitEvent::Flag::kBashAttack);
                            const auto hitBlocked = a_event->flags.any(RE::TESHitEvent::Flag::kHitBlocked);

                            auto eventArgs = RE::MakeFunctionArguments(
                                (TESObjectREFR*)aggressor, (TESForm*)source, (BGSProjectile*)projectile,
                                (bool)powerAttack, (bool)sneakAttack, (bool)bashAttack, (bool)hitBlocked);

                            bool impact = false;

                            if (source) {
                                auto sourceFormType = source->GetFormType();

                                // Enchantments (and maybe poisons/potions/ingredients) on weapons can
                                // cause multiple events to trigger at the same time. We'll only consider
                                // the actual weapon hit to be an impact
                                //
                                // NOTE: I don't think any of these checks actually ever trigger. At least
                                // not for enchantments, the source is always still the weapon. That's why
                                // we have the additional checks to stop multiple events within the same frame.
                                // Won't hurt to also have these FormType tests here though, just in case they
                                // ever do happen to work.
                                if (sourceFormType != RE::FormType::Ingredient &&
                                    sourceFormType != RE::FormType::AlchemyItem &&
                                    sourceFormType != RE::FormType::Enchantment) {

                                    if (sourceFormType == RE::FormType::Spell) {
                                        const auto sourceSpell = source->As<RE::SpellItem>();
                                        if (sourceSpell) {
                                            if (sourceSpell->GetCastingType() !=
                                                RE::MagicSystem::CastingType::kConcentration) {
                                                // Concentration spells will not count as impacts
                                                if (sourceSpell->hostileCount > 0) {
                                                    // Only hostile spells count as impacts
                                                    auto const delivery = sourceSpell->GetDelivery();
                                                    if (delivery != RE::MagicSystem::Delivery::kTouch &&
                                                        delivery != RE::MagicSystem::Delivery::kSelf) {
                                                        // Touch and self spells do not count as impacts
                                                        impact = true;
                                                    }
                                                }
                                            }
                                        }
                                    } else {
                                        impact = true;
                                    }
                                }
                            } else if (bashAttack) {
                                // Note: need to treat this case separately, because source is sometimes a nullptr
                                // even when bashing (e.g., when bashing with a torch).
                                // (see: https://www.creationkit.com/index.php?title=OnHit_-_ObjectReference)
                                impact = true;
                            } else if (projectile) {
                                // Projectile is actually usually nullptr when we wouldn't expect it to be, but
                                // sometimes it is there---and when it is there, it often means that source is
                                // null (i.e., a projectile that was not fired by a weapon or spell)
                                impact = true;
                            }

                            if (impact) {
                                // Memorise the hit data for this frame
                                recentHits.emplace_back(target, a_event->cause.get(), applicationRuntime);

                                // Send the OnImpact event
                                vm->SendEvent(handle, "OnImpact", eventArgs);

                                // Also send events to any active magic effects
                                auto magicTarget = target->GetMagicTarget();
                                if (magicTarget) {
                                    auto activeEffects = magicTarget->GetActiveEffectList();

                                    if (activeEffects) {
                                        for (const auto& activeEffect : *activeEffects) {
                                            if (activeEffect) {
                                                const auto mgefHandle = policy->GetHandleForObject(
                                                    static_cast<RE::VMTypeID>(RE::ActiveEffect::VMTYPEID),
                                                    activeEffect);

                                                if (mgefHandle != policy->EmptyHandle()) {
                                                    if (impact) {
                                                        vm->SendEvent(mgefHandle, "OnImpact", eventArgs);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Let other code process the same event next
    return RE::BSEventNotifyControl::kContinue;
}
