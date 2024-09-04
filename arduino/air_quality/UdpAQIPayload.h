/**
 * Library for defining the payload datastructures.
 */
#ifndef THEAPIUDPAQIPAYLOAD_h
#define THEAPIUDPAQIPAYLOAD_h

#include <stdint.h>

namespace theapi {
  class UdpAQIPayload {
    public:
      typedef struct {
          uint8_t MessageType;
          uint8_t MessageId;
          uint32_t DeviceId;
          int16_t Aqi;
      } payload_t;

      const static uint8_t SIZE = 8;

      UdpAQIPayload();

      // How big the payload is.
      uint8_t size();

      /**
       * The type of message.
       * Each Theapi payload has a unique message type,
       * so the receiver can read the first byte to know which type is arriving.
       */
      uint8_t getMsgType();
      void setMsgType();

      // The id of the device.
      uint32_t getDeviceId();
      void setDeviceId(uint32_t device_id);

      // The id, not neccessarily unique, of the message.
      uint8_t getMsgId();
      void setMsgId(uint8_t msg_id);

      // The AQI value.
      uint16_t getAqi();
      void setAqi(uint16_t Aqi);

      // Creates a byte array for sending via the radio
      void serialize(uint8_t payload[UdpAQIPayload::SIZE]);

      // Parse the read byte data from the radio
      void unserialize(uint8_t payload[UdpAQIPayload::SIZE]);

    private:
      payload_t _payload;

  };
}

#endif

