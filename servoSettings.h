#define SERVO_OPEN	0	 // 0: 30    1: 0    2:  0
#define SERVO_CLOSED 90 // 0: 110   1: 80   2:  90
#define SERVO_TIMEOUT_MS 200

// Vent commands
enum Vent_State : uint8_t
{
	open,
	close
};
