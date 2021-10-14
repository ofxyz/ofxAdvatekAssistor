#include "ofxAdvatekAssistor.h"

const char* ofxAdvatekAssistor::RGBW_Order[24] = {
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

const char* ofxAdvatekAssistor::DriverTypes[3] = {
		"RGB only",
		"RGBW only",
		"Either RGB or RGBW"
};

const char* ofxAdvatekAssistor::DriverSpeeds[5] = {
	"N/A (single fixed speed)",
	"Slow only (data only driver, fixed at the slow speed)",
	"Fast only (data only driver, fixed at the fast speed)",
	"Either slow or fast (selectable between 2 speeds)",
	"Adjustable clock speed"
};

const char* ofxAdvatekAssistor::DriverSpeedsMhz[12] = {
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

const char* ofxAdvatekAssistor::TestModes[9] = {
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
	poll();
}

void ofxAdvatekAssistor::connect() {
	udpConnection.Create();
	udpConnection.Connect("255.255.255.255", 49150);
	udpConnection.Bind(49150);
	udpConnection.SetEnableBroadcast(true);
	udpConnection.SetNonBlocking(true);
}


//--------------------------------------------------------------

void ofxAdvatekAssistor::updateDevice(int d) {
	std::cout << "Updating Device: " << d << std::endl;
}

void ofxAdvatekAssistor::update() {

    const int packetSize = 100000;
	char udpMessage[packetSize];
	memset(udpMessage, 0, sizeof(udpMessage));
	int newMessage = udpConnection.Receive(udpMessage, packetSize); //returns number of bytes received                                                                                                             
	//std::cout << "Packet Size: " << newMessage << std::endl;
	//std::cout << "UDP Packet: " << std::endl;

	if (newMessage > 1) { // we have data
		
		char ID[9];
		uint8_t* data = (uint8_t*)udpMessage;
		memcpy(ID, data, sizeof(uint8_t) * 9);
		data += 9;
		//std::cout << "ID: " << ID << std::endl;
		
		string sid;
		for (int i(0); i < 8; i++) { sid += ID[i]; }
		if( sid.compare("Advatech") != 0) return;
		
		//--------
		
		uint16_t OpCodes;
		memcpy(&OpCodes, data, sizeof(uint16_t));
		data += 2;
		// Swap bytes
		OpCodes = ((OpCodes & 0xff) << 8) | ((OpCodes & 0xff00) >> 8);
		//std::cout << "messageType: " << OpCodes << std::endl;

		// Only process OpPollReply
		if (OpCodes != 2) return;

		sAdvatekDevice * rec_data = new sAdvatekDevice();

		//--------

		//uint8_t ProtVer;
		memcpy(&rec_data->ProtVer, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "ProtVer: " << (int)rec_data->ProtVer << std::endl;
		
		//--------

		//uint8_t CurrentProtVer;
		memcpy(&rec_data->CurrentProtVer, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "CurrentProtVer: " << (int)rec_data->CurrentProtVer << std::endl;

		//--------

		//uint8_t MAC[6];
		memcpy(rec_data->Mac, data, 6);
		data += 6;

		std::stringstream ss;
		ss << std::hex << std::setfill('0');

		for (int i(0); i < 6; i++) {
			ss << std::hex << std::setw(2) << static_cast<int>(rec_data->Mac[i]);
			if(i < 5) ss << ":";
		}

		string macAddress = ss.str();
		//std::cout << "MAC Address: " << macAddress << std::endl;

		//--------

		//uint8_t LENGTH_MODEL;
		memcpy(&rec_data->ModelLength, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "LENGHT_MODEL: " << (int)LENGTH_MODEL << std::endl;

		//uint8_t* MODEL_NAME = new uint8_t[LENGTH_MODEL];
		rec_data->Model = new uint8_t[rec_data->ModelLength];
		memcpy(rec_data->Model, data, sizeof(uint8_t)*rec_data->ModelLength);
		data += rec_data->ModelLength;
		//std::cout << "MODEL_NAME: " << rec_data->Model << std::endl;

		//--------

		//uint8_t HwRev;
		memcpy(&rec_data->HwRev, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "HwRev: " << "V" << ((float)rec_data->HwRev / 10.f) << std::endl;

		//--------

		//uint8_t MinAssistantVer[3];
		memcpy(rec_data->MinAssistantVer, data, sizeof(uint8_t) * 3);
		data += 3;

		//std::cout << "MinAssistantVer: " << "V" << (int)rec_data->MinAssistantVer[0] << "." << (int)rec_data->MinAssistantVer[1] << "." << (int)rec_data->MinAssistantVer[2] << std::endl;

		//--------

		//uint8_t LENGTH_FW;
		memcpy(&rec_data->LenFirmware, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "LENGTH_FW: " << (int)LENGTH_FW << std::endl;

		//uint8_t* FirmWare = new uint8_t[rec_data->LenFirmware];
		rec_data->Firmware = new uint8_t[rec_data->LenFirmware];
		memcpy(rec_data->Firmware, data, sizeof(uint8_t)*rec_data->LenFirmware);
		data += rec_data->LenFirmware;
		//std::cout << "FirmWare: " << rec_data->Firmware << std::endl;
		
		//--------
		// BRAND: OEM Code, not needed
		memcpy(&rec_data->Brand, data, sizeof(uint8_t));
		data += 1;

		//--------

		//uint8_t CurrentIP[4];
		memcpy(rec_data->CurrentIP, data, sizeof(uint8_t) * 4);
		data += 4;
		
		//std::cout << "CurrentIP: " << (int)rec_data->CurrentIP[0] << "." << (int)rec_data->CurrentIP[1] << "." << (int)rec_data->CurrentIP[2] << "." << (int)rec_data->CurrentIP[3] << std::endl;

		//--------

		//uint8_t CurrentSM[4];
		memcpy(rec_data->CurrentSM, data, sizeof(uint8_t) * 4);
		data += 4;

		//std::cout << "CurrentSM: " << (int)rec_data->CurrentSM[0] << "." << (int)rec_data->CurrentSM[1] << "." << (int)rec_data->CurrentSM[2] << "." << (int)rec_data->CurrentSM[3] << std::endl;

		//--------

		//uint8_t DHCP;
		memcpy(&rec_data->DHCP, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "DHCP: " << ((rec_data->DHCP)?"On":"Off") << std::endl;

		//--------

		//uint8_t StaticIP[4];
		memcpy(rec_data->StaticIP, data, sizeof(uint8_t) * 4);
		data += 4;

		//std::cout << "StaticIP: " << (int)rec_data->StaticIP[0] << "." << (int)rec_data->StaticIP[1] << "." << (int)rec_data->StaticIP[2] << "." << (int)rec_data->StaticIP[3] << std::endl;

		//--------

		//uint8_t StaticSM[4];
		memcpy(rec_data->StaticSM, data, sizeof(uint8_t) * 4);
		data += 4;

		//std::cout << "StaticSM: " << (int)rec_data->StaticSM[0] << "." << (int)rec_data->StaticSM[1] << "." << (int)rec_data->StaticSM[2] << "." << (int)rec_data->StaticSM[3] << std::endl;

		//--------

		//uint8_t Protocol;
		memcpy(&rec_data->Protocol, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "Protocol: " << ((rec_data->Protocol) ? "ArtNet" : "sACN") << std::endl;

		//--------
		//uint8_t holdLastFrame;
		memcpy(&rec_data->HoldLastFrame, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "holdLastFrame: " << ((rec_data->HoldLastFrame) ? "True" : "False") << std::endl;

		//--------
		// SimpleConfig: not needed
		memcpy(&rec_data->SimpleConfig, data, sizeof(uint8_t));
		data += 1;
		
		//--------
		
		//uint16_t maxPixPerOutput;
		memcpy(&rec_data->MaxPixPerOutput, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		rec_data->MaxPixPerOutput = ((rec_data->MaxPixPerOutput & 0xff) << 8) | ((rec_data->MaxPixPerOutput & 0xff00) >> 8);

		//std::cout << "MaxPixPerOutput: " << (int)rec_data->MaxPixPerOutput << std::endl;

		//--------

		//uint8_t NUM_OUTPUTS;
		memcpy(&rec_data->NumOutputs, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_OUTPUTS: " << (int)NUM_OUTPUTS << std::endl;

		//--------

		rec_data->OutputPixels = new uint16_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputPixels, data, sizeof(uint16_t)*rec_data->NumOutputs);
		data += (rec_data->NumOutputs *2);

		//std::cout << "OutputPixels: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	rec_data->OutputPixels[i] = ((rec_data->OutputPixels[i] & 0xff) << 8) | ((rec_data->OutputPixels[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->OutputPixels[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		rec_data->OutputUniv = new uint16_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputUniv, data, sizeof(uint16_t)*rec_data->NumOutputs);
		data += (rec_data->NumOutputs * 2);

		//std::cout << "OutputUniverse: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	rec_data->OutputUniv[i] = ((rec_data->OutputUniv[i] & 0xff) << 8) | ((rec_data->OutputUniv[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->OutputUniv[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		rec_data->OutputChan = new uint16_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputChan, data, sizeof(uint16_t)*rec_data->NumOutputs);
		data += (rec_data->NumOutputs * 2);

		//std::cout << "OutputChannel: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	rec_data->OutputChan[i] = ((rec_data->OutputChan[i] & 0xff) << 8) | ((rec_data->OutputChan[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->OutputChan[i] << " ";
		//}
		//std::cout << std::endl;

		//---------
		
		rec_data->OutputNull = new uint8_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputNull, data, sizeof(uint8_t)*rec_data->NumOutputs);
		data += rec_data->NumOutputs;
		
		//std::cout << "OutputNull: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	std::cout << i << ":" << (int)rec_data->OutputNull[i] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->OutputZig = new uint16_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputZig, data, sizeof(uint16_t)*rec_data->NumOutputs);
		data += (rec_data->NumOutputs * 2);

		//std::cout << "OutputZigZag: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	rec_data->OutputZig[i] = ((rec_data->OutputZig[i] & 0xff) << 8) | ((rec_data->OutputZig[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->OutputZig[i] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->OutputReverse = new uint8_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputReverse, data, sizeof(uint8_t)*rec_data->NumOutputs);
		data += rec_data->NumOutputs;

		//std::cout << "OutputReverse: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	std::cout << i << ":" << (((int)rec_data->OutputReverse[i])?"True":"False") << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->OutputColOrder = new uint8_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputColOrder, data, sizeof(uint8_t)*rec_data->NumOutputs);
		data += rec_data->NumOutputs;

		//std::cout << "OutputColOrder: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	std::cout << i << ":" << RGBW_Order[(int)rec_data->OutputColOrder[i]] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->OutputGrouping = new uint16_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputGrouping, data, sizeof(uint16_t)*rec_data->NumOutputs);
		data += (rec_data->NumOutputs * 2);

		//std::cout << "OutputGrouping: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	rec_data->OutputGrouping[i] = ((rec_data->OutputGrouping[i] & 0xff) << 8) | ((rec_data->OutputGrouping[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->OutputGrouping[i] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->OutputBrightness = new uint8_t[rec_data->NumOutputs];
		memcpy(rec_data->OutputBrightness, data, sizeof(uint8_t)*rec_data->NumOutputs);
		data += rec_data->NumOutputs;

		//std::cout << "OutputBrightness: ";
		//for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		//	std::cout << i << ":" << (int)rec_data->OutputBrightness[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		//uint8_t NUM_DMX_OUT;
		memcpy(&rec_data->NumDMXOutputs, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_DMX_OUT: " << (int)NUM_DMX_OUT << std::endl;

		//--------

		//uint8_t ProtocolsOnDmxOut;
		memcpy(&rec_data->ProtocolsOnDmxOut, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "ProtocolsOnDmxOut: " << ((rec_data->ProtocolsOnDmxOut && 0x01) ? "ArtNet" : "") << ((rec_data->ProtocolsOnDmxOut<<1 && 0x01) ? ", sACN" : "") << std::endl;

		//---------

		rec_data->DmxOutOn = new uint8_t[rec_data->NumDMXOutputs];
		memcpy(rec_data->DmxOutOn, data, sizeof(uint8_t)*rec_data->NumDMXOutputs);
		data += rec_data->NumDMXOutputs;

		//std::cout << "DmxOutOn: ";
		//for (int i(0); i < (int)rec_data->NumDMXOutputs; i++) {
		//	std::cout << i << ":" << (((int)rec_data->DmxOutOn[i]) ? "On" : "Off") << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->DmxOutUniv = new uint16_t[rec_data->NumDMXOutputs];
		memcpy(rec_data->DmxOutUniv, data, sizeof(uint16_t)*rec_data->NumDMXOutputs);
		data += (rec_data->NumDMXOutputs *2);

		//std::cout << "DmxOutUniv: ";
		//for (int i(0); i < (int)rec_data->NumDMXOutputs; i++) {
		//	rec_data->DmxOutUniv[i] = ((rec_data->DmxOutUniv[i] & 0xff) << 8) | ((rec_data->DmxOutUniv[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (int)rec_data->DmxOutUniv[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		memcpy(&rec_data->NumDrivers, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_DRIVERS: " << (int)rec_data->NumDrivers << std::endl;

		//--------

		memcpy(&rec_data->DriverNameLength, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "DRIVER_NAME_LENGTH: " << (int)rec_data->DriverNameLength << std::endl;

		//---------

		rec_data->DriverType = new uint8_t[rec_data->NumDrivers];
		memcpy(rec_data->DriverType, data, sizeof(uint8_t)*rec_data->NumDrivers);
		data += rec_data->NumDrivers; // Mmmm, what if it is 0?

		//std::cout << "DriverType: ";
		//for (int i(0); i < (int)rec_data->NumDrivers; i++) {
		//	std::cout << i << ":" << DriverTypes[(int)rec_data->DriverType[i]] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->DriverSpeed = new uint8_t[rec_data->NumDrivers];
		memcpy(rec_data->DriverSpeed, data, sizeof(uint8_t)*rec_data->NumDrivers);
		data += rec_data->NumDrivers;

		//std::cout << "DriverSpeed: ";
		//for (int i(0); i < (int)rec_data->NumDrivers; i++) {
		//	std::cout << i << ":" << DriverSpeeds[(int)rec_data->DriverSpeed[i]] << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->DriverExpandable = new uint8_t[rec_data->NumDrivers];
		memcpy(rec_data->DriverExpandable, data, sizeof(uint8_t)*rec_data->NumDrivers);
		data += rec_data->NumDrivers;

		//std::cout << "DriverExpandable: ";
		//for (int i(0); i < (int)rec_data->NumDrivers; i++) {
		//	std::cout << i << ":" << (((int)rec_data->DriverExpandable[i]) ? "Capable of expanded mode" : "Normal mode only") << " ";
		//}
		//std::cout << std::endl;

		//---------

		rec_data->DriverNames = new uint8_t*[rec_data->NumDrivers];

		for (int i = 0; i < rec_data->NumDrivers; i++) {
			rec_data->DriverNames[i] = new uint8_t[rec_data->DriverNameLength +1];
			memset(rec_data->DriverNames[i], 0, sizeof(uint8_t)*rec_data->DriverNameLength +1);
			memcpy(rec_data->DriverNames[i], data, sizeof(uint8_t)*rec_data->DriverNameLength);
			data += rec_data->DriverNameLength;
		}
	
		//std::cout << "DriverNames: ";
		//for (int i(0); i < (int)rec_data->NumDrivers; i++) {
		//	std::cout << i << ":" << rec_data->DriverNames[i] << " ";
		//}
		//std::cout << std::endl;

		//--------

		//uint8_t CurrentDriver;
		memcpy(&rec_data->CurrentDriver, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "CurrentDriver: " << rec_data->DriverNames[(int)rec_data->CurrentDriver] << std::endl;

		//---------

		//uint8_t CurrentDriverType;
		memcpy(&rec_data->CurrentDriverType, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "CurrentDriverType: " << (((int)rec_data->CurrentDriverType)? "RGBW" : "RGB") << std::endl;

		//---------

		//uint8_t CurrentDriverSpeed;
		memcpy(&rec_data->CurrentDriverSpeed, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "CurrentDriverSpeed: " << DriverSpeedsMhz[(int)rec_data->CurrentDriverSpeed] << std::endl;

		//---------

		//uint8_t CurrentDriverExpanded;
		memcpy(&rec_data->CurrentDriverExpanded, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "CurrentDriverExpanded: " << (((int)rec_data->CurrentDriverType) ? "Expanded / Condensed mode)" : "Normal mode") << std::endl;

		//---------

		//uint8_t Gamma[4];
		memcpy(rec_data->Gamma, data, sizeof(uint8_t) * 4);
		data += 4;

		//std::cout << "Gamma: R" << (float)rec_data->Gamma[0] * 0.1 << " G" << (float)rec_data->Gamma[1] * 0.1 << " B" << (float)rec_data->Gamma[2] * 0.1 << " W" << (float)rec_data->Gamma[3] * 0.1 << std::endl;

		//---------

		//uint8_t Nickname[40];
		memcpy(rec_data->Nickname, data, sizeof(uint8_t) * 40);
		data += 40;

		//std::cout << "Nickname: " << rec_data->Nickname << std::endl;

		//--------

		//uint16_t Temperature;
		memcpy(&rec_data->Temperature, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		rec_data->Temperature = ((rec_data->Temperature & 0xff) << 8) | ((rec_data->Temperature & 0xff00) >> 8);

		//std::cout << "Temperature: " << (float)rec_data->Temperature*0.1 << std::endl;

		//---------

		//uint8_t MaxTargetTemp;
		memcpy(&rec_data->MaxTargetTemp, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "MaxTargetTemp: " << (float)rec_data->MaxTargetTemp*0.1 << std::endl;

		//---------

		//uint8_t NUM_BANKS;
		memcpy(&rec_data->NumBanks, data, sizeof(uint8_t));
		data += 1;
		//std::cout << "NUM_BANKS: " << (int)NUM_BANKS << std::endl;

		//---------

		rec_data->VoltageBanks = new uint16_t[rec_data->NumBanks];
		memcpy(rec_data->VoltageBanks, data, sizeof(uint16_t)*rec_data->NumBanks);
		data += (rec_data->NumBanks * 2);

		//std::cout << "VoltageBanks: ";
		//for (int i(0); i < (int)rec_data->NumBanks; i++) {
		//	rec_data->VoltageBanks[i] = ((rec_data->VoltageBanks[i] & 0xff) << 8) | ((rec_data->VoltageBanks[i] & 0xff00) >> 8);
		//	std::cout << i << ":" << (float)rec_data->VoltageBanks[i]*0.1 << " ";
		//}
		//std::cout << std::endl;

		//---------

		//uint8_t TestMode;
		memcpy(&rec_data->TestMode, data, sizeof(uint8_t));
		data += 1;
		
		//std::cout << "TestMode: " << TestModes[(int)rec_data->TestMode] << std::endl;

		//---------

		//uint8_t TestCols[4];
		memcpy(rec_data->TestCols, data, sizeof(uint8_t) * 4);
		data += 4;

		//std::cout << "TestColor: R" << (int)rec_data->TestCols[0] << " G" << (int)rec_data->TestCols[1] << " B" << (int)rec_data->TestCols[2] << " W" << (int)rec_data->TestCols[3] << std::endl;

		//---------

		//uint8_t TestOutputNum;
		memcpy(&rec_data->TestOutputNum, data, sizeof(uint8_t));
		data += 1;

		//std::cout << "TestOutputNum: " << (int)rec_data->TestOutputNum << std::endl;

		//--------

		//uint16_t TestPixelNum;
		memcpy(&rec_data->TestPixelNum, data, sizeof(uint16_t));
		data += 2;

		// Swap bytes
		rec_data->TestPixelNum = ((rec_data->TestPixelNum & 0xff) << 8) | ((rec_data->TestPixelNum & 0xff00) >> 8);

		//std::cout << "TestPixelNum: " << (int)rec_data->TestPixelNum << std::endl;

		//---------
		std::stringstream Title;
		Title << rec_data->Model <<  "	" << rec_data->Firmware << "	" << addressString(rec_data->CurrentIP) << "		" << "Temp: " << (float)rec_data->Temperature*0.1 << "		" << rec_data->Nickname;
		rec_data->Title = Title.str();

		if (!deviceExist(rec_data->Mac)) devices.emplace_back(rec_data);

	}
}

//--------------------------------------------------------------
void ofxAdvatekAssistor::poll() {
	connect();

	for (auto device : devices)
	{
		if (device)
		{
			delete device->Model;
			delete device->Firmware;
			delete[] device->OutputPixels;
			delete[] device->OutputUniv;
			delete[] device->OutputChan;
			delete[] device->OutputNull;
			delete[] device->OutputZig;
			delete[] device->OutputReverse;
			delete[] device->OutputColOrder;
			delete[] device->OutputGrouping;
			delete[] device->OutputBrightness;
			delete[] device->DmxOutOn;
			delete[] device->DmxOutUniv;
			delete[] device->DriverType;
			delete[] device->DriverSpeed;
			delete[] device->DriverExpandable;
			delete[] device->DriverNames;
			delete[] device->VoltageBanks;
			delete device;
		}
	}

	devices.clear();

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
vector<T_AdvatekDevice*>& ofxAdvatekAssistor::getDevices() {
	return devices;
}

//--------------------------------------------------------------

bool ofxAdvatekAssistor::deviceExist(uint8_t * Mac) {

	for (int d(0); d < devices.size(); d++) {
		bool exist = true;
		for (int i(0); i < 6; i++) {
			if (devices[d]->Mac[i] != Mac[i]) exist = false;
		}
		if (exist) return true;
	}

	return false;
}

string ofxAdvatekAssistor::addressString(uint8_t * address) {
	std::stringstream ss;

	for (int i(0); i < 4; i++) {
		ss << static_cast<int>(address[i]);
		if (i < 3) ss << ".";
	}

	return ss.str();
}