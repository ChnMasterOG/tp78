import usb
import sys
import time

VID = 0x434A
PID = 0x5539

dev = usb.core.find(idVendor=VID, idProduct=PID)

if dev is None:
    print("Can not find USB device!")
    sys.exit(1)

print(dev)

# dev.set_configuration()
# write the data
msg = b'\x33'

while 1:
    #try:
        wr = dev.write(0x3, msg, 1000)
        print("wr", wr)
        # reading
        print ("Waiting to read...")
        data = dev.read(0x83, 64, 1000)
        print(data)
        time.sleep(5)
    #except usb.core.USBError:
        print ("USB error")
        time.sleep(5)
    #except:
        print ("write failed")

usb.util.dispose_resources(dev)
