#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <mdr/Headphones.hpp>
#include <mdr/ProtocolV2.hpp>
#include <mdr-c/Base.h>
#include <mdr-c/Connection.h>
#include <mdr-c/Platform/PlatformWindows.h>
#include <mdr-c/Platform/PlatformWindowsBLE.h>

#include <Windows.h>
#include <shellapi.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;
using namespace std::chrono_literals;

namespace
{
constexpr char kBridgeVersion[] = "0.3.34";

enum class Transport
{
    Classic,
    Ble,
};

const char* TransportName(Transport transport)
{
    return transport == Transport::Ble ? "BLE" : "Classic";
}

std::string Trim(std::string value)
{
    const auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

std::string Lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

std::string SafeIni(std::string value)
{
    std::replace(value.begin(), value.end(), '\r', ' ');
    std::replace(value.begin(), value.end(), '\n', ' ');
    return value;
}

std::string Bool(bool value)
{
    return value ? "1" : "0";
}

void SendMediaKey(WORD virtualKey)
{
    std::array<INPUT, 2> inputs{};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = virtualKey;
    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

template <typename T>
T Clamp(T value, T low, T high)
{
    return std::min(high, std::max(low, value));
}

class Logger
{
public:
    explicit Logger(fs::path path) : path_(std::move(path)) {}

    void Write(std::string_view message) const
    {
        std::ofstream stream(path_, std::ios::app);
        if (!stream)
            return;
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm local{};
        localtime_s(&local, &now);
        char timestamp[32]{};
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local);
        stream << '[' << timestamp << "] " << message << '\n';
    }

private:
    fs::path path_;
};

struct Settings
{
    std::string deviceMac = "auto";
    std::string connectionMode = "classic";
    int refreshSeconds = 0;
};

Settings ReadSettings(const fs::path& path)
{
    Settings result;
    std::ifstream stream(path);
    std::string line;
    while (std::getline(stream, line))
    {
        line = Trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[')
            continue;
        const auto split = line.find('=');
        if (split == std::string::npos)
            continue;
        const auto key = Lower(Trim(line.substr(0, split)));
        const auto value = Trim(line.substr(split + 1));
        if (key == "devicemac")
            result.deviceMac = value;
        else if (key == "connectionmode")
        {
            const auto mode = Lower(value);
            if (mode == "ble")
                result.connectionMode = "ble";
            else if (mode == "auto" || mode == "classic")
                result.connectionMode = "classic";
        }
        else if (key == "refreshseconds")
        {
            try
            {
                const int seconds = std::stoi(value);
                result.refreshSeconds = seconds >= 300 ? Clamp(seconds, 300, 3600) : 0;
            }
            catch (...)
            {
            }
        }
    }
    return result;
}

const char* PlaybackText(mdr::v2::t1::PlaybackStatus value)
{
    using enum mdr::v2::t1::PlaybackStatus;
    switch (value)
    {
    case PLAY: return "playing";
    case PAUSE: return "paused";
    case STOP: return "stopped";
    default: return "unknown";
    }
}

const char* AncText(const mdr::MDRHeadphones& device)
{
    using enum mdr::v2::t1::NcAsmMode;
    if (!device.mNcAsmEnabled.current)
        return "off";
    return device.mNcAsmMode.current == ASM ? "ambient" : "noise_cancelling";
}

const char* PriorityText(mdr::v2::t1::PriorMode value)
{
    using enum mdr::v2::t1::PriorMode;
    switch (value)
    {
    case SOUND_QUALITY_PRIOR: return "Sound quality";
    case CONNECTION_QUALITY_PRIOR: return "Stable connection";
    case LOW_LATENCY_PRIOR_BETA: return "Low latency";
    default: return "Unknown";
    }
}

const char* AutoOffText(mdr::v2::t1::AutoPowerOffElements value)
{
    using enum mdr::v2::t1::AutoPowerOffElements;
    switch (value)
    {
    case POWER_OFF_IN_5_MIN: return "5 minutes";
    case POWER_OFF_IN_15_MIN: return "15 minutes";
    case POWER_OFF_IN_30_MIN: return "30 minutes";
    case POWER_OFF_IN_60_MIN: return "60 minutes";
    case POWER_OFF_IN_180_MIN: return "180 minutes";
    case POWER_OFF_DISABLE: return "Off";
    default: return "Unknown";
    }
}

const char* EqPresetText(mdr::v2::t1::EqPresetId value)
{
    using enum mdr::v2::t1::EqPresetId;
    switch (value)
    {
    case OFF: return "Off";
    case ROCK: return "Rock";
    case POP: return "Pop";
    case JAZZ: return "Jazz";
    case DANCE: return "Dance";
    case EDM: return "EDM";
    case R_AND_B_HIP_HOP: return "R&B / Hip-Hop";
    case ACOUSTIC: return "Acoustic";
    case BRIGHT: return "Bright";
    case EXCITED: return "Excited";
    case MELLOW: return "Mellow";
    case RELAXED: return "Relaxed";
    case VOCAL: return "Vocal";
    case TREBLE: return "Treble boost";
    case BASS: return "Bass boost";
    case SPEECH: return "Speech";
    case CUSTOM: return "Custom";
    case USER_SETTING1: return "Custom 1";
    case USER_SETTING2: return "Custom 2";
    case USER_SETTING3: return "Custom 3";
    default: return "Custom";
    }
}

const char* TouchPresetText(mdr::v2::t1::Preset value)
{
    using enum mdr::v2::t1::Preset;
    switch (value)
    {
    case PLAYBACK_CONTROL: return "Playback";
    case AMBIENT_SOUND_CONTROL_QUICK_ACCESS: return "Ambient + Quick Access";
    case NO_FUNCTION: return "No function";
    default: return "Other";
    }
}

const char* ButtonFunctionText(mdr::v2::t1::Function value)
{
    using enum mdr::v2::t1::Function;
    switch (value)
    {
    case NC_ASM_OFF: return "ANC / Ambient / Off";
    case NC_ASM: return "ANC / Ambient";
    case NC_OFF: return "ANC / Off";
    case ASM_OFF: return "Ambient / Off";
    case NO_FUNCTION: return "No function";
    default: return "Other";
    }
}

bool SupportsNoiseCancelling(const mdr::MDRHeadphones& device)
{
    using F = mdr::v2::MessageMdrV2FunctionType_Table1;
    const auto has = [&](F value) { return device.mSupport.contains(value); };
    return has(F::NOISE_CANCELLING_ONOFF)
        || has(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || has(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || has(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
}

bool SupportsAmbient(const mdr::MDRHeadphones& device)
{
    using F = mdr::v2::MessageMdrV2FunctionType_Table1;
    const auto has = [&](F value) { return device.mSupport.contains(value); };
    return has(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || has(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || has(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::AMBIENT_SOUND_MODE_ONOFF)
        || has(F::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::AMBIENT_SOUND_CONTROL_MODE_SELECT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || has(F::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || has(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
}

struct GeneralSettingRef
{
    const mdr::MDRHeadphones::GsCapability* capability{};
    mdr::MDRProperty<bool>* property{};
};

std::optional<GeneralSettingRef> FindGeneralSetting(mdr::MDRHeadphones& device, std::string_view subject)
{
    const std::array<std::pair<mdr::MDRHeadphones::GsCapability*, mdr::MDRProperty<bool>*>, 4> settings{{
        {&device.mGsCapability1, &device.mGsParamBool1},
        {&device.mGsCapability2, &device.mGsParamBool2},
        {&device.mGsCapability3, &device.mGsParamBool3},
        {&device.mGsCapability4, &device.mGsParamBool4},
    }};
    for (const auto& [capability, property] : settings)
    {
        if (capability->value.subject.value == subject)
            return GeneralSettingRef{capability, property};
    }
    return std::nullopt;
}

class Bridge
{
public:
    explicit Bridge(fs::path dataDirectory)
        : dataDirectory_(std::move(dataDirectory)),
          queueDirectory_(dataDirectory_ / "Queue"),
          logger_(dataDirectory_ / "bridge.log")
    {
        fs::create_directories(queueDirectory_);
        settings_ = ReadSettings(dataDirectory_ / "settings.ini");
        nextConnectAttempt_ = Clock::now();
        nextStateWrite_ = Clock::now();
        nextSync_ = settings_.refreshSeconds > 0
                        ? Clock::now() + std::chrono::seconds(settings_.refreshSeconds)
                        : Clock::time_point::max();
    }

    ~Bridge()
    {
        Disconnect();
    }

    int Run()
    {
        logger_.Write(std::string("SonyXM5Bridge ") + kBridgeVersion + " started");
        status_ = "searching";
        statusText_ = "Looking for WH-1000XM5";
        WriteState();

        while (running_)
        {
            ProcessCommandQueue();
            TickConnection();
            if (Clock::now() >= nextStateWrite_)
            {
                WriteState();
                nextStateWrite_ = Clock::now() + 100ms;
            }
            std::this_thread::sleep_for(20ms);
        }

        status_ = "stopped";
        statusText_ = "Bridge stopped";
        WriteState();
        logger_.Write("Bridge stopped");
        return 0;
    }

private:
    void TickConnection()
    {
        if (!connection_ && Clock::now() >= nextConnectAttempt_)
        {
            BeginConnect();
            return;
        }

        if (connecting_)
        {
            const int result = mdrConnectionPoll(connection_, 0);
            if (result == MDR_RESULT_OK)
            {
                connecting_ = false;
                device_ = std::make_unique<mdr::MDRHeadphones>(connection_);
                status_ = recovering_ ? "recovering" : "syncing";
                statusText_ = recovering_ ? "Restoring headphone controls" : "Syncing controls";
                error_.clear();
                try
                {
                    if (device_->Invoke(device_->RequestInitV2()) != MDR_RESULT_OK)
                        throw std::runtime_error("Could not start headphone initialization");
                }
                catch (const std::exception& exception)
                {
                    FailConnection(exception.what());
                }
                return;
            }
            if (result != MDR_RESULT_ERROR_TIMEOUT && result != MDR_RESULT_INPROGRESS)
            {
                FailConnection(mdrConnectionGetLastError(connection_));
                return;
            }
            if (Clock::now() - connectStarted_ > 15s)
                FailConnection("Timed out opening the Sony control service");
            return;
        }

        if (!device_)
            return;

        try
        {
            const int event = device_->PollEvents();
            if (event == MDR_HEADPHONES_ERROR)
            {
                std::string message = SafeIni(Trim(device_->GetLastError()));
                if (message.empty())
                    message = "Sony control link stopped responding";
                if (!pollErrorSince_)
                {
                    pollErrorSince_ = Clock::now();
                    ++pollErrorCount_;
                    lastPollError_ = message;
                    logger_.Write("Transient poll error; holding connection: " + message);
                }
                if (Clock::now() - *pollErrorSince_ >= 1500ms)
                    FailConnection(lastPollError_);
                return;
            }
            pollErrorSince_.reset();
            lastPollError_.clear();
            if (event == MDR_HEADPHONES_TASK_INIT_OK)
            {
                if (device_->Invoke(device_->RequestSyncV2()) != MDR_RESULT_OK)
                    throw std::runtime_error("Could not request headphone state");
                status_ = recovering_ ? "recovering" : "syncing";
                statusText_ = recovering_ ? "Restoring headphone controls" : "Reading battery and settings";
            }
            else if (event == MDR_HEADPHONES_TASK_SYNC_OK)
            {
                if (!linkActive_)
                {
                    if (hasConnected_)
                        ++reconnectCount_;
                    linkActive_ = true;
                    connectedSince_ = Clock::now();
                    lastConnectLatencyMs_ = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                        connectedSince_ - connectStarted_).count());
                }
                status_ = "connected";
                statusText_ = std::string("Connected via ") + TransportName(transport_);
                error_.clear();
                hasConnected_ = true;
                recovering_ = false;
                recoveryStarted_.reset();
                nextSync_ = settings_.refreshSeconds > 0
                                ? Clock::now() + std::chrono::seconds(settings_.refreshSeconds)
                                : Clock::time_point::max();
            }

            if (device_->IsReady())
            {
                if (device_->IsDirty())
                {
                    if (device_->Invoke(device_->RequestCommitV2()) != MDR_RESULT_OK)
                        throw std::runtime_error("Could not send control change");
                }
                else if (syncRequested_ || (settings_.refreshSeconds > 0 && Clock::now() >= nextSync_))
                {
                    syncRequested_ = false;
                    const int result = device_->Invoke(device_->RequestSyncV2());
                    if (result == MDR_RESULT_OK)
                        nextSync_ = settings_.refreshSeconds > 0
                                        ? Clock::now() + std::chrono::seconds(settings_.refreshSeconds)
                                        : Clock::time_point::max();
                    else
                    {
                        nextSync_ = Clock::now() + 10s;
                        logger_.Write("State refresh deferred after a transient send failure");
                    }
                }
            }
        }
        catch (const std::exception& exception)
        {
            FailConnection(exception.what());
        }
    }

    void BeginConnect()
    {
        Disconnect();
        settings_ = ReadSettings(dataDirectory_ / "settings.ini");

        transport_ = settings_.connectionMode == "ble" ? Transport::Ble : Transport::Classic;

        if (transport_ == Transport::Ble)
        {
            bleBackend_ = mdrConnectionWindowsBLECreate();
            if (bleBackend_)
                connection_ = mdrConnectionWindowsBLEGet(bleBackend_);
        }
        else
        {
            backend_ = mdrConnectionWindowsCreate();
            if (backend_)
                connection_ = mdrConnectionWindowsGet(backend_);
        }

        if (!connection_)
        {
            FailConnection(std::string("Could not initialize the ") + TransportName(transport_) + " Bluetooth backend");
            return;
        }

        std::string selectedMac;
        selectedDeviceName_ = "WH-1000XM5";
        if (recovering_ && !selectedMac_.empty())
        {
            selectedMac = selectedMac_;
        }
        else if (!settings_.deviceMac.empty() && Lower(settings_.deviceMac) != "auto")
        {
            selectedMac = settings_.deviceMac;
        }
        else
        {
            MDRDeviceInfo* devices = nullptr;
            int count = 0;
            const int listResult = mdrConnectionGetDevicesList(connection_, &devices, &count);
            if (listResult == MDR_RESULT_OK && count > 0)
            {
                int selected = -1;
                for (int pass = 0; pass < 3 && selected < 0; ++pass)
                {
                    for (int index = 0; index < count; ++index)
                    {
                        const std::string name = Lower(devices[index].szDeviceName);
                        const bool match = pass == 0 ? name.find("wh-1000xm5") != std::string::npos
                                         : pass == 1 ? name.find("wh-1000xm") != std::string::npos
                                                     : name.starts_with("wh-") || name.starts_with("wf-");
                        if (match)
                        {
                            selected = index;
                            break;
                        }
                    }
                }
                if (selected >= 0)
                {
                    selectedMac = devices[selected].szDeviceMacAddress;
                    selectedDeviceName_ = devices[selected].szDeviceName;
                }
            }
            if (devices)
                mdrConnectionFreeDevicesList(connection_, &devices);
        }

        if (selectedMac.empty())
        {
            FailConnection(std::string("No WH-1000XM5 was found over ") + TransportName(transport_));
            return;
        }

        selectedMac_ = selectedMac;
        status_ = recovering_ ? "recovering" : "connecting";
        statusText_ = recovering_ ? "Reconnecting headphone controls"
                                  : std::string("Opening Sony controls via ") + TransportName(transport_);
        error_.clear();
        const char* serviceUuid = transport_ == Transport::Ble
                                    ? MDR_BLE_SERVICE_UUID_TANDEM_OVER_BLE_HPC
                                    : MDR_SERVICE_UUID_XM5;
        const int result = mdrConnectionConnect(connection_, selectedMac.c_str(), serviceUuid);
        ++connectionAttemptCount_;
        if (result != MDR_RESULT_OK && result != MDR_RESULT_INPROGRESS)
        {
            FailConnection(mdrConnectionGetLastError(connection_));
            return;
        }
        connecting_ = true;
        connectStarted_ = Clock::now();
        logger_.Write(std::string("Connecting via ") + TransportName(transport_) + " to " + selectedDeviceName_ + " at " + selectedMac_);
    }

    void FailConnection(std::string message)
    {
        message = SafeIni(Trim(std::move(message)));
        if (message.empty())
            message = "Unknown Bluetooth control error";

        const auto lower = Lower(message);
        if (transport_ == Transport::Classic && lower.find("only one usage") != std::string::npos)
            message = "Another app is using the Classic Sony control link";

        const std::string labelledError = std::string(TransportName(transport_)) + ": " + message;
        lastDisconnectReason_ = labelledError;
        logger_.Write("Connection failed: " + labelledError);
        Disconnect();

        const auto now = Clock::now();
        if (hasConnected_ && !recoveryStarted_)
            recoveryStarted_ = now;
        recovering_ = hasConnected_ && recoveryStarted_ && now - *recoveryStarted_ < 30s;
        error_ = recovering_ ? "" : labelledError;
        status_ = recovering_ ? "recovering" : "disconnected";
        statusText_ = recovering_ ? "Reconnecting headphone controls" : "Headphone controls unavailable";
        transport_ = settings_.connectionMode == "ble" ? Transport::Ble : Transport::Classic;
        nextConnectAttempt_ = now + (recovering_ ? 500ms : 8s);
    }

    void Disconnect()
    {
        device_.reset();
        if (connection_)
            mdrConnectionDisconnect(connection_);
        connection_ = nullptr;
        if (backend_)
            mdrConnectionWindowsDestroy(backend_);
        backend_ = nullptr;
        if (bleBackend_)
            mdrConnectionWindowsBLEDestroy(bleBackend_);
        bleBackend_ = nullptr;
        connecting_ = false;
        linkActive_ = false;
        pollErrorSince_.reset();
        lastPollError_.clear();
    }

    void ProcessCommandQueue()
    {
        std::vector<fs::path> files;
        std::error_code error;
        for (const auto& entry : fs::directory_iterator(queueDirectory_, error))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".cmd")
                files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end());
        for (const auto& path : files)
        {
            std::error_code timestampError;
            const auto writtenAt = fs::last_write_time(path, timestampError);
            if (!timestampError)
            {
                const auto queueDelay = std::chrono::duration_cast<std::chrono::milliseconds>(
                    fs::file_time_type::clock::now() - writtenAt).count();
                lastCommandLatencyMs_ = static_cast<int>(Clamp<long long>(queueDelay, 0, 60000));
            }
            std::ifstream stream(path);
            std::string command;
            std::getline(stream, command);
            stream.close();
            if (!Trim(command).empty())
                ExecuteCommand(Trim(command));
            fs::remove(path, error);
        }
    }

    void ExecuteCommand(const std::string& command)
    {
        lastCommand_ = command;
        logger_.Write("Command: " + command);
        if (command == "shutdown")
        {
            running_ = false;
            return;
        }
        if (command == "reconnect")
        {
            Disconnect();
            nextConnectAttempt_ = Clock::now();
            status_ = "searching";
            statusText_ = "Looking for WH-1000XM5";
            return;
        }
        if (command == "refresh")
        {
            syncRequested_ = true;
            return;
        }

        std::istringstream input(command);
        std::string action;
        input >> action;

        // Playback remains useful even when the private Sony control service
        // is busy, so route it through Windows' media keys.
        if (action == "play-pause")
        {
            SendMediaKey(VK_MEDIA_PLAY_PAUSE);
            return;
        }
        if (action == "next")
        {
            SendMediaKey(VK_MEDIA_NEXT_TRACK);
            return;
        }
        if (action == "previous")
        {
            SendMediaKey(VK_MEDIA_PREV_TRACK);
            return;
        }
        if (action == "volume-delta")
        {
            int delta = 0;
            input >> delta;
            const WORD key = delta < 0 ? VK_VOLUME_DOWN : VK_VOLUME_UP;
            for (int index = 0; index < Clamp(std::abs(delta), 1, 10); ++index)
                SendMediaKey(key);
            return;
        }
        if (action == "volume-step")
        {
            int delta = 0;
            input >> delta;
            delta = Clamp(delta, -10, 10);
            if (delta == 0)
                return;

            // Prefer Sony's absolute-volume property while the private control
            // link is ready. Because desired is updated immediately, multiple
            // queued clicks accumulate before a single commit instead of all
            // resolving from the same stale current value.
            if (device_ && device_->IsReady())
            {
                device_->mPlayVolume.desired = Clamp(device_->mPlayVolume.desired + delta, 0, 30);
                return;
            }

            // Volume should still work during a brief control-link recovery.
            const WORD key = delta < 0 ? VK_VOLUME_DOWN : VK_VOLUME_UP;
            for (int index = 0; index < std::abs(delta); ++index)
                SendMediaKey(key);
            return;
        }

        if (!device_ || status_ == "connecting")
        {
            error_ = "The control is available after the headphones connect.";
            return;
        }

        auto& device = *device_;
        using namespace mdr::v2::t1;

        if (action == "volume")
        {
            int value = device.mPlayVolume.desired;
            input >> value;
            device.mPlayVolume.desired = Clamp(value, 0, 30);
        }
        else if (action == "anc")
        {
            std::string mode;
            input >> mode;
            if (mode == "noise")
            {
                device.mNcAsmEnabled.desired = true;
                device.mNcAsmMode.desired = NcAsmMode::NC;
            }
            else if (mode == "ambient")
            {
                device.mNcAsmEnabled.desired = true;
                device.mNcAsmMode.desired = NcAsmMode::ASM;
                if (device.mNcAsmAmbientLevel.desired == 0)
                    device.mNcAsmAmbientLevel.desired = 20;
            }
            else if (mode == "off")
                device.mNcAsmEnabled.desired = false;
        }
        else if (action == "ambient")
        {
            int value = device.mNcAsmAmbientLevel.desired;
            input >> value;
            device.mNcAsmAmbientLevel.desired = Clamp(value, 1, 20);
        }
        else if (action == "focus-voice")
            device.mNcAsmFocusOnVoice.desired = !device.mNcAsmFocusOnVoice.desired;
        else if (action == "speak-to-chat")
            device.mSpeakToChatEnabled.desired = !device.mSpeakToChatEnabled.desired;
        else if (action == "dsee")
            device.mUpscalingEnabled.desired = !device.mUpscalingEnabled.desired;
        else if (action == "auto-pause")
            device.mAutoPauseEnabled.desired = !device.mAutoPauseEnabled.desired;
        else if (action == "touch-panel")
        {
            if (const auto setting = FindGeneralSetting(device, "TOUCH_PANEL_SETTING"))
                setting->property->desired = !setting->property->desired;
        }
        else if (action == "multipoint")
        {
            if (const auto setting = FindGeneralSetting(device, "MULTIPOINT_SETTING"))
                setting->property->desired = !setting->property->desired;
        }
        else if (action == "eq-next" || action == "eq-previous")
        {
            static constexpr std::array presets{
                EqPresetId::OFF, EqPresetId::BRIGHT, EqPresetId::EXCITED, EqPresetId::MELLOW,
                EqPresetId::RELAXED, EqPresetId::VOCAL, EqPresetId::TREBLE, EqPresetId::BASS,
                EqPresetId::SPEECH, EqPresetId::CUSTOM, EqPresetId::USER_SETTING1, EqPresetId::USER_SETTING2,
            };
            auto found = std::find(presets.begin(), presets.end(), device.mEqPresetId.desired);
            std::size_t index = found == presets.end() ? 0 : static_cast<std::size_t>(std::distance(presets.begin(), found));
            if (action == "eq-next")
                index = (index + 1) % presets.size();
            else
                index = (index + presets.size() - 1) % presets.size();
            device.mEqPresetId.desired = presets[index];
        }
        else if (action == "eq-bass-delta")
        {
            int delta = 0;
            input >> delta;
            device.mEqClearBass.desired = Clamp(device.mEqClearBass.desired + delta, -10, 10);
        }
        else if (action == "eq-band-delta")
        {
            int band = 0;
            int delta = 0;
            input >> band >> delta;
            if (band >= 0 && static_cast<std::size_t>(band) < device.mEqConfig.desired.size())
                device.mEqConfig.desired[band] = Clamp(device.mEqConfig.desired[band] + delta, -10, 10);
        }
        else if (action == "priority")
        {
            device.mAudioPriorityMode.desired = device.mAudioPriorityMode.desired == PriorMode::SOUND_QUALITY_PRIOR
                                                    ? PriorMode::CONNECTION_QUALITY_PRIOR
                                                    : PriorMode::SOUND_QUALITY_PRIOR;
        }
        else if (action == "auto-off-next")
        {
            static constexpr std::array values{
                AutoPowerOffElements::POWER_OFF_DISABLE,
                AutoPowerOffElements::POWER_OFF_IN_5_MIN,
                AutoPowerOffElements::POWER_OFF_IN_15_MIN,
                AutoPowerOffElements::POWER_OFF_IN_30_MIN,
                AutoPowerOffElements::POWER_OFF_IN_60_MIN,
                AutoPowerOffElements::POWER_OFF_IN_180_MIN,
            };
            auto found = std::find(values.begin(), values.end(), device.mPowerAutoOff.desired);
            const std::size_t index = found == values.end() ? 0 : (static_cast<std::size_t>(std::distance(values.begin(), found)) + 1) % values.size();
            device.mPowerAutoOff.desired = values[index];
        }
        else if (action == "button-next")
        {
            static constexpr std::array values{
                Function::NC_ASM_OFF, Function::NC_ASM, Function::NC_OFF, Function::ASM_OFF, Function::NO_FUNCTION,
            };
            auto found = std::find(values.begin(), values.end(), device.mNcAsmButtonFunction.desired);
            const std::size_t index = found == values.end() ? 0 : (static_cast<std::size_t>(std::distance(values.begin(), found)) + 1) % values.size();
            device.mNcAsmButtonFunction.desired = values[index];
        }
        else if (action == "touch-left-next" || action == "touch-right-next")
        {
            static constexpr std::array values{Preset::PLAYBACK_CONTROL, Preset::AMBIENT_SOUND_CONTROL_QUICK_ACCESS, Preset::NO_FUNCTION};
            auto& property = action == "touch-left-next" ? device.mTouchFunctionLeft : device.mTouchFunctionRight;
            auto found = std::find(values.begin(), values.end(), property.desired);
            const std::size_t index = found == values.end() ? 0 : (static_cast<std::size_t>(std::distance(values.begin(), found)) + 1) % values.size();
            property.desired = values[index];
        }
        else if (action == "power-off")
            device.mShutdown.desired = true;
        else
            error_ = "Unknown bridge command: " + command;
    }

    void WriteState()
    {
        std::ostringstream stream;
        stream << "[State]\n";
        stream << "bridge_version=" << kBridgeVersion << '\n';
        stream << "status=" << status_ << '\n';
        stream << "status_text=" << SafeIni(statusText_) << '\n';
        stream << "device_name=" << SafeIni(device_ && !device_->mModelName.empty() ? device_->mModelName : selectedDeviceName_) << '\n';
        stream << "device_mac=" << SafeIni(selectedMac_) << '\n';
        stream << "transport=" << TransportName(transport_) << '\n';
        stream << "error=" << SafeIni(error_) << '\n';
        stream << "last_command=" << SafeIni(lastCommand_) << '\n';
        const auto uptimeSeconds = linkActive_
            ? std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - connectedSince_).count()
            : 0;
        stream << "connection_uptime_seconds=" << uptimeSeconds << '\n';
        stream << "connect_latency_ms=" << lastConnectLatencyMs_ << '\n';
        stream << "command_latency_ms=" << lastCommandLatencyMs_ << '\n';
        stream << "connection_attempts=" << connectionAttemptCount_ << '\n';
        stream << "reconnect_count=" << reconnectCount_ << '\n';
        stream << "poll_error_count=" << pollErrorCount_ << '\n';
        stream << "last_disconnect=" << SafeIni(lastDisconnectReason_) << '\n';

        if (device_)
        {
            auto& d = *device_;
            const auto touch = FindGeneralSetting(d, "TOUCH_PANEL_SETTING");
            const auto multipoint = FindGeneralSetting(d, "MULTIPOINT_SETTING");
            using F1 = mdr::v2::MessageMdrV2FunctionType_Table1;
            const bool supportsEq = d.mEqAvailable.current || !d.mEqConfig.current.empty();
            stream << "battery=" << static_cast<int>(d.mBatteryL.level) << '\n';
            stream << "charging=" << Bool(static_cast<int>(d.mBatteryL.charging) == 1) << '\n';
            stream << "volume=" << d.mPlayVolume.current << '\n';
            stream << "playback=" << PlaybackText(d.mPlayPause) << '\n';
            stream << "track_title=" << SafeIni(d.mPlayTrackTitle) << '\n';
            stream << "track_artist=" << SafeIni(d.mPlayTrackArtist) << '\n';
            stream << "codec=" << static_cast<int>(d.mAudioCodec) << '\n';
            stream << "anc_mode=" << AncText(d) << '\n';
            stream << "ambient_level=" << d.mNcAsmAmbientLevel.current << '\n';
            stream << "focus_voice=" << Bool(d.mNcAsmFocusOnVoice.current) << '\n';
            stream << "speak_to_chat=" << Bool(d.mSpeakToChatEnabled.current) << '\n';
            stream << "dsee=" << Bool(d.mUpscalingEnabled.current) << '\n';
            stream << "auto_pause=" << Bool(d.mAutoPauseEnabled.current) << '\n';
            stream << "touch_panel=" << Bool(touch && touch->property->current) << '\n';
            stream << "multipoint=" << Bool(multipoint && multipoint->property->current) << '\n';
            stream << "eq_preset=" << EqPresetText(d.mEqPresetId.current) << '\n';
            stream << "eq_bass=" << d.mEqClearBass.current << '\n';
            for (std::size_t index = 0; index < 5; ++index)
            {
                const int value = index < d.mEqConfig.current.size() ? d.mEqConfig.current[index] : 0;
                stream << "eq_band_" << (index + 1) << '=' << value << '\n';
            }
            stream << "priority=" << PriorityText(d.mAudioPriorityMode.current) << '\n';
            stream << "auto_off=" << AutoOffText(d.mPowerAutoOff.current) << '\n';
            stream << "button_function=" << ButtonFunctionText(d.mNcAsmButtonFunction.current) << '\n';
            stream << "touch_left=" << TouchPresetText(d.mTouchFunctionLeft.current) << '\n';
            stream << "touch_right=" << TouchPresetText(d.mTouchFunctionRight.current) << '\n';
            stream << "firmware=" << SafeIni(d.mFWVersion) << '\n';
            stream << "supported_anc=" << Bool(SupportsNoiseCancelling(d)) << '\n';
            stream << "supported_ambient=" << Bool(SupportsAmbient(d)) << '\n';
            stream << "supported_eq=" << Bool(supportsEq) << '\n';
            stream << "supported_speak_to_chat=" << Bool(d.mSupport.contains(F1::SMART_TALKING_MODE_TYPE2)) << '\n';
            stream << "supported_auto_pause=" << Bool(d.mSupport.contains(F1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF)) << '\n';
            stream << "supported_touch_panel=" << Bool(touch.has_value()) << '\n';
            stream << "supported_multipoint=" << Bool(multipoint.has_value()) << '\n';
            stream << "supported_assignable=" << Bool(d.mSupport.contains(F1::ASSIGNABLE_SETTING)) << '\n';
            stream << "supported_power_off=" << Bool(d.mSupport.contains(F1::POWER_OFF)) << '\n';
        }
        else
        {
            stream << "battery=0\ncharging=0\nvolume=0\nplayback=unknown\ntrack_title=\ntrack_artist=\ncodec=0\n";
            stream << "anc_mode=off\nambient_level=20\nfocus_voice=0\nspeak_to_chat=0\ndsee=0\nauto_pause=0\ntouch_panel=0\nmultipoint=0\n";
            stream << "eq_preset=--\neq_bass=0\neq_band_1=0\neq_band_2=0\neq_band_3=0\neq_band_4=0\neq_band_5=0\n";
            stream << "priority=--\nauto_off=--\nbutton_function=--\ntouch_left=--\ntouch_right=--\nfirmware=--\n";
            stream << "supported_anc=0\nsupported_ambient=0\nsupported_eq=0\nsupported_speak_to_chat=0\nsupported_auto_pause=0\n";
            stream << "supported_touch_panel=0\nsupported_multipoint=0\nsupported_assignable=0\nsupported_power_off=0\n";
        }

        const std::string payload = stream.str();
        const auto now = Clock::now();
        // Compare a stable view of state so uptime/latency heartbeats do not
        // force a disk write every second. Still republish at least every 2s
        // so installers cannot leave a stale state.ini behind during upgrades.
        std::string stablePayload = payload;
        auto blankField = [&stablePayload](const char* key) {
            const std::string prefix = std::string(key) + '=';
            const auto begin = stablePayload.find(prefix);
            if (begin == std::string::npos) return;
            const auto end = stablePayload.find('\n', begin);
            if (end == std::string::npos) return;
            stablePayload.replace(begin, end - begin, prefix);
        };
        blankField("connection_uptime_seconds");
        blankField("command_latency_ms");
        blankField("connect_latency_ms");
        if (stablePayload == lastStatePayload_ && now - lastStatePublish_ < 2s)
            return;

        const fs::path temporary = dataDirectory_ / "state.ini.tmp";
        const fs::path destination = dataDirectory_ / "state.ini";
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (!file)
            return;
        file.write(payload.data(), static_cast<std::streamsize>(payload.size()));
        file.flush();
        if (!file)
        {
            std::error_code error;
            fs::remove(temporary, error);
            return;
        }
        file.close();

        // Publishing must stay atomic. Rainmeter briefly opens state.ini while
        // polling, so retry sharing violations instead of truncating the live
        // destination with a non-atomic copy fallback.
        bool published = false;
        for (int attempt = 0; attempt < 6; ++attempt)
        {
            if (MoveFileExW(temporary.c_str(), destination.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            {
                published = true;
                break;
            }

            const DWORD error = GetLastError();
            if (error != ERROR_SHARING_VIOLATION && error != ERROR_LOCK_VIOLATION && error != ERROR_ACCESS_DENIED)
                break;
            std::this_thread::sleep_for(5ms);
        }

        if (published)
        {
            lastStatePayload_ = std::move(stablePayload);
            lastStatePublish_ = now;
        }
        else
        {
            std::error_code error;
            fs::remove(temporary, error);
        }
    }

    fs::path dataDirectory_;
    fs::path queueDirectory_;
    Logger logger_;
    Settings settings_;
    MDRConnectionWindows* backend_{};
    MDRConnectionWindowsBLE* bleBackend_{};
    MDRConnection* connection_{};
    std::unique_ptr<mdr::MDRHeadphones> device_;
    bool running_{true};
    bool connecting_{};
    bool syncRequested_{};
    bool hasConnected_{};
    bool recovering_{};
    bool linkActive_{};
    std::string status_{"starting"};
    std::string statusText_{"Starting bridge"};
    std::string error_;
    std::string lastCommand_;
    std::string selectedMac_;
    std::string selectedDeviceName_{"WH-1000XM5"};
    Transport transport_{Transport::Classic};
    Clock::time_point connectStarted_{};
    Clock::time_point connectedSince_{};
    Clock::time_point nextConnectAttempt_{};
    Clock::time_point nextStateWrite_{};
    Clock::time_point nextSync_{};
    std::optional<Clock::time_point> pollErrorSince_;
    std::optional<Clock::time_point> recoveryStarted_;
    std::string lastPollError_;
    std::string lastDisconnectReason_;
    int connectionAttemptCount_{};
    int reconnectCount_{};
    int pollErrorCount_{};
    int lastConnectLatencyMs_{-1};
    int lastCommandLatencyMs_{-1};
    std::string lastStatePayload_;
    Clock::time_point lastStatePublish_{};
};

fs::path ParseDataDirectory()
{
    int count = 0;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    fs::path result = fs::current_path() / "Data";
    if (arguments)
    {
        for (int index = 1; index + 1 < count; ++index)
        {
            if (std::wstring_view(arguments[index]) == L"--data-dir")
            {
                result = arguments[index + 1];
                break;
            }
        }
        LocalFree(arguments);
    }
    return fs::absolute(result);
}

bool HasArgument(std::wstring_view wanted)
{
    int count = 0;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    bool found = false;
    if (arguments)
    {
        for (int index = 1; index < count; ++index)
        {
            if (std::wstring_view(arguments[index]) == wanted)
            {
                found = true;
                break;
            }
        }
        LocalFree(arguments);
    }
    return found;
}

int LaunchDetachedBridge(const fs::path& dataDirectory)
{
    std::vector<wchar_t> executable(32768);
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return 1;

    const fs::path executablePath(std::wstring(executable.data(), length));
    std::wstring commandLine = L"\"" + executablePath.wstring() +
                               L"\" --background-child --data-dir \"" +
                               dataDirectory.wstring() + L"\"";
    std::vector<wchar_t> mutableCommand(commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    const DWORD detachedFlags = DETACHED_PROCESS | CREATE_NO_WINDOW;
    BOOL launched = CreateProcessW(executablePath.c_str(), mutableCommand.data(), nullptr, nullptr,
                                   FALSE, detachedFlags | CREATE_BREAKAWAY_FROM_JOB, nullptr,
                                   executablePath.parent_path().c_str(), &startup, &process);
    if (!launched)
    {
        mutableCommand.assign(commandLine.begin(), commandLine.end());
        mutableCommand.push_back(L'\0');
        launched = CreateProcessW(executablePath.c_str(), mutableCommand.data(), nullptr, nullptr,
                                  FALSE, detachedFlags, nullptr, executablePath.parent_path().c_str(),
                                  &startup, &process);
    }
    if (!launched)
        return 1;

    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return 0;
}
} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const fs::path dataDirectory = ParseDataDirectory();
    if (!HasArgument(L"--background-child"))
        return LaunchDetachedBridge(dataDirectory);

    HANDLE mutex = CreateMutexW(nullptr, TRUE, L"Local\\SonyXM5RainmeterBridge");
    if (!mutex || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (mutex)
            CloseHandle(mutex);
        return 0;
    }

    int result = 1;
    try
    {
        Bridge bridge(dataDirectory);
        result = bridge.Run();
    }
    catch (...)
    {
        result = 2;
    }
    ReleaseMutex(mutex);
    CloseHandle(mutex);
    return result;
}
