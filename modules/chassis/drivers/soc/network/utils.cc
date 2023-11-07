/*
 * @Date: 2023-09-13 19:38:29
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/utils.cc
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#include "utils.h"
using namespace mstf;
using namespace chss;
using namespace network;

std::vector<std::string> GetCurrentKernelModules() {
    std::vector<std::string> modules;
    return modules;
}


int network::Run(const std::string &cmd, std::vector<std::string> &output) {
    auto fp = popen(cmd.c_str(), "r");
    if (!fp) {
        std::cerr << "popen() failed!" << std::endl;
        return -1;
    }

    output.clear();

    char   *line = nullptr;
    size_t  len  = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        output.push_back(line);
    }

    if (line) {
        free(line);
    }
    pclose(fp);

    return 0;
}
int network::Run(const std::string &cmd, std::string &output) {
    int ret = 0;

    std::vector<std::string> lines;

    ret = network::Run(cmd, lines);

    if (0 != ret) {
        return ret;
    }

    output.clear();

    for (auto &l : lines) {
        output += l;
    }

    if (!output.empty() && output[output.length() - 1] == '\n') {
        output.erase(output.length() - 1);
    }

    return 0;
}
std::vector<int> network::GetProcessPid(const std::string name) {
    auto                     cmd = "ps -ef | grep " + name + " | grep -v grep | awk '{print $1}'";
    std::vector<std::string> output;
    std::vector<int>         result;
    ::Run(cmd, output);
    for (auto &o : output) {
        result.push_back(std::atoi(o.c_str()));
    }

    return result;
}

int network::TermProcess(const int pid) {
    if (pid <= 1) {
        return 0;
    }

    int ret = kill(pid, SIGTERM);
    return ret;
}

int network::TermProcess(const std::string &name) {
    auto pids = network::GetProcessPid(name);

    int ret = -1;
    for (auto &p : pids) {
        ret = network::TermProcess(p);
        if (0 != ret) {
            return 1;
        }
    }

    return 0;
}

void network::ReplaceString(std::string &str, const std::string &target, const std::string &replace) {
    if (str.empty()) {
        return;
    }

    // std::replace(str.begin(), str.end(), target.c_str(), replace.c_str());
}
