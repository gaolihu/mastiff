#include "cyber/common/log.h"

#include "modules/chassis/drivers/soc/audio/audio_ctrl.h"
#include "modules/chassis/drivers/soc/audio/audio_alsa.h"

namespace mstf {
namespace chss {
namespace driver {

AudioCtrl::AudioCtrl(const AudioConfig* c) {
    audio_ = std::make_shared<AudioAlsa>(c);
}

AudioCtrl::~AudioCtrl() { }
void AudioCtrl::Init() { }
void AudioCtrl::Start() { }
void AudioCtrl::Stop() { }
void AudioCtrl::Close() { }
void AudioCtrl::SetAudioCtrl(const chss::proto::AudioSetting& msg) {
    has_data_ = true;
    ctrl_msg_ = msg;
}
void AudioCtrl::PollingAudioRutine() {
    if (has_data_) {
        auto result = audio_->AudioOperate(ctrl_msg_);
        ctrl_msg_.Clear();
        has_data_ = false;
    }
}

}  // namespace audio
}  // namespace chss
}  // namespace mstf
