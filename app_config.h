#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>\n

// <h> Hardware Version

//==========================================================
// <o> SERIAL_NUMBER_ADDR

#ifndef SERIAL_NUMBER_ADDR
#define SERIAL_NUMBER_ADDR          0x1FDE0
#define SERIAL_NUMBER_LENGTH_MAX    32
#endif

//==========================================================
// <o> HW_VERSION_STRING

#ifndef HW_VERSION_STRING
#define HW_VERSION_STRING "LORA.REVA.V2.0"
#endif

// </h>

// <h> Firmware Version

//==========================================================
// <o> FW_VERSION_MAJOR

#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR 0
#endif

//==========================================================
// <o> FW_VERSION_MINOR

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR 0
#endif

//==========================================================
// <o> FW_VERSION_BUILD

#ifndef FW_VERSION_BUILD
#define FW_VERSION_BUILD 4
#endif

// </h>

// <<< end of configuration section >>>

#endif /* APP_CONFIG_H */
