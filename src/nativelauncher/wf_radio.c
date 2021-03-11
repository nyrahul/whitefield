#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki-ng-helper/contiki-ng-radio.h"
#include "wpcap.h"
#include "helpers.h"

extern struct radio_driver nullradio_driver;

static int wf_init(void)
{
    printf("Initing Whitefield Radio\n");
    return 0;
}

static int wf_transmit(unsigned short transmit_len)
{
    printf("TrANSMIT+LEN=%d\n", transmit_len);
    return RADIO_TX_OK;
}

static int wf_prepare(const void *data, unsigned short len)
{
    extern void *g_pcap_handle;
    printf("Prepare len=%d\n", len);
    pcap_write(g_pcap_handle, data, len);
    send_pkt2airline(data, len);
    return 1;
}

/* Override nullradio. Called from preload main. */
void wfradio_init(void)
{
    nullradio_driver.init     = wf_init;
    nullradio_driver.prepare  = wf_prepare;
    nullradio_driver.transmit = wf_transmit;
}
/*---------------------------------------------------------------------------*/
