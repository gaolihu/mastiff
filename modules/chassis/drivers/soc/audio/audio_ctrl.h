#pragma once
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "audio_alsa.h"
#include "modules/chassis/drivers/soc/audio/audio_alsa.h"
#include "modules/chassis/proto/chss_io.pb.h"

using namespace google;
using namespace protobuf;
namespace mstf {
namespace chss {
namespace driver {

class AudioCtrl {
public:
    AudioCtrl(const AudioConfig*);
    ~AudioCtrl();
    void Init();
    void Start();
    void Stop();
    void Close();
    void SetAudioCtrl(const chss::proto::AudioSetting& msg);

    void PollingAudioRutine();

private:
    bool has_data_ = false;
    proto::AudioSetting ctrl_msg_;

    std::shared_ptr<AudioAlsa> audio_;
};

}  // namespace driver
}  // namespace chss
}  // namespace mstf
