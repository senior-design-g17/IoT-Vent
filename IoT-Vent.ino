#include <RFM69.h>
#include <SPI.h>
#include <ArduinoLowPower.h>
#include <Servo.h>

#include "pinDefs.h"
#include "serialSettings.h"
#include "radioSettings.h"

char buff[61]; //61 max payload for radio

// Init Radio Object
RFM69 radio;
Payload payload;
bool newPayload = false;

// Servo
Servo servo;

void setup()
{
#ifdef SERIAL_EN
	Serial.begin(SERIAL_BAUD);
	delay(2000);
#endif

	// RADIO
	if (!radio.initialize(FREQUENCY, MYID, NETWORKID))
		DEBUGln("radio.init() FAIL");
	else
		DEBUGln("radio.init() SUCCESS");

	radio.setHighPower(true);
	//radio.setCS(RFM69_CS);

	if (ENCRYPT)
		radio.encrypt(ENCRYPTKEY);

	// Pins
	servo.attach(SERVO);
	servo.write(0);

	// LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, TEMP_ISR, CHANGE);

	payload.zoneID = ZONE_ID;
}

void loop()
{
	if (radio.receiveDone())
	{
		if (radio.DATALEN == sizeof(Payload))
			payload = *(Payload *)radio.DATA;

		if (radio.ACKRequested())
		{
			radio.sendACK();
			DEBUGln("ACK sent");
		}

		if(payload.type == vent_state)
		{
			servo.write(payload.data);
		}
	}
}
