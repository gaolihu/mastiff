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
                /*
                //visual
                visualize_ = std::make_unique<Visualization>(
                &conf->frg_module_conf().visual_ops());

                //data base
                database_ = std::make_unique<DatabaseClient>(
                &conf->frg_module_conf().sql_ops());

                //message pool
                process_eng_ = std::make_unique<ProcessEngine>(
                [&]() { database_->DbNotifyModification(); },
                conf->frg_module_conf().director_ops().
                scheduler_xml());
#ifdef CAMB_PKG_DBG
                AINFO << "sub-components construction finish!";
#endif
*/
            }

            virtual ~BotItfImpl() {
#ifdef CAMB_PKG_DBG
                AINFO << "ROBOT impl de-construct";
#endif
            }

            //common
            virtual void Init() {
                AINFO << "BotItfImpl init";
                BuildUpwardDataChannel();
                terminate_ = false;
#if USE_CYBER_TIMER
                tim_.reset(new cyber::Timer(cmb_conf_.lock()->
                        cmb_module_conf().modeule_entry_freq(),
                        [&]()->void {
                            BizLogicRoutine();
                            VisualizeRoutine();
                            DataBaseRoutine();
                        },
                        cmb_conf_.lock()->cmb_module_conf().
                        modeule_entry_once()));
#else
                main_routine_thread_ = std::thread(
                        [this] () {
                        while (!terminate_) {
                            //AINFO << "main thread start!";
                            if (running_) {
                                BizLogicRoutine();
                                VisualizeRoutine();
                                DataBaseRoutine();
                            } else {
                                AINFO << "stop & wait resume!";
                            }
                            std::this_thread::sleep_for(
                                    std::chrono::milliseconds(
                                        cmb_conf_.lock()->
                                        cmb_module_conf().
                                        modeule_entry_freq()));
                        }
                        AWARN << "main thread terminate!";
                });
#endif
#if 0
                if (database_.get() != nullptr) {
                    database_->DatabaseInit();
                } else {
                    AWARN << "database NULL!";
                }

                if (visualize_.get() != nullptr) {
                    visualize_->Init();
                    HandleQueryVolatileInfo(nullptr);
                    HandleQuerySystemInfo(nullptr);
                    HandleQueryModeStatus(nullptr);
                    HandleQueryZones(nullptr);
                } else {
                    AWARN << "visualization NULL!";
                }

                if (process_eng_.get() != nullptr) {
                    process_eng_->Init();
                } else {
                    AWARN << "process engine NULL!";
                }
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
#if 0
                if (visualize_.get() != nullptr) {
                    visualize_->Start();
                } else {
                    AWARN << "visualization NULL!";
                }
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
#if 0
                if (process_eng_.get() != nullptr) {
                    process_eng_->Stop();
                } else {
                    AWARN << "process engine null!";
                }

                if (visualize_.get() != nullptr) {
                    visualize_->Stop();
                } else {
                    AWARN << "visualization NULL!";
                }
#endif
                sys_processor_->Stop();
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
            virtual void BizLogicRoutine() {
                //AINFO << "business logic routine";
#if 1
                //tesing !!!!!!!!!
                loop_cnt_++;
                auto x = std::make_shared<ventura::common_msgs::nav_msgs::OccupancyGrid>();
                x->mutable_header()->set_seq(loop_cnt_);
                export_(x);
#endif
            }

            virtual void VisualizeRoutine() {
                //AINFO << "visualization routine";
            }

            virtual void DataBaseRoutine() {
                //AINFO << "storage routine";
            }

            //virtual void RobotHealth() {}
            //virtual void BotStorage() {}


            // >>>>> POUR IN >>>>>
            // cambrian subsribed messages comming
            // then distribute to sub modules
            template <typename MessageT>
            int MsgsInpouring(const std::shared_ptr<MessageT>&);

            // <<<<<< POUR OUT <<<<<
            //cambrian messages to be send for publishing
            template <typename RequestT, typename ResponseT>
            int MsgOutpouring(const std::shared_ptr<RequestT>&,
                    std::shared_ptr<ResponseT> = nullptr);

            void RegisterDataExport(const
                    std::function<int(const std::shared_ptr<Message>&)>&);

        private:
            void BuildUpwardDataChannel() {
#ifdef CAMB_PKG_DBG
                AINFO << "Build upstream msg channel";
#endif
                auto vs = [this](const std::shared_ptr
                        <Message>& msg)->int {
#if 0
#ifdef CAMB_PKG_DBG
                    AINFO << "Fringe publish message: " <<
                        msg->GetTypeName();
#endif
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

                sys_processor_->ImplementHub::RegisterPublishHandle(vs,
                        std::bind(&BotItfImpl::HandleDriveSlam, this,
                            std::placeholders::_1, std::placeholders::_2),
                        std::bind(&BotItfImpl::HandleDriveNavi, this,
                            std::placeholders::_1, std::placeholders::_2)
                        );
            }

        protected:
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

            std::function<int(const std::shared_ptr<Message>&)> export_ {};

            //chassis ctrl
            //FringeOutChsCtrlRelease frg_out_chs_rls_;
            //FringeSimulateHcrRelease sim_hcr_rls_;

            typedef int (BotItfImpl::*HandleRemoteMessage)( \
                    std::shared_ptr<Message>);

            template <typename MessageT>
            void RegisterRemoteMsgHandle(HandleRemoteMessage);
                    //ImplementHub::HandleRemoteFunc,
                    //Visualization::HandleRemoteFunc);

            //for remote message entry
            std::unordered_map<std::string, HandleRemoteMessage>
                remote_msg_handle_pair_;

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

            int HandleMissionCommand(std::shared_ptr<Message>);
            int HandleAppointmentTask(std::shared_ptr<Message>);
            int HandleChassisCtrl(std::shared_ptr<Message>);
            int HandleMotionCtrl(std::shared_ptr<Message>);

            //?
            int HandleSettingAudio(std::shared_ptr<Message>);
            int HandleSettingBother(std::shared_ptr<Message>);
            int HandleSettingConsumable(std::shared_ptr<Message>);

            //services
            int HandleDriveSlam(const std::shared_ptr<DriveSlam>&,
                    std::shared_ptr<SlamDriveAck>&);
            int HandleDriveNavi(const std::shared_ptr<DriveNavigation>&,
                    std::shared_ptr<NavigationAck>);
    };

    //message entrance
    template <typename MessageT>
    int BotItfImpl::MsgsInpouring(const std::shared_ptr<MessageT>& msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "inpour msg: " <<
            cyber::message::GetMessageName<MessageT>();
#endif
        sys_processor_->GetMessagePool()->EnqueueMessage<MessageT>(msg);
#if 0
        //visualize_->Fresh();
        //TODO

        if (p == nullptr || p->GetTypeName() ==
                GetMessageType<FrgNullMsg>()) {
            if (p == nullptr) {
#ifdef CAMB_PKG_DBG
                AWARN << "null message enqueued!" <<
                    cyber::message::GetMessageName<MessageT>();
#endif
            }

            return;
        }

        //upload remote side
        if (MessageTrait::Instance()->MsgTypeIsHcr<MessageT>()) {
            //hcr->ppi
            visualize_->HandleUpstreamMesages<PeriphInformation>(p);
        } else if (visualize_->HandleUpstreamMesages<MessageT>(p) < 0) {
#ifdef CAMB_PKG_DBG
            /*
            AWARN << "visual update message : " <<
                GetMessageType<MessageT>() << " failed!" <<
                ", p type: " << p->GetTypeName();
                */
#endif
        }
#endif
        return 0;
    }

    //message exit cambrian
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
            " no handler for " << cyber::message::GetMessageName<RequestT>();
#endif

        return -1;
    }

} //namespace brain
} //namespace camb
} //namespace mstf
