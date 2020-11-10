/*
    eth_addr.h
    Approximate Library
    -
    David Chatting - github.com/davidchatting/Approximate
    MIT License - Copyright (c) October 2020
*/

#ifndef eth_addr_h
#define eth_addr_h

#if defined(ESP8266)
    #include "netif/etharp.h"

#elif defined(ESP32)
    #include "lwip/etharp.h"

#endif

const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};

#ifndef ETHADDR16_COPY
  #define ETHADDR16_COPY(src, dst)  SMEMCPY(src, dst, ETHARP_HWADDR_LEN)
#endif

typedef struct {
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

#endif