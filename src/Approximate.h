/*
    Approximate.h
    Approximate Library
    -
    David Chatting - github.com/davidchatting/Approximate
    MIT License - Copyright (c) October 2020
*/

#ifndef Approximate_h
#define Approximate_h

#include "Arduino.h"
#include "Approximate/eth_addr.h"
#include "Approximate/wifi_pkt.h"

#include "Approximate/PacketSniffer.h"
#include "Approximate/Packet.h"
#include "Approximate/ArpTable.h"
#include "Approximate/Device.h"
#include "Approximate/Filter.h"

#include "ListLib.h"              //https://github.com/luisllamasbinaburo/Arduino-List

#define APPROXIMATE_INTIMATE_RSSI -15
#define APPROXIMATE_PERSONAL_RSSI -30
#define APPROXIMATE_SOCIAL_RSSI -60
#define APPROXIMATE_PUBLIC_RSSI -120  //TODO: interaction with min RSSI in PacketSniffer

class Approximate {
  public:
    typedef enum {
      PKT_MGMT,
      PKT_CTRL,
      PKT_DATA,
      PKT_MISC,
    } PacketType;

    typedef enum {
      UPLOAD,
      DOWNLOAD,
      ARRIVE,
      DEPART,
      INACTIVE
    } DeviceEvent;

    typedef void (*DeviceHandler)(Device *device, DeviceEvent event);

    static String toString(DeviceEvent e) {
      switch (e) {
        case Approximate::UPLOAD:     return("UPLOAD");
        case Approximate::DOWNLOAD:   return("DOWNLOAD");
        case Approximate::ARRIVE:     return("ARRIVE");
        case Approximate::DEPART:     return("DEPART");
        default:                      return("INACTIVE");
      }
    }

  private:
    static bool running;

    static PacketSniffer *packetSniffer;
    static ArpTable *arpTable;
    static bool proximateIPAddressRequired;

    String ssid;
    String password;

    wl_status_t currentWifiStatus = WL_IDLE_STATUS;
    void onWifiStatusChange(wl_status_t oldStatus, wl_status_t newStatus);

    //TODO: template this?
    typedef void (*voidFnPtr)();
    typedef void (*voidFnPtrWithStringPayload)(String);
    typedef void (*voidFnPtrWithBoolPayload)(bool);
    typedef void (*voidFnPtrWithFnPtrPayload)(voidFnPtr);    
    voidFnPtr onceWifiStatusFnPtr = NULL;
    voidFnPtrWithStringPayload onceWifiStatusWithStringPayloadFnPtr = NULL;
    voidFnPtrWithBoolPayload onceWifiStatusWithBoolPayloadFnPtr = NULL;
    voidFnPtrWithFnPtrPayload onceWifiStatusWithFnPtrPayloadFnPtr = NULL;
    String onceWifiStatusStringPayload;
    bool onceWifiStatusBoolPayload;
    voidFnPtr onceWifiStatusFnPtrPayload;
    wl_status_t triggerWifiStatus = WL_IDLE_STATUS;

    static void packetEventHandler(wifi_promiscuous_pkt_t *pkt, uint16_t len, int type);
    static void parseMgmtPacket(wifi_promiscuous_pkt_t *pkt);
    static void parseCtrlPacket(wifi_promiscuous_pkt_t *pkt);
    static void parseDataPacket(wifi_promiscuous_pkt_t *pkt, uint16_t payloadLength);
    static void parseMiscPacket(wifi_promiscuous_pkt_t *pkt);

    static DeviceHandler activeDeviceHandler;
    static DeviceHandler proximateDeviceHandler;

    void updateProximateDeviceList();

    static eth_addr localBSSID;
    static List<Filter *> activeDeviceFilterList;
    static bool applyDeviceFilters(Device *device);

    static List<Device *> proximateDeviceList;
    static Device *getProximateDevice(eth_addr &macAddress);
    static void onProximateDevice(Device *proximateDevice);
    static int proximateRSSIThreshold;
    static int proximateLastSeenTimeoutMs;

    void printWiFiStatus();

  public:
    Approximate();
    bool init(String ssid, String password = "", bool ipAddressResolution = false);

    void start(voidFnPtr thenFnPtr = NULL);
    void stop();

    void loop();

    //add one more filter
    void addActiveDeviceFilter(String macAddress, Filter::Direction direction = Filter::EITHER);
    void addActiveDeviceFilter(Device &device, Filter::Direction direction = Filter::EITHER);
    void addActiveDeviceFilter(Device *device, Filter::Direction direction = Filter::EITHER);
    void addActiveDeviceFilter(eth_addr &macAddress, Filter::Direction direction = Filter::EITHER);
    void addActiveDeviceFilter(int oui, Filter::Direction direction = Filter::EITHER);

    //set exactly one filter
    void setActiveDeviceFilter(String macAddress, Filter::Direction direction = Filter::EITHER);
    void setActiveDeviceFilter(Device &device, Filter::Direction direction = Filter::EITHER);
    void setActiveDeviceFilter(Device *device, Filter::Direction direction = Filter::EITHER);
    void setActiveDeviceFilter(eth_addr &macAddress, Filter::Direction direction = Filter::EITHER);
    void setActiveDeviceFilter(int oui, Filter::Direction direction = Filter::EITHER);

    void removeActiveDeviceFilter(String macAddress);
    void removeActiveDeviceFilter(Device &device);
    void removeActiveDeviceFilter(Device *device);
    void removeActiveDeviceFilter(eth_addr &macAddress);
    void removeActiveDeviceFilter(int oui);
    void removeAllActiveDeviceFilters();

    void setLocalBSSID(String macAddress);
    void setLocalBSSID(eth_addr &macAddress);

    bool isProximateDevice(String macAddress);
    bool isProximateDevice(eth_addr &macAddress);

    void setActiveDeviceHandler(DeviceHandler activeDeviceHandler, bool inclusive = true);
    void setProximateDeviceHandler(DeviceHandler deviceHandler, int rssiThreshold = APPROXIMATE_PERSONAL_RSSI, int lastSeenTimeoutMs = 60000, bool requireIPAddress = false);

    static void setProximateRSSIThreshold(int proximateRSSIThreshold);
    static void setProximateLastSeenTimeoutMs(int proximateLastSeenTimeoutMs);

    void connectWiFi(String ssid, String password);
    void connectWiFi();
    void disconnectWiFi();

    void onceWifiStatus(wl_status_t status, voidFnPtr callBackFnPtr);
    void onceWifiStatus(wl_status_t status, voidFnPtrWithStringPayload callBackFnPtr, String payload);
    void onceWifiStatus(wl_status_t status, voidFnPtrWithBoolPayload callBackFnPtr, bool payload);
    void onceWifiStatus(wl_status_t status, voidFnPtrWithFnPtrPayload callBackFnPtr, voidFnPtr payload);

    static bool MacAddr_to_eth_addr(MacAddr *in, eth_addr &out);
    static bool uint8_t_to_eth_addr(uint8_t *in, eth_addr &out);
    static bool oui_to_eth_addr(int oui, eth_addr &out);
    static bool String_to_eth_addr(String &in, eth_addr &out);
    static bool eth_addr_to_String(eth_addr &in, String &out);
    static bool wifi_pkt_to_Packet(wifi_promiscuous_pkt_t *in, uint16_t payloadLengthBytes, Packet *out);
    static bool Packet_to_Device(Packet *packet, eth_addr &bssid, Device *device);
};

#endif