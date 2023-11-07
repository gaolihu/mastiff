/*
 * @Date: 2023-10-17 20:15:11
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/angstrong_camera_node/angstrong_camera_node.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */
/**
 * @file      main.cpp
 * @brief     angstrong camera demo main program.
 *
 * Copyright (c) 2023 Angstrong Tech.Co.,Ltd
 *
 * @author    Angstrong SDK develop Team
 * @date      2023/05/12
 * @version   1.0

 */

#include <signal.h>

#include "modules/chassis/drivers/soc/camera/angstrong_camera_node/as_demo.h"

static Demo demo;

static void Get_CtrlC_handler(int sig) {
    signal(sig, SIG_IGN);
    AINFO << "get Ctrl-C, now to exit safely" << std::endl;
    demo.stop();
    AINFO << "angstrong camera sdk exit." << std::endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    int ret = 0;
    signal(SIGINT, Get_CtrlC_handler);

    demo.start();

    while (true) {
        char ch = (char)getchar();
        if (ch == 's') {
            demo.saveImage();
        }
        else if (ch == 'f') {
            /* calculate the frame rate */
            demo.logFps(!demo.getLogFps());
        }
        else if (ch == 'q') {
            break;
        }
        else if (ch == 'l') {
            /* calculate the frame rate */
            demo.logCfgParameter();
        }
        else if (ch == 'd') {
#ifdef CFG_OPENCV_ON
            /* disply the image */
            bool enable = demo.getDisplayStatus();
            demo.display(!enable);
            if (!enable) {
                cv::destroyAllWindows();
            }
#endif
        }
    }

    demo.stop();
    return ret;
}
