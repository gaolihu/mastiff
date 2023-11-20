/*
 * @Date: 2023-09-13 19:38:21
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/network/utils.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once

#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cyber/common/log.h"


namespace mstf {
namespace chss {
namespace network {
std::vector<std::string> GetCurrentKernelModules();

int Run(const std::string& cmd, std::string& output);
int Run(const std::string& cmd, std::vector<std::string>& output);

std::vector<int> GetProcessPid(const std::string name);

int TermProcess(const std::string& name);
int TermProcess(const int pid);

void ReplaceString(std::string& str, const std::string& target, const std::string& replace);
}  // namespace network
}  // namespace chss
}  // namespace mstf
