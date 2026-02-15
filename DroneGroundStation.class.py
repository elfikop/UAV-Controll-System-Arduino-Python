import pygame
import time
import serial

class DroneGroundStation:   
    """
    Main class for the Drone Ground Station responsible for controller input and data transmission.
    
    The class initializes the pygame joystick and serial communication to COM4. 
    It processes stick inputs to calculate elevon positions and throttle values.
    """
    def __init__(self):
        """
        Initializes the ground station, sets default parameters and opens serial port.
        
        Initializes pygame joystick (0) and serial connection at 115200 baud rate.
        Sets default offsets for elevons: rightPar(-21) and leftPar(8).
        """
        self.i=0
        self.pad=None
        self.left=76
        self.right=76
        self.rightPar=-21
        self.leftPar=8
        self.ly=0
        self.rx=0
        self.ry=0
        self.throttle=0
        self.speed=False
        self.verbose=False
        self.dpad=[]
        self.dpad_old=[]
        self.btn_str="-1"
        self.btn_str_old=None
        
        pygame.init()
        pygame.joystick.init()
        if pygame.joystick.get_count()==0:
            print("Nie wykryto kontrolera")
            exit()
        self.pad=pygame.joystick.Joystick(0)
        self.pad.init()
        print("Kontroler:",self.pad.get_name())
        time.sleep(4)
        self.ser=serial.Serial('COM4',115200,timeout=0.1)

    def __scaleControlSurfaces(self,val):
        """
        Scales raw joystick axis value to control surface range.
        
        val: raw axis value from -1.0 to 1.0.
        return: scaled integer value for servo position.
        """
        return round(val*18)

    def __scaleThrottle(self,val):
        """
        Scales raw joystick axis value to throttle range (0-180).
        
        val: raw axis value from -1.0 to 1.0.
        return: absolute throttle value.
        """
        if val>0:
            return 0
        return abs(int(val*180))

    def printPad(self):
        """Prints raw stick and button data to the console."""
        print(f"lewy:{self.ly}, prawy: {self.rx} {self.ry}, przycisk: {self.btn_str}")

    def padRead(self):
        """
        Reads current events from pygame and updates internal state of axes and buttons.
        
        Fetches 'ly', 'rx', 'ry' values and identifies which buttons are currently pressed.
        Updates 'btn_str' with comma-separated button indices.
        """
        for event in pygame.event.get():
            if event.type==pygame.QUIT:
                pygame.quit()
                exit()
        
        self.ly=self.__scaleThrottle(self.pad.get_axis(1))
        self.rx=self.__scaleControlSurfaces(self.pad.get_axis(2))
        self.ry=self.__scaleControlSurfaces(self.pad.get_axis(3))
        
        self.btn_str_old=self.btn_str
        pressed_buttons=[str(k) for k in range(self.pad.get_numbuttons()) if self.pad.get_button(k)]
        
        if pressed_buttons:
            self.btn_str=",".join(pressed_buttons)
        else:
            self.btn_str="-1"
            
        self.dpad=[self.pad.get_hat(k) for k in range(self.pad.get_numhats())]

    def elevonCalc(self):
        """
        Calculates left and right elevon positions using stick mixing logic.
        
        Uses 'ry' (pitch) and 'rx' (roll) to determine final 'left' and 'right' servo values 
        including 'leftPar' and 'rightPar' offsets.
        """
        self.left=76+self.leftPar
        self.right=76+self.rightPar
        self.left+=self.ry+1*self.rx
        self.right+=-1*self.ry+1*self.rx
    
    def ElevonConfig(self):
        """
        Enters a loop to configure elevon offsets using the D-pad.
        
        Allows real-time adjustment of 'leftPar' and 'rightPar'. 
        The mode is exited when button '3' is pressed.
        """
        print("Elevon Config Mode Activated!")
        while True:
            self.padRead()
            time.sleep(0.2)

            if ("3" in self.btn_str) and (self.btn_str_old!=self.btn_str):
                print("Exiting Elevon Config Mode")
                break

            if self.dpad and self.dpad[0]==(-1,0):
                self.leftPar-=1
                self.left-=1
                print(f"Left Elevon offset: {self.leftPar}")
            
            if self.dpad and self.dpad[0]==(1,0):
                self.leftPar+=1
                self.left+=1
                print(f"Left Elevon offset: {self.leftPar}")
            
            if self.dpad and self.dpad[0]==(0,1):
                self.rightPar+=1
                self.right+=1
                print(f"Right Elevon offset: {self.rightPar}")
            
            if self.dpad and self.dpad[0]==(0,-1):
                self.rightPar-=1
                self.right-=1
                print(f"Right Elevon offset: {self.rightPar}")
        
            self.sendData()
            
    def throttleChange(self):
        """
        Manages throttle logic including cruise control (Tempomat).
        
        If 'speed' (Tempomat) is ON, throttle is adjusted by 5 units using the D-pad.
        Otherwise, throttle follows the 'ly' axis input.
        """
        if ("4" in self.btn_str) and (self.btn_str_old!=self.btn_str):
            self.speed=not self.speed
            print(f"Tempomat: {'ON' if self.speed else 'OFF'}")
        
        if not self.speed:
            self.throttle=self.ly
            return
            
        if self.dpad!=self.dpad_old:
            if self.dpad and self.dpad[0]==(0,1):    
                self.throttle+=5                
            elif self.dpad and self.dpad[0]==(0,-1): 
                self.throttle-=5
        
        self.dpad_old=self.dpad[:] 
        self.throttle=max(0,min(180,self.throttle))

    def sendData(self):
        """
        Encodes 'throttle', 'left' and 'right' values into a frame and sends it via serial.
        
        The frame format is 'T:val;L:val;R:val\n' encoded in ASCII.
        """
        frame=f"T:{self.throttle};L:{int(self.left)};R:{int(self.right)}\n"
        self.ser.write(frame.encode('ascii'))
        self.ser.flush()

    def printData(self):
        """Prints calculated control data (elevons and throttle) to the console."""
        print(f"lEl:{self.left}, pEL: {self.right}, Throttle: {self.throttle}")
    
    def Start(self):
        """
        Main loop for the ground station.
        
        Handles timing (50Hz), updates pad reading, calculates flight surfaces, 
        manages UI verbosity and sends data frames to the drone.
        """
        while True:
            self.padRead()
            if "3" in self.btn_str:
                self.ElevonConfig()
            if ("2" in self.btn_str) and (self.btn_str_old!=self.btn_str):
                self.verbose=not self.verbose
            self.throttleChange()
            if self.verbose:
                self.printPad()
            self.elevonCalc()
            if self.i%50==0:
                self.printData()
            self.sendData()
            time.sleep(0.02)
            self.i+=1
            if self.i==10000:
                self.i=0


if __name__ == "__main__":
    gs=DroneGroundStation()
    gs.Start()