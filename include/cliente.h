#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <unistd.h>

#include <errno.h>

#define MEU_PROTOCOLO 0x88b5
#define INTERFACE "eno1"


int criasocket(char *interface);
