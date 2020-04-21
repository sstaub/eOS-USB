#include "eOS.h"

#ifdef BOARD_HAS_USB_SERIAL
	#include <SLIPEncodedUSBSerial.h>
	SLIPEncodedUSBSerial serialSLIP(thisBoardsSerialUSB);
#else
	#include <SLIPEncodedSerial.h>
	SLIPEncodedSerial serialSLIP(Serial);
#endif

void filter(String pattern) {
	OSCMessage filter("/eos/filter/add");
	filter.add(pattern.c_str());
	serialSLIP.beginPacket();
	filter.send(serialSLIP);
	serialSLIP.endPacket();
	}

void subscribe(String parameter) {
	String subPattern = "/eos/subscribe/param/" + parameter;
	OSCMessage sub(subPattern.c_str());
	sub.add(SUBSCRIBE);
	serialSLIP.beginPacket();
	sub.send(serialSLIP);
	serialSLIP.endPacket();
	}

void unSubscribe(String parameter) {
	String subPattern = "/eos/subscribe/param/" + parameter;
	OSCMessage sub(subPattern.c_str());
	sub.add(UNSUBSCRIBE);
	serialSLIP.beginPacket();
	sub.send(serialSLIP);
	serialSLIP.endPacket();
	}

void ping() {
	OSCMessage ping("/eos/ping");
	serialSLIP.beginPacket();
	ping.send(serialSLIP);
	serialSLIP.endPacket();
	}

void ping(String message) {
	OSCMessage ping("/eos/ping");
	ping.add(message.c_str());
	serialSLIP.beginPacket();
	ping.send(serialSLIP);
	serialSLIP.endPacket();
	}

void command(String cmd) {
	OSCMessage cmdPattern("/eos/cmd");
	cmdPattern.add(cmd.c_str());
	serialSLIP.beginPacket();
	cmdPattern.send(serialSLIP);
	serialSLIP.endPacket();
	}

void newCommand(String newCmd) {
	OSCMessage newCmdPattern("/eos/newcmd");
	newCmdPattern.add(newCmd.c_str());
	serialSLIP.beginPacket();
	newCmdPattern.send(serialSLIP);
	serialSLIP.endPacket();
	}

void user(int16_t userID) {
	OSCMessage userPattern("/eos/user");
	userPattern.add(userID);
	serialSLIP.beginPacket();
	userPattern.send(serialSLIP);
	serialSLIP.endPacket();
	}

Key::Key(uint8_t pin, String keyName) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	keyPattern = "/eos/key/" + keyName;	
	}

void Key::update() {
	if((digitalRead(pin)) != last) {
		OSCMessage keyUpdate(keyPattern.c_str());
		if (last == LOW) {
			last = HIGH;
			keyUpdate.add(EDGE_UP);
			}
		else {
			last = LOW;
			keyUpdate.add(EDGE_DOWN);
			}
		serialSLIP.beginPacket();
    keyUpdate.send(serialSLIP);
    serialSLIP.endPacket();
		}
	}

uint8_t shiftPin;

void shiftButton(uint8_t pin) {
	if (pin) {
		pinMode(pin, INPUT_PULLUP);
		shiftPin = pin;
		}
	}

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	}

void Encoder::button(uint8_t buttonPin, ButtonMode buttonMode) {
	if (buttonPin) {
		this->buttonPin = buttonPin;
		this->buttonMode = buttonMode;
		pinMode(buttonPin, INPUT_PULLUP);
		}
	}

void Encoder::parameter(String param) {
	this->param = param;
	}

String Encoder::parameter() {
	return param;
	}

void Encoder::update() {
	encoderMotion = 0;
	pinACurrent = digitalRead(pinA);	
	if ((pinALast) && (!pinACurrent)) {
		if (digitalRead(pinB)) {
			encoderMotion = - 1;
			}
		else {
			encoderMotion = 1;
			}
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;
	if (encoderMotion != 0) {
		String wheelMsg = "/eos/wheel";

		if (shiftPin) {
			if (param == "Intens" || param == "intens") {
				if (digitalRead(shiftPin) == LOW) encoderMotion *= INT_ACC;
				}
			else if (digitalRead(shiftPin) == LOW) wheelMsg += "/fine";
			}
		
		if (buttonPin && (buttonMode == FINE)) {
			if (param == "Intens" || param == "intens") {
				if (digitalRead(buttonPin) == LOW) encoderMotion *= INT_ACC;
				}
			else if (digitalRead(buttonPin) == LOW) wheelMsg += "/fine";
			}

		encoderMotion *= WHEEL_ACC;
		wheelMsg += '/' + param;
		OSCMessage wheelUpdate(wheelMsg.c_str());
		wheelUpdate.add(encoderMotion);
		serialSLIP.beginPacket();
		wheelUpdate.send(serialSLIP);
		serialSLIP.endPacket();
		}
	
	if (buttonPin) {
		if (buttonMode == HOME) {
			if((digitalRead(buttonPin)) != buttonPinLast) {
				OSCMessage buttonUpdate(("/eos/param/" + param + "/home").c_str());
				if(buttonPinLast == LOW) {
					buttonPinLast = HIGH;
					buttonUpdate.add(EDGE_UP);
					}
				else {
					buttonPinLast = LOW;
					buttonUpdate.add(EDGE_DOWN);
					}
				serialSLIP.beginPacket();
				buttonUpdate.send(serialSLIP);
				serialSLIP.endPacket();
				}	
			}
		}
	}

Wheel::Wheel(uint8_t pinA, uint8_t pinB, uint8_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	}

void Wheel::button(uint8_t buttonPin, ButtonMode buttonMode) {
	if (buttonPin && (buttonMode == FINE)) {
		this->buttonPin = buttonPin;
		this->buttonMode = buttonMode;
		pinMode(buttonPin, INPUT_PULLUP);
		}
	}

void Wheel::index(uint8_t idx) {
	this->idx = idx;
	}

uint8_t Wheel::index() {
	return idx;
	}

void Wheel::update() {
	encoderMotion = 0;
	pinACurrent = digitalRead(pinA);	
	if ((pinALast) && (!pinACurrent)) {
		if (digitalRead(pinB)) {
			encoderMotion = - 1;
			}
		else {
			encoderMotion = 1;
			}
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;
	if (encoderMotion != 0) {
		String wheelMsg ="/eos/active/wheel";


		if (shiftPin) {
			if (idx == 1) { // we assume idx 1 is the Intes parameter
				if (digitalRead(shiftPin) == LOW) encoderMotion *= INT_ACC;
				}
			else if (digitalRead(shiftPin) == LOW) wheelMsg += "/fine";
			}
		
		if (buttonPin && (buttonMode == FINE)) {
			if (idx == 1) {  // we assume idx 1 is the Intes parameter
				if (digitalRead(buttonPin) == LOW) encoderMotion *= INT_ACC;
				}
			else if (digitalRead(buttonPin) == LOW) wheelMsg += "/fine";
			}
		
		encoderMotion *= WHEEL_ACC;
		wheelMsg += '/' + String(idx);
		OSCMessage wheelUpdate(wheelMsg.c_str());
		wheelUpdate.add(encoderMotion);
		serialSLIP.beginPacket();
		wheelUpdate.send(serialSLIP);
		serialSLIP.endPacket();
		}
	}

Submaster::Submaster(uint8_t analogPin, uint8_t firePin, uint8_t sub) {
	this->analogPin = analogPin;
	this->firePin = firePin;
	analogLast = 0xFFFF; // forces an osc output of the fader
	if (firePin) pinMode(firePin, INPUT_PULLUP);
	fireLast = digitalRead(firePin);
	subPattern = "/eos/sub/" + String(sub);
	updateTime = millis();
	}

void Submaster::update() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analogRead(analogPin) >> 2; // reduce to 8 bit
		int16_t current = THRESHOLD;
		int16_t delta = raw - current;
		if (delta <= -THRESHOLD) current = raw + THRESHOLD;
		if (delta >= THRESHOLD) current = raw - THRESHOLD;
		if (current != analogLast) {
			float value = ((current - THRESHOLD) * 1.0 / (255 - 2 * THRESHOLD)) / 1.0; // normalize to values between 0.0 and 1.0
			analogLast = current;
			OSCMessage faderUpdate(subPattern.c_str());
			faderUpdate.add(value);
			serialSLIP.beginPacket();
			faderUpdate.send(serialSLIP);
			serialSLIP.endPacket();
			}
		updateTime = millis();
		}
	if (firePin) {
		if((digitalRead(firePin)) != fireLast) {
			OSCMessage fireUpdate((subPattern + "/fire").c_str());
			if(fireLast == LOW) {
				fireLast = HIGH;
				fireUpdate.add(EDGE_UP);
				}
			else {
				fireLast = LOW;
				fireUpdate.add(EDGE_DOWN);
				}
			serialSLIP.beginPacket();
			fireUpdate.send(serialSLIP);
			serialSLIP.endPacket();
			}
		}
	}

void initFaders(uint8_t page, uint8_t faders, uint8_t bank) {
	String faderInit = "/eos/fader/";
	faderInit += bank;
	faderInit += "/config/";
	faderInit += page;
	faderInit += '/';
	faderInit += faders;
	OSCMessage faderBank(faderInit.c_str());
	serialSLIP.beginPacket();
	faderBank.send(serialSLIP);
	serialSLIP.endPacket();
	}

Fader::Fader(uint8_t analogPin, uint8_t firePin, uint8_t stopPin, uint8_t fader, uint8_t bank) {
	this->bank = bank;
	this->fader = fader;
	this->analogPin = analogPin;
	this->firePin = firePin;
	this->stopPin = stopPin;
	analogLast = 0xFFFF; // forces an osc output of the fader
	if (firePin) pinMode(firePin, INPUT_PULLUP);
	if (stopPin) pinMode(stopPin, INPUT_PULLUP);
	fireLast = digitalRead(firePin);
	stopLast = digitalRead(stopPin);
	faderPattern = "/eos/fader/" + String(bank) + '/' + String(fader);
	updateTime = millis();
	}

void Fader::update() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analogRead(analogPin) >> 2; // reduce to 8 bit
		int16_t current = THRESHOLD;
		int16_t delta = raw - current;
		if (delta <= -THRESHOLD) current = raw + THRESHOLD;
		if (delta >= THRESHOLD) current = raw - THRESHOLD;
		if (current != analogLast) {
			float value = ((current - THRESHOLD) * 1.0 / (255 - 2 * THRESHOLD)) / 1.0; // normalize to values between 0.0 and 1.0
			analogLast = current;
			OSCMessage faderUpdate(faderPattern.c_str());
			faderUpdate.add(value);
			serialSLIP.beginPacket();
			faderUpdate.send(serialSLIP);
			serialSLIP.endPacket();
			}
		updateTime = millis();
		}	

	if (firePin) {
		if((digitalRead(firePin)) != fireLast) {
			OSCMessage fireUpdate((faderPattern + "/fire").c_str());
			if(fireLast == LOW) {
				fireLast = HIGH;
				fireUpdate.add(EDGE_UP);
				}
			else {
				fireLast = LOW;
				fireUpdate.add(EDGE_DOWN);
				}
			serialSLIP.beginPacket();
			fireUpdate.send(serialSLIP);
			serialSLIP.endPacket();
			}
		}

	if (stopPin) {
		if((digitalRead(stopPin)) != stopLast) {
			OSCMessage stopUpdate((faderPattern + "/stop").c_str());
			if(stopLast == LOW) {
				stopLast = HIGH;
				stopUpdate.add(EDGE_UP);
				}
			else {
				stopLast = LOW;
				stopUpdate.add(EDGE_DOWN);
				}
			serialSLIP.beginPacket();
			stopUpdate.send(serialSLIP);
			serialSLIP.endPacket();
			}
		}
	}


void Fader::faderBank(uint8_t bank) {
	this->bank = bank;
	}

uint8_t Fader::faderBank() {
	return bank;
	}

void Fader::faderNumber(uint8_t fader) {
	this->fader = fader;
	}

uint8_t Fader::faderNumber() {
	return fader;
	}

Macro::Macro(uint8_t pin, uint16_t macro) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	firePattern = "/eos/macro/" + String(macro) + "/fire";
	}

void Macro::update() {
	if ((digitalRead(pin)) != last) {
		OSCMessage fireUpdate(firePattern.c_str());
		if (last == LOW) {
			last = HIGH;
			fireUpdate.add(EDGE_UP);
			}
		else {
			last = LOW;
			fireUpdate.add(EDGE_DOWN);
			}
		serialSLIP.beginPacket();
    fireUpdate.send(serialSLIP);
    serialSLIP.endPacket();
		} 
	}