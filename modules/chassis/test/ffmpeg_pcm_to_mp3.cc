#include <iostream>

using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}


int main() {
    av_register_all();

    char  inputfile[]  = "input.pcm";
    char  outputfile[] = "output.mp3";
    int   ret          = 0;
    FILE* finput       = NULL;
    FILE* foutput      = NULL;

    //1 注册编码器 MP3
    AVCodec* codec = (AVCodec*)avcodec_find_encoder(AV_CODEC_ID_MP3);
    if (!codec) {
        cout << "avcodec_find_encoder failed" << endl;
        return -1;
    }

    //2 创建编码器上下文
    AVCodecContext* ctx = NULL;
    ctx                 = avcodec_alloc_context3(codec);  //分配空间
    if (!ctx) {
        cout << "avcodec_alloc_context3 failed" << endl;
        return -1;
    }
    ctx->bit_rate       = 64000;
    ctx->channels       = 2;
    ctx->channel_layout = AV_CH_LAYOUT_STEREO;
    ctx->sample_rate    = 44100;
    ctx->sample_fmt     = AV_SAMPLE_FMT_S16P;  //mp3 只支持s16平面格式

    //3 打开编码器        上下文 编码
    //c语言中返回0表示成功
    ret = avcodec_open2(ctx, codec, 0);
    if (ret < 0) {
        cout << "avcodec_open2 failed" << endl;
        return -1;
    }

    //4 打开输出文件
    foutput = fopen(outputfile, "wb");
    if (!foutput) {
        cout << "avcodec_open2 failed" << endl;
        return -1;
    }

    //5 AVFrame 接受重采样的每一帧的音频数据 每帧的样本大小为1152
    AVFrame* frame;
    frame = av_frame_alloc();
    if (!frame) {
        cout << "av_frame_alloc failed" << endl;
        return -1;
    }
    frame->nb_samples     = 1152;  //MP3 一帧的样本数量为1152
    frame->channels       = 2;
    frame->channel_layout = AV_CH_LAYOUT_STEREO;
    frame->format         = AV_SAMPLE_FMT_S16P;

    ret = av_frame_get_buffer(frame, 0);  //分配空间
    if (ret < 0) {
        cout << "av_frame_get_buffer failed" << endl;
        return -1;
    }

    //6 重采样  创建音频重采样上下文
    SwrContext* swr = swr_alloc();
    if (!swr) {
        cout << "swr_alloc failed" << endl;
        return -1;
    }

    //设置重采样输入pcm参数：通道布局：立体声 采样率：44100  样本格式 s16交错存储
    av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);  //pcm 样本格式

    //设置重采样输出mp3参数：通道布局：立体声 采样率：44100  样本格式 s16平面存储
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16P, 0);

    //初始化
    ret = swr_init(swr);
    if (ret < 0) {
        cout << "swr_init failed" << endl;
        return -1;
    }

    //打开输入文件
    finput = fopen(inputfile, "rb");
    if (!finput) {
        cout << "fopen inputfile failed" << endl;
        return -1;
    }
    //
    uint8_t** input_data  = NULL;
    //
    uint8_t** output_data = NULL;
    int       input_linesize, output_linesize;

    //给pcm文件数据分配空间
    ret = av_samples_alloc_array_and_samples(&input_data, &input_linesize, 2, 1152, AV_SAMPLE_FMT_S16, 0);
    if (ret < 0) {
        cout << "av_samples_alloc_array_and_samples input failed" << endl;
        return -1;
    }
    //缓存重采样数据的空间分配
    ret = av_samples_alloc_array_and_samples(&output_data, &output_linesize, 2, 1152, AV_SAMPLE_FMT_S16P, 0);
    if (ret < 0) {
        cout << "av_samples_alloc_array_and_samples out failed" << endl;
        return -1;
    }

    //存放编码后的数据
    AVPacket* pkt = av_packet_alloc();
    if (!pkt) {
        cout << "av_packet_alloc failed" << endl;
        return -1;
    }

    while (!feof(finput)) {
        int readsize = fread(input_data[0], 1, 1152 * 2 * 2, finput);
        if (!readsize) {
            break;
        }
        cout << readsize << endl;

        //重采样
        ret = swr_convert(swr, output_data, 1152, (const uint8_t**)input_data, 1152);
        if (ret < 0) {
            cout << "swr_convert failed" << endl;
            return -1;
        }
        //将重采样后的数据存入frame
        //MP3是s16p 先存放左声道的数据 后存放右声道的数据， data[0]是左声道，1是右声道
        frame->data[0] = output_data[0];
        frame->data[1] = output_data[1];

        //编码，写入mp3文件，实际上是对frame这个结构体里面的数据进行编码操作
        //发送到编码线程：使用编码器  和 存储数据的frame
        ret = avcodec_send_frame(ctx, frame);
        if (ret < 0) {
            cout << "avcodec_send_frame failed" << endl;
            return -1;
        }

        while (ret >= 0) {
            //接收编码后的数据，使用编码器 和 存储编码数据的pkt
            ret = avcodec_receive_packet(ctx, pkt);  //有可能需要多次才能接收完成

            //AVERROR(EEAGAIN) -11       AVERROR_EOF表示没有数据了 这两个错误不影响继续接收数据
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                continue;
            }
            else if (ret < 0) {
                break;
            }
            fwrite(pkt->data, 1, pkt->size, foutput);
            av_packet_unref(pkt);  //释放pkt空间，否则内存泄露
        }
    }

    //关闭缓存
    if (input_data) {
        av_freep(input_data);
    }
    if (output_data) {
        av_freep(output_data);
    }

    //关闭文件
    fclose(finput);
    fclose(foutput);

    //s释放 frame pkt
    av_frame_free(&frame);
    av_packet_free(&pkt);

    //释放重采样上下文
    swr_free(&swr);

    //释放编码器上下文
    avcodec_free_context(&ctx);

    return 0;
}
