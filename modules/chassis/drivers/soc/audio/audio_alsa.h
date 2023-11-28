/*
 * @Date: 2023-11-15 15:15:06
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/audio/audio_alsa.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/frame_down_stream.pb.h"
#include "modules/chassis/proto/chassis_config.pb.h"
#include "tinyalsa/asoundlib.h"

#define DEFAULT_SAMPLE_RATE   44100
#define DEFAULT_CHANNELS      2
#define DEFAULT_SAMPLE_FORMAT PCM_FORMAT_S16_LE

using namespace google;
using namespace protobuf;

#pragma pack(1)
struct wav_header {
    char     chunk_id[4];
    uint32_t chunk_size;
    char     format[4];
    char     subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char     subchunk2_id[4];
    uint32_t subchunk2_size;
};
#pragma pack(pop)

namespace mstf {
namespace chss {
namespace audio {

class AudioAlsa {
public:
    AudioAlsa(/* args */);
    ~AudioAlsa();
    bool PlayFromFile(const std::string& file);

    std::string AudioOperate(const mstf::chss::proto::AudioCtrl& msg);
    inline void SetChssConfig(RepeatedPtrField<proto::AudioDevConf> c) {
        aud_dev_ = c;
    }
private:
    void ParsePlay(const proto::AudioPlay& p);
    void ParseVolume(const proto::SetAduioVolume& v);
    void ParsePackage(const proto::SetAudioPack& p);

    RepeatedPtrField<proto::AudioDevConf> aud_dev_;
    std::string package_;
};

}  // namespace audio
}  // namespace chss
}  // namespace mstf
