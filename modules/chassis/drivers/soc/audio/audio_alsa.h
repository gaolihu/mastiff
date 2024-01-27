#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <iostream>
#include <string>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/log.h"
}

#include "modules/chassis/proto/chss_io.pb.h"
#include "modules/chassis/proto/chassis_config.pb.h"
#include "tinyalsa/asoundlib.h"

#define DEFAULT_SAMPLE_RATE   44100
#define DEFAULT_CHANNELS      2
#define DEFAULT_SAMPLE_FORMAT PCM_FORMAT_S16_LE
#define DEFAULT_BIT_PER_SAMPLE 16

using namespace google;
using namespace protobuf;

#pragma pack(push,1)
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
namespace driver {

    using namespace proto;

class AudioAlsa {
public:
    AudioAlsa(const AudioConfig*);
    ~AudioAlsa();
    /**
     * @brief 从文件读取wav文件并播放
     *
     * @param file
     * @return true
     * @return false
     */
    bool PlayFromFile(const std::string& file);
    /**
     * @brief 设置音量,百分比
     *
     * @param volume
     * @return true
     * @return false
     */
    bool SetVolume(size_t volume=50);
    bool Mute(); // 静音
    /**
     * @brief 参考MPlayer, 把mp3文件用ffmpeg转换为wav数据帧
     *
     * @param mp3
     * @return true
     * @return false
     */
    bool ConvertMp3ToWav(const std::string& mp3);
    /**
     * @brief PCM 播放参数设置
     *
     * @param header
     * @return true
     * @return false
     */
    bool SetPcmConfig(struct wav_header header);

    /**
     * @brief 接收控制指令
     *
     * @param msg protobuf类型的消息
     * @return std::string
     */
    std::string AudioOperate(const AudioSetting& msg);

private:
    void ParsePlay(const AudioPlay& p);
    void ParseVolume(const SetAduioVolume& v);
    void ParsePackage(const SetAudioPack& p);
    void VolumeControl(size_t volume);

    void PlayWavFile(const std::string& name);
    void PlayMp3File(const std::string& name);

    const AudioConfig* audio_conf_;
    std::string package_;
};

}  // namespace driver
}  // namespace chss
}  // namespace mstf
