import urllib3, urllib.request
import json
import time
import numpy as np
import serial

##% 
#write 'N4' or 'N5' depending of with node is yours
def get_radioPos(mynode, othersnode):

    http = urllib3.PoolManager()

    ##% Set UART port - Say True or False if you want to send data via UART or not
    wantUART = True
    
    if wantUART == True:
        ser = serial.Serial(
                port='/dev/serial0',
                baudrate = 9600, 
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE, 
                bytesize=serial.EIGHTBITS,
                timeout=1
            )
        print("serial is open: "  + str(ser.isOpen()))

    ##% Define variables and classes for data and initialize

    class LpsNode():
        def __init__(self, position, deadSince):
            self.position = position
            self.deadSince = deadSince

    class Position():
        def __init__(self, x, y, z):
            self.x = x
            self.y = y
            self.z = z

    pos1 = Position(float(0), float(0), float(0))
    pos2 = Position(float(0), float(0), float(0))

    veh1 = LpsNode(pos1, int(0))
    veh2 = LpsNode(pos2, int(0))

    ##% Loop
    timer_out = time.time()
    datatocompare_old = 'abc'

    try:
        while True:
            reset_time = time.time()
            time1s = 0
            valx1 = []
            valx2 = []
            valy1 = []
            valy2 = []
            counter1s = 0
            while time1s < 2:
            
                #call the data from server
                r = http.request('GET', 'http://192.168.1.1/website/src/val.js', retries = 100)
        
                try:
                    # Convert received string to JSON (just interpret python library)
                    jdata = (json.loads(r.data.decode('utf-8')))
            
                    # Assign JSON values to node estructures
                                 
                    veh1.position.x = jdata[mynode][0]
                    veh1.position.y = jdata[mynode][1]
                    veh1.position.z = jdata[mynode][2]
                    veh1.deadSince = jdata[mynode][3]
                                   
                    veh2.position.x = jdata[othersnode][0]
                    veh2.position.y = jdata[othersnode][1]
                    veh2.position.z = jdata[othersnode][2]
                    veh2.deadSince = jdata[othersnode][3]
                        
                    datatocompare = veh1.position.x + ',' + veh1.position.y + ',' + veh1.position.z + ',' + veh2.position.x + ',' + veh2.position.y + ',' + veh2.position.z + '\n\r'
            
                    if datatocompare != datatocompare_old:   #ignore measurement if is the same as previuos, that would mean sysem has not refreshed yet
                        counter1s += 1

                        # Add to vectors
                        valx1.append(float(veh1.position.x))
                        valy1.append(float(veh1.position.y))
                        valx2.append(float(veh2.position.x))
                        valy2.append(float(veh2.position.y))
                   
                        time1s = time.time() - reset_time
                        datatocompare_old = datatocompare

                except ValueError:
                    print('one package was fault, but you do not need to worry :)')
            
            if wantUART == True: 
                stringtosend = '$RP:' + str(2-np.mean(valy1)) + ':' + str(2-np.mean(valx1)) + ':' + str(2-np.mean(valy2)) + ':' + str(2-np.mean(valx2)) + '#\n\r' #Here you can modify the format, but becareful with keeping the 2- and the order of the coordinates
                ser.write(bytes(stringtosend.encode()))

            myPosition = [2-np.mean(valy1), 2-np.mean(valx1)]
            otherPosition = [2-np.mean(valy2), 2-np.mean(valx2)]
            
            return myPosition, otherPosition


    except KeyboardInterrupt:
        print('Manually interrpted by user')


try:
    while True:
        myPosition, otherPosition = get_radioPos('N5', 'N4')
        print(myPosition)
        print(otherPosition)
except KeyboardInterrupt:
    print('Manually interrupted by user')
