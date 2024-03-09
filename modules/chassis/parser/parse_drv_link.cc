#include "modules/chassis/parser/parse_drv_link.h"

using namespace std::placeholders;

namespace mstf {
namespace chss {
namespace parser {

    ParseDrvLink::ParseDrvLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink sigleton construct";
#endif
        if (CommonItf::Instance()->HasSocDevice()) {
            soc_data_ = std::make_shared<SocData>
                (std::bind(&ParseDrvLink::OnRecvSoc,
                           this, ::_1, ::_2, ::_3, ::_4),
                 CommonItf::Instance()->GetChasisConfig());
        }
    }

    ParseDrvLink::~ParseDrvLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink sigleton de-construct";
#endif
    }

    int ParseDrvLink::Init(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "bind specific driver & channel: " <<
            si->ihi().name();
#endif
        //1, CAN dev: servo motor & MCU
        if (CommonItf::Instance()->GetDevCanConf(si)) {
            AINFO << "bind CAN driver with\n" <<
                si->DebugString();
            dev_can_data_[si] = std::make_shared<CanData>
                (std::bind(&ParseDrvLink::OnRecvCan,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevCanConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_can_data_[si]);
            return dev_can_data_[si]->Init();
        }

        //2, UART dev: line laser & lidar & imu & mcu
        if (CommonItf::Instance()->GetDevUartConf(si)) {
            AINFO << "bind UART driver with\n" <<
                si->DebugString();
            dev_serial_data_[si] = std::make_shared<SerialData>
                (std::bind(&ParseDrvLink::OnRecvUart,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevUartConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_serial_data_[si]);
            return dev_serial_data_[si]->Init();
        }

        //3, I2C dev: imu
        if (CommonItf::Instance()->GetDevI2cConf(si)) {
            AINFO << "bind IIC driver with\n" <<
                si->DebugString();
            dev_iic_data_[si] = std::make_shared<IIcData>
                (std::bind(&ParseDrvLink::OnRecvIIc,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevI2cConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_iic_data_[si]);
            return dev_iic_data_[si]->Init();
        }

        //3, SPI dev: TODO
        //
        //4, SOC dev: joystick & audio & wifi & 4g
        if (CommonItf::Instance()->IsLacateSoc(si)) {
            AINFO << "bind SOC driver with " <<
                si->ihi().name();
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(soc_data_);
            return soc_data_->Init(si);
        }

        AERROR << "bind driver errror\n" <<
            si->DebugString();

        return -1;
    }

    int ParseDrvLink::Start(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink start device: " <<
            si->ihi().name();
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Start(si);
        }

        AWARN << "Start TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Stop(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink stop device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Stop(si);
        }

        AWARN << "Stop TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Resume(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink resume device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Resume(si);
        }

        AWARN << "Resume TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Close(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink close device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Close(si);
        }

        AWARN << "Close TODO for\n" << si->DebugString();

        return -1;
    }

    //write data downwards
    bool ParseDrvLink::WriteSoc(const SensorIndicator* si,
            const Message& info) {
        return soc_data_->SocWrite(si, info);
    }

    size_t ParseDrvLink::WriteUart(const SensorIndicator* si,
            const uint8_t* info,
            const size_t len) {
#if 1
        std::ostringstream oss;
        for (size_t i = 0; i < len; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(info[i]) << " ";
        }
        AINFO << "write UART: [ " << oss.str() << " ]";
#endif
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for UART: " << len;
            return 0;
        }
        if (auto s = _FindSerialData(si)) {
            return s->WritePort(info, len);
        }

        AERROR << "write UART error!";

        return 0;
    }

    size_t ParseDrvLink::WriteCan(const SensorIndicator* si,
            const int id,
            const uint8_t* info,
            const size_t len) {
#if 1
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for CAN: " << len;
            return 0;
        }
#endif
        if (auto s = _FindCanData(si)) {
            return s->WritePort(id, info, len);
        }

        return -1;
    }

    size_t ParseDrvLink::WriteI2c(const SensorIndicator* si,
            const uint8_t* info,
            const size_t len) {
#if 0
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for I2C: " << len;
            return 0;
        }
#endif
        if (auto s = _FindI2cData(si)) {
            return s->WritePort(info, len);
        }

        return -1;
    }

    //on receive data upwards
    void ParseDrvLink::OnRecvCan(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _CanMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvUart(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _UartMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvIIc(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _IIcMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvSoc(const SensorIndicator* si,
            const Message& msg,
            const uint8_t* buf,
            const size_t len) {
#ifdef CHSS_PKG_DBG
        //AINFO << "receive soc data: " << msg.DebugString();
#endif
        _SocMessageHandle(si, msg, buf, len);
    }

} //namespace parser
} //namespace chss
} //namespace mstf
