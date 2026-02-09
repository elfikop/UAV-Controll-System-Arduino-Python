import pygame
import time
import serial

i=0
pad = None
left = 76
right = 76
rightPar = -21
leftPar = 8
button = 0
ly = 0
rx = 0
ry = 0
throttle=0
speed = False
verbose = False
dpad = []
dpad_old = []
dpad_str = None
btn_str = None
btn_str_old = None

def scaleControlSurfaces(val):
    return round(val * 18)

def scaleThrottle(val):
    if val > 0:
        return 0
    return abs(int(val * 180))

def padIni():
    global pad
    pygame.init()
    pygame.joystick.init()
    if pygame.joystick.get_count() == 0:
        print("Nie wykryto kontrolera")
        exit()
    pad = pygame.joystick.Joystick(0)
    pad.init()
    print("Kontroler:", pad.get_name())
    time.sleep(4)

def printPad():
    global ly
    global rx
    global ry
    global btn_str
    global dpad_str
    print(f"lewy:{ly}, prawy: {rx} {ry}, przycisk: {btn_str} {dpad_str}")

def padRead():
    global ly
    global rx
    global ry
    global btn_str
    global btn_str_old
    global dpad
    global dpad_str

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
    ly = scaleThrottle(pad.get_axis(1))
    rx = scaleControlSurfaces(pad.get_axis(2))
    ry = scaleControlSurfaces(pad.get_axis(3))
    btn_str_old = btn_str
    pressed_buttons = [str(i) for i in range(pad.get_numbuttons()) if pad.get_button(i)]
    if pressed_buttons:
        btn_str = ",".join(pressed_buttons)
    else:
        btn_str = "-1"
    dpad = [pad.get_hat(i) for i in range(pad.get_numhats())]
    dpad_str = " ".join([f"{h}" for h in dpad if h != (0,0)]) if dpad else ""

def elevonCalc():
    global rx
    global ry
    global left
    global right
    left = 76 + leftPar
    right = 76 + rightPar
    left += ry + 1 * rx
    right += -1 * ry + 1 * rx

def ElevonConfig():
    global leftPar
    global rightPar
    global btn_str
    global dpad
    global dpad_str
    global right
    global left

    print("Elevon Config Mode Activated!")
    while True:
        padRead()
        time.sleep(0.2)

        if ("3" in btn_str) and (btn_str_old != btn_str):
            print("Exiting Elevon Config Mode")
            break

        if dpad and dpad[0] == (-1,0):
            leftPar -= 1
            left-=1
            print(f"Left Elevon offset: {leftPar}")
            
        if dpad and dpad[0] == (1,0):
            leftPar+=1
            left+=1
            print(f"Left Elevon offset: {leftPar}")
            
        if dpad and dpad[0] == (0,1):
            rightPar += 1
            right+=1
            print(f"Right Elevon offset: {rightPar}")
            
        if dpad and dpad[0] == (0,-1):
            rightPar -= 1
            right-=1
            print(f"Right Elevon offset: {rightPar}")
        
        sendData()
            


def throttleChange():
    global ly, throttle, speed, dpad, dpad_old, btn_str, btn_str_old
    if ("4" in btn_str) and (btn_str_old != btn_str):
        speed = not speed
        print(f"Tempomat: {'ON' if speed else 'OFF'}")

    if not speed:
        throttle = ly
        return
    if dpad != dpad_old:
        if dpad and dpad[0] == (0, 1):    
            throttle += 5                
        elif dpad and dpad[0] == (0, -1): 
            throttle -= 5

    dpad_old = dpad[:] 
    
    throttle = max(0, min(180, throttle))

def sendData():
    global throttle, left, right
    frame = f"T:{throttle};L:{int(left)};R:{int(right)}\n"
    ser.write(frame.encode('ascii'))
    ser.flush()


ser = serial.Serial('/dev/ttyACM0', 115200, timeout=0.1)  # sprawdzic com
time.sleep(2)
padIni()

def printData():
    global throttle
    global left
    global right
    global i
    print(f"lEl:{left}, pEL: {right},Throttle: {throttle}")


while True:
    padRead()
    if "3" in btn_str:
        ElevonConfig()
    if ("2" in btn_str) and (btn_str_old != btn_str):
        verbose = not verbose
    throttleChange()
    if(verbose==True):
        printPad()
    elevonCalc()
    if(i%50==0):
        printData()
    sendData()
    time.sleep(0.02)
    i=i+1
    if(i==10000):
        i=0


