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
namespace driver {
std::vector<std::string> GetCurrentKernelModules();

int Run(const std::string& cmd, std::string& output);
int Run(const std::string& cmd, std::vector<std::string>& output);

std::vector<int> GetProcessPid(const std::string name);

int TermProcess(const std::string& name);
int TermProcess(const int pid);

void ReplaceString(std::string& str, const std::string& target, const std::string& replace);
}  // namespace driver
}  // namespace chss
}  // namespace mstf
