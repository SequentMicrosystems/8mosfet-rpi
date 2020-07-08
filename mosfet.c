/*
 * mosfet8.c:
 *	Command-line interface to the Raspberry
 *	Pi's 8-Mosfet board.
 *	Copyright (c) 2016-2020 Sequent Microsystem
 *	<http://www.sequentmicrosystem.com>
 ***********************************************************************
 *	Author: Alexandru Burcea
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mosfet.h"
#include "comm.h"
#include "thread.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)0

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CMD_ARRAY_SIZE	7

const u8 mosfetMaskRemap[8] =
{
	0x20,
	0x40,
	0x08,
	0x04,
	0x02,
	0x01,
	0x80,
	0x10};
const int mosfetChRemap[8] =
{
	5,
	6,
	3,
	2,
	1,
	0,
	7,
	4};



int mosfetChSet(int dev, u8 channel, OutStateEnumType state);
int mosfetChGet(int dev, u8 channel, OutStateEnumType* state);
u8 mosfetToIO(u8 mosfet);
u8 IOToMosfet(u8 io);

static void doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
{
	"-h",
	1,
	&doHelp,
	"\t-h          Display the list of command options or one command option details\n",
	"\tUsage:      8mosfet -h    Display command options list\n",
	"\tUsage:      8mosfet -h <param>   Display help for <param> command option\n",
	"\tExample:    8mosfet -h write    Display help for \"write\" command option\n"};

static void doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v              Display the version number\n",
	"\tUsage:          8mosfet -v\n",
	"",
	"\tExample:        8mosfet -v  Display the version number\n"};

static void doWarranty(int argc, char* argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty       Display the warranty\n",
	"\tUsage:          8mosfet -warranty\n",
	"",
	"\tExample:        8mosfet -warranty  Display the warranty text\n"};

static void doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
{
	"-list",
	1,
	&doList,
	"\t-list:       List all 8mosfet boards connected,\n\treturn       nr of boards and stack level for every board\n",
	"\tUsage:       8mosfet -list\n",
	"",
	"\tExample:     8mosfet -list display: 1,0 \n"};

static void doMosfetWrite(int argc, char *argv[]);
const CliCmdType CMD_WRITE =
{
	"write",
	2,
	&doMosfetWrite,
	"\twrite:       Set mosfets On/Off\n",
	"\tUsage:       8mosfet <id> write <channel> <on/off>\n",
	"\tUsage:       8mosfet <id> write <value>\n",
	"\tExample:     8mosfet 0 write 2 On; Set Mosfet #2 on Board #0 On\n"};

static void doMosfetRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
{
	"read",
	2,
	&doMosfetRead,
	"\tread:        Read mosfets status\n",
	"\tUsage:       8mosfet <id> read <channel>\n",
	"\tUsage:       8mosfet <id> read\n",
	"\tExample:     8mosfet 0 read 2; Read Status of Mosfet #2 on Board #0\n"};

static void doTest(int argc, char* argv[]);
const CliCmdType CMD_TEST =
{
	"test",
	2,
	&doTest,
	"\ttest:        Turn ON and OFF the mosfets until press a key\n",
	"",
	"\tUsage:       8mosfet <id> test\n",
	"\tExample:     8mosfet 0 test\n"};

CliCmdType gCmdArray[CMD_ARRAY_SIZE];

char *usage = "Usage:	 8mosfet -h <command>\n"
	"         8mosfet -v\n"
	"         8mosfet -warranty\n"
	"         8mosfet -list\n"
	"         8mosfet <id> write <channel> <on/off>\n"
	"         8mosfet <id> write <value>\n"
	"         8mosfet <id> read <channel>\n"
	"         8mosfet <id> read\n"
	"         8mosfet <id> test\n"
	"Where: <id> = Board level id = 0..7\n"
	"Type 8mosfet -h <command> for more help"; // No trailing newline needed here.

char *warranty =
	"	       Copyright (c) 2016-2020 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";
u8 mosfetToIO(u8 mosfet)
{
	u8 i;
	u8 val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (mosfet & (1 << i)) != 0)
			val += mosfetMaskRemap[i];
	}
	return 0xff ^ val;
}

u8 IOToMosfet(u8 io)
{
	u8 i;
	u8 val = 0;

	io ^= 0xff;
	for (i = 0; i < 8; i++)
	{
		if ( (io & mosfetMaskRemap[i]) != 0)
		{
			val += 1 << i;
		}
	}
	return val;
}

int mosfetChSet(int dev, u8 channel, OutStateEnumType state)
{
	int resp;
	u8 buff[2];

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NR_MAX))
	{
		printf("Invalid mosfet nr!\n");
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return FAIL;
	}

	switch (state)
	{
	case ON:
		buff[0] &= ~ (1 << mosfetChRemap[channel - 1]);
		resp = i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
		break;
	case OFF:
		buff[0] |= 1 << mosfetChRemap[channel - 1];
		resp = i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
		break;
	default:
		printf("Invalid mosfet state!\n");
		return ERROR;
		break;
	}
	return resp;
}

int mosfetChGet(int dev, u8 channel, OutStateEnumType* state)
{
	u8 buff[2];

	if (NULL == state)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NR_MAX))
	{
		printf("Invalid mosfet nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return ERROR;
	}

	if (buff[0] & (1 << mosfetChRemap[channel - 1]))
	{
		*state = OFF;
	}
	else
	{
		*state = ON;
	}
	return OK;
}

int mosfetSet(int dev, int val)
{
	u8 buff[2];

	buff[0] = mosfetToIO(0xff & val);

	return i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
}

int mosfetGet(int dev, int* val)
{
	u8 buff[2];

	if (NULL == val)
	{
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	*val = IOToMosfet(buff[0]);
	return OK;
}

int doBoardInit(int stack)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	if((stack < 0) || (stack > 7))
	{
		printf("Invalid stack level [0..7]!");
		return ERROR;
	}
	add = (stack  + RELAY8_HW_I2C_BASE_ADD) ^ 0x07;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		printf("Mosfet8 id %d not detected\n", stack);
		return ERROR;
	}
	if (buff[0] != 0) //non initialized I/O Expander
	{
		// make all I/O pins output
		buff[0] = 0;
		if (0 > i2cMem8Write(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			return ERROR;
		}
		// put all pins in 0-logic state
		buff[0] = 0xff;
		if (0 > i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
		{
			return ERROR;
		}
	}

	return dev;
}

int boardCheck(int hwAdd)
{
	int dev = 0;
	uint8_t buff[8];

	hwAdd ^= 0x07;
	dev = i2cSetup(hwAdd);
	if (dev == -1)
	{
		return FAIL;
	}
	if (ERROR == i2cMem8Read(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	return OK;
}

/*
 * doMosfetWrite:
 *	Write coresponding mosfet channel
 **************************************************************************************
 */
static void doMosfetWrite(int argc, char *argv[])
{
	int pin = 0;
	OutStateEnumType state = STATE_COUNT;
	int val = 0;
	int dev = 0;
	OutStateEnumType stateR = STATE_COUNT;
	int valR = 0;
	int retry = 0;

	if ( (argc != 5) && (argc != 4))
	{
		printf("Usage: 8mosfet <id> write <mosfet number> <on/off> \n");
		printf("Usage: 8mosfet <id> write <mosfet reg value> \n");
		exit(1);
	}

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}
	if (argc == 5)
	{
		pin = atoi(argv[3]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NR_MAX))
		{
			printf("Mosfet number value out of range\n");
			exit(1);
		}

		/**/if ( (strcasecmp(argv[4], "up") == 0)
			|| (strcasecmp(argv[4], "on") == 0))
			state = ON;
		else if ( (strcasecmp(argv[4], "down") == 0)
			|| (strcasecmp(argv[4], "off") == 0))
			state = OFF;
		else
		{
			if ( (atoi(argv[4]) >= STATE_COUNT) || (atoi(argv[4]) < 0))
			{
				printf("Invalid mosfet state!\n");
				exit(1);
			}
			state = (OutStateEnumType)atoi(argv[4]);
		}

		retry = RETRY_TIMES;

		while ( (retry > 0) && (stateR != state))
		{
			if (OK != mosfetChSet(dev, pin, state))
			{
				printf("Fail to write mosfet\n");
				exit(1);
			}
			if (OK != mosfetChGet(dev, pin, &stateR))
			{
				printf("Fail to read mosfet\n");
				exit(1);
			}
			retry--;
		}
#ifdef DEBUG_I
		if(retry < RETRY_TIMES)
		{
			printf("retry %d times\n", 3-retry);
		}
#endif
		if (retry == 0)
		{
			printf("Fail to write mosfet\n");
			exit(1);
		}
	}
	else
	{
		val = atoi(argv[3]);
		if (val < 0 || val > 255)
		{
			printf("Invalid mosfet value\n");
			exit(1);
		}

		retry = RETRY_TIMES;
		valR = -1;
		while ( (retry > 0) && (valR != val))
		{

			if (OK != mosfetSet(dev, val))
			{
				printf("Fail to write mosfet!\n");
				exit(1);
			}
			if (OK != mosfetGet(dev, &valR))
			{
				printf("Fail to read mosfet!\n");
				exit(1);
			}
		}
		if (retry == 0)
		{
			printf("Fail to write mosfet!\n");
			exit(1);
		}
	}
}

/*
 * doMosfetRead:
 *	Read mosfet state
 ******************************************************************************************
 */
static void doMosfetRead(int argc, char *argv[])
{
	int pin = 0;
	int val = 0;
	int dev = 0;
	OutStateEnumType state = STATE_COUNT;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		pin = atoi(argv[3]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NR_MAX))
		{
			printf("Mosfet number value out of range!\n");
			exit(1);
		}

		if (OK != mosfetChGet(dev, pin, &state))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		if (state != 0)
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
	}
	else if (argc == 3)
	{
		if (OK != mosfetGet(dev, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%d\n", val);
	}
	else
	{
		printf("Usage: %s read mosfet value\n", argv[0]);
		exit(1);
	}
}

static void doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		for (i = 0; i < CMD_ARRAY_SIZE; i++)
		{
			if ( (gCmdArray[i].name != NULL ))
			{
				if (strcasecmp(argv[2], gCmdArray[i].name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i].help, gCmdArray[i].usage1,
						gCmdArray[i].usage2, gCmdArray[i].example);
					break;
				}
			}
		}
		if (CMD_ARRAY_SIZE == i)
		{
			printf("Option \"%s\" not found\n", argv[2]);
			printf("%s: %s\n", argv[0], usage);
		}
	}
	else
	{
		printf("%s: %s\n", argv[0], usage);
	}
}

static void doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("8mosfet v%d.%d.%d Copyright (c) 2016 - 2020 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: 8mosfet -warranty\n");

}

static void doList(int argc, char *argv[])
{
	int ids[8];
	int i;
	int cnt = 0;

	UNUSED(argc);
	UNUSED(argv);

	for (i = 0; i < 8; i++)
	{
		if (boardCheck(RELAY8_HW_I2C_BASE_ADD + i) == OK)
		{
			ids[cnt] = i;
			cnt++;
		}
	}
	printf("%d board(s) detected\n", cnt);
	if (cnt > 0)
	{
		printf("Id:");
	}
	while (cnt > 0)
	{
		cnt--;
		printf(" %d", ids[cnt]);
	}
	printf("\n");
}

/* 
 * Self test for production
 */
static void doTest(int argc, char* argv[])
{
	int dev = 0;
	int i = 0;
	int retry = 0;
	int relVal;
	int valR;
	int mosfetResult = 0;
	FILE* file = NULL;
	const u8 mosfetOrder[8] =
	{
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8};

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}
	if (argc == 4)
	{
		file = fopen(argv[3], "w");
		if (!file)
		{
			printf("Fail to open result file\n");
			//return -1;
		}
	}
//mosfet test****************************
	if (strcasecmp(argv[2], "test") == 0)
	{
		relVal = 0;
		printf(
			"Are all mosfets and LEDs turning on and off in sequence?\nPress y for Yes or any key for No....");
		startThread();
		while (mosfetResult == 0)
		{
			for (i = 0; i < 8; i++)
			{
				mosfetResult = checkThreadResult();
				if (mosfetResult != 0)
				{
					break;
				}
				valR = 0;
				relVal = (u8)1 << (mosfetOrder[i] - 1);

				retry = RETRY_TIMES;
				while ( (retry > 0) && ( (valR & relVal) == 0))
				{
					if (OK != mosfetChSet(dev, mosfetOrder[i], ON))
					{
						retry = 0;
						break;
					}

					if (OK != mosfetGet(dev, &valR))
					{
						retry = 0;
					}
				}
				if (retry == 0)
				{
					printf("Fail to write mosfet\n");
					if (file)
						fclose(file);
					exit(1);
				}
				busyWait(150);
			}

			for (i = 0; i < 8; i++)
			{
				mosfetResult = checkThreadResult();
				if (mosfetResult != 0)
				{
					break;
				}
				valR = 0xff;
				relVal = (u8)1 << (mosfetOrder[i] - 1);
				retry = RETRY_TIMES;
				while ( (retry > 0) && ( (valR & relVal) != 0))
				{
					if (OK != mosfetChSet(dev, mosfetOrder[i], OFF))
					{
						retry = 0;
					}
					if (OK != mosfetGet(dev, &valR))
					{
						retry = 0;
					}
				}
				if (retry == 0)
				{
					printf("Fail to write mosfet!\n");
					if (file)
						fclose(file);
					exit(1);
				}
				busyWait(150);
			}
		}
	}
	if (mosfetResult == YES)
	{
		if (file)
		{
			fprintf(file, "Mosfet Test ............................ PASS\n");
		}
		else
		{
			printf("Mosfet Test ............................ PASS\n");
		}
	}
	else
	{
		if (file)
		{
			fprintf(file, "Mosfet Test ............................ FAIL!\n");
		}
		else
		{
			printf("Mosfet Test ............................ FAIL!\n");
		}
	}
	if (file)
	{
		fclose(file);
	}
	mosfetSet(dev, 0);
}

static void doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
}

static void cliInit(void)
{
	int i = 0;

	memset(gCmdArray, 0, sizeof(CliCmdType) * CMD_ARRAY_SIZE);

	memcpy(&gCmdArray[i], &CMD_HELP, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WAR, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_LIST, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WRITE, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_READ, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_TEST, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_VERSION, sizeof(CliCmdType));

}

int main(int argc, char *argv[])
{
	int i = 0;

	cliInit();

	if (argc == 1)
	{
		printf("%s\n", usage);
		return 1;
	}
	for (i = 0; i < CMD_ARRAY_SIZE; i++)
	{
		if ( (gCmdArray[i].name != NULL ) && (gCmdArray[i].namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i].namePos], gCmdArray[i].name) == 0)
			{
				gCmdArray[i].pFunc(argc, argv);
				return 0;
			}
		}
	}
	printf("Invalid command option\n");
	printf("%s\n", usage);

	return 0;
}
