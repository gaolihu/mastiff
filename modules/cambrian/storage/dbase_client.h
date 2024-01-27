#pragma once

#include "modules/cambrian/storage/db_operation.h"

namespace mstf {
namespace camb {
namespace dbase {

#pragma pack(8)
    typedef struct {
        EE_CHK_DB_TYPE type;
        int mul_val_idx;
        int ival[8];
        float fval[8];
        uint64_t lval[8];
        char sval[8][512];
        bool ignore[8];
    } DbData;

    class DbaseClient {
        public:
            DbaseClient(const DataBaseOpts*);
            ~DbaseClient();

            void DatabaseInit();
            void DataBaseExit();
            void DataBaseLoop(const int);

            void DbNotifyModification();
            void DbStoreMltimZones(const MultiZonesParam&);
            std::shared_ptr<Message> DbExposeMltimZones();
            std::shared_ptr<Message> DbExposeSystemInfo();
            std::shared_ptr<Message> DbExposeVolatileInfo();

        private:
            int LoadRsvFromDb();//TODO

            int DbChkItem(DbData&,
                    const char*, //table
                    const char*, //item
                    const bool single = true, //if single data
                    const EE_CHK_DB_TYPE = E_CHK_TYPE_INT, //type
                    const EE_CHK_DB_SEQ = E_CHK_DESCEND); //data sequency
            int DbUpdtItem(const DbData&,
                    const char*, //table
                    const char*, //item
                    const EE_CHK_DB_TYPE = E_CHK_TYPE_INT); //type

            void DbChkCreateSysConf();
            void UpdatePowerupTime();

            void DbLoadSaveVolatile();
            void DbLoadSaveMultiZones();

            //volatile info
            inline int DbChkVolatile(DbData& dbd,
                    const char* item, //item
                    bool single = true, //if single data
                    const EE_CHK_DB_TYPE type = E_CHK_TYPE_INT, //type
                    const EE_CHK_DB_SEQ seq = E_CHK_DESCEND) { //data sequency
                return DbChkItem(dbd, TBL_VOLATILE_INFO, item, single, type, seq);
            }
            inline int DbUpdtVolatile(DbData& dbd,
                    const char* item, //item
                    const bool is_ready = true,
                    const EE_CHK_DB_TYPE type = E_CHK_TYPE_INT,
                    const int i_val = 0, //val int
                    const float f_val = 0, //val float
                    const char* s_val = nullptr) { //val string
                if (!is_ready) {
                    memset(&dbd, 0, sizeof(dbd));
                    dbd.mul_val_idx = 1;
                    if (type == E_CHK_TYPE_INT) {
                        dbd.ival[0] = i_val;
                    } else if (type == E_CHK_TYPE_FLOAT) {
                        dbd.fval[0] = f_val;
                    } else if (type == E_CHK_TYPE_STRING) {
                        strcpy(dbd.sval[0], s_val);
                    } else {
                        AWARN << "update volatile type error!";
                        return -1;
                    }
                }
                return DbUpdtItem(dbd, TBL_VOLATILE_INFO, item, type);
            }

            //muliti zones info
            inline int DbChkMultiZones(DbData& dbd,
                    const char* item, //item
                    bool single = false, //if single data
                    const EE_CHK_DB_TYPE type = E_CHK_TYPE_STRING, //type
                    const EE_CHK_DB_SEQ seq = E_CHK_DESCEND) { //data sequency
                return DbChkItem(dbd, TBL_MULTI_ZONES, item, single, type, seq);
            }
            inline int DbUpdtMultiZones(DbData& dbd,
                    const char* item, //item
                    const bool is_ready = true,
                    const EE_CHK_DB_TYPE type = E_CHK_TYPE_STRING,
                    const int i_val = 0, //val int
                    const float f_val = 0, //val float
                    const char* s_val = nullptr) { //val string
                if (!is_ready) {
                    memset(&dbd, 0, sizeof(dbd));
                    dbd.mul_val_idx = 1;
                    if (type == E_CHK_TYPE_INT) {
                        dbd.ival[0] = i_val;
                    } else if (type == E_CHK_TYPE_FLOAT) {
                        dbd.fval[0] = f_val;
                    } else if (type == E_CHK_TYPE_STRING) {
                        strcpy(dbd.sval[0], s_val);
                    } else {
                        AWARN << "update volatile type error!";
                        return -1;
                    }
                }
                return DbUpdtItem(dbd, TBL_MULTI_ZONES, item, type);
            }

        private:
            //sql operations
            std::shared_ptr<DbOperation> sql_;
            //appointment handle
            //std::shared_ptr<Appointment> app_;
            //database conf
            std::shared_ptr<DataBaseOpts> q_conf_;
            //sys info
            std::shared_ptr<SystemInformation> sys_info_;
            //volatile info
            std::shared_ptr<VolatileInformation> vola_info_;
            //multi zones info
            std::shared_ptr<MultiZonesParam> mzp_info_;

            std::atomic<bool> database_need_update_;

            uint32_t loop_cnt_ = 0;

            float last_note_history_time_ = 0.f;
    };

} //namespace dase
} //namespace camb
} //namespace mstf
