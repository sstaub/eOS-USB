![](under_construction.gif)

# eOS for USB
An object orientated library for Arduino to control ETCs EOS Family Consoles with OSC over USB (an Ethernet version will follow later). The goal of the library is to have a smart toolbox to create your own boxes which covers your needing by endless combinations of hardware elements.

The library depends on the OSC library from CNMAT https://github.com/CNMAT/OSC

This library and examples are a replacment for the #lighthack project.
It is written for the Arduino UNO and MEGA board which are nativly supported by ETC. Support for other microcontroller boards like the popular Teensy is planned for future by ETC.

The library support hardware elements like encoders, fader, buttons with some helper functions.

The library allows you to use hardware elements as an object and with the use of the helper functions, code becomes much easier to write and read and to understand. Also refer to the EOS manual for more information.

For use with PlatformIO https://platformio.org, as a recommanded IDE with MS VSCode, there is an extra start example folder called **#lighthack**.

If you have whishes for other functions or classes make an issue. If you find bugs also, nobody is perfect.

## Installation
1. Download from Releases
2. Follow the instruction on the Arduino website https://www.arduino.cc/en/Guide/Libraries
You can import the .zip file from the IDE with *Sketch / Include Library / Add .ZIP Library...*
3. For PlatformIO Unzip and move the folder to the lib folder of your project.

## Examples

## box1
It is based on the original box1, no extras

## box2A
Is a modifcation of the original box1 and uses the former Next and Last buttons to step thru a parameter list.

Because of the limited RAM of the Arduino UNO, the parameter list is limited and depends on the size of the parameter names.
On an Arduino UNO the maximum is 14 parameter names.

On an Arduino MEGA more than 14 parameters can be used, also on other boards like Teensy 3.x or 4.

For Teensy you need to fake the PID/VID of the USB connection to work with a console, have a look to the forum.

Added a keyword „none“ for gaps in the parameter list, former titeled as „empty“.

## box2B
This box uses 6 buttons for Next, Last, SelectLast, Shift and Parameter Up/Down. It also uses the buttons of the encoder for posting the Home position. So, all Pins of an Arduino UNO are used.

## faderwing
Is a small faderwing with 6 faders and 12 buttons for Stop / Go

## macrobox
Is a box with 12 buttons to fire Macros, actual 101 thru 112 but this can easily changed.

## #lighthack
Is a project folder for use with PlatformIO and includes the box1 code as a starting point.

## Helper functions

### Filters

```
void filter(String pattern);
```
With a Filter you can get messages from the console which you can use for proceeding  informations.

### Parameter Subscriptions

```
void subscribe(String parameter);
void unSubscribe(String parameter); // unsubscribe a parameter
```
With subscribtions you can get special informations about dedicated parameters.

### Ping

```
void ping(); // send a ping without a message
void ping(String message); // send a ping with a message 
```

With a ping you can get a reaction from the console which helps you to indentify your box and if is alive. You should send a ping regulary with message to identify your box on the console.


### Command Line

```
void command(String cmd); // send a command
void newCommand(String newCmd); // clears cmd line before applyieng
```
You can send a string to the command line.

### Users

```
void user(int16_t userID);
```

This function allows you to change the user ID e.g. 
- **0** is a background user
- **-1** is the current user
- or any other user

### Shift button

```
void shiftButton(uint8_t pin);
```
This function allows you to assign a hardware button as a **Shift** button. **Shift** set the encoder and wheel messages to the **Fine** mode or for opposite the acceleration of the **Intens** parameter. It can replaced by using the optional encoder buttons as a **Shift** function.

### Init faders

```
void initFaders(uint8_t page = 1, uint8_t faders = 10, uint8_t bank = 1);
```
The **initFader()** function is basic configuration and must use before you can use your Fader objects.
- **page** the fader page on your console
- **fader** is the number of fader on you console page
- **bank** is virtuell OSC fader bank

**initFaders();** without a parameter gives you a configuration for use your faders on page 1 of your console 

## Classes

### Encoder
The Encoder class creates an encoder object which allows to control parameters:
```
Encoder(uint8_t pinA, uint8_t pinB, uint8_t direction);
```
- **pinA** and **pinB** are the connection Pins for the encoder hardware
- **direction** is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD or REVERSE
```
// Example, this should done before the setup()

Encoder encoder1(A0, A1, REVERSE);
```
If the Encoder have an extro button build in, you can add it with following class member:
```
void button(uint8_t buttonPin, ButtonMode buttonMode = HOME);
```
- **buttonPin** is the pin for the extra button
- **buttonMode** is the function you can assign, following functions are available

	-	HOME this will post a home command
	- FINE this allows you to use the button as a **Shift** function
```
// Example, this must happen in the setup() before assigning a Parameter

encoder1.button(A3, HOME);
```
Before using you must assign a Parameter:
```
void parameter(String param); // set a parameter
String parameter(); // get the parameter name
```
- **param** is the Parameter which you want assign
```
// Example, this must happen in the setup() before assigning an encoder button

encoder1.parameter("Pan");
```
To get the actual encoder state you must call inside the loop():
```
void update();
```
```
// Example, this must happen in the loop() 
encoder1.update();
```

