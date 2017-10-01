/* 
* Eastron SDM220-Modbus (aka RS871-8291)
* Single-Phase Two Module DIN rail Meter
* 
* Input registers and string descriptions
*/


// Input registers to read
#define input_reg_count 14
const uint16_t input_reg_address[input_reg_count] = {
	0x0000,
	0x0006,
	0x000C,
	0x0012,
	0x0018,
	0x001E,
	0x0024,
	0x0046,
	0x0048,
	0x004A,
	0x004C,
	0x004E,
	0x0156,
	0x0158
};
const char* input_reg_address_description[input_reg_count] = {
	"Line to neutral volts",
	"Current",
	"Active power",
	"Apparent power",
	"Reactive power",
	"Power factor",
	"Phase angle",
	"Frequency",
	"Import active energy",
	"Export active energy",
	"Import reactive energy",
	"Export reactive energy",
	"Total active energy",
	"Total reactive energy"
};

const char* input_reg_address_unit[input_reg_count] = {
	"Volts",
	"Amps",
	"Watts",
	"VoltAmps",
	"VAr",
	"",
	"Degrees",
	"Hz",
	"kwh",
	"kwh",
	"kvarh",
	"kvarh",
	"kwh",
	"kvarh"
};