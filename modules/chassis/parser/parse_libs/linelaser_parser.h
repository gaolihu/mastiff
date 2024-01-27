#pragma once

#include <unordered_map>

#include "modules/chassis/parser/parse_libs/ydlidar_protocol.h"
#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

#pragma pack(1)
//雷达节点信息
struct GSHeaderInfo {
    uint8_t  address;
    uint8_t  cmd_type;
    uint16_t data_len;
};

//GS设备参数
struct gs_device_para {
    uint16_t k0;
    uint16_t b0;
    uint16_t k1;
    uint16_t b1;
    int8_t bias;
};

#pragma pack()


class LineLaserParser : public ParserBaseItf {
public:
    LineLaserParser(const
            SensorIndicator&);
    virtual ~LineLaserParser() final;

    virtual int Init() override;
    virtual int Start() override;
    virtual int Stop() override;
    virtual int Resume() override;
    virtual int Close() override;

    virtual int WriteUartMessage(const
            std::vector<uint8_t>&) override;

private:
    virtual int ParseUartBuffer(const SensorIndicator*, const uint8_t *, const size_t) override;
    virtual int ParseSigleFrame(const std::vector<uint8_t> &, const size_t) override;

    void ParseAddress(const std::vector<uint8_t> &, const size_t);
    void ParseCameraParam(const std::vector<uint8_t> &, const size_t);
    void AngTransform(uint16_t dist, int n, double *dstTheta, uint16_t *dstDist);
    bool CheckCrc(const std::vector<uint8_t> &, const size_t);

    /**
     * @brief 如果有串联方式连接，需要建立地址映射
     *
     * @param name
     * @return size_t
     */
    size_t CreateAddressIndexMap(const std::string& name);

    // 相机参数
    double k0[PackageMaxModuleNums];
    double k1[PackageMaxModuleNums];
    double b0[PackageMaxModuleNums];
    double b1[PackageMaxModuleNums];
    double bias[PackageMaxModuleNums];
    bool have_params_{false};
    size_t frame_seq_{0};

    //如果有串联方式连接，需要建立地址映射,地址从传感器里面读取
    std::unordered_map<std::string, size_t> address_index_map_;
};

}  //namespace parser
}  //namespace chss
}  //namespace mstf
