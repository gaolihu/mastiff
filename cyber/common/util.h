/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBER_COMMON_UTIL_H_
#define CYBER_COMMON_UTIL_H_

#include <string>
#include <type_traits>

#include <tuple>

namespace apollo {
namespace cyber {
namespace common {

#define SET_BIT_VAL(data, bit, val) \
    ((data) |= ((val) << (bit)))
#define STT_BIT_VAL(data, bit, v) \
    ((data) = (((data) & ~(1 << (bit))) | ((v) << (bit))))
#define TST_BIT_VAL(data, bit) \
    (((data) & (1 << (bit))) >> (bit))

inline std::size_t Hash(const std::string& key) {
  return std::hash<std::string>{}(key);
}

template <typename Enum>
auto ToInt(Enum const value) -> typename std::underlying_type<Enum>::type {
  return static_cast<typename std::underlying_type<Enum>::type>(value);
}

inline void GetMacIp(const std::string dev,
        std::string& ip, std::string& mac) {
    std::string::size_type cursor = 0, cursor1 = 0;
    char buf[128] = {0};
    std::string ps, cmd;

    if (dev.empty()) {
        cmd = "ifconfig wlan0 ";
    } else {
        cmd = "ifconfig " + dev;
    }

    FILE*fp = popen(cmd.data(), "r");
    if (fp != nullptr) {
        while (fgets(buf, sizeof(buf), fp) != nullptr) {
            ps += buf;
        }

        cursor = ps.find("HWaddr ");
        if (cursor != std::string::npos)
            mac.assign(ps, cursor + sizeof("HWaddr"), 17);

        cursor = ps.find("inet addr:");
        cursor1 = ps.find("  Bcast");
        if (cursor != std::string::npos)
            ip.assign(ps, cursor + sizeof("inet addr"),
                    cursor1 - cursor - sizeof("inet addr"));

        pclose(fp);
    }
}

inline std::string GetSysUpTime() {
    char buf_s[32] = {0};
    FILE* p = popen("uptime -s", "r");
    if (p == nullptr) {
        return "";
    }

    if (fgets(buf_s, sizeof(buf_s), p) == nullptr)
        return "";

    pclose(p);
    buf_s[19] = 0;
    return std::move(std::string(buf_s));
}

inline float GetSysUpSecs() {
    float r = -1.f;
    char buf_u[32] = {0};

    FILE* p = popen("cat /proc/uptime", "r");
    if (p == nullptr) {
        return -1;
    }

    if (fgets(buf_u, sizeof(buf_u), p) == nullptr)
        return -1;

    sscanf(buf_u, "%f ", &r);
    pclose(p);

    return r;
}

inline std::tuple<float, float, float> GetSysLoad() {
    char buf_l[72] = {0};
    char* cursor = nullptr;
    float l1 = -1.f, l2 = -1.f, l3 = -1.f;

    FILE* p = popen("w", "r");
    if (p == nullptr) {
        return {-1.f, -1.f, -1.f};
    }

    if (fgets(buf_l, sizeof(buf_l), p) == nullptr) {
        pclose(p);
        return {-1.f, -1.f, -1.f};
    }

    cursor = strstr(buf_l, ",  load average: ");
    if (cursor == nullptr) {
        pclose(p);
        return {-1.f, -1.f, -1.f};
    }

    sscanf(cursor, ",  load average: %f, %f, %f",
            &l1, &l2, &l3);

    pclose(p);

    return {l1, l2, l3};
}

inline float Byte2Float(uint8_t c0,
        uint8_t c1, uint8_t c2, uint8_t c3) {
    union uf {
        float f_data;
        uint8_t byte[4];
    } x;

    x.byte[0] = c0;
    x.byte[1] = c1;
    x.byte[2] = c2;
    x.byte[3] = c3;

    return x.f_data;
}

inline float RByte2Float(uint8_t c0,
        uint8_t c1, uint8_t c2, uint8_t c3) {
    return Byte2Float(c3, c2, c1, c0);
}

}  // namespace common
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMMON_UTIL_H_
