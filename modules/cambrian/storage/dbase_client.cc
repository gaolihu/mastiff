#include "modules/cambrian/storage/dbase_client.h"

namespace mstf {
namespace camb {
namespace dbase {


    DbaseClient::DbaseClient(const DataBaseOpts* q_conf) {
#ifdef CAMB_PKG_DBG
        AINFO << "DbaseClient construct ~ 3\n" <<
            q_conf->DebugString();
#endif
        q_conf_ = std::make_shared<DataBaseOpts>(*q_conf);

        sql_ = std::make_shared<DbOperation>();
        //app_ = std::make_shared<Appointment>();
    }

    DbaseClient::~DbaseClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "DbaseClient de-construct!";
#endif

        //update total uptime at last
        DbLoadSaveVolatile();

        DataBaseExit();
    }

    void DbaseClient::DatabaseInit() {
#ifdef CAMB_PKG_DBG
        AINFO << "Data base init start!";
#endif
        database_need_update_.store(false);

        if (access(q_conf_->sys_path().c_str(), F_OK) != 0) {
            //sys file not exist, create
            AWARN << "create db file: " <<
                q_conf_->sys_path();

            char cmd[64] = {0};
            const char* tmp = strrchr(q_conf_->sys_path().
                    c_str(), '/');
            int len = tmp - q_conf_->sys_path().c_str() +
                strlen("mkdir -p %s");
            int ret = snprintf(cmd, len, "mkdir -p %s",
                    q_conf_->sys_path().c_str());
            if (system(cmd) < 0) {
                AWARN << "mkdir: " << cmd <<
                    ", len: " << len <<
                    ", ret: " << ret;
            }

            memset(cmd, 0, sizeof(cmd));
            sprintf(cmd, "touch %s", q_conf_->sys_path().c_str());
            if (system(cmd) < 0) {
                AWARN << "touch: " << cmd;
            }
        }

        //main sql
        sql_->SqlInit(q_conf_->db_path());

        //volatile
        DbLoadSaveVolatile();

        //sys conf
        DbChkCreateSysConf();

        //zones
        DbLoadSaveMultiZones();

        //appointment task
        LoadRsvFromDb();

#ifdef CAMB_PKG_DBG
        AINFO << "Database init finished!";
#endif
    }

    void DbaseClient::DataBaseExit() {
#ifdef CAMB_PKG_DBG
        AINFO << "Data base exit!\n";
#endif
        sql_->SqlExit();
    }

    void DbaseClient::DataBaseLoop(const int loop_cnt) {
        loop_cnt_++;
        if (database_need_update_.load()) {
            //update database
            DbLoadSaveVolatile();
            database_need_update_.store(false);
        } else {
#ifdef CAMB_PKG_DBG
#endif
            //check database routine loop
            if (loop_cnt_ % (loop_cnt * 2) == 0) {
                UpdatePowerupTime();
            }
        }
    }

    void DbaseClient::DbNotifyModification() {
#ifdef CAMB_PKG_DBG
        //AINFO << "DB notified to be updated!";
#endif
        database_need_update_.store(true);
    }

    void DbaseClient::DbStoreMltimZones(const
            MultiZonesParam& mzp) {
#ifdef CAMB_PKG_DBG
        AINFO << "store db mzp:\n" <<
#if 0
            mzp.DebugString();
#else
            "";
#endif
#endif
        if (mzp_info_.get() != nullptr) {
            mzp_info_->Clear();
            mzp_info_->CopyFrom(mzp);
        }

        DbLoadSaveMultiZones();
    }

    std::shared_ptr<Message> DbaseClient::DbExposeMltimZones() {
#ifdef CAMB_PKG_DBG
        AINFO << "expose mzp" <<
            ", use_count: " << mzp_info_.use_count() <<
            ", content p: " << mzp_info_.get() <<
            ", is unique: " << (mzp_info_.unique() ? "YES" : "NO");
#endif
        if (mzp_info_.get() != nullptr)
            return mzp_info_;
        else
            return nullptr;
    }

    std::shared_ptr<Message> DbaseClient::DbExposeSystemInfo() {
#ifdef CAMB_PKG_DBG
        AINFO << "expose system info" <<
            ", use_count: " << sys_info_.use_count() <<
            ", content p: " << sys_info_.get() <<
            ", is unique: " << (sys_info_.unique() ? "YES" : "NO");
#endif
        if (sys_info_.get() != nullptr)
            return sys_info_;
        else
            return nullptr;
    }

    std::shared_ptr<Message> DbaseClient::DbExposeVolatileInfo() {
#ifdef CAMB_PKG_DBG
        AINFO << "expose volatile info: " << vola_info_ <<
            ", use_count: " << vola_info_.use_count() <<
            ", content p: " << vola_info_.get() <<
            ", is unique: " << (vola_info_.unique() ? "YES" : "NO");
#endif
        //need to checkout all the infos
        if (vola_info_.get() != nullptr)
            return vola_info_;
        else
            return nullptr;
    }

    int DbaseClient::LoadRsvFromDb() {
#ifdef CAMB_PKG_DBG
        AINFO << "Load sql reservation infomation!\n";
#endif
        int ret = 0;
        //TODO
        //
        return ret;
    }

    int DbaseClient::DbChkItem(DbData& dbd,
            const char* tbl,
            const char* item,
            bool is_single,
            EE_CHK_DB_TYPE type,
            EE_CHK_DB_SEQ seq) {
        int ret = 0;
        char* cursor = NULL;
        int row = 0, col = 0;
        std::string res = "";

        memset(&dbd, 0, sizeof(DbData));
        dbd.type = type;

        ret = sql_->SqlCheckOut(res, tbl, item, type, seq);

        if (ret != 0 || res.empty()) {
            AWARN << "checkout \"" << item << "\" failed!";
            return -1;
        }

        dbd.mul_val_idx = -1;
        if (is_single) {
            //single data checkout
            cursor = strchr(const_cast<char*>(res.c_str()), '\n') + 1;
            if (cursor == NULL) {
                AWARN << "checkout single data: " << item << ", failed!";
                return -1;
            }

            dbd.mul_val_idx = 0;
            switch (dbd.type) {
            case E_CHK_TYPE_INT:
                ret = sscanf(cursor, GET_TABLE_FORMAT_INT, &row, &col,
                        &dbd.ival[0]);
#ifdef DB_DATA_DBG
                AINFO << "single data checkout, item: " << item <<
                    ", (int?)type: " << type <<
                    ", data: {" << cursor << "}" <<
                    ", val: " << dbd.ival[0];
#endif
                break;
            case E_CHK_TYPE_FLOAT:
                ret = sscanf(cursor, GET_TABLE_FORMAT_FLOAT, &row, &col,
                        &dbd.fval[0]);
#ifdef DB_DATA_DBG
                AINFO << "single data checkout, item: " << item <<
                    ", (float?)type: " << type <<
                    ", data: {" << cursor << "}" <<
                    ", val: " << dbd.fval[0];
#endif
                break;
            case E_CHK_TYPE_STRING:
                memset(dbd.sval[0], 0, sizeof(dbd.sval));
                ret = sscanf(cursor, GET_TABLE_FORMAT_STRING, &row, &col,
                        dbd.sval[0]);
#ifdef DB_DATA_DBG
                AINFO << "single data checkout, item: " << item <<
                    ", (string?)type: " << type <<
                    ", data: {" << cursor << "}" <<
                    ", val: " << dbd.sval[0];
#endif
                break;
            default: break;
            }
        } else {
            //multi data checkout
            dbd.mul_val_idx = 0;
            cursor = strchr(const_cast<char*>(res.c_str()), '\n') + 1;
            if (cursor == NULL) {
                AWARN << "checkout: " << item << "null!";
                return -1;
            }

            do {
                switch (dbd.type) {
                case E_CHK_TYPE_INT:
                    dbd.ival[dbd.mul_val_idx] = 0;
                    ret = sscanf(cursor, GET_TABLE_FORMAT_INT, &row, &col,
                            &dbd.ival[dbd.mul_val_idx]);
#ifdef DB_DATA_DBG
                    AINFO << "multi data checkout, item: " << item <<
                        ", (int?)type: " << type <<
                        ", data: {" << cursor << "}" <<
                        ", idx: " << dbd.mul_val_idx <<
                        ", val: " << dbd.ival[dbd.mul_val_idx];
#endif
                    break;
                case E_CHK_TYPE_ULONG:
                    dbd.ival[dbd.mul_val_idx] = 0;
                    ret = sscanf(cursor, GET_TABLE_FORMAT_ULL, &row, &col,
                            (unsigned long long *)
                            &dbd.lval[dbd.mul_val_idx]);
#ifdef DB_DATA_DBG
                    AINFO << "multi data checkout, item: " << item <<
                        ", (ulong?)type: " << type <<
                        ", data: {" << cursor << "}" <<
                        ", idx: " << dbd.mul_val_idx <<
                        ", val: " << dbd.ival[dbd.mul_val_idx];
#endif
                    break;
                case E_CHK_TYPE_FLOAT:
                    dbd.fval[dbd.mul_val_idx] = 0;
                    ret = sscanf(cursor, GET_TABLE_FORMAT_FLOAT, &row, &col,
                            &dbd.fval[dbd.mul_val_idx]);
#ifdef DB_DATA_DBG
                    AINFO << "multi data checkout, item: " << item <<
                        ", (float?)type: " << type <<
                        ", data: {" << cursor << "}" <<
                        ", idx: " << dbd.mul_val_idx <<
                        ", val: " << dbd.ival[dbd.mul_val_idx];
#endif
                    break;
                case E_CHK_TYPE_STRING:
                    memset(dbd.sval[dbd.mul_val_idx], 0, sizeof(dbd.sval[0]));
                    ret = sscanf(cursor, GET_TABLE_FORMAT_STRING, &row, &col,
                            dbd.sval[dbd.mul_val_idx]);
#ifdef DB_DATA_DBG
                    AINFO << "multi data checkout, item: " << item <<
                        ", (string?)type: " << type <<
                        ", data: {" << cursor << "}" <<
                        ", idx: " << dbd.mul_val_idx <<
                        ", val: " << dbd.sval[dbd.mul_val_idx];
#endif
                    break;
                default: break;
                }

                dbd.mul_val_idx++;
                if ((unsigned int)dbd.mul_val_idx > (sizeof(dbd.ival) / \
                            sizeof(dbd.ival[0])))
                    break;
                cursor = strchr(cursor, '\n') + 1;
            } while ((cursor <= res.c_str() + res.size()) &&
                    ret != EOF && strlen(cursor) > 1 &&
                    cursor != nullptr);
        }

        return 0;
    }

    int DbaseClient::DbUpdtItem(const DbData& dbd,
            const char* tbl,
            const char* item,
            const EE_CHK_DB_TYPE type) {
#ifdef DB_DATA_DBG
        AWARN << "update table: " << tbl <<
            ", item: " << item <<
            ", type: " << type;
#endif

        switch (type) {
        case E_CHK_TYPE_INT:
        case E_CHK_TYPE_ULONG:
            for (int i = 0; i < dbd.mul_val_idx; i++) {
                if (!dbd.ignore[i])
                    sql_->SqlUpdateInt(tbl, item, dbd.ival[i]);
            }
            break;
        case E_CHK_TYPE_FLOAT:
            for (int i = 0; i < dbd.mul_val_idx; i++) {
                if (!dbd.ignore[i])
                    sql_->SqlUpdateFloat(tbl, item, dbd.fval[i]);
            }
            break;
        case E_CHK_TYPE_STRING:
            for (int i = 0; i < dbd.mul_val_idx; i++) {
                if (!dbd.ignore[i])
                    sql_->SqlUpdateString(tbl, item, dbd.sval[i]);
            }
            break;
        default:
            AWARN << "type not support!";
            break;
        }

        return 0;
    }

    void DbaseClient::DbChkCreateSysConf() {
#ifdef CAMB_PKG_DBG
        AINFO << "check & create sys config file";
#endif

        if (sys_info_.get() == nullptr)
            sys_info_ = std::make_shared<SystemInformation>();

        if (access(q_conf_->sys_path().c_str(), F_OK) != 0) {
            AERROR << "sys file: " << q_conf_->sys_path() << ", not exist!";
            return;
        }

        if (!cyber::common::GetProtoFromFile(q_conf_->sys_path(),
                    sys_info_.get())) {
            AERROR << "load sys conf NG!";
            return;
        } else if (!sys_info_->has_bot_type()) {
            //sys conf empty
            sys_info_->mutable_bot_type()->set_bot_name(BOT_NAME);
            sys_info_->mutable_bot_type()->set_bot_type(BOT_TYPE);
            sys_info_->mutable_bot_type()->set_serial_num(BOT_SN);
            sys_info_->mutable_bot_type()->set_uuid(BOT_UUID);
            sys_info_->mutable_bot_type()->set_product_key(PRODUCT_KEY);

            sys_info_->mutable_sw_ver()->set_sw_version(SW_VERSION);
            sys_info_->mutable_sw_ver()->set_sys_version(SYS_VERSION);
            sys_info_->mutable_sw_ver()->set_mcu_version(MCU_VERSION);
            sys_info_->mutable_sw_ver()->set_release_date(RLS_DATE);
            sys_info_->mutable_sw_ver()->set_ota_file_path(OTA_PATH);

            sys_info_->mutable_voice()->set_voice_path(VOICE_PATH);

            std::string ip, mac;

            //if in docker
            cyber::common::GetMacIp("eth0", ip, mac);

            AINFO << "bot ip: " << ip <<
                ", mac: " << mac;

            if (vola_info_.get() != nullptr) {
                vola_info_->mutable_wireless()->
                    set_ip_addr(ip);
            }

            sys_info_->mutable_wireless()->set_self_ssid(SELF_AP);
            sys_info_->mutable_wireless()->set_mac_addr(mac);

            //save the sys.conf file
            cyber::common::SetProtoToASCIIFile(*sys_info_.get(),
                    q_conf_->sys_path());

#ifdef CAMB_PKG_DBG
            AINFO << "write system info to disk: \n{\n" <<
                sys_info_->DebugString() << "}";
#endif
        } else {
            //sys conf OK
#ifdef CAMB_PKG_DBG
            AINFO << "load sys conf OK: \n{\n" <<
                sys_info_->DebugString() << "}";
#endif
        }
    }

    void DbaseClient::UpdatePowerupTime() {
        //1, powerup time is fixed

        //2, update current powerup time
        vola_info_->mutable_job_statistics()->
            set_poweron_time_sec(cyber::common::GetSysUpSecs());

        //3, update history powerup time
        vola_info_->mutable_job_statistics()->
            set_poweron_time_total_sec(vola_info_->
                    job_statistics().poweron_time_sec() +
                    last_note_history_time_);
    }

    void DbaseClient::DbLoadSaveVolatile() {
        bool load = false;

        if (!sql_->IsDbOperationOK()) {
            AWARN << "db operation is not ready!";
            return;
        }

        if (vola_info_.get() == nullptr) {
            load = true;
            vola_info_ = std::make_shared<VolatileInformation>();
        }

#ifdef CAMB_PKG_DBG
        AINFO << "check & " << (load ? "LOAD" : "SAVE") <<
            " volatile infomations";
#endif

        DbData dbd_s {};
        DbData dbd_g {};

        //checkout volatile infos
        //
        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_NDSTMD, false) == 0) {
            for (int i = 0; i < dbd_g.mul_val_idx; i++) {
#ifdef DB_DATA_DBG
                AINFO << "DB nodisturb switch, idx: " << i <<
                    ", all: " << dbd_g.mul_val_idx <<
                    ", sw: " << dbd_g.ival[i];
#endif
                if (dbd_g.ival[i] <= 0) {
                    dbd_g.ival[i] = 0;
                } else {
                    dbd_g.ival[i] = 1;
                }

                if (load) {
                    //memory load from db
                    vola_info_->add_no_disturb()->
                        set_no_disturb_sw(dbd_g.ival[i]);
                }
            }

            if (!load) {
                //save
                memset(&dbd_s, 0, sizeof(dbd_s));

                for (int i = 0; i < dbd_g.mul_val_idx; i++) {
                    dbd_s.mul_val_idx++;
                    if (dbd_g.ival[i] == vola_info_->
                            no_disturb(i).no_disturb_sw()) {
                        dbd_s.ignore[i] = true;
                    }

                    dbd_s.ival[i] = vola_info_->
                        no_disturb(i).no_disturb_sw();
#ifdef DB_DATA_DBG
                    AINFO << "change nd switch, idx: " << i <<
                        ", val: " << dbd_s.ival[i];
#endif
                }

                for (int j = dbd_g.mul_val_idx; j < vola_info_->
                        no_disturb().size(); j++) {
                        //increase data
#ifdef DB_DATA_DBG
                    AINFO << "increase nd mode, idx: " << j <<
                        ", val: " << dbd_s.ival[j];
#endif
                        dbd_s.ival[j] = vola_info_->
                            no_disturb(j).no_disturb_sw();
                }
                //rewrite
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_NDSTMD);
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_NDSSTA, false,
                    E_CHK_TYPE_STRING) == 0) {
            for (int i = 0; i < dbd_g.mul_val_idx; i++) {
#ifdef DB_DATA_DBG
                AINFO << "DB nodisturb start, idx: " << i <<
                    ", all: " << dbd_g.mul_val_idx <<
                    ", val: " << dbd_g.sval[i];
#endif
                if (load && vola_info_) {
                    //memory load from db
                    vola_info_->mutable_no_disturb(i)->
                        set_no_disturb_from(dbd_g.sval[i]);
                }
            }

            if (!load) {
                //save
                memset(&dbd_s, 0, sizeof(dbd_s));

                for (int i = 0; i < dbd_g.mul_val_idx; i++) {
                    dbd_s.mul_val_idx++;
                    if (vola_info_->no_disturb(i).
                            no_disturb_from() == dbd_g.sval[i]) {
                        dbd_s.ignore[i] = true;
                    } else {
                        strcpy(dbd_s.sval[i], vola_info_->
                                no_disturb(i).no_disturb_from().data());
                    }
#ifdef DB_DATA_DBG
                    AINFO << "change nd start, idx: " << i <<
                        ", val: " << dbd_s.sval[i];
#endif
                }

                for (int j = dbd_g.mul_val_idx; j < vola_info_->
                        no_disturb().size(); j++) {
                        //increase data
#ifdef DB_DATA_DBG
                    AINFO << "increase nd start, idx: " << j <<
                        ", val: " << dbd_s.ival[j];
#endif
                        strcpy(dbd_s.sval[j], vola_info_->
                                no_disturb(j).no_disturb_from().data());
                }
                //update
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_NDSSTA, true,
                        E_CHK_TYPE_STRING);
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_NDSEND, false,
                    E_CHK_TYPE_STRING) == 0) {
            for (int i = 0; i < dbd_g.mul_val_idx; i++) {
#ifdef DB_DATA_DBG
                AINFO << "DB nodisturb end, idx: " << i <<
                    ", all: " << dbd_g.mul_val_idx <<
                    ", val: " << dbd_g.sval[i];
#endif
                if (load && vola_info_) {
                    //memory load from db
                    vola_info_->mutable_no_disturb(i)->
                        set_no_disturb_to(dbd_g.sval[i]);
                }
            }
            if (!load) {
                memset(&dbd_s, 0, sizeof(dbd_s));

                for (int i = 0; i < dbd_g.mul_val_idx; i++) {
                    dbd_s.mul_val_idx++;
                    if (vola_info_->no_disturb(i).
                            no_disturb_to() == dbd_g.sval[i]) {
                        dbd_s.ignore[i] = true;
                    } else {
                        strcpy(dbd_s.sval[i], vola_info_->
                                no_disturb(i).no_disturb_to().data());
                    }
#ifdef DB_DATA_DBG
                    AINFO << "change nd end, idx: " << i <<
                        ", val: " << dbd_s.sval[i];
#endif
                }

                for (int j = dbd_g.mul_val_idx; j < vola_info_->
                        no_disturb().size(); j++) {
                        //increase data
#ifdef DB_DATA_DBG
                    AINFO << "increase nd end, idx: " << j <<
                        ", val: " << dbd_s.ival[j];
#endif
                        strcpy(dbd_s.sval[j], vola_info_->
                                no_disturb(j).no_disturb_to().data());
                }
                //update
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_NDSEND, true,
                        E_CHK_TYPE_STRING);
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_BATLVL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB battery: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_battery(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().battery() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_BATLVL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().battery());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_FANSUC) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB fan: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_suction(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().suction() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_FANSUC, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().suction());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_WTFSPD) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB water flow speed: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_flow_speed(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().flow_speed() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_WTFSPD, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().flow_speed());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_BOOSTM) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB boost sw: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_boost_sw(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().boost_sw() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_BOOSTM, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().boost_sw());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_BREAKP) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB break sw: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_break_sw(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().break_sw() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_BREAKP, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().break_sw());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_WTRLVL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB water level: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_water_level(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().water_level() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_WTRLVL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().water_level());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_MOPSET) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB mop is set: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_brief_stat()->
                    set_is_mop_set(dbd_g.ival[0]);
            } else if (vola_info_->brief_stat().is_mop_set() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_MOPSET, false,
                        E_CHK_TYPE_INT,
                        vola_info_->brief_stat().is_mop_set());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_PWRUPT, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB power up time string: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_powerup_time(dbd_g.sval[0]);
            } else if ((vola_info_->job_statistics().powerup_time() !=
                        dbd_g.sval[0]) || (vola_info_->job_statistics().
                            powerup_time() == "unknown")) {
                //get system start up time
                vola_info_->mutable_job_statistics()->
                    set_powerup_time(cyber::common::GetSysUpTime());
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_PWRUPT, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_->job_statistics().powerup_time().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_PWRSEC, true,
                    E_CHK_TYPE_FLOAT) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB power on current up time total: " << dbd_g.fval[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_poweron_time_sec(dbd_g.fval[0]);
            } else {
                //always update sys uptime
                vola_info_->mutable_job_statistics()->
                    set_poweron_time_sec(cyber::common::GetSysUpSecs());
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_PWRSEC, false,
                        E_CHK_TYPE_FLOAT, 0,
                        vola_info_->job_statistics().poweron_time_sec());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_PWRTTL, true,
                    E_CHK_TYPE_FLOAT) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB power on history time total: " << dbd_g.fval[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_poweron_time_total_sec(dbd_g.fval[0]);
                last_note_history_time_ = dbd_g.fval[0];
            } else {
                //update history sys uptime
                vola_info_->mutable_job_statistics()->set_poweron_time_total_sec(
                        vola_info_->job_statistics().poweron_time_sec() +
                        last_note_history_time_);
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_PWRTTL, false,
                        E_CHK_TYPE_FLOAT, 0,
                        vola_info_->job_statistics().
                        poweron_time_total_sec());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_MSSCNT) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB mission count: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_mission_count(dbd_g.ival[0]);
            } else if (vola_info_->job_statistics().mission_count() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_MSSCNT, false,
                        E_CHK_TYPE_INT,
                        vola_info_->job_statistics().mission_count());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_MLGTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB mileage total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_mileage_statistic(dbd_g.ival[0]);
            } else if (vola_info_->job_statistics().mileage_statistic() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_MLGTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->job_statistics().mileage_statistic());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_TCTIME) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB clean time total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_clean_time_total(dbd_g.ival[0]);
            } else if (vola_info_->job_statistics().clean_time_total() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_TCTIME, false,
                        E_CHK_TYPE_INT,
                        vola_info_->job_statistics().clean_time_total());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_TCAREA, true,
                    E_CHK_TYPE_FLOAT) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB clean area total: " << dbd_g.fval[0];
#endif
            if (load) {
                vola_info_->mutable_job_statistics()->
                    set_clean_area_total(dbd_g.fval[0]);
            } else if (vola_info_->job_statistics().clean_area_total() !=
                    dbd_g.fval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_TCAREA, false,
                        E_CHK_TYPE_FLOAT, 0,
                        vola_info_->job_statistics().clean_area_total());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_LANGUG, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB language: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_audio_info()->
                    set_language(dbd_g.sval[0]);
            } else if (vola_info_->audio_info().language() !=
                    dbd_g.sval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_LANGUG, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_-> audio_info().language().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_AUDVOL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB audio volume: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_audio_info()->
                    set_volume(dbd_g.ival[0]);
            } else if (vola_info_->audio_info().volume() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_AUDVOL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->audio_info().volume());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_IFCONF, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB network conf: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_wireless()->
                    set_configured(dbd_g.sval[0]);
            } else if (vola_info_->wireless().configured() !=
                    dbd_g.sval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_IFCONF, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_->wireless().configured().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_WFSSID, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB wifi ssid: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_wireless()->
                    set_con_ssid(dbd_g.sval[0]);
            } else if (vola_info_->wireless().con_ssid() !=
                    dbd_g.sval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_WFSSID, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_->wireless().con_ssid().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_PASSWD, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB wifi pswd: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_wireless()->
                    set_wifi_pswd(dbd_g.sval[0]);
            } else if (vola_info_->wireless().wifi_pswd() !=
                    dbd_g.sval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_PASSWD, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_->wireless().wifi_pswd().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_IPADDR, true,
                    E_CHK_TYPE_STRING) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB ip addr: " << dbd_g.sval[0];
#endif
            if (load) {
                vola_info_->mutable_wireless()->
                    set_ip_addr(dbd_g.sval[0]);
            } else if (vola_info_->wireless().ip_addr() !=
                    dbd_g.sval[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_IPADDR, false,
                        E_CHK_TYPE_STRING, 0, 0.f,
                        vola_info_->wireless().ip_addr().data());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_STNUSE) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB strainer use: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_strainer_used_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().strainer_used_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_STNUSE, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().strainer_used_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_STNTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB strainer total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_strainer_total_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().strainer_total_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_STNTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().strainer_total_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_MBSUSE) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB main brush use: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_mbrush_used_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().mbrush_used_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_MBSUSE, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().mbrush_used_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_MBSTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB main brush total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_mbrush_total_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().mbrush_total_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_MBSTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().mbrush_total_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_SDBUSE) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB side brush use: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_sbrush_used_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().sbrush_used_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_SDBUSE, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().sbrush_used_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_SDBTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB side brush total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_sbrush_total_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().sbrush_total_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_SDBTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().sbrush_total_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_SNSUSE) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB sensor use: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_sensor_used_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().sensor_used_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_SNSUSE, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().sensor_used_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_SNSTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB sensor total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_sensor_total_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().sensor_total_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_SNSTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().sensor_total_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_WTRUSE) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB filter use: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_filter_used_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().filter_used_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_WTRUSE, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().filter_used_time());
            }
        }

        if (DbChkVolatile(dbd_g, ITEM_VOLATILE_WTRTTL) == 0) {
#ifdef DB_DATA_DBG
            AINFO << "DB filter total: " << dbd_g.ival[0];
#endif
            if (load) {
                vola_info_->mutable_comsumble()->
                    set_filter_total_time(dbd_g.ival[0]);
            } else if (vola_info_->comsumble().filter_total_time() !=
                    dbd_g.ival[0]) {
                DbUpdtVolatile(dbd_s, ITEM_VOLATILE_WTRTTL, false,
                        E_CHK_TYPE_INT,
                        vola_info_->comsumble().filter_total_time());
            }
        }

#ifdef DB_DATA_DBG
        AINFO_EVERY(1) << "update/load vola_info_:\n{\n" <<
            vola_info_->DebugString() << "}";
#endif
    }

    void DbaseClient::DbLoadSaveMultiZones() {
        bool load = false;

        if (mzp_info_.get() == nullptr) {
            load = true;
            mzp_info_ = std::make_shared<MultiZonesParam>();
        }

#ifdef CAMB_PKG_DBG
        AINFO << "check & " << (load ? "LOAD" : "SAVE") <<
            " multi zones info";
#endif

        DbData dbd_s {};
        DbData dbd_g {};

        if (DbChkMultiZones(dbd_g, ITEM_MULTI_ZONES) == 0) {
            for (int i = 0; i < dbd_g.mul_val_idx; i++) {
#ifdef CAMB_PKG_DBG
                AINFO << "DB rooms, idx: " << i <<
                    ", all: " << dbd_g.mul_val_idx <<
                    ", val: " << dbd_g.sval[i];
#endif
                if (load && mzp_info_) {
                    //memory load from db
                    if (!mzp_info_->add_szp()->
                            ParseFromString(dbd_g.sval[i])) {
                        AERROR << "parse mzp error!";
                        continue;
                    }
                }
            }

            if (!load) {
                //store mzp
                memset(&dbd_s, 0, sizeof(dbd_s));

                for (int i = 0; i < dbd_g.mul_val_idx; i++) {
                    dbd_s.mul_val_idx++;
                    if (mzp_info_->szp(i).SerializeAsString() ==
                            dbd_g.sval[i]) {
                        dbd_s.ignore[i] = true;
                    } else {
                        strcpy(dbd_s.sval[i], mzp_info_->
                                szp(i).SerializeAsString().data());
                    }
#ifdef CAMB_PKG_DBG
                    AINFO << "change slp, idx: " << i <<
                        ", val: " << dbd_s.sval[i];
#endif
                }

                for (int j = dbd_g.mul_val_idx; j < mzp_info_->
                        szp().size(); j++) {
                        //increase data
#ifdef CAMB_PKG_DBG
                    AINFO << "increase slp, idx: " << j <<
                        ", val: " << dbd_s.ival[j];
#endif
                        strcpy(dbd_s.sval[j], mzp_info_->
                                szp(j).SerializeAsString().data());
                }
                //update
                DbUpdtMultiZones(dbd_s, ITEM_MULTI_ZONES);
            }
        }
#ifdef CAMB_PKG_DBG
        AINFO_EVERY(1) <<
            (load ? "load" : "update") <<
            " mzp_info_:\n{" <<
#if 0
            \nmzp_info_->DebugString() << "}";
#endif
            "hide}";
#endif
    }

} //namespace dase
} //namespace camb
} //namespace mstf
