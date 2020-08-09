#ifndef _WPCAP_H_
#define _WPCAP_H_

void *pcap_init(const char *fname);
void pcap_close(void *handle);
void pcap_write(void *handle, const uint8_t *buf, int buflen);

#endif // _WPCAP_H_
