#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/parser/packer_base_itf.h"
#include "modules/chassis/parser/parse_libs/ds20270da_driver.h"

namespace mstf {
namespace chss {
namespace parser {

    class ServoPacker : public PackerBaseItf{
        public:
            ServoPacker(const std::string&);
            virtual ~ServoPacker();

            virtual std::tuple<const int, const std::vector<uint8_t>>
            PackMotorMessageRaw(const DownToServoData&) override;

            virtual std::vector<std::tuple<const int, const std::vector<uint8_t>>>
            PackMotorMessageArrayRaw(const DownToServoData&) override;

            virtual const std::vector<uint8_t>
            PackMotorMessageString(const DownToServoData&) override;

        private:
            void ConvertDiffSpeed2Wheel(const DiffSpeed&, WheelSpeed&);
            void ConvertDiffSpeed2Rpm(const DiffSpeed&, WheelRpm&);

            bool FillDataContainer(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const std::string& des,
                    const int* addr,
                    const int size) {
                std::vector<uint8_t> data;

                if (addr == nullptr || size <= 0) {
                    AERROR << "adress / size error";
                    return false;
                }

                for (int i = 0; i < size; i++) {
                    data.clear();
                    data.emplace_back(DS_MOTOR_READ_BOTH_CMD);
                    data.emplace_back(addr[i] & 0xff);
                    data.emplace_back((addr[i] & 0xff00) >> 8);
                    data.emplace_back(0x00);

                    data.emplace_back(0);
                    data.emplace_back(0);
                    data.emplace_back(0);
                    data.emplace_back(0);
#if 0
                    AWARN << "get servo inner address " << des <<
                        "-" << std::setw(2) << std::setfill('0') << i <<
                        ", addr: " << std::hex << addr[i] <<
                        "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) << " ]";
#endif
                    td.emplace_back<std::tuple<const int,
                            const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_ID, data});
                }

                return true;
            }
    };

} //namespace parser
} //namespace chss
} //namespace mstf
