#include "cyber/common/log.h"

#include "modules/chassis/drivers/soc/audio/audio_ctrl.h"
#include "modules/chassis/drivers/soc/audio/audio_alsa.h"

namespace mstf {
namespace chss {
namespace driver {

AudioCtrl::AudioCtrl(const AudioConfig* c) {
    can_running_ = false;
    audio_ = std::make_shared<AudioAlsa>(c);
}

AudioCtrl::~AudioCtrl() {
    can_running_ = false;
    if (audio_th_.joinable()) {
        audio_th_.join();
    }
}
void AudioCtrl::Init() {}
void AudioCtrl::Start() {
    can_running_ = true;

    audio_th_ = std::thread(std::bind(&AudioCtrl::AudioRunner, this));
    audio_th_.detach();
}
void AudioCtrl::Stop() {
    can_running_ = false;
    if (audio_th_.joinable()) {
        audio_th_.join();
    }
}
void AudioCtrl::Close() {
    can_running_ = false;
    if (audio_th_.joinable()) {
        audio_th_.join();
    }
}
bool AudioCtrl::SetAudioCtrl(const chss::proto::AudioSetting& msg) {
    std::unique_lock<std::mutex> lk(audio_mtx_);
    ctrl_msg_ = msg;
    lk.unlock();

    audio_cv_.notify_one();
    return true;
}
void AudioCtrl::AudioRunner() {
    AINFO << "Audio control start success!";
    while (can_running_) {
        std::unique_lock<std::mutex> lk(audio_mtx_);
        audio_cv_.wait(lk);
        auto result = audio_->AudioOperate(ctrl_msg_);
        ctrl_msg_.Clear();
        AINFO << "Audio thread play run once...";
    }
}

}  // namespace audio
}  // namespace chss
}  // namespace mstf
