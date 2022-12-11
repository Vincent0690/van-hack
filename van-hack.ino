#ifdef ARDUINO_ARCH_ESP8266
	#include <ESP8266WiFi.h>
#endif

#include <VanBusRx.h>

#ifdef ARDUINO_ARCH_ESP32
	const int RX_PIN = GPIO_NUM_22; // Set to GPIO pin connected to VAN bus transceiver output
#else
	#if defined ARDUINO_ESP8266_GENERIC || defined ARDUINO_ESP8266_ESP01
		//For ESP-01 board we use GPIO 2 (internal pull-up, keep disconnected or high at boot time)
		#define D2 (2)
	#endif //defined ARDUINO_ESP8266_GENERIC || defined ARDUINO_ESP8266_ESP01

	const int RX_PIN = D2;
#endif

void setup() {
	delay(1000);

	Serial.begin(115200);
	Serial.println("Starting VAN bus receiver");

	#ifdef ARDUINO_ARCH_ESP8266
		WiFi.disconnect(true);

		delay(1);

		WiFi.mode(WIFI_OFF);

		delay(1);

		WiFi.forceSleepBegin();

		delay(1);
	#endif

	VanBusRx.Setup(RX_PIN);
	Serial.printf_P(PSTR("VanBusRx queue of size %d is set up\n"), VanBusRx.QueueSize());
};

void loop() {
	TVanPacketRxDesc pkt;

	if(VanBusRx.Receive(pkt)) {
		bool crcOk = pkt.CheckCrcAndRepair();

		pkt.DumpRaw(Serial);

		#ifdef VAN_RX_ISR_DEBUGGING
			if(!crcOk) pkt.getIsrDebugPacket().Dump(Serial);
		#endif
	};

	static unsigned long lastUpdate = 0;
	
	if(millis() - lastUpdate >= 60000UL) {
		lastUpdate = millis();
		VanBusRx.DumpStats(Serial);
	};
};
