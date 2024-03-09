#pragma once

#include "cyber/common/log.h"
#include "cyber/time/clock.h"
#include "cyber/timer/timer.h"
#include "cyber/message/message_traits.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"

#include "modules/cambrian/brain/sys_processor.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace google::protobuf;

    class BotItfImpl {
        public:
            BotItfImpl(BotItfImpl&) = delete;
            BotItfImpl(const BotItfImpl&) = delete;
            BotItfImpl& operator=(const BotItfImpl&) = delete;

            BotItfImpl(const std::shared_ptr<CambrianConfig>& cc) {
#ifdef CAMB_PKG_DBG
                AINFO << "ROBOT impl construct";
#endif
                cmb_conf_ = cc;

                sys_processor_ =
                    std::make_unique<SysProcessor>(cc);
#ifdef CAMB_PKG_DBG
                AINFO << "sub-bot-components finish!";
#endif
            }

            virtual ~BotItfImpl() {
#ifdef CAMB_PKG_DBG
                AINFO << "ROBOT impl de-construct";
#endif
            }

            //common
            virtual void Init(const BaseMsgCaller& ex) {
#ifdef CAMB_PKG_DBG
                AINFO << "register data export handles";
                //=&Transactor::OnTransferMessageOut()
                msg_publish_ = ex;
#endif
                BuildUpwardDataChannel();
                terminate_ = false;
#if USE_CYBER_TIMER
                tim_.reset(new cyber::Timer(cmb_conf_.lock()->
                        cmb_module_conf().modeule_entry_freq(),
                        [&]()->void {
                            CambrianRoutine();
                        },
                        cmb_conf_.lock()->cmb_module_conf().
                        modeule_entry_once()));
#else
                main_routine_thread_ = std::thread(
                        [this] () {
                        AINFO << "cambrain main thread start!";
                        while (!terminate_) {
                            if (running_) {
                                CambrianRoutine();
                            } else {
                                AINFO << "cambrain stop & wait resume!";
                            }
                            std::this_thread::sleep_for(
                                    std::chrono::milliseconds(
                                        cmb_conf_.lock()->
                                        cmb_module_conf().
                                        modeule_entry_freq()));
                        }
                        AWARN << "cambrian main thread terminate!";
                });
#endif
                sys_processor_->Init();
                is_inited_ = true;
            }

            virtual void Start() {
                AINFO << "BotItfImpl start";
#if USE_CYBER_TIMER
                if (tim_)
                    tim_->Start();
#else
                running_ = true;
                terminate_ = false;
#endif
                if (sys_processor_)
                    sys_processor_->Start();
            }

            virtual void Stop() {
                AINFO << "BotItfImpl stop";
#if USE_CYBER_TIMER
                if (tim_)
                    tim_->Stop();
#else
                running_ = false;
#endif
            }

            virtual void Resume() {
                AINFO << "BotItfImpl Resume";
                sys_processor_->Resume();
            }

            virtual void Close() {
                AINFO << "BotItfImpl close";
#if USE_CYBER_TIMER
                if (tim_)
                    tim_.reset();
#else
                terminate_ = true;
                std::this_thread::sleep_for(std::chrono::
                        milliseconds(300));

                if (is_inited_) {
                    AINFO << "main routine join";
                    main_routine_thread_.join();
                    AWARN << "recycle thread OK!";
                    is_inited_ = false;
                }
#endif
                sys_processor_->Close();
            }

            //routines
            virtual void CambrianRoutine() {
                //AINFO << "biz logic routine cambrian";
#if 0
                //testing
                loop_cnt_++;
                auto x = std::make_shared<
                    //ventura::common_msgs::nav_msgs::OccupancyGrid>();
                //x->mutable_header()->set_seq(loop_cnt_);

                //simulate receiving chassis info
                    ChassisMiscInfo>();
                msg_publish_(x);
#endif
            }

            /*
             * cambrian subsribed messages comming in
             * then distribute to sub modules
             */
            // >>>>> Subscribe Topic ITF >>>>>
            template <typename MessageT>
            int MsgsInpouring(const std::shared_ptr<MessageT>&);

            /*
             * cambrian messages to be send for publishing
             */
            // <<<<<< Service ITF <<<<<
            // TODO
            template <typename RequestT, typename ResponseT>
            int MsgOutpouring(const std::shared_ptr<RequestT>&,
                    std::shared_ptr<ResponseT> = nullptr);

        private:
            void BuildUpwardDataChannel() {
#ifdef CAMB_PKG_DBG
                AINFO << "Build upward msgs channel";
#endif
                auto vs = [this](const std::shared_ptr
                        <Message>& msg)->int {
#ifdef CAMB_PKG_DBG
                    AINFO << "Cambrian publish message: " <<
                        msg->GetTypeName();
#endif
                    if (msg.get() == nullptr) {
                        AWARN << "publish null message!";
                        return -1;
                    }

                    for (auto handle : remote_msg_handle_pair_) {
                        if (handle.first == msg->GetTypeName() &&
                                handle.second != nullptr) {
                            return (this->*this-> \
                                    remote_msg_handle_pair_ \
                                    [msg->GetTypeName()])(msg);
                        }
                    }

                    if (msg->GetTypeName().find("DriveSlam") !=
                            std::string::npos) {
                        auto s_tmp = std::make_shared<SlamDriveAck>();
                        return HandleDriveSlam(std::dynamic_pointer_cast
                                <DriveSlam>(msg), s_tmp);
                    }

                    if (msg->GetTypeName().find("DriveNavigation") !=
                            std::string::npos) {
                        return HandleDriveNavi(std::dynamic_pointer_cast
                                <DriveNavigation>(msg),
                                std::make_shared<NavigationAck>());
                    }

                    AWARN << "no handle for: " <<
                        msg->GetTypeName() <<
                        " \n" << msg->DebugString();

                    return -1;
                };

                sys_processor_->ImplementHub::RegisterMsgsDistributer(vs,
                        std::bind(&BotItfImpl::HandleDriveSlam, this,
                            std::placeholders::_1, std::placeholders::_2),
                        std::bind(&BotItfImpl::HandleDriveNavi, this,
                            std::placeholders::_1, std::placeholders::_2)
                        );
            }

        private:
            int loop_cnt_ = 0;
#if USE_CYBER_TIMER
            std::unique_ptr<cyber::Timer> tim_ = nullptr;
#else
            bool running_ = false;
            bool terminate_ = true;
            bool is_inited_ = false;
            std::thread main_routine_thread_;
#endif

            std::weak_ptr<CambrianConfig> cmb_conf_ {};
            std::unique_ptr<SysProcessor> sys_processor_;

        protected:
            // <<<<<< Msg Publish ITF <<<<<
            BaseMsgCaller msg_publish_ {};

            //chassis ctrl
            typedef int (BotItfImpl::*HandleRemoteMessage)( \
                    std::shared_ptr<Message>);
            //remote message entry
            std::unordered_map<std::string, HandleRemoteMessage>
                remote_msg_handle_pair_;
            template <typename MessageT>
            void RegisterRemoteMsgHandle(HandleRemoteMessage);

            //1, querys
            int HandleQuerySystemInfo(std::shared_ptr<Message>);
            int HandleQueryPeriphInfo(std::shared_ptr<Message>);
            int HandleQueryVolatileInfo(std::shared_ptr<Message>);
            int HandleQueryChassisRaw(std::shared_ptr<Message>);
            int HandleQueryModeStatus(std::shared_ptr<Message>);
            int HandleQueryCoverTrack(std::shared_ptr<Message>);
            int HandleQueryLocArray(std::shared_ptr<Message>);
            int HandleQueryGridMap(std::shared_ptr<Message>);
            int HandleQueryMapStorage(std::shared_ptr<Message>);
            int HandleQueryMapID(std::shared_ptr<Message>);
            int HandleQueryZones(std::shared_ptr<Message>);

            //2, controls
            int HandleMissionCommand(std::shared_ptr<Message>);
            int HandleAppointmentTask(std::shared_ptr<Message>);
            int HandleChassisCtrl(std::shared_ptr<Message>);
            int HandleSettingBother(std::shared_ptr<Message>);
            int HandleSettingConsumable(std::shared_ptr<Message>);

            //3, services
            int HandleDriveSlam(const std::shared_ptr<DriveSlam>&,
                    std::shared_ptr<SlamDriveAck>&);
            int HandleDriveNavi(const std::shared_ptr<DriveNavigation>&,
                    std::shared_ptr<NavigationAck>);
    };

    //message entrance
    template <typename MessageT>
    int BotItfImpl::MsgsInpouring(const
            std::shared_ptr<MessageT>& msg) {
#ifdef CAMB_PKG_DBG
        //AINFO << "inpour msg: " <<
            //cyber::message::GetMessageName<MessageT>();
#endif
        return sys_processor_->PushInMessage(msg);
    }

    //services for cambrian
    //TODO
    template <typename RequestT, typename ResponseT>
    int BotItfImpl::MsgOutpouring(const
            std::shared_ptr<RequestT>& msg,
            std::shared_ptr<ResponseT> ack) {
        //if (MessageTrait::Instance()->MsgTypeChsCtrlRls<RequestT>()) {
        if (0) {
            //return frg_out_chs_rls_(reinterpret_cast<const
                    //std::shared_ptr<ChassisCtrl>&>(msg));
        }

#ifdef CAMB_PKG_DBG
        AWARN << "[" << __func__ << "]" <<
            " no handler for " <<
            cyber::message::GetMessageName<RequestT>();
#endif

        return -1;
    }

} //namespace brain
} //namespace camb
} //namespace mstf
