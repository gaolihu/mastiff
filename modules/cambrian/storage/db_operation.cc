#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <sstream>

#include "modules/cambrian/storage/db_operation.h"

namespace mstf {
namespace camb {
namespace dbase {

    DbOperation::DbOperation() {
#ifdef CAMB_PKG_DBG
        AINFO << "DbOperation construct";
#endif
    }

    DbOperation::~DbOperation() {
#ifdef CAMB_PKG_DBG
        AINFO << "DbOperation de-construct";
#endif

        SqlExit();
    }

    int DbOperation::SqlInit(const std::string& db_path) {
#ifdef CAMB_PKG_DBG
        AINFO << "sql operation init, db: " << db_path;
#endif
        int ret = -1;
        char* err_msg = nullptr;
        char buf[1024] {};

        //off_map_tbl_ = OFFLINE_MAP_AREAS_INFO_STORAGE_TABLE;
        //map_tbl_ = HISTORY_MAP_TABLE;
        //path_tbl_ = HISTORY_PATH_TABLE;
        //app_tbl_ = RESERVE_INFO_TABLE;

        if (access(db_path.data(), F_OK) != 0) {
            AINFO << "create db file: " << db_path;
            //map data base initialize
            sqlite3_initialize();
            if ((ret = sqlite3_open_v2(db_path.data(),
                            &sql3_,
                            SQLITE_OPEN_READWRITE |
                            SQLITE_OPEN_CREATE |
                            SQLITE_OPEN_NOMUTEX |
                            SQLITE_OPEN_SHAREDCACHE,
                            NULL)) != SQLITE_OK) {
                AWARN << "create data base error: " << ret;
                if (!sql3_) {
                    sqlite3_close(sql3_);
                    sql3_ = nullptr;
                }

                return -1;
            }
            sqlite3_close(sql3_);
            sql3_ = nullptr;

            if ((ret = sqlite3_open(db_path.data(),
                            &sql3_)) != SQLITE_OK) {
                AWARN << "open map data base error!";
                if (sql3_) {
                    sqlite3_close(sql3_);
                    sql3_ = nullptr;
                }

                return -1;
            }

            //create volatile table
            sqlite3_exec(sql3_, SQL_CREATE_TBL_VOLATILE, 0, 0, &err_msg);
            AINFO << "create volatile table: " << (err_msg == NULL ? "OK" : err_msg);
            //insert volatile infos
            sprintf(buf, SQL_INSERT_TBL_VOLATILE,
                    -1, "nul-start", "nul-end", //no disturb, placeholder
                    100, //battery full,
                    1, //fan suction 1
                    1, //flow speed 1
                    3, //boost sw
                    3, //break sw
                    2, //water level
                    3, //mop is set?
                    "unknown", //poweron time
                    7.77f, //poweron time seconds
                    77.77f, //poweron time seconds
                    3, //mission count
                    3, //mileage total
                    3, //clean time total
                    0.3f, //clean area total
                    "Chinese", //language Chinese
                    51, //volume 50
                    "no", //configured?
                    "ssid-null", //no wifi
                    "pswd-null", //no pswd
                    "ip-null", //no ip
                    1, //strainer used
                    720000, //strainer total
                    1, //main brush used
                    720000, //main brush total
                    1, //side brush used
                    720000, //side brush total
                    1, //sensor used
                    720000, //sensor total
                    1, //filter used
                    720000); //filter total

            sqlite3_exec(sql3_, buf, 0, 0, &err_msg);
#ifdef CAMB_PKG_DBG
            AINFO << "insert vola buf: " << buf;
#endif
            AINFO << "insert volatile table: " << (err_msg == NULL ? "OK" : err_msg);

            if (sql3_) {
                sqlite3_close(sql3_);
                sql3_ = nullptr;
            }
        } else {
            //update system info
            if ((ret = sqlite3_open(db_path.data(),
                            &sql3_)) != SQLITE_OK) {
                AERROR << "open map data base error!";
                sqlite3_close(sql3_);
                sql3_ = nullptr;

                return -1;
            }

            AINFO << "sql3 load db file over!";
        }

        if ((ret = sqlite3_open(db_path.data(),
                        &sql3_)) != SQLITE_OK) {
            AERROR << "open map data base error!";
            sqlite3_close(sql3_);
            sql3_ = nullptr;

            return -1;
        }

        AINFO << "init sqlite3 OK" ;

        sql_inited_ = true;

        return 0;
    }

    void DbOperation::SqlExit() {
        AWARN << "sql DbOperation exit!\n";

        if (sql3_ != nullptr)
            sqlite3_close(sql3_);

        sql3_ = nullptr;
    }

    int DbOperation::SqlUpdateInt(const char* tbl, const char* item,
            const int& up) {
        char buf[1024] = {0};
        char* err_msg = nullptr;

#ifdef DB_DATA_DBG
        AINFO << "update int item: " << item <<
            ", in table: " << tbl <<
            ", contents: " << up;
#endif

        memset(buf, 0, sizeof(buf));
        sprintf(buf, SQL_OPERATION_UPDATE_INT, tbl, item, up);

#ifdef CAMB_PKG_DBG
        AINFO << "update sql: {" << buf << "}";
#endif

        sqlite3_exec(sql3_, buf, 0, 0, &err_msg);
        if (err_msg != nullptr) {
            AERROR << "UPDATE: {" << buf <<
                "}" <<
                ", error: " << err_msg;
        }

        return err_msg == NULL ? 0 : -1;
    }

    int DbOperation::SqlUpdateFloat(const char* tbl, const char* item,
            const float& up) {
        char buf[1024] = {0};
        char* err_msg = nullptr;

#ifdef DB_DATA_DBG
        AINFO << "update float item: " << item <<
            ", in table: " << tbl;
#endif

        memset(buf, 0, sizeof(buf));
        sprintf(buf, SQL_OPERATION_UPDATE_FLOAT, tbl, item, up);

#ifdef CAMB_PKG_DBG
        AINFO << "update sql: {" << buf << "}";
#endif

        sqlite3_exec(sql3_, buf, 0, 0, &err_msg);
        if (err_msg != nullptr) {
            AERROR << "UPDATE: {" << buf <<
                "}" <<
                ", error: " << err_msg;
        }

        return err_msg == NULL ? 0 : -1;
    }

    int DbOperation::SqlUpdateString(const char* tbl, const char* item,
            const std::string& up) {
        char buf[1024] = {0};
        char* err_msg = nullptr;

#ifdef DB_DATA_DBG
        AINFO << "update string item: " << item <<
            ", in table: " << tbl;
#endif

        memset(buf, 0, sizeof(buf));
        sprintf(buf, SQL_OPERATION_UPDATE_STRING, tbl, item, up.c_str());

#ifdef CAMB_PKG_DBG
        AINFO << "update sql: {" << buf << "}";
#endif

        sqlite3_exec(sql3_, buf, 0, 0, &err_msg);
        if (err_msg != nullptr) {
            AERROR << "UPDATE: {" << buf <<
                "}" <<
                ", error: " << err_msg;
        }

        return err_msg == NULL ? 0 : -1;
    }

    int DbOperation::SqlCheckOut(std::string& result,
            const char* tbl,
            const char* item,
            EE_CHK_DB_TYPE type,
            EE_CHK_DB_SEQ seq) {
        int i = 0;
        char buf[512] = {};
        char chk_s[128] = {};
        int nrow = 0, ncol = 0;
        char* chks = nullptr;
        char* err_msg = nullptr;
        char** all_chk = nullptr;

        if (!sql_inited_) {
#ifdef CAMB_PKG_DBG
            AWARN << "sql not initialized!";
#endif
            return -1;
        }

        memset(chk_s, 0, sizeof(chk_s));

        if (seq == E_CHK_DESCEND)
            sprintf(chk_s, "ORDER BY %s DESC", item);
        else if (seq == E_CHK_ASCEND)
            sprintf(chk_s, "ORDER BY %s ASC", item);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, SQL_OPERATION_CHECKOUT, item, tbl, chk_s);
        sqlite3_get_table(sql3_, buf, &all_chk, &nrow, &ncol, &err_msg);

        if (err_msg != NULL) {
            AERROR << "CHECKOUT: {" << buf <<
                "} row: " << nrow <<
                ", col: " << ncol <<
                ", error: {" << err_msg << "}";
            if (strncmp(err_msg, "no such table:",
                        strlen("no such table:")) == 0) {
                return -2;
            }
        } else {
            for (i = 0; i < (nrow + 1) * ncol; i++) {
                memset(buf, 0, sizeof(buf));
                chks = all_chk[i];

                snprintf(buf, 24 + strlen(chks),
                        SET_TABLE_FORMAT_N, nrow, ncol);

                result.append(buf);
                result.append(chks, 0, strlen(chks));
                result.append("\n");
            }

#ifdef DB_DATA_DBG
            AINFO << "checkout item:" << item <<
                ", from table: " << tbl <<
                ", ressult:\n" << result;
#endif
        }

        sqlite3_free_table(all_chk);
        return err_msg == NULL ? 0 : -1;
    }

} //namespace dase
} //namespace camb
} //namespace mstf
