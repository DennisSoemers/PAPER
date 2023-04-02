#include <OnContainerChangedEventHandler.h>
#include <SKSE/SKSE.h>

using namespace OnContainerChangedEvents;

static RE::BSFixedString OnBatchItemsAddedEventName = "OnBatchItemsAdded";
static RE::BSFixedString OnBatchItemsRemovedEventName = "OnBatchItemsRemoved";

namespace {
    inline const auto ItemsAddedRecord = _byteswap_ulong('IAEV');
    inline const auto ItemsRemovedRecord = _byteswap_ulong('IREV');
}


OnContainerChangedEventHandler& OnContainerChangedEventHandler::GetSingleton() noexcept {
    static OnContainerChangedEventHandler instance;
    return instance;
}

RE::BSEventNotifyControl OnContainerChangedEventHandler::ProcessEvent(
    const RE::TESContainerChangedEvent* a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>* a_eventSource) {

    if (a_event) {
        if (a_event->baseObj > 0) {
            if (a_event->oldContainer > 0) {
                {
                    std::lock_guard<std::mutex> lockGuard(batchedItemRemovedEventsMapMutex);

                    batchedItemRemovedEventsMap[a_event->oldContainer].emplace_back(a_event->newContainer,
                                                                                    a_event->baseObj, a_event->itemCount);

                    if (!haveQueuedUpTaskRemovedEvents) {
                        SKSE::GetTaskInterface()->AddTask([this]() {
                            auto vm = RE::SkyrimVM::GetSingleton();

                            if (vm) {
                                // Process all the item-removed events we've batched up
                                {
                                    std::lock_guard<std::mutex> lockGuard(batchedItemRemovedEventsMapMutex);

                                    for (auto& entry : batchedItemRemovedEventsMap) {
                                        auto oldContainer = RE::TESForm::LookupByID<RE::TESObjectREFR>(entry.first);

                                        if (oldContainer) {
                                            const auto handle = vm->handlePolicy.GetHandleForObject(
                                                static_cast<RE::VMTypeID>(RE::FormType::Reference), oldContainer);

                                            std::vector<RE::TESForm*> baseItems;
                                            std::vector<std::int32_t> itemCounts;
                                            std::vector<RE::TESObjectREFR*> destContainers;

                                            vm->InventoryEventFilterMapLock.Lock();

                                            RE::SkyrimVM::InventoryEventFilterLists* filterLists = nullptr;
                                            auto it = vm->InventoryEventFilterMap.find(handle);
                                            if (it != vm->InventoryEventFilterMap.end()) {
                                                filterLists = it->second;
                                            }

                                            for (auto& eventData : entry.second) {
                                                const RE::FormID baseObjID = eventData.baseObj;

                                                if (ItemPassesInventoryFilterLists(baseObjID, filterLists)) {
                                                    baseItems.emplace_back(RE::TESForm::LookupByID(baseObjID));
                                                    itemCounts.emplace_back(eventData.itemCount);
                                                    destContainers.emplace_back(
                                                        RE::TESForm::LookupByID<RE::TESObjectREFR>(
                                                            eventData.otherContainer));
                                                }
                                            }

                                            vm->InventoryEventFilterMapLock.Unlock();

                                            auto eventArgs = RE::MakeFunctionArguments(
                                                std::move(baseItems), std::move(itemCounts), std::move(destContainers));
                                            vm->SendAndRelayEvent(handle, &OnBatchItemsRemovedEventName, eventArgs, nullptr);
                                        }
                                    }

                                    haveQueuedUpTaskRemovedEvents = false;
                                    batchedItemRemovedEventsMap.clear();
                                }
                            }
                        });
                    }
                }
            }

            if (a_event->newContainer > 0) {
                {
                    std::lock_guard<std::mutex> lockGuard(batchedItemAddedEventsMapMutex);

                    batchedItemAddedEventsMap[a_event->newContainer].emplace_back(
                        a_event->oldContainer, a_event->baseObj, a_event->itemCount);

                    if (!haveQueuedUpTaskAddedEvents) {
                        SKSE::GetTaskInterface()->AddTask([this]() {
                            auto vm = RE::SkyrimVM::GetSingleton();

                            if (vm) {
                                // Process all the item-added events we've batched up
                                {
                                    std::lock_guard<std::mutex> lockGuard(batchedItemAddedEventsMapMutex);

                                    for (auto& entry : batchedItemAddedEventsMap) {
                                        auto newContainer = RE::TESForm::LookupByID<RE::TESObjectREFR>(entry.first);

                                        if (newContainer) {
                                            const auto handle = vm->handlePolicy.GetHandleForObject(
                                                static_cast<RE::VMTypeID>(RE::FormType::Reference), newContainer);

                                            std::vector<RE::TESForm*> baseItems;
                                            std::vector<std::int32_t> itemCounts;
                                            std::vector<RE::TESObjectREFR*> sourceContainers;

                                            vm->InventoryEventFilterMapLock.Lock();

                                            RE::SkyrimVM::InventoryEventFilterLists* filterLists = nullptr;
                                            auto it = vm->InventoryEventFilterMap.find(handle);
                                            if (it != vm->InventoryEventFilterMap.end()) {
                                                filterLists = it->second;
                                            }

                                            for (auto& eventData : entry.second) {
                                                const RE::FormID baseObjID = eventData.baseObj;

                                                if (ItemPassesInventoryFilterLists(baseObjID, filterLists)) {
                                                    baseItems.emplace_back(RE::TESForm::LookupByID(baseObjID));
                                                    itemCounts.emplace_back(eventData.itemCount);
                                                    sourceContainers.emplace_back(
                                                        RE::TESForm::LookupByID<RE::TESObjectREFR>(
                                                            eventData.otherContainer));
                                                }
                                            }

                                            vm->InventoryEventFilterMapLock.Unlock();

                                            auto eventArgs =
                                                RE::MakeFunctionArguments(std::move(baseItems), std::move(itemCounts),
                                                                          std::move(sourceContainers));
                                            vm->SendAndRelayEvent(handle, &OnBatchItemsAddedEventName, eventArgs,
                                                                  nullptr);
                                        }
                                    }

                                    haveQueuedUpTaskAddedEvents = false;
                                    batchedItemAddedEventsMap.clear();
                                }
                            }
                        });
                    }
                }
            }
        }
    }

    // Let other code process the same event next
    return RE::BSEventNotifyControl::kContinue;
}

bool OnContainerChangedEventHandler::ItemPassesInventoryFilterLists(
    const RE::FormID itemID, const RE::SkyrimVM::InventoryEventFilterLists* filterLists) const {

    if (!filterLists) {
        return true;
    }

    for (auto it = filterLists->itemsForFiltering.begin(); it != filterLists->itemsForFiltering.end(); ++it) {
        if ((*it) == itemID) {
            return true;
        }
    }

    for (auto it = filterLists->itemListsForFiltering.begin(); it != filterLists->itemListsForFiltering.end(); ++it) {
        auto formList = RE::TESForm::LookupByID<RE::BGSListForm>(*it);
        if (!formList) {
            logger::error("Expected form to be FormList: {}", *it);
            continue;
        }

        if (formList->HasForm(itemID)) {
            return true;
        }
    }

    return false;
}

void OnContainerChangedEventHandler::OnRevert(SKSE::SerializationInterface*) {
    auto& singleton = GetSingleton();

    { 
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemAddedEventsMapMutex); 
        singleton.batchedItemAddedEventsMap.clear();
    }

    {
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemRemovedEventsMapMutex);
        singleton.batchedItemRemovedEventsMap.clear();
    }
}

void OnContainerChangedEventHandler::OnGameLoaded(SKSE::SerializationInterface* serde) {
    std::uint32_t type;
    std::uint32_t size;
    std::uint32_t version;

    auto& singleton = GetSingleton();

    { 
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemAddedEventsMapMutex); 
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemRemovedEventsMapMutex);

        while (serde->GetNextRecordInfo(type, version, size)) {
            if (type == ItemsAddedRecord) {
                // First read how many items follow in this record, so we know how many times to iterate.
                std::size_t itemsAddedMapSize;
                serde->ReadRecordData(&itemsAddedMapSize, sizeof(itemsAddedMapSize));

                // Iterate over the remaining data in the record.
                for (; itemsAddedMapSize > 0; --itemsAddedMapSize) {
                    RE::FormID keyForm;
                    serde->ReadRecordData(&keyForm, sizeof(keyForm));
                    RE::FormID newKeyForm;
                    if (!serde->ResolveFormID(keyForm, newKeyForm)) {
                        logger::warn("Form ID {:X} could not be found after loading the save.", keyForm);
                    }

                    size_t vecSize;
                    serde->ReadRecordData(&vecSize, sizeof(vecSize));

                    for (int i = 0; i < vecSize; ++i) {
                        RE::FormID otherContainerForm;
                        serde->ReadRecordData(&otherContainerForm, sizeof(otherContainerForm));
                        RE::FormID newOtherContainerForm;
                        if (!serde->ResolveFormID(otherContainerForm, newOtherContainerForm)) {
                            logger::warn("Form ID {:X} could not be found after loading the save.", otherContainerForm);
                        }

                        RE::FormID baseObjForm;
                        serde->ReadRecordData(&baseObjForm, sizeof(baseObjForm));
                        RE::FormID newBaseObjForm;
                        if (!serde->ResolveFormID(baseObjForm, newBaseObjForm)) {
                            logger::warn("Form ID {:X} could not be found after loading the save.", baseObjForm);
                        }

                        std::int32_t itemCount;
                        serde->ReadRecordData(&itemCount, sizeof(itemCount));

                        singleton.batchedItemAddedEventsMap[keyForm].emplace_back(otherContainerForm, baseObjForm,
                                                                                  itemCount);
                    }
                }
            } else if (type == ItemsRemovedRecord) {
                // TODO implement this one
            } else {
                logger::warn("Unknown record type in cosave.");
                __assume(false);
            }
        }
    }

    // TODO if we loaded any data, we also need to queue up task to send the events
}

void OnContainerChangedEventHandler::OnGameSaved(SKSE::SerializationInterface* serde) {
    auto& singleton = GetSingleton();

    { 
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemAddedEventsMapMutex); 

        if (!serde->OpenRecord(ItemsAddedRecord, 0)) {
            logger::error("Unable to open record to write cosave data.");
            return;
        }

        auto itemsAddedMapSize = singleton.batchedItemAddedEventsMap.size();
        serde->WriteRecordData(&itemsAddedMapSize, sizeof(itemsAddedMapSize));
        for (auto& entry : singleton.batchedItemAddedEventsMap) {
            serde->WriteRecordData(&entry.first, sizeof(entry.first));

            auto& vec = entry.second;
            auto vecSize = vec.size();
            serde->WriteRecordData(&vecSize, sizeof(vecSize));

            for (auto& vectorEntry : vec) {
                serde->WriteRecordData(&(vectorEntry.otherContainer), sizeof(vectorEntry.otherContainer));
                serde->WriteRecordData(&(vectorEntry.baseObj), sizeof(vectorEntry.baseObj));
                serde->WriteRecordData(&(vectorEntry.itemCount), sizeof(vectorEntry.itemCount));
            }
        }
    }

    {
        std::lock_guard<std::mutex> lockGuard(singleton.batchedItemRemovedEventsMapMutex);

        if (!serde->OpenRecord(ItemsRemovedRecord, 0)) {
            logger::error("Unable to open record to write cosave data.");
            return;
        }

        auto itemsRemovedMapSize = singleton.batchedItemRemovedEventsMap.size();
        serde->WriteRecordData(&itemsRemovedMapSize, sizeof(itemsRemovedMapSize));
        for (auto& entry : singleton.batchedItemRemovedEventsMap) {
            serde->WriteRecordData(&entry.first, sizeof(entry.first));
            
            auto& vec = entry.second;
            auto vecSize = vec.size();
            serde->WriteRecordData(&vecSize, sizeof(vecSize));

            for (auto& vectorEntry : vec) {
                serde->WriteRecordData(&(vectorEntry.otherContainer), sizeof(vectorEntry.otherContainer));
                serde->WriteRecordData(&(vectorEntry.baseObj), sizeof(vectorEntry.baseObj));
                serde->WriteRecordData(&(vectorEntry.itemCount), sizeof(vectorEntry.itemCount));
            }
        }
    }
}
