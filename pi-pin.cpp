//********************************************************
//* pi-pin is program for manipulating Raspberry Pi pins
//* from CLI or shell scripts
//* Author: Slavko Novak slavko.novak.esen@gmail.com
//* Version: see PROG_VERSION define
//* License: GPL
//********************************************************

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstdint>
#include <chrono>
#include <thread>

#define PROG_NAME "pi-pin"
#define PROG_VERSION "v0.2.9"

using namespace std;

class RaspiPins
{
	map<string, string> _pins;
	
	void checkvalidity(const string &pin)
	{
		if(this->_pins.find(pin) == this->_pins.end())
		{
			throw "Pin " + pin + " is not GPIO or it does not exist!";
		}
	}
	
public:
	RaspiPins()
	{
		this->_pins["3"] = "2";
		this->_pins["5"] = "3";
		this->_pins["7"] = "4";
		this->_pins["11"] = "17";
		this->_pins["12"] = "18";
		this->_pins["13"] = "27";
		this->_pins["15"] = "22";
		this->_pins["16"] = "23";
		this->_pins["18"] = "24";
		this->_pins["19"] = "10";
		this->_pins["21"] = "9";
		this->_pins["22"] = "25";
		this->_pins["23"] = "11";
		this->_pins["24"] = "8";
		this->_pins["26"] = "7";
		this->_pins["29"] = "5";
		this->_pins["31"] = "6";
		this->_pins["32"] = "12";
		this->_pins["33"] = "13";
		this->_pins["35"] = "19";
		this->_pins["36"] = "16";
		this->_pins["37"] = "26";
		this->_pins["38"] = "20";
		this->_pins["40"] = "21";
	}

	~RaspiPins()
	{
		
	}
	
	void setPinModeInput(const string &pin)
	{
		checkvalidity(pin);
		
		ofstream fout;
		fout.open("/sys/class/gpio/export");
		fout << this->_pins[pin];
		fout.close();
		
		fout.open(("/sys/class/gpio/gpio" + this->_pins[pin] + "/direction").c_str());
		fout << "in";
		fout.close();		
	}
	
	void setPinModeOutput(const string &pin)
	{
		checkvalidity(pin);
		
		ofstream fout;
		fout.open("/sys/class/gpio/export");
		fout << this->_pins[pin];
		fout.close();
		
		fout.open(("/sys/class/gpio/gpio" + this->_pins[pin] + "/direction").c_str());
		fout << "out";
		fout.close();
	}
	
	void setPin(const string &pin, bool value)
	{
		checkvalidity(pin);
		
		ofstream fout;
		fout.open(("/sys/class/gpio/gpio" + this->_pins[pin] + "/value").c_str());
		fout << value ? "1" : "0";
		fout.close();
	}
		
	bool getPin(const string &pin)
	{
		checkvalidity(pin);
		
		string value;

		ifstream fin;
		fin.open(("/sys/class/gpio/gpio" + this->_pins[pin] + "/value").c_str());
		fin >> value;
		fin.close();
		
		return "1" == value;
	}
	
	void pwmPin(const string &pin, uint8_t value)
	{
		const uint64_t period = 2000;
		double onTime = period / 255 * value;
		
		for(;;)
		{			
			setPin(pin, true);
			this_thread::sleep_for(chrono::microseconds(static_cast<uint64_t>(onTime)));
			
			setPin(pin, false);
			this_thread::sleep_for(chrono::microseconds(period - static_cast<uint64_t>(onTime))); 
		}
	}
	
	void releasePin(const string &pin)
	{
		checkvalidity(pin);
		
		ofstream fout;
		fout.open("/sys/class/gpio/unexport");
		fout << this->_pins[pin];
		fout.close();
	}
};

class PinCommands
{
public:
	static void setPin(const string &pin, bool value)
	{
		RaspiPins pins;
		pins.setPinModeOutput(pin);
		pins.setPin(pin, value);
	}
	
	static bool getPin(const string &pin)
	{
		RaspiPins pins;
		pins.setPinModeInput(pin);
		return pins.getPin(pin);
	}

	static void togglePin(const string &pin)
	{
		RaspiPins pins;
		pins.setPin(pin, !pins.getPin(pin));	
	}
	
	static void pwmPin(const string &pin, uint8_t value)
	{
		RaspiPins pins;
		pins.pwmPin(pin, value);
	}
	
	static void releasePin(const string &pin)
	{
		RaspiPins pins;
		pins.releasePin(pin);
	}
};

void usage()
{
	cout << endl << PROG_NAME << " " << PROG_VERSION
		 << " by slavko.novak.esen@gmail.com" << endl << endl
		 << "USAGE: " << PROG_NAME << " pin command [PWM value (0-255)]" << endl << endl
		 << "command options:" << endl
		 << "\ts0\t-\tset pin LOW" << endl
		 << "\ts1\t-\tset pin HIGH" << endl
		 << "\tt\t-\ttoggle pin" << endl
		 << "\tp\t-\tPWM" << endl
		 << "\tg\t-\tget pin value" << endl
		 << "\tr\t-\trelease pin" << endl << endl;
}

int main(int argc, char **argv)
{
	try
	{
		string pin = "7";
		if(argc > 2)
		{
			pin = argv[1];
		}
		else
		{
			usage();
			return 1;
		}
		
		string command = argv[2];
		
		if("s0" == command)
		{
			PinCommands::setPin(pin, false);
		}
		else if("s1" == command)
		{
			PinCommands::setPin(pin, true);
		}
		else if("t" == command)
		{
			PinCommands::togglePin(pin);
		}
		else if("p" == command)
		{
			if(argc < 4) throw string("For PWM command we need PWM value!");
			uint8_t value = atoi(argv[3]);
			
			PinCommands::pwmPin(pin, value);
		}
		else if("g" == command)
		{
			cout << PinCommands::getPin(pin) << endl;
		}
		else if("r" == command)
		{
			PinCommands::releasePin(pin);
		}
		else
		{
			usage();
			cerr << endl << "INVALID COMMAND: " << command << endl;
			return 2;
		}
	}
	catch(const string &ex)
	{
		cerr << "ERROR: " << ex << endl;
		return 3;
	}
		
	return 0;
}
