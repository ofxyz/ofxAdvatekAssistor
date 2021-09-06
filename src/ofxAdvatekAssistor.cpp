#include "ofxAdvatekAssistor.h"

const char* RGBW_Order[24] = {
		"R-G-B/R-G-B-W",
		"R-B-G/R-B-G-W",
		"G-R-B/G-R-B-W",
		"B-R-G/B-R-G-W",
		"G-B-R/G-B-R-W",
		"B-G-R/B-G-R-W",
		"R-G-W-B",
		"R-B-W-G",
		"G-R-W-B",
		"B-R-W-G",
		"G-B-W-R",
		"B-G-W-R",
		"R-W-G-B",
		"R-W-B-G",
		"G-W-R-B",
		"B-W-R-G",
		"G-W-B-R",
		"B-W-G-R",
		"W-R-G-B",
		"W-R-B-G",
		"W-G-R-B",
		"W-B-R-G",
		"W-G-B-R",
		"W-B-G-R"
};

const char* DriverTypes[3] = {
		"RGB only",
		"RGBW only",
		"Either RGB or RGBW"
};

const char* DriverSpeeds[5] = {
	"N/A (single fixed speed)",
	"Slow only (data only driver, fixed at the slow speed)",
	"Fast only (data only driver, fixed at the fast speed)",
	"Either slow or fast (selectable between 2 speeds)",
	"Adjustable clock speed"
};

const char* DriverSpeedsMhz[12] = {
	"0.4 MHz", // Data Only Slow
	"0.6 MHz", // Data Only Fast
	"0.8 MHz",
	"1.0 MHz",
	"1.2 MHz",
	"1.4 MHz",
	"1.6 MHz",
	"1.8 MHz",
	"2.0 MHz",
	"2.2 MHz",
	"2.5 MHz",
	"2.9 MHz"
};

const char* TestModes[9] = {
	"None(Live Control Data)",
	"RGBW Cycle",
	"Red",
	"Green",
	"Blue",
	"White",
	"Set Color",
	"Color Fade",
	"Single Pixel"
};

//--------------------------------------------------------------
void ofxAdvatekAssistor::setup() {
    udpConnection.Create();
	udpConnection.Connect("255.255.255.255", 49150);
	udpConnection.Bind(49150);
	udpConnection.SetEnableBroadcast(true);
	udpConnection.SetNonBlocking(true);
}

//--------------------------------------------------------------
void ofxAdvatekAssistor::update() {

    const int packetSize = 100000;
	char udpMessage[packetSize];
	memset(udpMessage, 0, sizeof(udpMessage));
	int newMessage = udpConnection.Receive(udpMessage, packetSize); //returns number of bytes received                                                                                                             
	//std::cout << "Packet Size: " << newMessage << std::endl;
	//std::cout << "UDP Packet: " << std::endl;

	if (newMessage > 1) {

		std::cout << "===================================================" << std::endl;

		/*for (int i = 0; i < newMessage; i++) {
			printf("0x%02X", (unsigned char)udpMessage[i]);
		}
		printf("\n");*/
		char ID[9];
		uint8_t* data = (uint8_t*)udpMessage;
		memcpy(ID, data, sizeof(uint8_t) * 9);
		data += 9;
		std::cout << "ID: " << ID << std::endl;
		
		//--------
		
		uint16_t OpCodes;// = (uint16_t)data;
		memcpy(&OpCodes, data, sizeof(uint16_t));
		data += 2;
		// Swap bytes
		OpCodes = ((OpCodes & 0xff) << 8) | ((OpCodes & 0xff00) >> 8);
		std::cout << "messageType: " << OpCodes << std::endl;

		if (OpCodes == 1) return;

		//--------

		uint8_t ProtVer;
		memcpy(&ProtVer, data, sizeof(uint8_t));
		data += 1;
		std::cout << "ProtVer: " << (int)ProtVer << std::endl;
		
		//--------

		uint8_t CurrentProtVer;
		memcpy(&CurrentProtVer, data, sizeof(uint8_t));
		data += 1;
		std::cout << "CurrentProtVer: " << (int)CurrentProtVer << std::endl;

		//--------

		uint8_t MAC[6];
		memcpy(MAC, data, sizeof(uint8_t) * 6);
		data += 6;

		std::stringstream ss;
		ss << std::hex << std::setfill('0');

		for (int i(0); i < 6; i++) {
			ss << std::hex << std::setw(2) << static_cast<int>(MAC[i]);
			if(i < 5) ss << ":";
		}

		string macAddress = ss.str();
		std::cout << "MAC Address: " << macAddress << std::endl;

		//--------

		uint8_t LENGTH_MODEL;
		memcpy(&LENGTH_MODEL, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "LENGHT_MODEL: " << (int)LENGTH_MODEL << std::endl;

		uint8_t* MODEL_NAME = new uint8_t[LENGTH_MODEL];
		memcpy(MODEL_NAME, data, sizeof(uint8_t)*LENGTH_MODEL);
		data += LENGTH_MODEL;
		std::cout << "MODEL_NAME: " << MODEL_NAME << std::endl;

		//--------

		uint8_t HwRev;
		memcpy(&HwRev, data, sizeof(uint8_t));
		data += 1;
		std::cout << "HwRev: " << "V" << ((float)HwRev/10.f) << std::endl;

		//--------

		uint8_t MinAssistantVer[3];
		memcpy(MinAssistantVer, data, sizeof(uint8_t) * 3);
		data += 3;

		std::cout << "MinAssistantVer: " << "V" << (int)MinAssistantVer[0] << "." << (int)MinAssistantVer[1] << "." << (int)MinAssistantVer[2] << std::endl;

		//--------

		uint8_t LENGTH_FW;
		memcpy(&LENGTH_FW, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "LENGTH_FW: " << (int)LENGTH_FW << std::endl;

		uint8_t* FirmWare = new uint8_t[LENGTH_FW];
		memcpy(FirmWare, data, sizeof(uint8_t)*LENGTH_FW);
		data += LENGTH_FW;
		std::cout << "FirmWare: " << FirmWare << std::endl;
		
		//--------
		// BRAND: OEM Code, not needed
		data += 1;

		//--------

		uint8_t CurrentIP[4];
		memcpy(CurrentIP, data, sizeof(uint8_t) * 4);
		data += 4;
		
		std::cout << "CurrentIP: " << (int)CurrentIP[0] << "." << (int)CurrentIP[1] << "." << (int)CurrentIP[2] << "." << (int)CurrentIP[3] << std::endl;

		//--------

		uint8_t CurrentSM[4];
		memcpy(CurrentSM, data, sizeof(uint8_t) * 4);
		data += 4;

		std::cout << "CurrentSM: " << (int)CurrentSM[0] << "." << (int)CurrentSM[1] << "." << (int)CurrentSM[2] << "." << (int)CurrentSM[3] << std::endl;

		//--------

		uint8_t DHCP;
		memcpy(&DHCP, data, sizeof(uint8_t));
		data += 1;
		std::cout << "DHCP: " << ((DHCP)?"On":"Off") << std::endl;

		//--------

		uint8_t StaticIP[4];
		memcpy(StaticIP, data, sizeof(uint8_t) * 4);
		data += 4;

		std::cout << "StaticIP: " << (int)StaticIP[0] << "." << (int)StaticIP[1] << "." << (int)StaticIP[2] << "." << (int)StaticIP[3] << std::endl;

		//--------

		uint8_t StaticSM[4];
		memcpy(StaticSM, data, sizeof(uint8_t) * 4);
		data += 4;

		std::cout << "StaticSM: " << (int)StaticSM[0] << "." << (int)StaticSM[1] << "." << (int)StaticSM[2] << "." << (int)StaticSM[3] << std::endl;

		//--------

		uint8_t Protocol;
		memcpy(&Protocol, data, sizeof(uint8_t));
		data += 1;
		std::cout << "Protocol: " << ((Protocol) ? "ArtNet" : "sACN") << std::endl;

		//--------
		uint8_t holdLastFrame;
		memcpy(&holdLastFrame, data, sizeof(uint8_t));
		data += 1;
		std::cout << "holdLastFrame: " << ((holdLastFrame) ? "True" : "False") << std::endl;

		//--------
		// SimpleConfig: not needed
		data += 1;
		
		//--------
		
		uint16_t maxPixPerOutput;
		memcpy(&maxPixPerOutput, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		maxPixPerOutput = ((maxPixPerOutput & 0xff) << 8) | ((maxPixPerOutput & 0xff00) >> 8);

		std::cout << "maxPixPerOutput: " << (int)maxPixPerOutput << std::endl;

		//--------

		uint8_t NUM_OUTPUTS;
		memcpy(&NUM_OUTPUTS, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_OUTPUTS: " << (int)NUM_OUTPUTS << std::endl;

		//--------

		uint16_t* OutputPixels = new uint16_t[NUM_OUTPUTS];
		memcpy(OutputPixels, data, sizeof(uint16_t)*NUM_OUTPUTS);
		data += (NUM_OUTPUTS*2);

		std::cout << "OutputPixels: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			OutputPixels[i] = ((OutputPixels[i] & 0xff) << 8) | ((OutputPixels[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)OutputPixels[i] << " ";
		}
		std::cout << std::endl;

		//--------

		uint16_t* OutputUniv = new uint16_t[NUM_OUTPUTS];
		memcpy(OutputUniv, data, sizeof(uint16_t)*NUM_OUTPUTS);
		data += (NUM_OUTPUTS * 2);

		std::cout << "OutputUniverse: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			OutputUniv[i] = ((OutputUniv[i] & 0xff) << 8) | ((OutputUniv[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)OutputUniv[i] << " ";
		}
		std::cout << std::endl;

		//--------

		uint16_t* OutputChan = new uint16_t[NUM_OUTPUTS];
		memcpy(OutputChan, data, sizeof(uint16_t)*NUM_OUTPUTS);
		data += (NUM_OUTPUTS * 2);

		std::cout << "OutputChannel: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			OutputChan[i] = ((OutputChan[i] & 0xff) << 8) | ((OutputChan[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)OutputChan[i] << " ";
		}
		std::cout << std::endl;

		//---------
		
		uint8_t* outputNull = new uint8_t[NUM_OUTPUTS];
		memcpy(outputNull, data, sizeof(uint8_t)*NUM_OUTPUTS);
		data += NUM_OUTPUTS;
		
		std::cout << "OutputNull: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			std::cout << i << ":" << (int)outputNull[i] << " ";
		}
		std::cout << std::endl;

		//---------

		uint16_t* OutputZig = new uint16_t[NUM_OUTPUTS];
		memcpy(OutputZig, data, sizeof(uint16_t)*NUM_OUTPUTS);
		data += (NUM_OUTPUTS * 2);

		std::cout << "OutputZigZag: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			OutputZig[i] = ((OutputZig[i] & 0xff) << 8) | ((OutputZig[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)OutputZig[i] << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t* outputReverse = new uint8_t[NUM_OUTPUTS];
		memcpy(outputReverse, data, sizeof(uint8_t)*NUM_OUTPUTS);
		data += NUM_OUTPUTS;

		std::cout << "outputReverse: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			std::cout << i << ":" << (((int)outputReverse[i])?"True":"False") << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t* outputColOrder = new uint8_t[NUM_OUTPUTS];
		memcpy(outputColOrder, data, sizeof(uint8_t)*NUM_OUTPUTS);
		data += NUM_OUTPUTS;

		std::cout << "outputColOrder: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			std::cout << i << ":" << RGBW_Order[(int)outputColOrder[i]] << " ";
		}
		std::cout << std::endl;

		//---------

		uint16_t* OutputGrouping = new uint16_t[NUM_OUTPUTS];
		memcpy(OutputGrouping, data, sizeof(uint16_t)*NUM_OUTPUTS);
		data += (NUM_OUTPUTS * 2);

		std::cout << "OutputGrouping: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			OutputGrouping[i] = ((OutputGrouping[i] & 0xff) << 8) | ((OutputGrouping[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)OutputGrouping[i] << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t* OutputBrightness = new uint8_t[NUM_OUTPUTS];
		memcpy(OutputBrightness, data, sizeof(uint8_t)*NUM_OUTPUTS);
		data += NUM_OUTPUTS;

		std::cout << "OutputBrightness: ";
		for (int i(0); i < (int)NUM_OUTPUTS; i++) {
			std::cout << i << ":" << (int)OutputBrightness[i] << " ";
		}
		std::cout << std::endl;

		//--------

		uint8_t NUM_DMX_OUT;
		memcpy(&NUM_DMX_OUT, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_DMX_OUT: " << (int)NUM_DMX_OUT << std::endl;

		//--------

		uint8_t ProtocolsOnDmxOut;
		memcpy(&ProtocolsOnDmxOut, data, sizeof(uint8_t));
		data += 1;
		std::cout << "ProtocolsOnDmxOut: " << ((ProtocolsOnDmxOut && 0x01) ? "ArtNet" : "") << ((ProtocolsOnDmxOut<<1 && 0x01) ? ", sACN" : "") << std::endl;

		//---------

		uint8_t* DmxOutOn = new uint8_t[NUM_DMX_OUT];
		memcpy(DmxOutOn, data, sizeof(uint8_t)*NUM_DMX_OUT);
		data += NUM_DMX_OUT;

		std::cout << "DmxOutOn: ";
		for (int i(0); i < (int)NUM_DMX_OUT; i++) {
			std::cout << i << ":" << (((int)DmxOutOn[i]) ? "On" : "Off") << " ";
		}
		std::cout << std::endl;

		//---------

		uint16_t* DmxOutUniv = new uint16_t[NUM_DMX_OUT];
		memcpy(DmxOutUniv, data, sizeof(uint16_t)*NUM_DMX_OUT);
		data += (NUM_DMX_OUT*2);

		std::cout << "DmxOutUniv: ";
		for (int i(0); i < (int)NUM_DMX_OUT; i++) {
			DmxOutUniv[i] = ((DmxOutUniv[i] & 0xff) << 8) | ((DmxOutUniv[i] & 0xff00) >> 8);
			std::cout << i << ":" << (int)DmxOutUniv[i] << " ";
		}
		std::cout << std::endl;

		//--------

		uint8_t NUM_DRIVERS;
		memcpy(&NUM_DRIVERS, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_DRIVERS: " << (int)NUM_DRIVERS << std::endl;

		//--------

		uint8_t DRIVER_NAME_LENGTH;
		memcpy(&DRIVER_NAME_LENGTH, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "DRIVER_NAME_LENGTH: " << (int)NUM_DRIVERS << std::endl;

		//---------

		uint8_t* DriverType = new uint8_t[NUM_DRIVERS];
		memcpy(DriverType, data, sizeof(uint8_t)*NUM_DRIVERS);
		data += NUM_DRIVERS; // Mmmm, what if it is 0?

		std::cout << "DriverType: ";
		for (int i(0); i < (int)NUM_DRIVERS; i++) {
			std::cout << i << ":" << DriverTypes[(int)DriverType[i]] << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t* DriverSpeed = new uint8_t[NUM_DRIVERS];
		memcpy(DriverSpeed, data, sizeof(uint8_t)*NUM_DRIVERS);
		data += NUM_DRIVERS;

		std::cout << "DriverSpeed: ";
		for (int i(0); i < (int)NUM_DRIVERS; i++) {
			std::cout << i << ":" << DriverSpeeds[(int)DriverSpeed[i]] << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t* DriverExpandable = new uint8_t[NUM_DRIVERS];
		memcpy(DriverExpandable, data, sizeof(uint8_t)*NUM_DRIVERS);
		data += NUM_DRIVERS; 

		std::cout << "DriverExpandable: ";
		for (int i(0); i < (int)NUM_DRIVERS; i++) {
			std::cout << i << ":" << (((int)DriverExpandable[i]) ? "Capable of expanded mode" : "Normal mode only") << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t** DriverNames = new uint8_t*[NUM_DRIVERS];

		for (int i = 0; i < NUM_DRIVERS; i++) {
			DriverNames[i] = new uint8_t[DRIVER_NAME_LENGTH+1];
			memset(DriverNames[i], 0, sizeof(uint8_t)*DRIVER_NAME_LENGTH+1);
			memcpy(DriverNames[i], data, sizeof(uint8_t)*DRIVER_NAME_LENGTH);
			data += DRIVER_NAME_LENGTH;
		}
	
		//std::cout << "DriverNames: ";
		//for (int i(0); i < (int)NUM_DRIVERS; i++) {
		//	std::cout << i << ":" << DriverNames[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		uint8_t CurrentDriver;
		memcpy(&CurrentDriver, data, sizeof(uint8_t));
		data += 1;
		std::cout << "CurrentDriver: " << DriverNames[(int)CurrentDriver] << std::endl;

		//---------

		uint8_t CurrentDriverType;
		memcpy(&CurrentDriverType, data, sizeof(uint8_t));
		data += 1;
		std::cout << "CurrentDriverType: " << (((int)CurrentDriverType)? "RGBW" : "RGB") << std::endl;

		//---------

		uint8_t CurrentDriverSpeed;
		memcpy(&CurrentDriverSpeed, data, sizeof(uint8_t));
		data += 1;
		std::cout << "CurrentDriverSpeed: " << DriverSpeedsMhz[(int)CurrentDriverSpeed] << std::endl;

		//---------

		uint8_t CurrentDriverExpanded;
		memcpy(&CurrentDriverExpanded, data, sizeof(uint8_t));
		data += 1;
		std::cout << "CurrentDriverExpanded: " << (((int)CurrentDriverType) ? "Expanded / Condensed mode)" : "Normal mode") << std::endl;

		//---------

		uint8_t Gamma[4];
		memcpy(Gamma, data, sizeof(uint8_t) * 4);
		data += 4;

		std::cout << "Gamma: R" << (float)Gamma[0] * 0.1 << " G" << (float)Gamma[1] * 0.1 << " B" << (float)Gamma[2] * 0.1 << " W" << (float)Gamma[3] * 0.1 << std::endl;

		//---------

		uint8_t Nickname[40];
		memcpy(Nickname, data, sizeof(uint8_t) * 40);
		data += 40;

		std::cout << "Nickname: " << Nickname << std::endl;

		//--------

		uint16_t Temperature;
		memcpy(&Temperature, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		Temperature = ((Temperature & 0xff) << 8) | ((Temperature & 0xff00) >> 8);

		std::cout << "Temperature: " << (float)Temperature*0.1 << std::endl;

		//---------

		uint8_t MaxTargetTemp;
		memcpy(&MaxTargetTemp, data, sizeof(uint8_t));
		data += 1;
		std::cout << "MaxTargetTemp: " << (float)MaxTargetTemp*0.1 << std::endl;

		//---------

		uint8_t NUM_BANKS;
		memcpy(&NUM_BANKS, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_BANKS: " << (int)NUM_BANKS << std::endl;

		//---------

		uint16_t* VoltageBanks = new uint16_t[NUM_BANKS];
		memcpy(VoltageBanks, data, sizeof(uint16_t)*NUM_BANKS);
		data += (NUM_BANKS * 2);

		std::cout << "VoltageBanks: ";
		for (int i(0); i < (int)NUM_BANKS; i++) {
			VoltageBanks[i] = ((VoltageBanks[i] & 0xff) << 8) | ((VoltageBanks[i] & 0xff00) >> 8);
			std::cout << i << ":" << (float)VoltageBanks[i]*0.1 << " ";
		}
		std::cout << std::endl;

		//---------

		uint8_t TestMode;
		memcpy(&TestMode, data, sizeof(uint8_t));
		data += 1;
		
		std::cout << "TestMode: " << TestModes[(int)TestMode] << std::endl;

		//---------

		uint8_t TestCols[4];
		memcpy(TestCols, data, sizeof(uint8_t) * 4);
		data += 4;

		std::cout << "TestColor: R" << (int)TestCols[0] << " G" << (int)TestCols[1] << " B" << (int)TestCols[2] << " W" << (int)TestCols[3] << std::endl;

		//---------

		uint8_t TestOutputNum;
		memcpy(&TestOutputNum, data, sizeof(uint8_t));
		data += 1;

		std::cout << "TestOutputNum: " << (int)TestOutputNum << std::endl;

		//--------

		uint16_t TestPixelNum;
		memcpy(&TestPixelNum, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		TestPixelNum = ((TestPixelNum & 0xff) << 8) | ((TestPixelNum & 0xff00) >> 8);

		std::cout << "TestPixelNum: " << (int)TestPixelNum << std::endl;

		//---------

	}
}

//--------------------------------------------------------------
void ofxAdvatekAssistor::poll() {
    char buf[12];
	memset(buf, '\0', 12);
	buf[0] = 'A';
	buf[1] = 'd';
	buf[2] = 'v';
	buf[3] = 'a';
	buf[4] = 't';
	buf[5] = 'e';
	buf[6] = 'c';
	buf[7] = 'h';
	buf[8] = 0x00;
	buf[9] = 0x00;
	buf[10] = 0x01;
	buf[11] = 0x08;
	udpConnection.Send(buf, 12);
}

//--------------------------------------------------------------
