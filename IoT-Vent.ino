#include <RFM69.h>
#include <SPI.h>
#include <ArduinoLowPower.h>
#include <Servo.h>

#include "pinDefs.h"
#include "serialSettings.h"
#include "radioSettings.h"
#include "servoSettings.h"

char buff[61]; //61 max payload for radio

// Init Radio Object
RFM69 radio;
Payload payload;
bool newPayload = false;

// Servo
Servo servo;
Vent_State ventState;

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
	pinMode(SERVO_LATCH, OUTPUT);
	servo.attach(SERVO);

	ventAction(open);

	// TODO: handle low power
	// LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, SERVO_ISR, CHANGE);

	payload.zoneID = ZONE_ID;
}

void loop()
{
	if (radio.receiveDone())
	{
		payload = getLoad();

		if (payload.type == vent_state)
		{
			ventAction((Vent_State)payload.data);
		}
	}

	// LowPower.sleep((int)SERVO_TIMEOUT_MS);
}

Payload getLoad()
{
	Payload load;

	if (radio.DATALEN == sizeof(Payload))
		load = *(Payload *)radio.DATA;

	if (radio.ACKRequested())
	{
		radio.sendACK();
		DEBUGln("ACK sent");
	}

	DEBUGln(load.zoneID);
	DEBUGln(load.type);
	DEBUGln(load.data);

	return load;
}

void SERVO_ISR()
{
	digitalWrite(SERVO_LATCH, LOW);
}

void ventAction(Vent_State state)
{
	state == open ? Debugln("open") : DEBUGln("close");
	
	if (ventState == state)
		return;

	digitalWrite(SERVO_LATCH, HIGH);

	if (state == open)
	{
		servo.write(SERVO_OPEN);
		ventState = open;
	}
	else
	{
		servo.write(SERVO_CLOSED);
		ventState = close;
	}

	delay(SERVO_TIMEOUT_MS);
	digitalWrite(SERVO_LATCH, LOW);
}

// todo make an askForState function