/*
 * @Date: 2023-11-20 13:58:21
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/audio/audio_alsa.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "audio_alsa.h"
#include "cyber/common/log.h"

using namespace apollo;
using namespace cyber;

namespace mstf {
namespace chss {
namespace audio {
AudioAlsa::AudioAlsa(/* args */) {}

AudioAlsa::~AudioAlsa() {}
std::string AudioAlsa::AudioOperate(const mstf::chss::proto::AudioCtrl& msg)
{
    if(msg.has_vol()){
        ParseVolume(msg.vol());
    }
    if(msg.has_pack())
    {
        ParsePackage(msg.pack());
    }
    if(msg.has_play()){
        ParsePlay(msg.play());
    }
    return "";
}
void AudioAlsa::ParsePlay(const proto::AudioPlay& p)
{
    // chss_conf_;
    int32_t id = p.audio_id();
    std::string pack = p.pack().value();
    int32_t play_count = p.play_cnt().value();
    bool can_be_break = p.interrupt().value();

    if(!pack.empty()){
        package_ = pack;
    }

    auto audio_conf = aud_dev_[0].audio_conf();

    std::string audio_path_ =  audio_conf.audio_path();
    pack = audio_conf.voice_pack();
    for(auto& map: audio_conf.a_maps()){
        std::string sub_dir = map.sub_dir();
        if(sub_dir.empty()){
            AERROR << "Unknown voice sub directory";
            continue;
        }
        if(sub_dir != pack)
            continue;

        for(size_t i=0; i< map.voice_size(); i++){
            if(map.voice(i).idx() == id){
                std::string file = map.voice(i).name();
                auto full_path = audio_path_ +"/" + sub_dir + "/" + file;
                PlayFromFile(full_path);
            }
        }
    }
}
void AudioAlsa::ParseVolume(const proto::SetAduioVolume& v)
{
    int32_t volume = v.volmue();
    bool mute_sw = v.mute_sw().value();
    AINFO << "set system volumn to : " << volume;
}
void AudioAlsa::ParsePackage(const proto::SetAudioPack& p)
{
    package_ = p.dft_pack();
    AINFO << "set audio package to : " << package_;
}

bool AudioAlsa::PlayFromFile(const std::string& name)
{
    struct pcm_config config;
    struct pcm* pcm;
    FILE* file;
    struct wav_header header;
    int buffer_size;
    void* buffer;
    int data_size;

    file = fopen(name.c_str(), "rb");
    if(!file){
        AERROR << "Error opening wav file: " << name;
        return false;
    }

    if(fread(&header, 1, sizeof(wav_header), file) != sizeof(wav_header)){
        AERROR << "Error reading wav file header" << name;
        fclose(file);
        return false;
    }

    if(strncmp(header.chunk_id , "RIFF", 4) != 0 ||
        strncmp(header.format , "WAVE", 4) != 0){
        AERROR << "Error wav file format" << name;
        fclose(file);
        return false;
    }

    memset(&config, 0, sizeof(config));
    config.channels = header.num_channels;
    config.rate = header.sample_rate;
    if(header.bits_per_sample == 16)
        config.format = PCM_FORMAT_S16_LE;
    else if(header.bits_per_sample == 24)
        config.format = PCM_FORMAT_S24_3LE;
    else if(header.bits_per_sample == 32)
        config.format = PCM_FORMAT_S32_LE;
    else
        config.format = PCM_FORMAT_MAX; //FIXME, Glh, 2023/11/28 11:51
    config.period_size = 1024;
    config.period_count = 2;
    config.silence_threshold = 0;
    //config.silence_size = 0;          //FIXME, Glh, 2023/11/28 11:52
    config.start_threshold = 0;
    config.stop_threshold = 0;

    pcm = pcm_open(2,0, PCM_OUT, &config);
    if(!pcm || !pcm_is_ready(pcm)){
        AERROR << "unable to open pcm device: " << pcm_get_error(pcm);
        fclose(file);
        return false;
    }

    buffer_size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(buffer_size);

    fseek(file, sizeof(header), SEEK_CUR);

    while((data_size = fread(buffer, 1, buffer_size, file)) > 0){
        if(pcm_write(pcm, buffer, data_size)){
            AERROR << "Error writing to pcm device: " << pcm_get_error(pcm);
            break;
        }
    }

    free(buffer);
    buffer = NULL;
    fclose(file);
    pcm_close(pcm);

    return true;
}

}  // namespace audio
}  // namespace chss
}  // namespace mstf
