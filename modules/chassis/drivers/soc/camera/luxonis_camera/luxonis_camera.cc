#include "cyber/cyber.h"
#include "cyber/common/log.h"

#include "depthai/depthai.hpp"

#include "modules/chassis/drivers/soc/camera/luxonis_camera/luxonis_camera.h"

namespace mstf {
namespace chss {
namespace driver {

    static std::unordered_map<dai::RawImgFrame::Type, std::string>
        encodingEnumMap =
        {
            {dai::RawImgFrame::Type::YUV422i, "yuv422"},
            {dai::RawImgFrame::Type::RGBA8888, "rgba8"},
            {dai::RawImgFrame::Type::RGB888i, "rgb8"},
            {dai::RawImgFrame::Type::BGR888i, "bgr8"},
            {dai::RawImgFrame::Type::GRAY8, "mono8"},
            {dai::RawImgFrame::Type::RAW8, "mono8"},
            {dai::RawImgFrame::Type::RAW16, "16UC1"},
            {dai::RawImgFrame::Type::YUV420p, "YUV420"}
        };
    static std::unordered_map<dai::RawImgFrame::Type, std::string>
        planarEncodingEnumMap = {
            {dai::RawImgFrame::Type::BGR888p, "rgb8"},
            {dai::RawImgFrame::Type::RGB888p, "rgb8"},
            {dai::RawImgFrame::Type::NV12, "rgb8"},
            {dai::RawImgFrame::Type::YUV420p, "rgb8"}
        };

    static dai::Pipeline pipeline_;
    static std::shared_ptr<dai::Device> device_ {};

    static void Convert2RosMsg(const
            std::shared_ptr<dai::ImgFrame>& dai_img_in,
            ventura::common_msgs::sensor_msgs::Image* img_out,
            double sec, uint64_t nsec, size_t seq) {
        img_out->mutable_header()->set_seq(seq);
        img_out->mutable_header()->set_frame_id("oak-camera");
        img_out->mutable_header()->mutable_stamp()->set_sec(sec);
        img_out->mutable_header()->mutable_stamp()->set_nsec(nsec);

        if(planarEncodingEnumMap.find(dai_img_in->getType()) !=
                planarEncodingEnumMap.end()) {
            int type = 0;
            cv::Mat mat, cv_mat_out;
            cv::Size size = {0, 0};
            switch(dai_img_in->getType()) {
                case dai::RawImgFrame::Type::BGR888p:
                case dai::RawImgFrame::Type::RGB888p:
                    size = cv::Size(dai_img_in->getWidth(), dai_img_in->getHeight());
                    type = CV_8UC3;
                    break;
                case dai::RawImgFrame::Type::YUV420p:
                case dai::RawImgFrame::Type::NV12:
                    size = cv::Size(dai_img_in->getWidth(), dai_img_in->getHeight() * 3 / 2);
                    type = CV_8UC1;
                    break;

                default:
                    std::runtime_error("Invalid dataType inputs..");
                    break;
            }
            mat = cv::Mat(size, type, dai_img_in->getData().data());

            switch(dai_img_in->getType()) {
                case dai::RawImgFrame::Type::RGB888p: {
                    cv::Size s(dai_img_in->getWidth(), dai_img_in->getHeight());
                    std::vector<cv::Mat> channels;
                    // RGB
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 2));
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 1));
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 0));
                    cv::merge(channels, cv_mat_out);
                } break;

                case dai::RawImgFrame::Type::BGR888p: {
                    cv::Size s(dai_img_in->getWidth(), dai_img_in->getHeight());
                    std::vector<cv::Mat> channels;
                    // BGR
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 0));
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 1));
                    channels.push_back(cv::Mat(s, CV_8UC1, dai_img_in->getData().data() + s.area() * 2));
                    cv::merge(channels, cv_mat_out);
                } break;

                case dai::RawImgFrame::Type::YUV420p:
                    cv::cvtColor(mat, cv_mat_out, cv::ColorConversionCodes::COLOR_YUV2BGR_IYUV);
                    break;

                case dai::RawImgFrame::Type::NV12:
                    cv::cvtColor(mat, cv_mat_out, cv::ColorConversionCodes::COLOR_YUV2BGR_NV12);
                    break;

                default:
                    cv_mat_out = mat.clone();
                    break;
            }
            /*
             * TODO
            cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8,
                    cv_mat_out).toImageMsg(img_out);
                    */
            img_out->set_height(cv_mat_out.rows);
            img_out->set_width(cv_mat_out.cols);
            img_out->set_encoding("CV_8UC3");
            img_out->set_is_bigendian(false);
            img_out->set_step(cv_mat_out.cols * cv_mat_out.elemSize());
            size_t sz = img_out->step() * cv_mat_out.rows;
            std::string img_str;
            if (cv_mat_out.isContinuous()) {
                //
                img_str = std::string(static_cast<const char *>
                        ((char*)cv_mat_out.data), sz);
            } else {
                // Copy by row by row
                uchar* ros_data_ptr = (uchar*)(img_str.data());
                uchar* cv_data_ptr = cv_mat_out.data;
                for (int i = 0; i < cv_mat_out.rows; ++i) {
                    memcpy(ros_data_ptr, cv_data_ptr, img_out->step());
                    ros_data_ptr += img_out->step();
                    cv_data_ptr += cv_mat_out.step;
                }
            }
            img_out->set_data(img_str);
        } else if(encodingEnumMap.find(dai_img_in->getType()) !=
                encodingEnumMap.end()) {
            std::string temp_str(encodingEnumMap[dai_img_in->getType()]);
            img_out->set_encoding(temp_str);
            img_out->set_height(dai_img_in->getHeight());
            img_out->set_width(dai_img_in->getWidth());
            img_out->set_step(dai_img_in->getData().size() /
                    dai_img_in->getHeight());
            if(img_out->encoding() == "16UC1")
                img_out->set_is_bigendian(false);
            else
                img_out->set_is_bigendian(true);

            size_t size = dai_img_in->getData().size();
            std::string img_str = std::string(static_cast<const char *>
                    ((char*)dai_img_in->getData().data()), size);
            img_out->set_data(img_str);
        }
    }
    //////////////////////////////////////////////////////////////////////////////

    int LuxonisCamera::Init() {
        auto monoLeft = pipeline_.create<dai::node::MonoCamera>();
        auto xoutLeft = pipeline_.create<dai::node::XLinkOut>();

        auto monoRight = pipeline_.create<dai::node::MonoCamera>();
        auto xoutRight = pipeline_.create<dai::node::XLinkOut>();

        auto camRgb = pipeline_.create<dai::node::ColorCamera>();
        auto xoutRgb = pipeline_.create<dai::node::XLinkOut>();

        xoutLeft->setStreamName("left");
        xoutRight->setStreamName("right");
        xoutRgb->setStreamName("rgb");

        // Properties
        monoLeft->setCamera("left");
        monoLeft->setResolution(dai::MonoCameraProperties
                ::SensorResolution::THE_720_P);
        monoRight->setCamera("right");
        monoRight->setResolution(dai::MonoCameraProperties
                ::SensorResolution::THE_720_P);
        camRgb->setPreviewSize(1280, 720);
        camRgb->setBoardSocket(dai::CameraBoardSocket::CAM_A);
        camRgb->setResolution(dai::ColorCameraProperties
                ::SensorResolution::THE_1080_P);
        camRgb->setInterleaved(false);
        camRgb->setColorOrder(dai::ColorCameraProperties
                ::ColorOrder::RGB);

        // Linking
        monoRight->out.link(xoutRight->input);
        monoLeft->out.link(xoutLeft->input);
        camRgb->preview.link(xoutRgb->input);

        AINFO << "init OAK pipeline ok ";

        return 0;
    }

    int LuxonisCamera::Start() {
        AINFO << "start oak camera: " << dev_;

        // Connect to device and start pipeline
        if (!device_) {
            device_ = std::make_shared<dai::Device>(pipeline_, dai::UsbSpeed::SUPER);
        } else {
            return -1;
        }


        AINFO << "Connected cameras: " <<
            device_->getConnectedCameraFeatures();

        // Print USB speed
        AINFO << "Usb speed: " << device_->getUsbSpeed();

        // Bootloader version
        if(device_->getBootloaderVersion()) {
            AINFO << "Bootloader version: " <<
                device_->getBootloaderVersion()->toString();
        }

        // Device name
        AINFO << "Set device OK, device: " << device_->getDeviceName() <<
            ", product: " << device_->getProductName();

        return 0;
    }

    int LuxonisCamera::Stop() {
        AINFO << "stop oak camera: " << dev_;
        return 0;
    }

    int LuxonisCamera::Resume() {
        AINFO << "resume oak camera: " << dev_;
        return 0;
    }

    int LuxonisCamera::Close() {
        AINFO << "close oak camera: " << dev_;
        return 0;
    }

    int LuxonisCamera::PollingCameraRutine(CameraPopDatas& data) {
        auto qLeft = device_->getOutputQueue("left", 4, false);
        auto qRight = device_->getOutputQueue("right", 4, false);
        auto qRgb = device_->getOutputQueue("rgb", 4, false);

        auto inLeft = qLeft->tryGet<dai::ImgFrame>();
        auto inRight = qRight->tryGet<dai::ImgFrame>();
        auto inRgb = qRgb->get<dai::ImgFrame>();

        auto sec  = apollo::cyber::Time::Now().ToSecond();
        auto nsec = apollo::cyber::Time::Now().ToNanosecond() %
            static_cast<uint64_t>(1e9);

        if (inLeft || inRight || inRgb) {
            frame_seq_++;
        }

        if(inLeft) {
            Convert2RosMsg(inLeft, data.mutable_mono_l(), sec, nsec, frame_seq_);
        }

        if(inRight) {
            Convert2RosMsg(inRight, data.mutable_mono_r(), sec, nsec, frame_seq_);
        }

        if(inRgb){
            Convert2RosMsg(inRgb, data.mutable_rgb(), sec, nsec, frame_seq_);
            /*
            cv::Mat img = inRgb->getCvFrame();
            AINFO << "get rgb frame row: " << img.rows <<
                ", col: " << img.cols;
                */
        }

        std::this_thread::sleep_for(
                std::chrono::milliseconds(10));

        return 0;
    }

}  // namespace driver
}  // namespace chss
}  // namespace mstf
