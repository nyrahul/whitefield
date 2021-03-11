#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "commline/commline.h"

int send_pkt2airline(const void *data, unsigned short len)
{
    DEFINE_MBUF(mbuf);

    mbuf->len = len;
    memcpy(mbuf->buf, data, len);
    mbuf->src_id = 2;//gNodeID;
    mbuf->dst_id = 1;

    if(CL_SUCCESS != cl_sendto_q(MTYPE(AIRLINE, CL_MGR_ID), mbuf, mbuf->len + sizeof(msg_buf_t))) {
        ERROR("cl_sendto_q failed\n");
        return FAILURE;
    }
    return SUCCESS;
}

