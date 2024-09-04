#include "UdpAQIPayload.h"

namespace theapi {

  UdpAQIPayload::UdpAQIPayload() {
    setMsgType();
    _payload.DeviceId = 0;
    _payload.MessageId = 0;
    _payload.Aqi = 0;
  }
  
  uint8_t UdpAQIPayload::size() {
    return SIZE;
  }
  
  void UdpAQIPayload::setMsgType() {
    _payload.MessageType = 10;
  }
  
  uint8_t UdpAQIPayload::getMsgType() {
    return _payload.MessageType;
  }
  
  uint32_t UdpAQIPayload::getDeviceId() {
    return _payload.DeviceId;
  }
  
  void UdpAQIPayload::setDeviceId(uint32_t val) {
    _payload.DeviceId = val;
  }
  
  // The id, not neccessarily unique, of the message.
  uint8_t UdpAQIPayload::getMsgId() {
    return _payload.MessageId;
  }
  
  void UdpAQIPayload::setMsgId(uint8_t id) {
    _payload.MessageId = id;
  }
  
  uint16_t UdpAQIPayload::getAqi() {
    return _payload.Aqi;
  }
  
  void UdpAQIPayload::setAqi(uint16_t val) {
    _payload.Aqi = val;
  }
  
  // Populates the given array with the payload data
  void UdpAQIPayload::serialize(uint8_t buffer[UdpAQIPayload::SIZE]) {
    buffer[0] = _payload.MessageType;
    buffer[1] = _payload.MessageId;
    buffer[2] = (_payload.DeviceId >> 24);
    buffer[3] = (_payload.DeviceId >> 16);
    buffer[4] = (_payload.DeviceId >> 8);
    buffer[5] = _payload.DeviceId; 
    buffer[6] = (_payload.Aqi >> 8);
    buffer[7] = _payload.Aqi;
  }
  
  // Parse the read byte data
  void UdpAQIPayload::unserialize(uint8_t buffer[UdpAQIPayload::SIZE]) {
    _payload.MessageType = buffer[0];
    _payload.MessageId = buffer[1];
    _payload.DeviceId = (buffer[2] << 24) | (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];
    _payload.Aqi = (buffer[6] << 8) | buffer[7];
  }

}

