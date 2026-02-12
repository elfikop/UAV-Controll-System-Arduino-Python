	This is a short brief of the information included in the "UAV software project (non autonomus).vpp" file.
I Recomend reading this file as the project is not finished and the vpp project might be messy or missleading.

The main purpose of the project is to establish an uav software, both for the ground station and the vessel.
Components:
	The airframe was chosen to be an "FT Versa Wing" made by FliteTest with an arduino nano r4 and nrf24l01 radio module.
	The ground station contains a PC with a Python software and Arduino UNO R3 with nrf radio module
	The  controller is the Esperanza Gamepad z wibracją do PC PS2 PS3 EG106 USB CORSAIR
	2 9g Servos.
	A2212 1000/1400KV motor.
	8/9 inch proppellers.
	1800mAh DualSky lipo Battery
	Total weight of the vessel is appropox 650g.
	

Functionality brief (As far as its developed:
	The ground station is capable of reading, validating the controller data and sending it thru the serial port
	to the Arduino UNO r3 module with nrf24l01 radio.
	Arduino UNO r3 software sents the packets of data, containg the thrust, left and right Elevon angles to the nano R4 on the vessel.
	if the data are valid, the nano module converts them to the valid data for controll of esc and sevos and controlls the UAV.
	It is worh to add that the ground station has various modes to configure and adjust changes to the data sent by the controller and
	then received by the UAV (elevons default configuration mode) as well as some flight assistance modes ( such as throttle changing 
	by using arrows instead of left analog)

The folder named "old" contains some codes that were needed to test the functionality of some components such as esc and nrf radio, as well
as the aproaches that turned out to fail ( connecting ps2 controller and nrf radio on the same SPI bus.

Future functionality plans:
	-Attaching the an I2C gyroscope to the UAV and implementing flight assist mode.
	-Attaching an analog thermometter to the UAV and collecting the wether data of the flight.
	if I switch to the "Blunt nose FT Versa Wing" vessel with additional modifications that will increase the wingspan to 150cm
		-Attaching 3dprinted retrackable landing gear operated by 3 5g servos
