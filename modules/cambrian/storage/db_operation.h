#pragma once

#include <vector>

#include "cyber/common/log.h"
#include "cyber/common/util.h"
#include "cyber/common/file.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/cambrian/proto/cambrian_data.pb.h"

extern "C" {
#include "sqlite3.h"
}

#define TIME_STAMP_FORMAT "%Y-%m-%d %I:%M:%S"
#define TIME_DATE_FORMAT "%Y-%m-%d"
#define TIME_TIME_FORMAT "%H:%M:%S"
#define SET_TABLE_FORMAT "row: %02d, col: %02d, key: %s\n"
#define SET_TABLE_FORMAT_N "row: %02d, col: %02d, key: "
#define GET_TABLE_FORMAT_INT "row: %d, col: %d, key: %d\n"
#define GET_TABLE_FORMAT_ULL "row: %d, col: %d, key: %llu\n"
#define GET_TABLE_FORMAT_UL "row: %d, col: %d, key: %lu\n"
#define GET_TABLE_FORMAT_FLOAT "row: %d, col: %d, key: %f\n"
#define GET_TABLE_FORMAT_STRING "row: %d, col: %d, key: %[^\n]\n"
#define GET_TABLE_FORMAT_ROOM "row: %d, col: %d, key: %[^\n]\n"

//map
#define ITEM_MAPP_IDENTY "MapIDPrime"
#define ITEM_MAPP_SLAMID "SlamMapID"
#define ITEM_MAPP_TIMFRM "TimeFrom"
#define ITEM_MAPP_TIMEND "TimeEnd"
#define ITEM_MAPP_CLNTIM "CleanTime"
#define ITEM_MAPP_CLNARE "CleanArea"
#define ITEM_MAPP_ORIGIX "OriginalX"
#define ITEM_MAPP_ORIGIY "OriginalY"
#define ITEM_MAPP_DOCKEX "DockerX"
#define ITEM_MAPP_DOCKEY "DockerY"
#define ITEM_MAPP_WIDTHS "MapWidth"
#define ITEM_MAPP_HEIGHT "MapHeight"
#define ITEM_MAPP_CLNMOD "CleanMode"
#define ITEM_MAPP_USERTP "UserType"
#define ITEM_MAPP_CLNCMP "Complete"
#define ITEM_MAPP_FRAMEN "FrameNum"
#define ITEM_MAPP_BYTESS "MapBytes"
#define ITEM_MAPP_RESLUT "MapResolution"
#define ITEM_MAPP_COMRAT "CompressRate"
#define ITEM_MAPP_COMDAT "CompressBindMap"
#define TBL_HISTORY_MAP "HistoryMapData"
#define HISTORY_MAP_TABLE "CREATE TABLE HistoryMapData("\
			"MapIDPrime INTEGER PRIMARY KEY,"\
			"SlamMapID INTEGER,"\
			"TimeFrom INTERGER,"\
			"TimeEnd INTERGER,"\
			"CleanTime REAL,"\
			"CleanArea REAL,"\
			"OriginalX REAL,"\
			"OriginalY REAL,"\
			"DockerX REAL,"\
			"DockerY REAL,"\
			"MapWidth REAL,"\
			"MapHeight REAL,"\
			"CleanMode INTERGER,"\
			"UserType INTERGER,"\
			"Complete INTERGER,"\
			"FrameNum INTERGER,"\
			"MapBytes INTERGER,"\
			"MapResolution REAL,"\
			"CompressRate REAL,"\
			"CompressBindMap BLOB);"

//chassis, x
#define CHASSIS_DATA_TABLE "CREATE TABLE ChassisData("\
			"DropSensor TEXT,"\
		       	"IMU TEXT,"\
			"AlongWall TEXT,"\
			"TOF INTEGER);"
#define ITEM_CHASSIS_DROP "DropSensor"
#define ITEM_CHASSIS_IMU "IMU"
#define ITEM_CHASSIS_ALONGWALL "AlongWall"
#define ITEM_CHASSIS_TOF "TOF"
#define TBL_CHASSIS_DATA "ChassisData"

//reserve
#define ITEM_RES_ID "ReserveId"
#define ITEM_RES_TYPE "ReserveType"
#define ITEM_RES_ENABLE "ReserveSwitch"
#define ITEM_RES_HOUR "Hour"
#define ITEM_RES_MIN "Min"
#define ITEM_RES_SEC "Sec"
#define ITEM_RES_WEEK "Week"
#define ITEM_RES_MAP "MapId"
#define ITEM_RES_JOBS 	"Jobs"
#define TBL_RESERVE_DATA "ReserveData"
#define RESERVE_INFO_TABLE "CREATE TABLE ReserveData("\
			"ReserveId INTEGER PRIMARY KEY,"\
			"ReserveType INTEGER,"\
			"ReserveSwitch INTEGER,"\
			"Hour INTEGER,"\
			"Min INTEGER,"\
			"Sec INTEGER,"\
			"MapId INTEGER,"\
			"Week TEXT,"\
			"Jobs TEXT);"

//path
#define ITEM_PATH_SRLNUM "PathSerialPrime"
#define ITEM_PATH_IDENTY "PathOnceCleanID"
#define ITEM_PATH_SECTID "PathSectionID"
#define ITEM_PATH_COMPID "PathCorrespondMapID"
#define ITEM_PATH_TIMFRM "PathTimeFrom"
#define ITEM_PATH_TIMEND "PathTimeEnd"
#define ITEM_PATH_POSEFM "PoseFrom"
#define ITEM_PATH_POSETO "PoseTo"
#define ITEM_PATH_PSPONT "PosePoints"
#define TBL_HISTORY_PATH "HistoryPathData"
#define HISTORY_PATH_TABLE "CREATE TABLE HistoryPathData("\
			"PathSerialPrime INTEGER PRIMARY KEY,"\
			"PathOnceCleanID INTERGER,"\
			"PathSectionID INTERGER,"\
			"PathCorrespondMapID INTERGER,"\
			"PathTimeFrom INTERGER(8),"\
			"PathTimeEnd VARCHAR(8),"\
			"PoseFrom INTERGER,"\
			"PoseTo INTERGER,"\
			"PosePoints BLOB);"

//---------device info------------//
//obsolete, no need to save in database
#define ITEM_DEVINFO_PRMKEY "SysPrimeKey"

#define ITEM_DEVINFO_DEVNAM "DeviceName"
#define ITEM_DEVINFO_DEVTYP "DeviceType"
#define ITEM_DEVINFO_SERLSN "SerialNumber"
#define ITEM_DEVINFO_DVUUID "DeviceUuid"
#define ITEM_DEVINFO_DBPATH "DatabasePath"
#define ITEM_DEVINFO_PDTKEY "ProductKey"

#define ITEM_DEVINFO_SWVRSN "SoftwareVersion"
#define ITEM_DEVINFO_SYSVER "SystemVersion"
#define ITEM_DEVINFO_MCUVER "McuVersion"
#define ITEM_DEVINFO_RLDATE "ReleaseDate"
#define ITEM_DEVINFO_OTAPAH "OtaFilePath"

#define ITEM_DEVINFO_VCPATH "VoicePath"

#define ITEM_DEVINFO_SELFAP "SelfAp"
#define ITEM_DEVINFO_MACADR "MacAddr"

#define TBL_DEVICE_INFO "SysInfo"
#define SYSTEM_INFO_TABLE "CREATE TABLE SysInfo("\
			"SysPrimeKey INTEGER PRIMARY KEY,"\
			"DeviceName TEXT,"\
			"DeviceType TEXT,"\
			"SerialNumber TEXT,"\
			"DeviceUuid TEXT,"\
			"DatabasePath TEXT,"\
			"ProductKey TEXT,"\
			"SoftwareVersion TEXT,"\
			"SystemVersion TEXT,"\
			"McuVersion TEXT,"\
			"ReleaseDate TEXT,"\
			"OtaFilePath TEXT,"\
			"VoicePath TEXT,"\
			"SelfAp TEXT,"\
			"MacAddr TEXT);"
//---------device info------------//

//---------volatile info------------//
#define ITEM_VOLATILE_PRMKEY "VolatilePrimeKey"

#define ITEM_VOLATILE_NDSTMD "NoDisturbMode"
#define ITEM_VOLATILE_NDSSTA "NoDisturbStart"
#define ITEM_VOLATILE_NDSEND "NoDisturbEnd"

#define ITEM_VOLATILE_BATLVL "BatteryLevel"
#define ITEM_VOLATILE_FANSUC "FanSuction"
#define ITEM_VOLATILE_WTFSPD "FlowSpeed"
#define ITEM_VOLATILE_BOOSTM "BoostSw"
#define ITEM_VOLATILE_BREAKP "BreakSw"
#define ITEM_VOLATILE_WTRLVL "WaterLevel"
#define ITEM_VOLATILE_MOPSET "IsMopSet"

#define ITEM_VOLATILE_PWRUPT "PowerupTime"
#define ITEM_VOLATILE_PWRSEC "PoweronTimeSec"
#define ITEM_VOLATILE_PWRTTL "PoweronTimeTotal"
#define ITEM_VOLATILE_MSSCNT "MissionCount"
#define ITEM_VOLATILE_MLGTTL "MileageTotal"
#define ITEM_VOLATILE_TCTIME "CleanTimeTotal"
#define ITEM_VOLATILE_TCAREA "CleanAreaTotal"

#define ITEM_VOLATILE_LANGUG "Language"
#define ITEM_VOLATILE_AUDVOL "Volume"

#define ITEM_VOLATILE_IFCONF "IfConfed"
#define ITEM_VOLATILE_WFSSID "WifiSsid"
#define ITEM_VOLATILE_PASSWD "WifiPswd"
#define ITEM_VOLATILE_IPADDR "IpAddr"

#define ITEM_VOLATILE_STNUSE "StrainerCur"
#define ITEM_VOLATILE_STNTTL "StrainerTotal"
#define ITEM_VOLATILE_MBSUSE "MainBrushCur"
#define ITEM_VOLATILE_MBSTTL "MainBrushTotal"
#define ITEM_VOLATILE_SDBUSE "SideBrushCur"
#define ITEM_VOLATILE_SDBTTL "SideBrushTotal"
#define ITEM_VOLATILE_SNSUSE "SensorCur"
#define ITEM_VOLATILE_SNSTTL "SensorTotal"
#define ITEM_VOLATILE_WTRUSE "WaterFilterCur"
#define ITEM_VOLATILE_WTRTTL "WaterFilterTotal"

#define TBL_VOLATILE_INFO "Volatile"
#define SQL_CREATE_TBL_VOLATILE\
    "CREATE TABLE Volatile("\
    "NoDisturbMode INTEGER,"\
    "NoDisturbStart TEXT,"\
    "NoDisturbEnd TEXT,"\
    "BatteryLevel INTEGER,"\
    "FanSuction INTEGER,"\
    "FlowSpeed INTEGER,"\
    "BoostSw INTEGER,"\
    "BreakSw INTEGER,"\
    "WaterLevel INTEGER,"\
    "IsMopSet INTEGER,"\
    "PowerupTime TEXT,"\
    "PoweronTimeSec REAL,"\
    "PoweronTimeTotal REAL,"\
    "MissionCount INTEGER,"\
    "MileageTotal INTEGER,"\
    "CleanTimeTotal INTEGER,"\
    "CleanAreaTotal REAL,"\
    "Language TEXT,"\
    "Volume INTEGER,"\
    "IfConfed TEXT,"\
    "WifiSsid TEXT,"\
    "WifiPswd TEXT,"\
    "IpAddr TEXT,"\
    "StrainerCur INTEGER,"\
    "StrainerTotal INTEGER,"\
    "MainBrushCur INTEGER,"\
    "MainBrushTotal INTEGER,"\
    "SideBrushCur INTEGER,"\
    "SideBrushTotal INTEGER,"\
    "SensorCur INTEGER,"\
    "SensorTotal INTEGER,"\
    "WaterFilterCur INTEGER,"\
    "WaterFilterTotal INTEGER);"

#define SQL_INSERT_TBL_VOLATILE "INSERT INTO \"Volatile\" "\
	"VALUES(%d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, '%s', %f, %f, %d, %d, %d, %f, "\
    "'%s', %d, '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);"
//---------volatile info------------//

//app信息保存
#define ITEM_APPINFO_DATAONE "DataOne"
#define ITEM_APPINFO_CUSTOMMADE "CustomMade"
#define TBL_APP_INFO "AppInfo"
#define APP_INFO_TABLE "CREATE TABLE AppInfo("\
			"AppInfoKeyPrime INTEGER PRIMARY KEY,"\
			"CustomMade INTEGER,"\
			"DataOne INTEGER,"\
			"DataTwo INTEGER,"\
			"DataThree INTEGER,"\
			"DataFour INTEGER,"\
			"DataFive INTEGER,"\
			"DataSix INTEGER,"\
			"DataSeven INTEGER,"\
			"DataBO BLOB,"\
			"DataBT BLOB);"


//---------multi zones------------//
#define  TBL_MULTI_ZONES "MultiZones"
#define AREA_STORAGE_TABLE "CREATE TABLE Areas("\
			"PathID INTEGER PRIMARY KEY,"\
			"AreaType,"\
			"AreaCordinate BLOB);"

#define  ITEM_MAPINFO_MAPID "MapIdPrime"
#define  ITEM_MAPINFO_UPLOADID "UploadMapId"
#define  ITEM_MAPINFO_SLAMID "SlampId"
#define  ITEM_MAPINFO_DOCK "DockLocation"
#define  ITEM_MULTI_ZONES "MultiZones"
#define  ITEM_MULTI_ZONES_AREA "ZoneArea"
#define  ITEM_MAPINFO_VIRTUALWALL "VirtualWall"
//---------multi zones------------//

#define  TBL_MAP_AREAS "OfflineMapAreasInfo"

//作废
#define MAP_AREAS_INFO_STORAGE_TABLE "CREATE TABLE MapAreasInfo("\
			"MapIdPrime INTEGER PRIMARY KEY,"\
			"DockLocation BLOB,"\
			"SmartRoom BLOB,"\
			"VirtualWall BLOB);"
//离线地图表
#define OFFLINE_MAP_AREAS_INFO_STORAGE_TABLE "CREATE TABLE OfflineMapAreasInfo("\
			"MapIdPrime INTEGER PRIMARY KEY,"\
			"UploadMapId INTEGER,"\
			"SlampId INTEGER,"\
			"DockLocation BLOB,"\
			"SmartRoom BLOB,"\
			"VirtualWall BLOB,"\
			"OtherInfo BLOB);"
#define TIME_STAMP_STYLE "%Y-%m-%d %H:%M:%S"

//1, insert
//map
#define SQL_OPERATION_INSERT_MAP "INSERT INTO \"HistoryMapData\" "\
	"VALUES(%d, %lu, '%d', '%d', %f, %f, %f, %f, "\
	"%f, %f, %d, %d, %d, %d, %d, %d, %d, %f, %f, '%s');"
//报废mapareas
#define SQL_OPERATION_INSERT_MAP_AREAS "INSERT INTO \"MapAreasInfo\" "\
	"VALUES(%lu,'%s', '%s', '%s');"
//离线地图信息插入
#define SQL_OPERATION_INSERT_OFFLINE_MAP_AREAS "INSERT INTO \"OfflineMapAreasInfo\" "\
	"VALUES(%lu,%lu,%d,'%s', '%s', '%s', '%s');"
//path
#define SQL_OPERATION_INSERT_PATH "INSERT INTO \"HistoryPathData\" "\
	"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, '"
#define SQL_OPERATION_INSERT_PEND "\');"
//reserve
#define SQL_OPREATION_INSERT_RESERVE "INSERT INTO \"ReserveData\" "\
	"VALUES(%llu, %d, %d, %d, %d, %d, %llu, %d, '%s');"
//infoapp
#define SQL_OPREATION_INSERT_APPINFO "INSERT INTO \"AppInfo\" "\
	"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s');"

//2, delete
#define SQL_OPERATION_DELETE "DELETE FROM %s WHERE %s = %d;"
#define SQL_OPERATION_DROP "DROP FROM %s;"

//3, update
#define SQL_OPERATION_ALTER "ALTER * FROM %s;"
#define SQL_OPERATION_UPDATE_INT "UPDATE %s SET %s = %d;"
#define SQL_OPERATION_UPDATE_FLOAT "UPDATE %s SET %s = %f;"
#define SQL_OPERATION_UPDATE_STRING "UPDATE %s SET %s = '%s';"

#define SQL_OPERATION_UPDATE_ONE "UPDATE %s SET %s = '%s' %s;"
#define SQL_OPERATION_UPDATE_UPLOADID_ONE "UPDATE %s SET %s = %lu %s;"
#define SQL_OPERATION_CONDITION "WHERE %s = %lu;"
//4, ckeckout
#define SQL_OPERATION_CHECKOUT "SELECT %s FROM %s %s;"

//AUTO, fan/water speed
#define SQL_OPERATION_INSERT_RESERVE_AUTO "JOBS-auto-%d,%d,END "
//ZONE, index/repeated times/fan/water speed
#define SQL_OPERATION_INSERT_RESERVE_ZONE "JOBS-zone-idx:%d,%d,%d,%d,END "
//ROOM, index/repeated times/fan/water speed/room name
#define SQL_OPERATION_INSERT_RESERVE_ROOM "JOBS-room-idx:%d,%d,%d,%d,%s,END "
//delete Res
#define SQL_DELETE_RESID "DELETE FROM ReserveData WHERE ReserveId = %llu;"

namespace mstf {
namespace camb {
namespace dbase {

    using namespace /*mstf::camb:;*/proto;
    using namespace apollo;
    using namespace google::protobuf;

    class DbOperation {
        public:
            DbOperation();
            ~DbOperation();

            int SqlInit(const std::string&);
            void SqlExit();

            int SqlCheckOut(std::string& result, //get RESULT
                    const char* tbl, //from TABLE
                    const char* item, //checkout ITEM
                    EE_CHK_DB_TYPE = E_CHK_TYPE_STRING, //data TYPE
                    EE_CHK_DB_SEQ = E_CHK_ASCEND); //data sequency

            int SqlUpdateInt(const char* tbl,
                    const char* item,
                    const int&);
            int SqlUpdateFloat(const char* tbl,
                    const char* item,
                    const float&);
            int SqlUpdateString(const char* tbl,
                    const char* item,
                    const std::string&);

            inline bool IsDbOperationOK() {
                return sql_inited_;
            }

        private:
            //sqlite3 core
            sqlite3* sql3_ = nullptr;

            bool sql_inited_ = false;
    };

} //namespace dase
} //namespace camb
} //namespace mstf
