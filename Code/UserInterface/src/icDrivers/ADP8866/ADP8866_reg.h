#ifndef ADP8866_REG_H_
#define ADP8866_REG_H_

#include <stdint.h>

typedef struct{
	/*
	 * Interrupt configuration.
		1 = processor interrupt deasserts for 50 us and reasserts with pending events.
		0 = processor interrupt remains asserted if the host tries to clear the interrupt while there is a pending event.
	 */
	uint8_t INT_CFG;
	/*
	  	1 = device is in normal mode.
		0 = device is in standby, only I2C is enabled.
	 */
	uint8_t NSTBY;
	/*
	 	1 = charge pump gain is automatically set to 1× every time that the BLMX (Register 0x13) is written to.
		0 = writing to BLMX (Register 13) has no unique effect on the charge pump gain.
	 */
	uint8_t ALT_GSEL;
	/*
	 	1 = the charge pump does not switch down in gain until all LEDs are off. The charge pump switches up in gain
			as needed. This feature is useful if the ADP8866 charge pump is used to drive an external load.
		0 = the charge pump automatically switches up and down in gain. This provides optimal efficiency but is not
			suitable for driving external loads (other than those connected to the ADP8866 diode drivers).
	 */
	uint8_t GDWN_DIS;
	/*
	 * Master enable for independent sinks.
		1 = enables all LED current sinks designated as independent sinks. This bit has no effect if any of the SCx_EN
			bits that are part of the independent sinks group in Register 0x1A and Register 0x1B are set.
		0 = disables all sinks designated as independent sinks. This bit has no effect if any of the SCx_EN bits that are
			part of the independent sinks group in Register 0x1A and Register 0x1B are set.
	 */
	uint8_t SIS_EN;
	/*
	 * Master enable for backlight sinks.
		1 = enables all LED current sinks designated as backlight.
		0 = disables all sinks designated as backlight
	 */
	uint8_t BL_EN;
} ADP8866_ModeControl_t;
void ADP8866_SetModeControl(ADP8866_ModeControl_t mode);

#endif /* ADP8866_REG_H_ */
