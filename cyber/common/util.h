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

}  // namespace common
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMMON_UTIL_H_
