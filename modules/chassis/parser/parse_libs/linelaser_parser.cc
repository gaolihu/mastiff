#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/linelaser_parser.h"

namespace mstf {
namespace chss {
namespace parser {

using namespace ventura;
using namespace common_msgs;

LineLaserParser::LineLaserParser(const
        SensorIndicator& si) :
    ParserBaseItf(si) {
    AINFO << "LineLaserParser construct";

    ParseDrvLink::Instance()->RegisterUartListener
        (std::bind(&ParserBaseItf::OnOriginalDataUart,
                   this, ::_1, ::_2, ::_3), s_idc_);
}

LineLaserParser::~LineLaserParser() {
    AINFO << "LineLaserParser de-construct";
}

int LineLaserParser::Init() {
    ParseDrvLink::Instance()->Init(s_idc_);

    return ParserBaseItf::Init();
}

    int LineLaserParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "LineLaserParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int LineLaserParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "LineLaserParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int LineLaserParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "LineLaserParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int LineLaserParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "LineLaserParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

int LineLaserParser::WriteUartMessage(const
        std::vector<uint8_t>& data) {
    if (ParseDrvLink::Instance()->
            WriteUart(s_idc_, data) ==
            data.size()) {
        return 0;
    }

    return -1;
}

int LineLaserParser::ParseUartBuffer(const SensorIndicator* si, const uint8_t* buf, const size_t len) {
    int min_len  = 9;  // 一帧数据最小长度
    int cbuf_len = cbuf_raw_->CbufSize();
    if (cbuf_len < min_len) {
        return 0;
    }

    std::vector<uint8_t> raw_data(cbuf_len, 0);

    if (!cbuf_raw_->ReadSafely(raw_data)) {
        AFATAL << "read circular buffer for Line Laser failed";
        return -1;
    }

    int head     = 0;
    int left_len = cbuf_len;  // 组成一帧数据还需要多少长度
    for (head = 0; head < cbuf_len && left_len >= min_len; left_len = cbuf_len - head) {
        if (raw_data[head] == LIDAR_CMD_SYNC_BYTE && raw_data[head + 1] == LIDAR_CMD_SYNC_BYTE && raw_data[head + 2] == LIDAR_CMD_SYNC_BYTE
            && raw_data[head + 3] == LIDAR_CMD_SYNC_BYTE) {
            GSHeaderInfo gs_header;
            memcpy(&gs_header, &raw_data[head + 4], sizeof(GSHeaderInfo));
            int total_len = 4 + sizeof(GSHeaderInfo) + gs_header.data_len + 1;

            if (left_len < total_len) {
                // 剩余长度不够一帧
                break;
            }

            std::vector<uint8_t> single_buf(total_len, 0);
            memcpy(single_buf.data(), &raw_data[head], total_len);

            head += total_len;
            cbuf_raw_->Restore(total_len);

            switch (gs_header.cmd_type) {
            case GS_LIDAR_CMD_GET_ADDRESS:
                // 地址信息
                ParseAddress(single_buf, total_len);
                break;
            case GS_LIDAR_CMD_STOP:
                // 停止扫描
                break;
            case GS_LIDAR_CMD_GET_PARAMETER:
                // 获取参数
                ParseCameraParam(single_buf, total_len);
                break;
            case GS_LIDAR_CMD_GET_VERSION:
                // 版本信息
                break;
            case GS_LIDAR_ANS_SCAN:
                // 点云数据
                ParseSigleFrame(single_buf, total_len);
                break;
            default:
                AINFO << "get gs data wait todo, len: " << single_buf.size();
                break;
            }
        }
        else {
            head++;
            cbuf_raw_->Restore(1);
        }
    }

    // clang-format off
#if 0
        AINFO << "ParseRawBuffer len ~ " <<
            len << ", [" <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[0]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[1]) <<
            " | " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[2]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[3]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[4]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[5]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[6]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[7]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[8]) <<
            " " << std::hex << std::setw(2)<< std::setfill('0') << static_cast<int>(buf[9]) <<
            " ]";
#endif
        //if the current frame is good enough to parse
        //a whole frame of data, send upstream directly
    // clang-format on

    return 0;
}

//|  4  |  1   |   1    |    2    |   n    | 1   |
//| 包头 | 地址 | 命令类型 | 数据长度 | 点云数据 | crc |
//-------------------------------------------------
//|    2    |      2*(n-1)      |
//| 环境信息 |  高7位强度，低9位距离 |
int LineLaserParser::ParseSigleFrame(const std::vector<uint8_t>& data, const size_t len) {
    if (!have_params_) {
        // 还没读取到相机参数，无法解析数据
        // TODO: 发送读取配置消息
        return -1;
    }
    if (!CheckCrc(data, len)) {
        return -1;
    }

    ventura::common_msgs::sensor_msgs::PointCloud2 response;

    size_t             point_idx   = 0;
    bool               get_a_frame = false;
    std::vector<float> point_cloud;
    for (size_t i = 10; i < len - 1; i += 2) {
        uint16_t point = 0;
        memcpy(&point, &data[i], 2);
        //uint8_t  intensity = point >> 9;
        uint16_t distance  = (point & 0x01FF);
        // AINFO << "line laser intensity: " << (int)intensity << "  distance: " << (int)distance << " mm";

        double   theta   = 0.0;
        float    dist_m  = 0.0;
        uint16_t dist_mm = 0.0;

        AngTransform(distance, point_idx, &theta, &dist_mm);
        // AINFO << "idx point: " << point_idx << " 角度 " << theta << " 距离 " << (int)dist_mm;

        // 角度转换成弧度
        theta  = M_PI_2 - (theta / 180.0) * M_PI;
        dist_m = (float)dist_mm / 1000.0;

        // AINFO_EVERY(80) << "theta: " << theta << " distance: " << dist_m;
        // 极坐标转换为线激光的笛卡尔坐标， 单位 mm 转成 m
        float x = dist_m * (float)sin(theta);
        float y = dist_m * (float)cos(theta);
        float z = 0.0f;
        point_cloud.push_back(x);
        point_cloud.push_back(y);
        point_cloud.push_back(z);

        point_idx++;
        get_a_frame = true;
    }

    if (!get_a_frame) {
        return 0;
    }

    //header
    std_msgs::Header header;
    header.set_seq(frame_seq_);
    header.set_frame_id("map");
    header.mutable_stamp()->set_sec(cyber::Time::Now().ToSecond());
    header.mutable_stamp()->set_nsec(cyber::Time::Now().ToNanosecond());
    response.mutable_header()->CopyFrom(header);

    // meta data
    size_t pc2_buffer_len = point_idx * sizeof(float) * 3;
    response.set_width(point_idx);
    response.set_height(1);
    response.set_point_step(sizeof(float) * 3);
    response.set_row_step(pc2_buffer_len);
    response.set_is_dense(true);
    response.set_is_bigendian(false);

    //field
    auto field1 = response.add_fields();
    field1->set_name("x");
    field1->set_offset(0);
    field1->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field1->set_count(1);
    auto field2 = response.add_fields();
    field2->set_name("y");
    field2->set_offset(4);
    field2->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field2->set_count(1);
    auto field3 = response.add_fields();
    field3->set_name("z");
    field3->set_offset(8);
    field3->set_datatype(sensor_msgs::PointField::TYPE::PointField_TYPE_FLOAT32);
    field3->set_count(1);

    std::string pc2_data(reinterpret_cast<const char*>(point_cloud.data()), pc2_buffer_len);
    response.set_data(pc2_data);

    // send
    if (frame_processor_sp_) {
        AINFO_EVERY(200) << "send a line laser frame point cloud 2";
        return frame_processor_sp_(std::make_shared<ventura::common_msgs::sensor_msgs::PointCloud2>(response),
                CommonItf::Instance()->GetTopic1(s_idc_));
    }
    else {
        AFATAL << "No line laser publisher handle";
    }

    frame_seq_++;

    return point_idx;  // 一帧点的数量
}
void LineLaserParser::AngTransform(uint16_t dist, int n, double* dstTheta, uint16_t* dstDist) {
    double pixelU    = n, Dist, theta, tempTheta, tempDist, tempX, tempY;
    //TODO:
    int    nodeCount = 160;
    int    moduleNum = 1;

    uint8_t mdNum = 0x03 & (moduleNum >> 1);  //1,2,4
    if (n < nodeCount / 2) {
        pixelU = nodeCount / 2 - pixelU;
        if (b0[mdNum] > 1) {
            tempTheta = k0[mdNum] * pixelU - b0[mdNum];
        }
        else {
            tempTheta = atan(k0[mdNum] * pixelU - b0[mdNum]) * 180 / M_PI;
        }
        tempDist  = (dist - Angle_Px) / cos(((Angle_PAngle + bias[mdNum]) - (tempTheta)) * M_PI / 180);
        tempTheta = tempTheta * M_PI / 180;
        tempX     = cos((Angle_PAngle + bias[mdNum]) * M_PI / 180) * tempDist * cos(tempTheta)
              + sin((Angle_PAngle + bias[mdNum]) * M_PI / 180) * (tempDist * sin(tempTheta));
        tempY = -sin((Angle_PAngle + bias[mdNum]) * M_PI / 180) * tempDist * cos(tempTheta)
              + cos((Angle_PAngle + bias[mdNum]) * M_PI / 180) * (tempDist * sin(tempTheta));
        tempX = tempX + Angle_Px;
        tempY = tempY - Angle_Py;  //5.315
        Dist  = sqrt(tempX * tempX + tempY * tempY);
        theta = atan(tempY / tempX) * 180 / M_PI;
    }
    else {
        pixelU = nodeCount - pixelU;
        if (b1[mdNum] > 1) {
            tempTheta = k1[mdNum] * pixelU - b1[mdNum];
        }
        else {
            tempTheta = atan(k1[mdNum] * pixelU - b1[mdNum]) * 180 / M_PI;
        }
        tempDist  = (dist - Angle_Px) / cos(((Angle_PAngle + bias[mdNum]) + (tempTheta)) * M_PI / 180);
        tempTheta = tempTheta * M_PI / 180;
        tempX     = cos(-(Angle_PAngle + bias[mdNum]) * M_PI / 180) * tempDist * cos(tempTheta)
              + sin(-(Angle_PAngle + bias[mdNum]) * M_PI / 180) * (tempDist * sin(tempTheta));
        tempY = -sin(-(Angle_PAngle + bias[mdNum]) * M_PI / 180) * tempDist * cos(tempTheta)
              + cos(-(Angle_PAngle + bias[mdNum]) * M_PI / 180) * (tempDist * sin(tempTheta));
        tempX = tempX + Angle_Px;
        tempY = tempY + Angle_Py;  //5.315
        Dist  = sqrt(tempX * tempX + tempY * tempY);
        theta = atan(tempY / tempX) * 180 / M_PI;
    }
    if (theta < 0) {
        theta += 360;
    }
    *dstTheta = theta;
    *dstDist  = Dist;
}
void LineLaserParser::ParseAddress(const std::vector<uint8_t>& param, const size_t len) {
    if (!CheckCrc(param, len)) {
        return;
    }

    if (len != 9) {
        AERROR << "get gs address len incorrect: " << len;
        return;
    }

    //uint8_t address = param[4];

    //AINFO << "the line laser address is: " << (int)address << " it's name: " << ssi_.name() << " information: " << ssi_.information();
}
void LineLaserParser::ParseCameraParam(const std::vector<uint8_t>& param, const size_t len) {
    if (!CheckCrc(param, len)) {
        return;
    }

    if (len != 18) {
        AERROR << "get gs camera param len incorrect: " << len;
        return;
    }

    //CreateAddressIndexMap(ssi_.name());

    uint8_t address = param[4];
    int     mdNum   = address >> 1;  // 1,2,4

    gs_device_para info;
    memcpy(&info, &param[8], sizeof(gs_device_para));

    k0[mdNum]   = info.k0 / 10000.00;
    k1[mdNum]   = info.k1 / 10000.00;
    b0[mdNum]   = info.b0 / 10000.00;
    b1[mdNum]   = info.b1 / 10000.00;
    bias[mdNum] = double(info.bias) * 0.1;

    have_params_ = true;

    AINFO << "Get camera param k0: " << k0[mdNum];
    AINFO << "Get camera param k1: " << k1[mdNum];
    AINFO << "Get camera param b0: " << b0[mdNum];
    AINFO << "Get camera param b1: " << b1[mdNum];
    AINFO << "Get camera param bias: " << bias[mdNum];
    AINFO << "gs address: " << (int)address;
}
bool LineLaserParser::CheckCrc(const std::vector<uint8_t>& data, const size_t len) {
    uint8_t sum = 0;
    for (size_t i = 4; i < len - 1; i++) {
        sum += data[i];
    }

    if (sum != data[len - 1]) {
        // 数据校验失败
        return false;
    }
    return true;
}
size_t LineLaserParser::CreateAddressIndexMap(const std::string& name) {
    size_t id = 0;
    for (auto it = address_index_map_.begin(); it != address_index_map_.end(); it++) {
        if (it->first == name) {
            id = it->second;
            return id;
        }

        id++;
    }

    // 不在map里，插入
    address_index_map_[name] = id;
    return id;
}

}  //namespace parser
}  //namespace chss
}  //namespace mstf
