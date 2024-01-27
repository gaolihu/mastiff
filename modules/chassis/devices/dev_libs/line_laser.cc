#include "modules/chassis/parser/parse_libs/linelaser_parser.h"
#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/parser/pack_libs/llaser_packer.h"
#include "modules/chassis/devices/dev_libs/line_laser.h"

namespace mstf {
namespace chss {
namespace device {

    LineLaser::LineLaser(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser construct";
#endif
        data_packer_ = std::make_unique
            <LlaserPacker>(idc.ihi().name());
        data_parser_ = std::make_unique
            <LineLaserParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    LineLaser::~LineLaser() {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser de-construct";
#endif
    }

    int LineLaser::Init(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser init";
#endif
        DeviceBaseItf::Init();

        LineLaserSetting lst;
        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_INIT);
        std::vector<uint8_t> data =
            data_packer_->PackLlaserRawSetting(lst);

        return data_parser_->WriteUartMessage(data);
    }

    int LineLaser::Start(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser start";
#endif
        DeviceBaseItf::Start();

        LineLaserSetting lst;
        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_STOP);
        auto data = data_packer_->PackLlaserRawSetting(lst);
        data_parser_->WriteUartMessage(data);

        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_RAW);
        lst.mutable_dev_manage()->set_cmd(0x60);
        data = data_packer_->PackLlaserRawSetting(lst);
        data_parser_->WriteUartMessage(data);

        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_RAW);
        lst.mutable_dev_manage()->set_cmd(0x61);
        data = data_packer_->PackLlaserRawSetting(lst);
        data_parser_->WriteUartMessage(data);

        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_RAW);
        lst.mutable_dev_manage()->set_cmd(0x62);
        data = data_packer_->PackLlaserRawSetting(lst);
        data_parser_->WriteUartMessage(data);

        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_START);
        data = data_packer_->PackLlaserRawSetting(lst);
        return data_parser_->WriteUartMessage(data);
    }

    int LineLaser::Stop(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "LineLaser stop";
#endif
        LineLaserSetting lst;
        lst.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_STOP);
        auto data = data_packer_->PackLlaserRawSetting(lst);
        data_parser_->WriteUartMessage(data);

        return DeviceBaseItf::Stop();
    }

} //namespace device
} //namespace chss
} //namespace mstf
