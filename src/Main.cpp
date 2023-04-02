#include <OnContainerChangedEventHandler.h>
#include <OnEquipEventHandler.h>
#include <OnHitEventHandler.h>
#include <Papyrus.h>

#include <stddef.h>

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {
    /**
     * Setup logging.
     */
    void InitializeLogging() {
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }

#ifndef NDEBUG
        const auto level = spdlog::level::trace;
#else
        const auto level = spdlog::level::info;
#endif

        log->set_level(level);
        log->flush_on(level);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    }

   /**
    * Initialize the event sink, which lets us respond to events
    * sent by the game engine.
    */
    void InitializeEventSink() {
        log::trace("Initializing event sink...");
        auto scriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
        if (scriptEventSource) {
            scriptEventSource->AddEventSink(&OnEquipEvents::OnEquipEventHandler::GetSingleton());
            scriptEventSource->AddEventSink(&OnHitEvents::OnHitEventHandler::GetSingleton());
            scriptEventSource->AddEventSink(&OnContainerChangedEvents::OnContainerChangedEventHandler::GetSingleton());
            log::trace("Event sink initialized.");
        } else {
            stl::report_and_fail("Failed to initialize event sink.");
        }
    }

    /** 
     * Register new Papyrus functions.
     */
    void InitializePapyrus() { 
        log::trace("Initializing Papyrus bindings...");
        if (GetPapyrusInterface()->Register(PAPER::Bind)) {
            log::debug("Papyrus functions bound.");
        } else {
            stl::report_and_fail("Failure to register Papyrus bindings.");
        }
    }

    /**
     * Initialize serialization.
     */
    void InitializeSerialization() {
        log::trace("Initializing cosave serialization...");
        auto* serde = GetSerializationInterface();
        serde->SetUniqueID(_byteswap_ulong('BPAP'));
        serde->SetSaveCallback(OnContainerChangedEvents::OnContainerChangedEventHandler::OnGameSaved);
        serde->SetRevertCallback(OnContainerChangedEvents::OnContainerChangedEventHandler::OnRevert);
        serde->SetLoadCallback(OnContainerChangedEvents::OnContainerChangedEventHandler::OnGameLoaded);
        log::trace("Cosave serialization initialized.");
    }

}

/**
 * This is the main callback for initializing the SKSE plugin, called just before Skyrim runs its main function.
 */
SKSEPluginLoad(const LoadInterface* skse) {
    InitializeLogging();

    auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    log::info("{} {} is loading...", plugin->GetName(), version);

    Init(skse);
    InitializeEventSink();
    InitializeSerialization();
    InitializePapyrus();

    log::info("{} has finished loading.", plugin->GetName());
    return true;
}
