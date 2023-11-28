/*
 * @Date: 2023-11-15 15:14:46
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/audio/audio_ctrl.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

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
#include "cyber/common/log.h"
#include "modules/chassis/drivers/soc/audio/audio_alsa.h"
#include "modules/chassis/drivers/soc/soc_defs.h"
#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/frame_down_stream.pb.h"
#include "modules/chassis/proto/frame_up_stream.pb.h"

using namespace google;
using namespace protobuf;
namespace mstf {
namespace chss {
namespace audio {
class AudioCtrl {
public:
    AudioCtrl(/* args */);
    ~AudioCtrl();
    void Init();
    void Start();
    void Stop();
    void Close();
    bool SetAudioCtrl(const chss::proto::AudioCtrl& msg);

    inline void SetAudioConfig(const RepeatedPtrField<proto::AudioDevConf>& c) {
        aud_dev_ = c;
        audio_.SetChssConfig(c);
    }

private:
    void AudioRunner();

    std::mutex              audio_mtx_;
    std::condition_variable audio_cv_;
    bool                    can_running_{false};
    std::thread             audio_th_;

    proto::AudioCtrl ctrl_msg_;

    AudioAlsa audio_;

    RepeatedPtrField<proto::AudioDevConf> aud_dev_;
};

}  // namespace audio
}  // namespace chss
}  // namespace mstf
