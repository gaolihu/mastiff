/*
 * @Date: 2023-11-21 10:49:42
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/test/audio_play_test.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "../drivers/soc/audio/audio_alsa.h"
#include "../drivers/soc/audio/audio_ctrl.h"

using namespace mstf;
using namespace chss;
using namespace audio;

int main(int argc, char const *argv[])
{
    AudioAlsa aa;
    // aa.PlayFromFile("/userdata/file_example_WAV_10MG.wav");
    aa.PlayFromFile("/userdata/tts.wav");
    return 0;
}
