#include "audio_alsa.h"

#include "tinyalsa/asoundlib.h"
#include "cyber/common/log.h"

#define MAX_AUDIO_FRAME_SIZE 192000  // 1 second of 48khz 32bit audio

using namespace apollo;
using namespace cyber;

namespace mstf {
namespace chss {
namespace driver {

static struct pcm_config config_;
static struct pcm*       pcm_;
static bool              continue_running_;
static bool              thread_exist_ = true;

AudioAlsa::AudioAlsa(const AudioConfig* c) {
    audio_conf_ = c;
}

AudioAlsa::~AudioAlsa() {}
bool AudioAlsa::SetVolume(size_t volume) {
    if (volume == 0) {
        return Mute();
    }
    if (volume > 100)
        volume = 100;

    VolumeControl(volume);
    return true;
}
bool AudioAlsa::Mute() {
    VolumeControl(0);
    AINFO << "audio playback set to mute";
    return true;
}
std::string AudioAlsa::AudioOperate(const AudioSetting& msg) {
    if (msg.has_vol()) {
        ParseVolume(msg.vol());
    }
    if (msg.has_pack()) {
        ParsePackage(msg.pack());
    }
    if (msg.has_play()) {
        continue_running_ = false;
        while (!thread_exist_) {
            usleep(1000);
        }
        ParsePlay(msg.play());
    }
    return "";
}
void AudioAlsa::ParsePlay(const AudioPlay& p) {
    // chss_conf_;
    int32_t     id           = p.sound_id();
    std::string pack         = p.pack().value();

    //TODO: 重复次数， 是否可以打断
    // int32_t     play_count   = p.play_cnt().value();
    // bool        can_be_break = p.interrupt().value();

    if (!pack.empty()) {
        package_ = pack;
    }

    std::string audio_path_ = audio_conf_->audio_path();
    pack                    = audio_conf_->voice_pack();
    for (auto& map : audio_conf_->a_maps()) {
        std::string sub_dir = map.sub_dir();
        if (sub_dir.empty()) {
            AERROR << "Unknown voice sub directory";
            continue;
        }
        if (sub_dir != pack)
            continue;

        for (int i = 0; i < map.voice_size(); i++) {
            if (map.voice(i).idx() == id) {
                std::string file      = map.voice(i).name();
                auto        full_path = audio_path_ + "/" + sub_dir + "/" + file;
                PlayFromFile(full_path);
            }
        }
    }
}
void AudioAlsa::ParseVolume(const SetAduioVolume& v) {
    int32_t volume  = v.volume();
    bool    mute_sw = v.mute_sw().value();

    if (mute_sw) {
        Mute();
    }
    else {
        SetVolume(volume);
    }

    AINFO << "set system volumn to : " << volume;
}
void AudioAlsa::ParsePackage(const SetAudioPack& p) {
    package_ = p.dft_pack();
    AINFO << "set audio package to : " << package_;
}
void AudioAlsa::VolumeControl(size_t vol) {
    std::string cmd = "amixer -M set PCM " + std::to_string(vol) + "%";
    if (system(cmd.c_str()) != 0) {
        AERROR << "vol control failed!";
    }
}

bool AudioAlsa::SetPcmConfig(struct wav_header header) {
    memset(&config_, 0, sizeof(config_));
    config_.channels = header.num_channels;
    config_.rate     = header.sample_rate;
    if (header.bits_per_sample == 16)
        config_.format = PCM_FORMAT_S16_LE;
    else if (header.bits_per_sample == 24)
        config_.format = PCM_FORMAT_S24_3LE;
    else if (header.bits_per_sample == 32)
        config_.format = PCM_FORMAT_S32_LE;
    else
        config_.format = PCM_FORMAT_MAX;

    config_.period_size       = 1024;
    config_.period_count      = 2;
    config_.silence_threshold = 0;
    config_.start_threshold   = 0;
    config_.stop_threshold    = 0;

    pcm_ = pcm_open(2, 0, PCM_OUT, &config_);
    if (!pcm_ || !pcm_is_ready(pcm_)) {
        AERROR << "unable to open pcm device: " << pcm_get_error(pcm_);
        thread_exist_ = true;
        return false;
    }
    return true;
}

bool AudioAlsa::PlayFromFile(const std::string& name) {
    if (!thread_exist_) {
        AWARN << "wait audio play thread to exist...";
        return false;
    }

    AINFO << "Go to play music: " << name;

    if (name.find(".mp3") != name.npos) {
        PlayMp3File(name);
    }
    else if (name.find(".wav") != name.npos) {
        PlayWavFile(name);
    }
    else {
        AERROR << "music play file suffix not support: " << name;
    }

    return true;
}

void AudioAlsa::PlayWavFile(const std::string& name) {
    std::thread t([name, this]()->bool {
        thread_exist_ = false;

        FILE*             file;
        struct wav_header header;
        int               buffer_size;
        void*             buffer;
        int               data_size;

        file = fopen(name.c_str(), "rb");
        if (!file) {
            AERROR << "Error opening wav file: " << name;
            thread_exist_ = true;
            return false;
        }

        // 读取 wav 文件头
        if (fread(&header, 1, sizeof(wav_header), file) != sizeof(wav_header)) {
            AERROR << "Error reading wav file header" << name;
            fclose(file);
            thread_exist_ = true;
            return false;
        }

        // 检查 wav 文件头
        if (strncmp(header.chunk_id, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0) {
            AERROR << "Error wav file format" << name;
            fclose(file);
            thread_exist_ = true;
            return false;
        }

        // 设置播放参数
        if (!SetPcmConfig(header)) {
            AERROR << "Set pcm config fail!";
            fclose(file);
            thread_exist_ = true;
            return false;
        }

        buffer_size = pcm_frames_to_bytes(pcm_, pcm_get_buffer_size(pcm_));
        buffer      = malloc(buffer_size);

        fseek(file, sizeof(header), SEEK_CUR);

        continue_running_ = true;
        while ((data_size = fread(buffer, 1, buffer_size, file)) > 0 && continue_running_) {
            if (pcm_write(pcm_, buffer, data_size)) {
                AERROR << "Error writing to pcm device: " << pcm_get_error(pcm_) << "buffer size: " << buffer_size;
                break;
            }
            usleep(5000);  // 必须加，防止写入失败
        }

        if (buffer) {
            free(buffer);
            buffer = NULL;
        }
        if (file) {
            fclose(file);
            file = NULL;
        }
        pcm_drain(pcm_);
        pcm_close(pcm_);
        thread_exist_ = true;

        return true;
    });
    t.detach();
}
void AudioAlsa::PlayMp3File(const std::string& name) {
    std::thread t([name, this]()->bool {
        thread_exist_ = false;

        {
            struct wav_header header;
            header.audio_format    = PCM_FORMAT_S16_LE;
            header.num_channels    = DEFAULT_CHANNELS;
            header.bits_per_sample = DEFAULT_BIT_PER_SAMPLE;
            header.sample_rate     = DEFAULT_SAMPLE_RATE;

            // 设置播放参数
            if (!SetPcmConfig(header)) {
                thread_exist_ = true;
                return false;
            }
            ConvertMp3ToWav(name);
        }

        pcm_drain(pcm_);
        pcm_close(pcm_);
        thread_exist_ = true;

        return true;
    });
    t.detach();
}
bool AudioAlsa::ConvertMp3ToWav(const std::string& mp3) {
    // av_log_set_level(AV_LOG_DEBUG);

    AVFormatContext*   pFormatCtx;
    int                i, audioStream;
    AVCodecContext*    pCodecCtx;
    AVCodec*           pCodec;
    AVPacket*          packet;
    uint8_t*           out_buffer;
    AVFrame*           pFrame;
    int                ret;
    // uint32_t           len = 0;
    int                got_picture;
    int                index = 0;
    int64_t            in_channel_layout;
    struct SwrContext* au_convert_ctx;


    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    //Open
    if (avformat_open_input(&pFormatCtx, mp3.c_str(), NULL, NULL) != 0) {
        AERROR << "Couldn't open input stream.";
        return -1;
    }
    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        AERROR << "Couldn't find stream information.";
        return -1;
    }
    // Dump valid information onto standard error
    av_dump_format(pFormatCtx, 0, mp3.c_str(), false);

    // Find the first audio stream
    audioStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            break;
        }

    if (audioStream == -1) {
        AERROR << "Didn't find a audio stream.";
        return -1;
    }

    // Get a pointer to the codec context for the audio stream
    pCodecCtx = pFormatCtx->streams[audioStream]->codec;

    // Find the decoder for the audio stream
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        AERROR << "Codec not found.";
        return -1;
    }

    // Open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        AERROR << "Could not open codec.";
        return -1;
    }

    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    av_init_packet(packet);

    //Out Audio Param
    uint64_t       out_channel_layout = AV_CH_LAYOUT_STEREO;
    //nb_samples: AAC-1024 MP3-1152
    int            out_nb_samples     = pCodecCtx->frame_size;
    AVSampleFormat out_sample_fmt     = AV_SAMPLE_FMT_S16;
    int            out_sample_rate    = 44100;
    int            out_channels       = av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int            out_buffer_size    = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

    out_buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
    pFrame     = av_frame_alloc();

    //FIX:Some Codec's Context Information is missing
    in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
    //Swr
    au_convert_ctx    = swr_alloc();
    au_convert_ctx    = swr_alloc_set_opts(au_convert_ctx,
                                        out_channel_layout,
                                        out_sample_fmt,
                                        out_sample_rate,
                                        in_channel_layout,
                                        pCodecCtx->sample_fmt,
                                        pCodecCtx->sample_rate,
                                        0,
                                        NULL);
    swr_init(au_convert_ctx);

    continue_running_ = true;
    while (av_read_frame(pFormatCtx, packet) >= 0 && continue_running_) {
        if (packet->stream_index == audioStream) {
            ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0) {
                AERROR << "Error in decoding audio frame.";
                return -1;
            }
            if (got_picture > 0) {
                swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t**)pFrame->data, pFrame->nb_samples);

                //Write PCM
                pcm_write(pcm_, out_buffer, out_buffer_size);
                usleep(5000);  // 必须加，防止写入失败
                index++;
            }
        }
        av_free_packet(packet);
    }

    swr_free(&au_convert_ctx);

    av_free(out_buffer);
    // Close the codec
    avcodec_close(pCodecCtx);
    // Close the video file
    avformat_close_input(&pFormatCtx);
    return true;
}

}  // namespace driver
}  // namespace chss
}  // namespace mstf
