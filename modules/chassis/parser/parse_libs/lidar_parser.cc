#include <algorithm>

#include "modules/chassis/parser/parse_libs/lidar_parser.h"
#include "modules/chassis/proto/frame_up_stream.pb.h"

namespace mstf {
namespace chss {
namespace parser {

    LidarParser::LidarParser(const ChassisConfig* cc,
            const SensorInfo* si) :
        ParserBaseItf(cc, si) {
            AINFO << "LidarParser construct";
            fan_segs_ = new FanSegment_AA*;
            *fan_segs_ = nullptr;
            //userdata_.type = FRAMEDATA;
            timeval tv;
            gettimeofday(&tv, NULL);
            point_cloud_last_ = (double)(tv.tv_sec + tv.tv_usec / 1e6);
        }

    LidarParser::~LidarParser() {
        AINFO << "LidarParser de-construct";
        delete fan_segs_;
    }

    int LidarParser::Init() {
        return ParserBaseItf::Init(chs_conf_->
                lidar_dev().sn_ind().port(),
                &chs_conf_->lidar_dev().
                uart_conf().buf_setting());
    }

    //lidar protocol related
    static int find(std::vector<RawData>a, int n, int x)
    {
        int i;
        int min = abs(a.at(0).angle - x);
        int r = 0;

        for (i = 0; i < n; ++i)
        {
            if (abs(a.at(i).angle - x) < min)
            {
                min = abs(a.at(i).angle - x);
                r = i;
            }
        }

        return a[r].angle;
    }

    static void DecTimestamp(uint32_t ts, uint32_t* ts2) {
        timeval tv;
        gettimeofday(&tv, NULL);

        uint32_t sec = tv.tv_sec % 3600;
        if (sec < 5 && ts / 1000 > 3595)
        {
            ts2[0] = (tv.tv_sec / 3600 - 1) * 3600 + ts / 1000;
        }
        else {
            ts2[0] = (tv.tv_sec / 3600) * 3600 + ts / 1000;
        }

        ts2[1] = (ts % 1000) * 1000;
    }

    static bool checkPointsLengthZero(UserLidarData *tmp, float scale)
    {
        int lengthZeroNum = 0;
        for (int i = 0; i < tmp->data.framedata.N; i++)
        {
            if (tmp->data.framedata.data[i].distance == 0)
            {
                lengthZeroNum++;
            }
        }
        // printf("lengthZeroNum:%d N:%d scale:%f\n", lengthZeroNum, tmp.N, tmp.N * scale);
        if (tmp->data.framedata.N * scale < lengthZeroNum)
            return true;
        return false;
    }

    static int GetFanPointCount(FanSegment_C7* seg)
    {
        int n = 0;

        while (seg) { n += seg->hdr.N;  seg = seg->next; }

        return n;
    }

    static int GetFanPointCount(FanSegment_AA* seg)
    {
        int n = 0;

        while (seg) { n += seg->hdr.N;  seg = seg->next; }

        return n;
    }

    static int autoGetFirstAngle(const RawData &raw, /*bool from_zero, */ std::vector<RawData>& raws,std::string& error)
    {
        int angles = 0;
        char tmp_error[128]={0};
        int size = raws.size();

        if(size>=1)
        {
            RawData tmp = raws.at(size - 1);
            RawData tmp2 = raws.at(0);
            if(raw.angle==tmp2.angle)
            {
                for (int i=0;i<size;i++)
                {
                    tmp = raws.at(i);
                    angles += tmp.span;
                }
                if (angles != 3600)
                {

                    sprintf(tmp_error,"angle sum %d\n", angles);
                    error = tmp_error;
                    raws.clear();

                    AWARN << "autoGetFirstAngle, size: " << size <<
                        "ret: " << -2;

                    return -2;
                }
                else
                {
                    int ret = -1;
                    ret = find(raws, raws.size(), 1800);
                    raws.clear();

                    return ret;
                }
            }
            if(raw.angle == (tmp.angle+tmp.span) % 3600)
            {
                //说明是连续的扇区
                raws.push_back(raw);

            }

        }
        else
            raws.push_back(raw);

        //do not get first angle

        return -1;
    }

    static int whole_data_process(const RawData& raw, int collect_angle, std::vector<RawData>& whole_datas, std::string& error)
    {
        char tmp[128] = {0};

        whole_datas.push_back(raw);
        if ((raw.angle + raw.span) % 3600 == collect_angle)
        {
            //int a=1;
#if 0
            AINFO << "0, size: " << whole_datas.size() <<
                ", points: " << raw.N <<
                ", angle: " << raw.angle <<
                ", span: " << raw.span;
#endif
        }
        else
        {
            // whole_datas.push_back(raw);
#if 0
            AINFO << "1, size: " << whole_datas.size() <<
                ", points: " << raw.N <<
                ", angle: " << raw.angle <<
                ", span: " << raw.span;
#endif
            return 0;
        }

        int count = 0, n = 0, angles = 0;
        for (std::vector<RawData>::iterator it = whole_datas.begin(); it != whole_datas.end(); ++it)
        {
            RawData tmp = *it;
            angles += tmp.span;
            count += tmp.N;
            n++;
        }
        if (angles != 3600)
        {
            sprintf(tmp,"angle sum %d, drop %d fans %d points", angles, n, count);
            error = tmp;
            whole_datas.clear();
            return -1;
        }

        return 1;
    }

    static void PackFanData(FanSegment_C7* seg, RawData& rdat)
    {
        RawData* dat = &rdat;

        dat->code = 0xfac7;
        dat->N = seg->hdr.whole_fan;
        dat->angle = seg->hdr.beg_ang / 100; // 0.1 degree
        dat->span = (seg->hdr.end_ang - seg->hdr.beg_ang) / 100; // 0.1 degree
        dat->fbase = 0;
        dat->first = 0;
        dat->last = 0;
        dat->fend = 0;
        dat->flags = seg->hdr.flags;

        DecTimestamp(seg->hdr.timestamp, dat->ts);

        int count = 0;
        while (seg)
        {
            double s = PI / 180000.0;
            for (int i = 0; i < seg->hdr.N; i++, count++)
            {
                dat->points[count].confidence = seg->energy[i];
                dat->points[count].distance = seg->dist[i] / 1000.0;
                dat->points[count].angle = (seg->angle[i] + seg->hdr.beg_ang) * s;
            }

            seg = seg->next;
        }
        //return dat;
    }

    static void PackFanData(FanSegment_AA* seg, RawData& rdat)
    {
        RawData* dat = &rdat;

        dat->code = 0xfaaa;
        dat->N = seg->hdr.whole_fan;
        dat->angle = seg->hdr.beg_ang / 100; // 0.1 degree
        dat->span = (seg->hdr.end_ang - seg->hdr.beg_ang) / 100; // 0.1 degree
        dat->fbase = 0;
        dat->first = 0;
        dat->last = 0;
        dat->fend = 0;
        dat->flags = seg->hdr.flags;

        dat->ts[0] = seg->hdr.second;
        dat->ts[1] = seg->hdr.nano_sec / 1000;

        int count = 0;
        while (seg)
        {
            double s = PI / 180000.0;
            for (int i = 0; i < seg->hdr.N; i++, count++)
            {
                dat->points[count].confidence = seg->energy[i];
                dat->points[count].distance = seg->dist[i] / 1000.0;
                dat->points[count].angle = (seg->angle[i] + seg->hdr.beg_ang) * s;
            }

            seg = seg->next;
        }
        //return dat;
    }

    static FanSegment_C7* GetFanSegment(const RawDataHdr7& hdr, uint8_t* pdat, bool with_chk)
    {
        UNUSED(with_chk);
        FanSegment_C7* fan_seg = new FanSegment_C7;
        if (!fan_seg) {
            printf("out of memory\n");
            return NULL;
        }
        fan_seg->hdr = hdr;
        fan_seg->next = NULL;

        uint16_t sum = 0;
        //if (with_chk)
        {
            uint16_t* pchk = (uint16_t*)pdat;
            for (int i = 1; i < HDR7_SIZE / 2; i++)
                sum += pchk[i];
        }


        uint8_t* pDist = pdat + HDR7_SIZE;
        uint8_t* pAngle = pdat + HDR7_SIZE + 2 * hdr.N;
        uint8_t* energy = pdat + HDR7_SIZE + 4 * hdr.N;

        for (int i = 0; i < hdr.N; i++, pDist += 2, pAngle += 2)
        {
            fan_seg->dist[i] = ((uint16_t)(pDist[1]) << 8) | pDist[0];
            fan_seg->angle[i] = ((uint16_t)(pAngle[1]) << 8) | pAngle[0];
            fan_seg->energy[i] = energy[i];

            sum += fan_seg->dist[i];
            sum += fan_seg->angle[i];
            sum += energy[i];
        }

        uint8_t* pchk = pdat + HDR7_SIZE + 5 * hdr.N;
        uint16_t chksum = ((uint16_t)(pchk[1]) << 8) | pchk[0];
        if (chksum != sum) {
            printf("checksum error\n");
            delete fan_seg;
            return NULL;
        }

        return fan_seg;
    }

    static FanSegment_AA* GetFanSegment(const RawDataHdrAA& hdr, uint8_t* pdat, bool with_chk)
    {
        UNUSED(with_chk);
        FanSegment_AA* fan_seg = new FanSegment_AA;
        if (!fan_seg) {
            printf("out of memory\n");
            return NULL;
        }
        fan_seg->hdr = hdr;
        fan_seg->next = NULL;

        uint16_t sum = 0;
        //if (with_chk)
        {
            uint16_t* pchk = (uint16_t*)pdat;
            for (int i = 1; i < (HDRAA_SIZE) / 2; i++)
                sum += pchk[i];
        }


        uint8_t* pDist = pdat + HDRAA_SIZE;
        uint8_t* pAngle = pdat + HDRAA_SIZE + 2 * hdr.N;
        uint8_t* energy = pdat + HDRAA_SIZE + 4 * hdr.N;

        for (int i = 0; i < hdr.N; i++, pDist += 2, pAngle += 2)
        {
            fan_seg->dist[i] = ((uint16_t)(pDist[1]) << 8) | pDist[0];
            fan_seg->angle[i] = ((uint16_t)(pAngle[1]) << 8) | pAngle[0];
            fan_seg->energy[i] = energy[i];

            sum += fan_seg->dist[i];
            sum += fan_seg->angle[i];
            sum += energy[i];
        }

        uint8_t* pchk = pdat + HDRAA_SIZE + 5 * hdr.N;
        uint16_t chksum = ((uint16_t)(pchk[1]) << 8) | pchk[0];
        if (chksum != sum) {
            printf("checksum error\n");
            delete fan_seg;
            return NULL;
        }

        return fan_seg;
    }

    static bool GetData0xC7(const RawDataHdr7& hdr, uint8_t* pdat, bool with_chk, RawData& dat,char *result, FanSegment_C7** last_fan_seg) {
        bool got = false;

        FanSegment_C7* fan_seg = GetFanSegment(hdr, pdat, with_chk);
        if (!fan_seg) {
            return NULL;
        }
        if (*last_fan_seg != NULL)
        {
            FanSegment_C7* seg = (*last_fan_seg);

            if (seg->hdr.timestamp != fan_seg->hdr.timestamp)
            {
                //printf("drop old fan segments\n");
                strcpy(result,"drop old fan segments");
                while (seg) {
                    (*last_fan_seg) = seg->next;
                    delete seg;
                    seg = (*last_fan_seg);
                }
                (*last_fan_seg) = fan_seg;
            }
            else {
                while (seg) {
                    if (seg->hdr.ofset == fan_seg->hdr.ofset) {
                        strcpy(result,"drop duplicated segment");
                        //printf("drop duplicated segment\n");
                        delete fan_seg;
                        fan_seg = NULL;
                        break;
                    }
                    if (seg->next == NULL) {
                        seg->next = fan_seg;
                        break;
                    }
                    seg = seg->next;
                }
            }
        }

        if (*last_fan_seg == NULL && fan_seg != NULL)
        {
            *last_fan_seg = fan_seg;
        }

        int N = GetFanPointCount((*last_fan_seg));

        if (N >= (*last_fan_seg)->hdr.whole_fan)
        {
            if (N == (*last_fan_seg)->hdr.whole_fan)
            {
                if (N > (int)(sizeof(dat.points) / sizeof(dat.points[0])))
                {
                    //printf("too many %d points in 1 fan\n", N);
                    sprintf(result,"too many %d points in 1 fan",N);

                }
                else
                {
                    PackFanData((*last_fan_seg), dat);
                    got = true;
                }
            }

            // remove segments
            FanSegment_C7* seg = (*last_fan_seg);
            while (seg) {
                (*last_fan_seg) = seg->next;
                delete seg;
                seg = (*last_fan_seg);
            }
        }
        return got;
    }

    static bool GetData0xAA(const RawDataHdrAA& hdr, uint8_t* pdat, bool with_chk, RawData& dat, char* result, FanSegment_AA** last_fan_seg)
    {
        bool got = false;

        FanSegment_AA* fan_seg = GetFanSegment(hdr, pdat, with_chk);
        if (!fan_seg) {
            return false;
        }
        if (*last_fan_seg != NULL)
        {
            FanSegment_AA* seg = (*last_fan_seg);

            if ((seg->hdr.second != fan_seg->hdr.second) && (seg->hdr.nano_sec != fan_seg->hdr.nano_sec))
            {
                //printf("drop old fan segments\n");
                strcpy(result, "drop old fan segments");
                while (seg) {
                    (*last_fan_seg) = seg->next;
                    delete seg;
                    seg = (*last_fan_seg);
                }
                (*last_fan_seg) = fan_seg;
            }
            else {
                while (seg) {
                    if (seg->hdr.ofset == fan_seg->hdr.ofset) {
                        strcpy(result, "drop duplicated segment");
                        //printf("drop duplicated segment\n");
                        delete fan_seg;
                        fan_seg = NULL;
                        break;
                    }
                    if (seg->next == NULL) {
                        seg->next = fan_seg;
                        break;
                    }
                    seg = seg->next;
                }
            }
        }

        if (*last_fan_seg == NULL && fan_seg != NULL)
        {
            *last_fan_seg = fan_seg;
        }

        unsigned int N = GetFanPointCount((*last_fan_seg));

        if (N >= (*last_fan_seg)->hdr.whole_fan)
        {
            if (N == (*last_fan_seg)->hdr.whole_fan)
            {
                if (N > sizeof(dat.points) / sizeof(dat.points[0]))
                {
                    //printf("too many %d points in 1 fan\n", N);
                    sprintf(result, "too many %d points in 1 fan", N);

                }
                else
                {
                    PackFanData((*last_fan_seg), dat);
                    got = true;
                }
            }

            // remove segments
            FanSegment_AA* seg = (*last_fan_seg);
            while (seg) {
                (*last_fan_seg) = seg->next;
                delete seg;
                seg = (*last_fan_seg);
            }
        }
        return got;
    }

    static bool GetData0xCE(const RawDataHdr& hdr, unsigned char* pdat, int span, int with_chk, RawData& dat,char*result)
    {
        // calc checksum
        unsigned short sum = hdr.angle + hdr.N, chk;
        for (int i = 0; i < hdr.N; i++)
        {
            dat.points[i].confidence = *pdat++;
            sum += dat.points[i].confidence;

            unsigned short v = *pdat++;
            unsigned short v2 = *pdat++;
            unsigned short vv = (v2 << 8) | v;
            dat.points[i].distance = vv / 1000.0;
            sum += vv;
            dat.points[i].angle = (hdr.angle + (double)span * i / hdr.N) * PI / 1800;
        }

        memcpy(&chk, pdat, 2);

        if (with_chk != 0 && chk != sum)
        {
            printf("chksum ce error");
            strcpy(result,"chksum  error");
            return false;
        }

        memcpy(&dat, &hdr, HDR_SIZE);
        dat.span = span;
        return true;
    }

    static bool GetData0x9D(const RawDataHdr2& hdr, unsigned char* pdat, int with_chk, RawData& dat,char*result)
    {
        unsigned short sum = hdr.angle + hdr.N + hdr.span, chk;

        for (int i = 0; i < hdr.N; i++)
        {
            dat.points[i].confidence = 1;// *pdat++;
                                         //sum += dat.points[i].confidence;

            unsigned short v = *pdat++;
            unsigned short v2 = *pdat++;

            unsigned short vv = (v2 << 8) | v;

            sum += vv;
            dat.points[i].distance = vv / 1000.0;
            dat.points[i].angle = (hdr.angle + hdr.span * i / (double)hdr.N) * PI / 1800;
        }

        memcpy(&chk, pdat, 2);

        if (with_chk != 0 && chk != sum)
        {
            printf("chksum cf error");
            strcpy(result,"chksum  error");
            return 0;
        }

        memcpy(&dat, &hdr, sizeof(hdr));
        return true;

    }


    static bool GetData0xCF(const RawDataHdr2& hdr, unsigned char* pdat, int with_chk, RawData& dat,char*result)
    {
        unsigned short sum = hdr.angle + hdr.N + hdr.span, chk;

        for (int i = 0; i < hdr.N; i++)
        {
            dat.points[i].confidence = *pdat++;
            sum += dat.points[i].confidence;

            unsigned short v = *pdat++;
            unsigned short v2 = *pdat++;

            unsigned short vv = (v2 << 8) | v;

            sum += vv;
            dat.points[i].distance = vv / 1000.0;
            dat.points[i].angle = (hdr.angle + hdr.span * i / (double)hdr.N) * PI / 1800;
        }

        memcpy(&chk, pdat, 2);

        if (with_chk != 0 && chk != sum)
        {
            printf("chksum cf error");
            strcpy(result,"chksum  error");
            return 0;
        }

        memcpy(&dat, &hdr, sizeof(hdr));
        //memcpy(dat.data, buf+idx+HDR_SIZE, 2*hdr.N);
        //printf("get3 %d(%d)\n", hdr.angle, hdr.N);

        return true;

    }

    static bool GetData0xDF(const RawDataHdr3& hdr, unsigned char* pdat, int with_chk, RawData& dat,char*result)
    {
        unsigned short sum = hdr.angle + hdr.N + hdr.span, chk;

        sum += hdr.fbase;
        sum += hdr.first;
        sum += hdr.last;
        sum += hdr.fend;

        double dan = (hdr.last - hdr.first) / double(hdr.N - 1);

        for (int i = 0; i < hdr.N; i++)
        {
            dat.points[i].confidence = *pdat++;
            sum += dat.points[i].confidence;

            unsigned short v = *pdat++;
            unsigned short v2 = *pdat++;
            unsigned short vv = (v2 << 8) | v;
            sum += vv;
            dat.points[i].distance = vv / 1000.0;
            dat.points[i].angle = (hdr.first + dan * i) * PI / 18000;
        }

        memcpy(&chk, pdat, 2);

        if (with_chk != 0 && chk != sum)
        {
            //printf("chksum df error");
            strcpy(result,"chksum  error");
            return 0;
        }

        memcpy(&dat, &hdr, HDR2_SIZE);
        //memcpy(dat.data, buf+idx+HDR_SIZE, 2*hdr.N);
        //printf("get3 %d(%d)\n", hdr.angle, hdr.N);

        return true;

    }

    static bool GetData0x99(const RawDataHdr99& hdr, unsigned char* pdat, int with_chk, RawData& dat,char*result)
    {
        dat.code = hdr.code;
        dat.N = hdr.N;
        dat.angle = hdr.from * 3600 / hdr.total; // 0.1 degree
        dat.span = hdr.N * 3600 / hdr.total; // 0.1 degree
                                             //dat.fbase = ;
                                             //dat.first;
                                             //dat.last;
                                             //dat.fend;
        DecTimestamp(hdr.timestamp, dat.ts);

        pdat += HDR99_SIZE;

        uint8_t* dist = pdat;
        uint8_t* energy = pdat + 2 * hdr.N;

        for (int i = 0; i < hdr.N; i++)
        {
            uint16_t lo = *dist++;
            uint16_t hi = *dist++;
            dat.points[i].distance = ((hi << 8) | lo) / 1000.0;
            dat.points[i].angle = ((i + hdr.from) * 360.0 / hdr.total) * PI / 180;
            dat.points[i].confidence = energy[i];
        }
        return true;
    }

    int LidarParser::TryParseRawData(int len, unsigned char* buf,
            UartState* uartstate, RawData& dat, int& consume,
            int with_chk, char* result, CmdHeader* cmdheader,
            void** fan_segs) {
        //int pack_format = 0xce;
        int idx = 0;
        int span=360;
        while (idx < len - 12) {
            //LMSG
            if (buf[idx] == 0x4c && buf[idx + 1] == 0x4d &&
                    (unsigned char)buf[idx + 2] == 0x53 &&
                    (unsigned char)buf[idx + 3] == 0x47) {
                LidarMsgHdr* hdr = (LidarMsgHdr*)(buf + idx);
                memcpy(result, hdr, sizeof(LidarMsgHdr));
                consume = idx + sizeof(LidarMsgHdr);
                return 2;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xac &&
                    (unsigned char)buf[idx + 3] == 0xb8) {
                //EPRM
                memcpy(cmdheader, buf, sizeof(CmdHeader));
                EEpromV101*eepromv101 = (EEpromV101*)(buf + 8+idx);
                memcpy(result, eepromv101, sizeof(EEpromV101));
                consume = idx + sizeof(EEpromV101);
                return 3;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xbe &&
                    (unsigned char)buf[idx + 3] == 0xb4) {
                //时间同步返回的应答
                consume = idx + 2;
                return 4;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xbc &&
                    (unsigned char)buf[idx + 3] == 0xff) {
                //C_PACK
                consume = idx+2;
                memcpy(cmdheader, buf, sizeof(CmdHeader));
                return 5;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xac &&
                    (unsigned char)buf[idx + 3] == 0xff) {
                //S_PACK
                consume = idx + 2;
                memcpy(cmdheader, buf, sizeof(CmdHeader));
                memcpy(result,buf+idx+8,2);
                return 6;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xb8 &&
                    (unsigned char)buf[idx + 3] == 0xa5) {
                //readzone
                consume = idx+4;
                memcpy(cmdheader, buf, sizeof(CmdHeader));
                return 7;
            } else if (buf[idx] == 0x4c && buf[idx + 1] == 0x48 &&
                    (unsigned char)buf[idx + 2] == 0xac &&
                    (unsigned char)buf[idx + 3] == 0xa5) {
                //writezone
                consume = idx + 4;
                memcpy(cmdheader, buf, sizeof(CmdHeader));
                return 8;
            }

            if (buf[idx] == 'S' && buf[idx + 1] == 'T' &&
                    buf[idx + 6] == 'E' && buf[idx + 7] == 'D') {
                unsigned char flag = buf[idx + 2];
                uartstate->unit_mm=flag & 1;
                uartstate->with_conf = flag & 2;
                uartstate->with_smooth = flag & 4;
                uartstate->with_fitter = flag & 8;
                uartstate->span_9 = flag & 0x10;
                uartstate->span_18 = flag & 0x20;
                uartstate->span_other = flag & 0x40;
                uartstate->resampele = flag & 0x80;
                if(flag & 0x10)
                    span = 180;
                if(flag & 0x20)
                    span = 90;
                consume = idx + 8;
                return 9;
            }

            if (buf[idx + 1] == 0xfa &&
                    (buf[idx] == 0xdf || buf[idx] == 0xce ||
                     buf[idx] == 0xcf || buf[idx] == 0xc7 ||
                     buf[idx] == 0x9d || buf[idx] == 0x99 ||
                     buf[idx] == 0xaa)) {
                // found frame head
                //pack_format = buf[idx];
#if 0
                AINFO << "found head, idx: " << idx <<
                    "[ " <<
                    " " << std::hex << static_cast<int>(buf[idx]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 1]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 2]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 3]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 4]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 5]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 6]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 6]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 7]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 8]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 9]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 10]) <<
                    " " << std::hex << static_cast<int>(buf[idx + 11]) <<
                    " ]";
#endif
            } else {
                idx++;
                continue;
            }

            RawDataHdr hdr;
            memcpy(&hdr, buf + idx, HDR_SIZE);
            /*
            if (buf[idx] == 0x99) {
                RawDataHdr99 hdr99;
                memcpy(&hdr99, buf + idx, HDR99_SIZE);
                if (hdr99.total == 0) {
                    printf("bad num hdr99 \n");
                    idx += 2;
                    continue;
                }
                int hdr99_span = hdr99.N * 3600 / hdr99.total;
                if (hdr99_span % 90 != 0) {
                    printf("bad angle %d \n", hdr99_span);
                    idx += 2;
                    continue;
                }
            } else if (buf[idx] != 0xc7 && buf[idx] != 0xaa) {
                if (hdr.angle % 90 != 0) {
                    printf("bad angle %d \n", hdr.angle);
                    idx += 2;
                    continue;
                }
            }
            */

            if (hdr.N > MAX_POINTS) {
                printf("points number %d seem not correct\n", hdr.N);
                idx += 2;
                continue;
            }

            bool got;
            if (buf[idx] == 0xce && idx + HDR_SIZE + hdr.N * 3 + 2 <= len) {
                got = GetData0xCE(hdr, buf + idx + HDR_SIZE,
                        hdr.angle == 3420 ? span * 2 : span,
                        with_chk, dat,result);
                consume = idx + HDR_SIZE + 3 * hdr.N + 2;
            } else if (buf[idx] == 0x9d && idx + hdr.N * 2 + 10 <= len) {
                RawDataHdr2 hdr2;
                memcpy(&hdr2, buf + idx, HDR2_SIZE);
                got = GetData0x9D(hdr2, buf + idx + HDR2_SIZE, with_chk, dat,result);
                consume = idx + HDR2_SIZE + 2 * hdr.N + 2;
            } else if (buf[idx] == 0xdf && idx + hdr.N * 3 + 18 <= len) {
                RawDataHdr3 hdr3;
                memcpy(&hdr3, buf + idx, HDR3_SIZE);
                got = GetData0xDF(hdr3, buf + idx + HDR3_SIZE, with_chk, dat,result);
                consume = idx + HDR3_SIZE + 3 * hdr.N + 2;
            } else if (buf[idx] == 0xcf && idx + HDR2_SIZE + hdr.N * 3 + 2 <= len) {
                RawDataHdr2 hdr2;
                memcpy(&hdr2, buf + idx, HDR2_SIZE);
                got = GetData0xCF(hdr2, buf + idx + HDR2_SIZE, with_chk, dat,result);
                consume = /*idx + */HDR2_SIZE + 3 * hdr.N + 2;
#if 0
                AINFO << "get? " << (got ? "yes" : "no") <<
                    ", count: " << hdr2.N <<
                    ", angle: " << hdr2.angle <<
                    ", span: " << hdr2.span <<
                    ", idx: " << idx <<
                    ", cbuf len: " << len <<
                    ", comsume: " << consume;
#endif
            } else if (buf[idx] == 0xc7 && idx + HDR7_SIZE + hdr.N * 5 + 2 <= len) {
                RawDataHdr7 hdr7;
                memcpy(&hdr7, buf + idx, HDR7_SIZE);

                got = GetData0xC7(hdr7, buf + idx, with_chk, dat, result,(FanSegment_C7**)fan_segs);

                consume = idx + HDR7_SIZE + 5 * hdr.N + 2;
            } else if (buf[idx] == 0x99 && idx + HDR99_SIZE + hdr.N * 3 + 2 <= len) {
                RawDataHdr99 hdr99;
                memcpy(&hdr99, buf + idx, HDR99_SIZE);

                got = GetData0x99(hdr99, buf + idx, with_chk, dat,result);

                consume = idx + HDR99_SIZE + 3 * hdr.N + 2;
            } else if (buf[idx] == 0xaa && idx + HDRAA_SIZE + hdr.N * 3 + 2 <= len) {
                RawDataHdrAA hdraa;
                memcpy(&hdraa, buf + idx, HDRAA_SIZE);

                got = GetData0xAA(hdraa, buf + idx, with_chk, dat, result, (FanSegment_AA**)fan_segs);

                consume = idx + HDRAA_SIZE + 3 * hdr.N + 2;
            } else {
                // data packet not complete
                break;
            }
            return got;
        }

        if (idx > 1024) consume = idx / 2;

        return -1;
    }

    int LidarParser::ParseRawBuffer(const uint8_t* buf,
            const size_t len) {
        int cbuf_len = cbuf_raw_->CbufSize();

        if (cbuf_len <= 0)
            //no data in cbuf
            return -1;

        int cbuf_idx = 0;
        std::vector<uint8_t> raw_data;
        raw_data.resize(cbuf_len);

        if (!cbuf_raw_->ReadSafely(raw_data)) {
            AFATAL << "read lidar circular buffer failed";
            return -1;
        }
#if 0
        std::ostringstream os_data_raw;
        os_data_raw.str("[ ");
        for (int i = 0; i < cbuf_len; i++) {
            os_data_raw << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(raw_data[i]) << " ";
        }
        os_data_raw << " ]";

        AINFO << "LIDAR data flow len: " <<
            std::hex << std::setw(3) << std::setfill('0') << cbuf_len <<
            ", from cbuf: " << os_data_raw.str();
#endif
        int local_in = 0;
        int consume = 0;
        do {
            RawData dat;
            UartState uartstate;
            char result[512] = {0};
            consume = 0;
            local_in++;

            int r = TryParseRawData(cbuf_len, &raw_data[cbuf_idx], &uartstate,
                    dat, consume, true, result, &cmdheader_, (void**)fan_segs_);
#if 0
            AINFO << "parse cbuf raw data len " << cbuf_len <<
                ", result: " << r <<
                ", consume: " << consume <<
                ", local_in: " << local_in <<
                ", cbuf idx: " << cbuf_idx;
#endif
            switch (r) {
                case 1: {
                    if (start_angle_ == -1) {
                        // 获取当前雷达的起始统计角度
                        if (state_ < 0) {
                            state_ = autoGetFirstAngle(dat, /*0,*/ whole_datas_, error_);
                        }

                        if (state_ >= 0) {
                            start_angle_ = state_;
                            action_ = ONLINE;
                            AINFO << "Lidar start work, first span angle is " << start_angle_;
                        }
                        break;
                    }

                    int ret = whole_data_process(dat, start_angle_, whole_datas_, error_);

                    if (action_ >= RUN && ret == -1) {
                        AINFO << "span error code: " << ret <<
                            ", error info: " << error_;
                        error_ = "";
                    }

                    if (ret == 1) {
                        int sum = 0;
                        for (std::size_t i = 0; i < whole_datas_.size(); i++)
                        {
                            for (int j = 0; j < whole_datas_.at(i).N; j++)
                            {
                                userdata_.data.framedata.data[sum] = whole_datas_.at(i).points[j];
                                sum++;
                            }
                        }
                        userdata_.data.framedata.N = sum;
                        // 执行回调函数
                        if (sum > 0)
                        {
                            /*
                               if (cfg->runscript.shadows_filter.enable)
                               {
                               AlgorithmAPI_E100::ShadowsFilter(&userdata_, cfg->runscript.shadows_filter);
                               }
                               if (cfg->runscript.median_filter.enable)
                               {
                               AlgorithmAPI_E100::MedianFilter(&userdata_, cfg->runscript.median_filter);
                               }
                               */
                            if (checkPointsLengthZero(&userdata_, 0.9))
                                error_num_++;
                            else
                                error_num_ = 0;
                            if (3 <= error_num_)
                            {
                                printf("%s %d There are many points with a distance of 0 in the current lidar operation", "/dev/ttyS3", 921600);
                                error_num_ = 0;
                            }
                        }
                        userdata_.idx++;
                        {
                            struct timeval tv;
                            gettimeofday(&tv, NULL);
                            userdata_.data.framedata.ts[0] = tv.tv_sec;
                            userdata_.data.framedata.ts[1] = tv.tv_usec;
                        }
                        whole_datas_.clear();
                        action_ = RUN;
                    } else
                        break;

                    ParseSigleFrame({}, 0);

                    //          
                    //避免累加越界
                    if (userdata_.idx >= MAX_FRAMEIDX)
                        userdata_.idx = 0;
                    break;
                }

                ///////////////////////////////////////
                case 2: {
                    AINFO << "alarm info";
                    // 报警信息
                    memcpy(&zonemsg_, &result, sizeof(LidarMsgHdr));
                    //((void (*)(int, void*))cfg->callback)(2, &result);
                    action_ = FINISH;
                    break;
                }
                case 3: {
                    AINFO << "global param";
                    // 全局参数
                    memcpy(&eepromv101_, &result, sizeof(EEpromV101));
                    //((void (*)(int, void*))cfg->callback)(3, &result);
                    action_ = FINISH;
                    break;
                }
                case 4: {
                    AINFO << "time sync ack";
                    // 时间同步返回的应答
                    break;
                }
                case 5:
                    {
                        AINFO << "C_PACK";
                        // C_PACK
                        action_ = FINISH;
                        break;
                    }
                case 6:
                    {
                        AINFO << "S_PACK";
                        // S_PACK
                        action_ = FINISH;
                        break;
                    }
                case 7:
                    {
                        AINFO << "readzone";
                        // readzone
                        break;
                    }
                case 8:
                    {
                        AINFO << "writezone";
                        // writezone
                        break;
                    }
                case 9:
                    {
                        // 串口每圈头发送的状态信息
                        // uartstate.with_fitter,uartstate.with_smooth
                        break;
                    }
            }

            if (consume > 0) {
                // data is not whole fan,drop it
                if (!r)
                {
                    printf("drop %d bytes: %02x %02x %02x %02x %02x %02x",
                            consume,
                            buf[0], buf[1], buf[2],
                            buf[3], buf[4], buf[5]);
                }

                cbuf_idx += consume;
                cbuf_len -= consume;
                cbuf_raw_->RestoreCbuf(consume);
            }

            // 存在需要操作的指令
            switch (action_) {
                case CONTROL:
                case GETALLPARAMS:
                case SETPARAM:
                case READZONE:
                case WRITEZONE:
                case ONLINE:
                    //AERROR << "action: " << action_;
                    break;
                default:
                    break;
            }
        } while (cbuf_idx <= cbuf_len && consume > 0);

        return 0;
    }

    int LidarParser::ParseSigleFrame(const
            std::vector<uint8_t>& data,
            const size_t len) {
#if 0
        AWARN << "frame idx: " << userdata_.idx <<
            ", laser point number: " << userdata_.data.framedata.N <<
            ", time s: " << userdata_.data.framedata.ts[0] <<
            ", time us: " <<userdata_.data.framedata.ts[1];
#if 0
        for (int i = 0; i <userdata_.data.framedata.N; i++) {
            AINFO << std::setw(4) << std::setfill('0') << i <<
                " -> angle: " << std::setw(7) << std::setfill('0') << userdata_.data.framedata.data[i].angle <<
                ", distance: " << std::setw(5) << std::setfill('0') << userdata_.data.framedata.data[i].distance <<
                ", confidence: " << static_cast<int>(userdata_.data.framedata.data[i].confidence);
        }
#endif
#endif
        if (!frame_processor_) {
            AWARN << "no frame processor!";
            return -1;
        }

        ventura::common_msgs::sensor_msgs::PointCloud pcs;

        pcs.mutable_header()->set_seq(userdata_.idx);
        pcs.mutable_header()->mutable_stamp()->set_sec(userdata_.data.framedata.ts[0]);
        pcs.mutable_header()->mutable_stamp()->set_nsec(userdata_.data.framedata.ts[1]);
        pcs.mutable_header()->set_frame_id("laser_link");

        auto intensity = pcs.add_channels();
        intensity->set_name("intensity");

        for (int i = 0; i <userdata_.data.framedata.N; i++) {
            auto p = pcs.add_points();
            p->set_x(sin(userdata_.data.framedata.data[i].angle) * userdata_.data.framedata.data[i].distance);
            p->set_y(cos(userdata_.data.framedata.data[i].angle) * userdata_.data.framedata.data[i].distance);
            p->set_z(0);

            intensity->add_values(userdata_.data.framedata.data[i].confidence);
        }

        double this_time = userdata_.data.framedata.ts[0] + userdata_.data.framedata.ts[1] / 1e9;
        auto time_increment = pcs.add_channels();
        intensity->set_name("time_increment");
        time_increment->add_values((this_time - point_cloud_last_) / 1200.f);
        point_cloud_last_ = this_time;

        return frame_processor_(&pcs, "ventura::common_msgs::sensor_msgs::PointCloud");
    }

} //namespace parser
} //namespace chss
} //namespace mstf
