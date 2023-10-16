#ifndef _UAPI_CAN_RAW_H
#define _UAPI_CAN_RAW_H

#include "modules/chassis/drivers/can/comm/include/linux/can.h"

#define SOL_CAN_RAW (SOL_CAN_BASE + CAN_RAW)
enum {
    SCM_CAN_RAW_ERRQUEUE = 1,
};

/* for socket options affecting the socket (not the global system) */

enum {
    CAN_RAW_FILTER = 1,	/* set 0 .. n can_filter(s)          */
    CAN_RAW_ERR_FILTER,	/* set filter for error frames       */
    CAN_RAW_LOOPBACK,	/* local loopback (default:on)       */
    CAN_RAW_RECV_OWN_MSGS,	/* receive my own msgs (default:off) */
    CAN_RAW_FD_FRAMES,	/* allow CAN FD frames (default:off) */
    CAN_RAW_JOIN_FILTERS,	/* all filters must match to trigger */
};

#endif /* !_UAPI_CAN_RAW_H */
