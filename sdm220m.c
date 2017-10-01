/*
* Eastron SDM220-Modbus
* aka RS871-8291
* aka Smartrail X100-MID (OEM Manufactuer)
* Single-Phase Two Module DIN rail Meter
*
* Read and print input registers on the SDM220M
*/

#include <stdlib.h> // exit
#include <stdio.h> // console output
#include <getopt.h> // getopt
#include <modbus.h> // Part of libmodbus
#include "sdm220m.h"
#include "swap_float.h"

// RTU Serial settings
#define default_port "/dev/ttyUSB0"
char* port = default_port;
#define default_baud 9600
int baud = default_baud;
#define default_parity 'N'
char parity = default_parity;
#define default_data_bits 8
int data_bits = default_data_bits;
#define default_stop_bits 1
int stop_bits = default_stop_bits;

// Default RTU Slave
#define default_slave 1
int slave = default_slave;

/* Flag set by ‘--csv’. */
static int csv_flag;

/* Flag set by ‘--csv-no-head’. */
static int csv_no_head_flag;

int process_modbus() {
	modbus_t *mb;
	int errnum;
	int ret;

	// Setup modbus
	mb = modbus_new_rtu(port, baud, parity, data_bits, stop_bits);
	if (mb == NULL) {
		fprintf(stderr, "Unable to create libmodbus context: %s\n", modbus_strerror(errnum));
		return EXIT_FAILURE;
	}
	if (modbus_set_slave(mb, slave) == -1) {
		fprintf(stderr, "Set slave failed: %s\n", modbus_strerror(errnum));
                modbus_free(mb);
                return EXIT_FAILURE;
	}

	// Connect
	if (modbus_connect(mb) == -1) {
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errnum));
		modbus_free(mb);
		return EXIT_FAILURE;
	}

	// Read required input registers
	uint16_t input_reg_address_max = input_reg_address[input_reg_count - 1] + 1;
	uint16_t input_reg_results[input_reg_address_max];
	for (int n = 0; n < input_reg_count; n++) {
		ret = modbus_read_input_registers(
			mb,
			input_reg_address[n],
			2, // Parameters are two conscutive 16bits registers
			&input_reg_results[input_reg_address[n]]);
		if (ret == -1) {
			fprintf(stderr, "Read failed register 0x%04x (%s)\n",
				input_reg_address[n],
				input_reg_address_description[n]);
			input_reg_results[input_reg_address[n]] = 0;
			input_reg_results[input_reg_address[n] + 1] = 0;
		}
	}

	// Print results
	float f;
	if (csv_flag & !csv_no_head_flag) {
		// CSV Header
		for (int n = 0; n < input_reg_count; n++) {
			printf("%s (%s)",
				input_reg_address_description[n],
				input_reg_address_unit[n]);
			if (n < input_reg_count - 1)
				putchar(',');
			else
				putchar('\n');
		}
	}
	for (int n = 0; n < input_reg_count; n++) {
		f = modbus_get_float(&input_reg_results[input_reg_address[n]]);
		if (csv_flag) {
			printf("%.3f",swap_float(f));
			if (n < input_reg_count - 1)
				putchar(',');
			else
				putchar('\n');
		} else {
			printf("%s: %.3f %s\n",
				input_reg_address_description[n],
				swap_float(f),
				input_reg_address_unit[n]);
		}
	}

	// Clean up
	modbus_close(mb);
	modbus_free(mb);

	return EXIT_SUCCESS;
}


int main (int argc, char **argv)
{
	// Process arguments
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"csv",		no_argument,	&csv_flag, 1},
			{"table",	no_argument,	&csv_flag, 0},
			{"csv-no-head", no_argument,	&csv_no_head_flag, 1},
			/* These options don’t set a flag.
			We distinguish them by their indices. */
			{"port",	required_argument,	0,	'p'},
			{"address",	required_argument,	0,	'a'},
			{"baud",	required_argument,	0,	'b'},
			{"parity",	required_argument,	0,	'c'},
			{"bits",	required_argument,	0,	'd'},
			{"stop",	required_argument,	0,	's'},
			{"help",	no_argument,		0,	'h'},
			{NULL, 		no_argument, 		NULL,	0 }
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "p:a:b:c:d:s:h?",
			       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;

		switch (c)
		{
			case 'p':
				port = optarg;
				break;

			case 'a':
				slave = atoi(optarg);
				break;

			case 'b':
				baud = atoi(optarg);
				break;

			case 'c':
				parity = optarg[0];
				break;

			case 'd':
				data_bits = atoi(optarg);
				break;

			case 's':
				stop_bits = atoi(optarg);
				break;


			case 'h':
			case '?':
				fprintf(stderr, "Usage: %s {flags}\n", argv[0]);
				fprintf(stderr, "--port {port} -p{port} -- Set serial port (default %s)\n", default_port);
				fprintf(stderr, "--address {rtu_address} -a{rtu_address} -- Set the RTU Slave number (default %d)\n", default_slave);
				fprintf(stderr, "--baud {baud} -b{baud} -- Set baud rate (default %d)\n", default_baud);
				fprintf(stderr, "--parity {parity} -c{parity} -- Set parity (N,E,O) (default %c)\n", default_parity);
				fprintf(stderr, "--data {databits} -d{databits} -- Set data bits (default %d)\n", default_data_bits);
				fprintf(stderr, "--stop {stopbits} -s{stopbits} -- Set stop bits (default %d)\n", default_stop_bits);
				fprintf(stderr, "--csv -- Output CSV format\n");
				fprintf(stderr, "--csv-no-head -- Output CSV format, no header\n");
				fprintf(stderr, "--table -- Output table format (default)\n");
				exit(EXIT_FAILURE);
				break;
			default :
				break;
		}
	}

	// If csv-no-head set, assume csv
	if (csv_no_head_flag)
		csv_flag = csv_no_head_flag;

	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		fprintf(stderr, "non-option ARGV-elements: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
	}

	// Now do the modbus stuff
	int ret = process_modbus();

	exit(ret);
}
