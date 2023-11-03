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
            PackMotorWheelSpeed(const DownToServoData&) override;

            virtual std::vector<std::tuple<const int, const std::vector<uint8_t>>>
            PackMotorDiffSpeedDouble(const DownToServoData&) override;

            virtual std::tuple<const int, const std::vector<uint8_t>>
            PackMotorDiffSpeed(const DownToServoData&) override;

            virtual std::vector<std::tuple<const int, const std::vector<uint8_t>>>
            PackMotorMessageArrayRaw(const DownToServoData&) override;

            virtual const std::vector<uint8_t>
            PackMotorMessageString(const DownToServoData&) override;

        private:
            void ConvertDiffSpeed2Wheel(const ServoSpeed&, WheelSpeed&);
            void ConvertDiffSpeed2Rpm(const ServoSpeed&, WheelRpm&);

            //////////////////////////////////////////////////////////////////
            //double reading registers
            bool FillDataContainerFreeRead(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const std::string& des,
                    const int16_t* addr,
                    const int* value,
                    const int size) {
                std::vector<uint8_t> data;

                if (addr == nullptr || size <= 0) {
                    AERROR << "fill motor data adress / size error";
                    return false;
                }

                for (int i = 0; i < size; i++) {
                    data.clear();
                    data.emplace_back(DS_MOTOR_READ_BOTH_CMD);
                    data.emplace_back(addr[i] & 0xff);
                    data.emplace_back((addr[i] & 0xff00) >> 8);
                    data.emplace_back(0x00);

                    data.emplace_back(value[i] & 0xff);
                    data.emplace_back((value[i] & 0xff00) >> 8);
                    data.emplace_back((value[i] & 0xff0000) >> 16);
                    data.emplace_back((value[i] & 0xff000000) >> 24);
#if 0
                    AWARN << "get servo double read " << des <<
                        "-" << std::setw(3) << std::setfill('0') << i <<
                        ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                        "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                        " ]" << ", write value: " << std::hex << value[i];
#endif
                    td.emplace_back<std::tuple<const int,
                            const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
                }

                return true;
            }
            //double write registers
            bool FillDataContainerFreeWrite(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const std::string& des,
                    const int16_t* addr,
                    const int* value,
                    const int size) {
                std::vector<uint8_t> data;

                if (addr == nullptr || size <= 0) {
                    AERROR << "fill motor data adress / size error";
                    return false;
                }

                for (int i = 0; i < size; i++) {
                    data.clear();
                    data.emplace_back(DS_MOTOR_WRITE_BOTH_CMD);
                    data.emplace_back(addr[i] & 0xff);
                    data.emplace_back((addr[i] & 0xff00) >> 8);
                    data.emplace_back(0x00);

                    data.emplace_back((value[i] & 0xff0000) >> 16);
                    data.emplace_back((value[i] & 0xff000000) >> 24);
                    data.emplace_back(value[i] & 0xff);
                    data.emplace_back((value[i] & 0xff00) >> 8);
#if 1
                    AWARN << "get servo double write " << des <<
                        "-" << std::setw(3) << std::setfill('0') << i <<
                        ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                        "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                        " ]" << ", write value: " << std::hex << value[i];
#endif
                    td.emplace_back<std::tuple<const int,
                            const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////

            bool FillDataContainerCanOpenConfig(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const uint8_t* cmd,
                    const int16_t* addr,
                    const uint8_t* sub_id,
                    const int* value,
                    const int size) {
                std::vector<uint8_t> data;

                if (addr == nullptr || size <= 0) {
                    AERROR << "fill motor data adress / size error";
                    return false;
                }

                for (int i = 0; i < size; i++) {
                    data.clear();

                    data.emplace_back(cmd[i]);
                    data.emplace_back(addr[i] & 0xff);
                    data.emplace_back((addr[i] & 0xff00) >> 8);
                    data.emplace_back(sub_id[i]);

                    data.emplace_back((value[i] & 0xff0000) >> 16);
                    data.emplace_back((value[i] & 0xff000000) >> 24);
                    data.emplace_back(value[i] & 0xff);
                    data.emplace_back((value[i] & 0xff00) >> 8);

                    td.emplace_back<std::tuple<const int,
                            const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1, data});
                    td.emplace_back<std::tuple<const int,
                            const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2, data});
#if 1
                    AWARN << "get servo can config " <<
                        " " << std::setw(3) << std::setfill('0') << i <<
                        ", addr: " << std::hex << std::setw(4) << std::setfill('0') << addr[i] <<
                        "[ " << std::setw(2) << std::setfill('0') << static_cast<int>(data[0]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[1]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[2]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[3]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[4]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[5]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[6]) <<
                        " " << std::setw(2) << std::setfill('0') << static_cast<int>(data[7]) <<
                        " ]" << ", write value: " << std::hex << value[i];
#endif
                }

                return true;
            }
            void FillDataContainerCanOpenActivate(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //activate axle
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {0, {01, 01}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {0, {01, 02}});
                AWARN << "config activate motor " <<
                    " id 0x00, data[0x01, 0x01 / 0x01, 0x02]";
            }
            void FillDataContainerCanOpenStartReporting(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //stop reporting data
                //speed & encoder
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2f, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2f, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
                //status
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2f, 0x01, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2f, 0x01, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00}});

                AWARN << "config start reporting data" <<
                    " id 0x00, data[0x02, 0x01 / 0x02, 0x02]";
            }
            void FillDataContainerCanOpenStopReporting(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //start reporting data
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2f, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2f, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
                //status
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2f, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2f, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00}});
                AWARN << "config stop reporting data" <<
                    " id 0x00, data[0x02, 0x01 / 0x02, 0x02]";
            }
            void FillDataContainerCanOpenSync(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //sync
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {0x80, {}});
                AWARN << "config sync canOpen" <<
                    " id 0x80, data[]";
            }
            void FillDataContainerCanOpenResetStatus(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //reset speed
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x23, 0x69, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x23, 0x69, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
                //reset encoder
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x23, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x23, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
                AWARN << "config canOpen reset speed & encoder";
            }
            void FillDataContainerCanOpenAxleSpeed(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const int32_t left,
                    const int32_t right) {
                //setting canopen speed
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
#if 1
                            {0x401, {
#else
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1, {0x23, 0xff, 0x60, 0x00,
#endif
                                (uint8_t)((~right + 1) & 0xff),
                                (uint8_t)(((~right + 1) & 0xff00) >> 8),
                                (uint8_t)(((~right + 1) & 0xff0000) >> 16),
                                (uint8_t)(((~right + 1) & 0xff000000) >> 24)}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
#if 1
                            {0x402, {
#else
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2, {0x23, 0xff, 0x60, 0x00,
#endif
                                (uint8_t)(left & 0xff),
                                (uint8_t)((left & 0xff00) >> 8),
                                (uint8_t)((left & 0xff0000) >> 16),
                                (uint8_t)((left & 0xff000000) >> 24)}});
#if 0
                //read speed
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x43, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x43, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}});
#endif
                AWARN << "canOpen speed setting left: " << left <<
                    ", right: " << right;
            }
            void FillDataContainerCanOpenAxleEnable(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //axle enable
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00}});
                AWARN << "motor axle enable" <<
                    " id " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_1 <<
                    " / " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_2 <<
                    ", data[0x2B, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00]";
            }
            void FillDataContainerCanOpenAxleDisable(std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td) {
                //axle disable
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_1,
                            {0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00}});
                td.emplace_back<std::tuple<const int,
                    const std::vector<uint8_t>>>(
                            {DS_MOTOR_DRIVE_MOTOR_AXLE_2,
                            {0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00}});
                AWARN << "motor axle disable" <<
                    " id " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_1 <<
                    " / " << std::hex << DS_MOTOR_DRIVE_MOTOR_AXLE_2 <<
                    ", data[0x2B, 0x40, 0x60, 0x00, 0x06/0x07, 0x00, 0x00, 0x00]";

            }
    };

} //namespace parser
} //namespace chss
} //namespace mstf
