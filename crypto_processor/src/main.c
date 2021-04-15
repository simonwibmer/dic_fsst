/*
Title: crypto_processor/main.c
Author: Simon Wibmer
Wichtig:
https://docs.zephyrproject.org/latest/reference/peripherals/uart.html

https://docs.zephyrproject.org/2.4.0/reference/crypto/index.html



//\\//\\//\\ Important //\\//\\//\\
Die Funktionen validate_hw_compatibility() und cbc_mode() stammen aus dem crypto example: zephyrproject/zephyr/samples/drivers/crypto/src/main.c und wurden nur etwas angepasst.
Außerdem muss ich hier den Mitschülern Fabio Plunser und Lorenz Mitterhuber für ihre Hilfe bei manchen Fehlern/Problemen und der seelischen sowie mentalen Unterstützung danken.
//\\//\\//\\//\\//\\//\\//\\//\\//\\
*/
//Includes
//###################################################################
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <drivers/uart.h>
#include <crypto/cipher.h>
#include <crypto/cipher_structs.h>
#include <logging/log.h>
#include <pthread.h>
#include <posix/mqueue.h>
#include <posix/posix_types.h>
#include <posix/unistd.h>
#include <posix/time.h>
//####################################################################



//Globale Variablen 
//####################################################################

//For Device Configs
#define UART_NAME "UART_0"
#define CRYPTO_NAME "CRYPTO_TC"
const struct device *my_uart_dev;
const struct device *my_crypto_dev;

//States for State machine 
enum enum_state {INIT, BUSY, AVAIL, DECRYPT, DLEN, KEY, IV, DATA, OP};
enum enum_state State = INIT;
enum enum_op {opDECRYPT,opKEY, opIV};
enum enum_op Operation;

//To Check if processing thread available 
int iAvailability = 1;
//Variable for Message Length 
int iLength;
//Input Data to Decrypt 
unsigned char* my_uartdata;
//Buffer for Data to Decrypt
unsigned char* my_cipher_buffer;
//Decrypted data
unsigned char* my_decdata;

//for MSGQ
K_MSGQ_DEFINE(my_uart_msgq, sizeof(char*),20,4);
K_MSGQ_DEFINE(my_proc_msgq, sizeof(char*),20,4);

//for HW comp check
uint32_t cap_flags;

//For Logs
LOG_MODULE_REGISTER();

//Static Key and IV
unsigned char my_KEY[16] = {
		0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
		0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42
};
unsigned char* my_IV = my_KEY;


//####################################################################
//Put into Message Queues
//###################################################################
void my_uart_msgq_put (unsigned char* message)
{
	if (k_msgq_put(&my_uart_msgq, &message, K_FOREVER) != 0)
	{
		LOG_ERR("Putting into UART MSGQ");
	}
}
void my_proc_msgq_put (unsigned char* message2)
{
	if (k_msgq_put(&my_proc_msgq, &message2, K_FOREVER) != 0)
	{
		LOG_ERR("Putting into Processing MSGQ");
	}
}

//UART Ouptut
//####################################################################
void *uart_out (void *vali)
{
	unsigned char* data;
	LOG_INF("UART Output Thread startet");
	
	for(;;)
	{
	if(!k_msgq_get(&my_uart_msgq, &data, K_NO_WAIT))
		{
		if (data[0] == 'D') // Falls Decrypt Operation muss die Nullterminierung mit beachtet werden, es wird einen Stelle leerer Daten angehängt
		{
			for(int i=0; i<strlen(data)+1; i++)
			{
				uart_poll_out(my_uart_dev, data[i]);

			}
			
		}
		else
		{
			for(int i=0; i<strlen(data) ; i++) // Bei jeder anderen Operation werden einfach alle Daten gesendet
			{
				uart_poll_out(my_uart_dev, data[i]);
			}
		}
		}
	}
	return vali;
	
}

//####################################################################
//UART Input
//####################################################################
void *uart_in (void *vali)
{
	LOG_INF("UART Input Thread startet");

	unsigned char my_uartchar, my_uartchar_buffer;
	int iPos = 0;

for(;;)
{	//State Machine 
	switch (State)
		{
		case INIT:
			if(!uart_poll_in(my_uart_dev, &my_uartchar)) // Einlesen des ersten Zeichens
			{
				LOG_INF("Recieved Char: %c", my_uartchar);
				switch (my_uartchar)		//Abfrage in welchen State gewechselt werden soll
				{
				case '.':					//Besitzt keinen State wird deswegen sofort behandelt 
					my_uart_msgq_put(".\n");
					if (iAvailability == 0) // Falls der Processing Thread nicht Available ist wird zusätzlich BUSY ausgegeben 
					{
						sleep(0.1); //sleep wird benötigt da sonst der 2 Test sehr oft fehlschlägt, wahrscheinlich auf die abweichende Prozessorzeit zurückzuführen (Danke Fabio)
						my_uart_msgq_put("BUSY\n"); 
					}
					break;
				
				case 'P':
					State = AVAIL;
					break;

				case 'W':
					State = BUSY;
					break;
				
				case 'D':
					State = DECRYPT;
					break;

				case 'K':
					State = KEY;
					break;

				case 'I':
					State = IV;
					break;
				default:
					State = INIT;
					break;
				}
			}
			break;
		case BUSY:
			my_proc_msgq_put("W\n");  // Um Trotz der 10 sekunden Wartezeit die BUSY meldung ausgeben zu können wird W in den Processing Thread weitergegeben
			State = INIT; 
			break;
		
		case AVAIL:
			my_proc_msgq_put("P\n"); //Laut Aufgabenstellung soll die PROCESSING AVAILABLE rückmeldung durch den processing Thread erfolgen weswegen P weitergegeben wird
			State = INIT;
			break;

		case DECRYPT:
			Operation = opDECRYPT;
			State = DLEN;
			break;
		case KEY:
			Operation = opKEY;
			iLength = 16;
			my_uartdata = malloc(iLength);
			State = DATA;
			break;
		case IV:
			Operation = opIV;
			iLength = 16;
			my_uartdata = malloc(iLength);
			State = DATA;
			break;
		case DLEN:
			for(;;)
			{
				if (!uart_poll_in(my_uart_dev, &my_uartchar_buffer)) // Einlesen der Länge 
				{
					iLength = my_uartchar_buffer;
					break;
				}
			}
				my_uartdata = malloc(16+iLength); //aus dem Crypto-Example ist ersichtlich, dass der Anfange der Daten dem IV entspricht, weswegen auch dementsprechend 16 Bytes(IV) + Datenlänge gemalloced wird. 
				memcpy(my_uartdata,my_IV,16); // IV wird in my_uartdata kopiert 
				my_uartdata += 16; //Pointer hinter den IV verschieben (Danke Lorenz)
				State = DATA;	
			break;

		case DATA:
			while (iPos < iLength)
			{	
				if(!uart_poll_in(my_uart_dev,&my_uartchar_buffer)) //Daten der UART einlesen 
				{
					my_uartdata[iPos++] = my_uartchar_buffer;
				}

			}
			iPos = 0;
			State = OP;
			break;

		case OP:
			switch (Operation)
			{
			case opKEY:
				memcpy(my_KEY, my_uartdata, iLength); // Eingelesene Daten in my_Key kopieren 
				State = INIT; 
				break;

			case opIV:
				memcpy(my_IV, my_uartdata, iLength); // Eingelesene Daten in my_IV kopieren
				State = INIT;
				break;
			
			case opDECRYPT:
				my_uartdata -= 16; // Pointer an den Start der Daten versetzten 
				my_cipher_buffer = my_uartdata; // Daten in den Cipher Buffer für die Decrypt Operation schreiben
				my_proc_msgq_put("D\n"); // Ein D an Processing Thread übergeben um Decryption zu starten
				State = INIT;
				break;

			default:
				State = INIT;
				break;
			}
			break;
	default:
		State = INIT;
		break;
		}
}
	return vali;
}


int cbc_mode()
{
    uint32_t rec_data_length = (16+iLength); //Datenlänge
    uint32_t my_decdata_length = iLength; // Größe des Output Buffers 
    my_decdata = malloc(iLength); 

    struct cipher_ctx ini = { //Structs für die verschiedenen Parameter
		.keylen = 16,
		.key.bit_stream = my_KEY,
		.flags = cap_flags,
	};

	struct cipher_pkt decrypt = {
		.in_buf = my_cipher_buffer,
		.in_len = rec_data_length,
		.out_buf = my_decdata,
		.out_buf_max = my_decdata_length,
	};

	if(cipher_begin_session(my_crypto_dev, &ini, CRYPTO_CIPHER_ALGO_AES,CRYPTO_CIPHER_MODE_CBC,CRYPTO_CIPHER_OP_DECRYPT)) // Session wird gestartet und es werden die Verschiedenen Parameter wie der Modus gesetzt
    {
        LOG_ERR("CBC Session BEGIN - Failed");
        my_uart_msgq_put("XERROR\n");
		goto out;
	}

	/* TinyCrypt keeps IV at the start of encrypted buffer */
	if (cipher_cbc_op(&ini, &decrypt, my_cipher_buffer)) { //Hier wird die Decrypt-Operation durchgeführt
		LOG_ERR("CBC mode DECRYPT - Failed");
        my_uart_msgq_put("XERROR\n");
		goto out;
	}

    cipher_free_session(my_crypto_dev, &ini); // Session wird wieder aufgelöst, Decrypt hat funktioniert gibt 1 zurück
    return 1;

out:
	cipher_free_session(my_crypto_dev, &ini); // Session wird wieder aufgelöst, Decrypt hat nicht funktioniert gibt 0 zurück
    return 0; 
}
//####################################################################


// Processing Funktion
//####################################################################
void *processing (void *vali)
{
	LOG_INF("Processing Thread startet");
	unsigned char* data;
	unsigned char* my_decdata_outformat;
	for(;;)
	{
		if(!k_msgq_get(&my_proc_msgq, &data, K_NO_WAIT)) //An den Processing Thread übergeben Daten auslesen
		{
			if(data[0] == 'P')  // Bei P wird PROCESSING AVAILABLE zurückgegeben 
			{

				if (iAvailability == 1)
				{
				my_uart_msgq_put("PROCESSING AVAILABLE\n");
				}
			}
			
			else if(data[0] == 'W') // Bei W wird 10 sekunden gewartet, dadurch dass dies im Processing Thread geschieht kann der Uart_in Thread trotzdem mit BUSY antworten
			{
				iAvailability = 0;
				sleep(10);
				iAvailability = 1;
			}


			else if(data[0] == 'D') // Bei D wird die Dekryption gestartet 
			{
				iAvailability = 0;
				if (cbc_mode()) // cbc_mode() wird ausgeführt 
				{
					printk("Decrypted Data:%s\n",my_decdata);
					my_decdata_outformat = malloc(strlen(my_decdata)+3); // Um die tests zu bestehen wird ein bestimmtes Format benötigt. Am Anfang muss ein D*space* stehen, nach den Daten eine Null terminierung
					memcpy(my_decdata_outformat,"D ",strlen("D ")); // Es wird wie geforderd ein D*space* nach my_decdata_outformat kopiert
					memcpy(my_decdata_outformat+2, my_decdata, strlen(my_decdata)); // Die eigentlichen Daten werden ebenfalls nach my_decdata_outformat kopiert, die Nullterminierung wird im uart_out Thread behandelt
					my_uart_msgq_put(my_decdata_outformat); // die neue Formatierung wird ausgegeben (Danke Lorenz)
				}
				iAvailability = 1;
			}	
		}
	}
	return vali;
	
}
//Validate HW
//###################################################################
int validate_hw_compatibility(const struct device *dev) // Wurde so aus dem Zephyr Crypto Beispiel übernommen
{
	uint32_t flags = 0U;

	flags = cipher_query_hwcaps(dev);// cipher_query_hwcaps checkt beim Übergebenen Device alle Crypto Capabilitys, Daraus resultieren die Flags für den cbc_mode()
	if ((flags & CAP_RAW_KEY) == 0U) {
		LOG_INF("Please provision the key separately "
			"as the module doesnt support a raw key");
		return -1;
	}

	if ((flags & CAP_SYNC_OPS) == 0U) {
		LOG_INF("The app assumes sync semantics. "
		  "Please rewrite the app accordingly before proceeding");
		return -1;
	}

	if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) {
		LOG_INF("The app assumes distinct IO buffers. "
		"Please rewrite the app accordingly before proceeding");
		return -1;
	}

	cap_flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;

	return 0;

}
//####################################################################
//UART configuration
//####################################################################
void setup (void)
{
	const struct uart_config my_uart_config = { // das uart_config struct wird für die Uart Configuration benötigt. Die Eingestellten Werte sind bei einem virtuellen µC unwichtig
	.baudrate = 115200,
	.parity = UART_CFG_PARITY_NONE,
	.stop_bits = UART_CFG_STOP_BITS_1,
	.data_bits = UART_CFG_DATA_BITS_8,
	.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

	my_uart_dev = device_get_binding(UART_NAME); // Uart-Device "UART_0" wird gebinded
	if(!my_uart_dev){
        LOG_ERR("UART Device Binding");
	}
	my_crypto_dev = device_get_binding(CRYPTO_NAME);// Crypto-Device "CRYPTO_TC" wird gebinded
	if(!my_crypto_dev){
        LOG_ERR("CRYPTO Device Binding");
	}

	if(!uart_configure(my_uart_dev, &my_uart_config)){ // Uart wird configured 
		LOG_ERR("UART Configuring");
	}

	if(validate_hw_compatibility(my_crypto_dev)) // validate HW Funktion wird aufgerufen 
	{
		LOG_ERR("HW Compatibility");
	}
	LOG_INF("Setup is Complete");

}
//####################################################################
//Thread Create Funktion
//####################################################################
void thread_create(void) // Threads werden erstellt
{
	pthread_t th_uart_out, th_uart_in, th_processing;
	if (pthread_create(&th_uart_out, NULL, &uart_out, NULL) != 0)
	{
      LOG_ERR("Creating uart_out thread");
	}
	sleep(1);
	if (pthread_create(&th_uart_in, NULL, &uart_in, NULL) != 0)
	{
        LOG_ERR("Creating uart_in thread");
	}
	sleep(1);
	if (pthread_create(&th_processing, NULL, &processing, NULL) != 0)
	{
        LOG_ERR("Creating processing thread");
	}
}
//####################################################################


//Main
//####################################################################
void main(void)
{
	printk("\033[1;35m\e[4m******My Crypto Processor******\e\033[0m\n");
	setup(); //  setup wird ausgeführt
	thread_create();// threads werden erstellt
	for(;;)
	{
		sleep(5);
		printk("\033[1;32m\e[4mMain Alive and Well!\e\033[0m\n");
	}
}