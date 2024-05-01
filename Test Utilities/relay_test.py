import serial
import time

ser = serial.Serial(
   port='COM14',
   baudrate=19200,
   parity=serial.PARITY_NONE,
   stopbits=serial.STOPBITS_ONE,
   bytesize=serial.EIGHTBITS,
   xonxoff=True
)

""" # Set Relays Command (0x55)
time.sleep(0.5)
ser.write(b(chr(92)))
ser.write(b'7')

# Get Relays Command 
time.sleep(0.5)
ser.write(chr(91))
ser.write(b'7')
time.sleep(0.5)
out = ''
while ser.inWaiting() > 0:
   out += str(ser.read(1))
print(out)
# Set Relays Command (0xAA)
#time.sleep(0.5)
#ser.write(b'7')

 """



#All Relays On Command
time.sleep(0.5)
ser.write(b'd')

# All Realays Off Command
time.sleep(0.5)
ser.write(b'n')

# Realay 1 On Command
time.sleep(0.5)
ser.write(b'e')

# Realay 2 On Command
time.sleep(0.5)
ser.write(b'f')

# Realay 3 On Command
time.sleep(0.5)
ser.write(b'g')

# Realay 4 On Command
time.sleep(0.5)
ser.write(b'h')

# Realay 5 On Command
time.sleep(0.5)
ser.write(b'i')

# Realay 6 On Command
time.sleep(0.5)
ser.write(b'j')

# Realay 7 On Command
time.sleep(0.5)
ser.write(b'k')

# Realay 8 On Command
time.sleep(0.5)
ser.write(b'l')

# Realay 1 Off Command
time.sleep(0.5)
ser.write(b'o')

# Realay 2 Off Command
time.sleep(0.5)
ser.write(b'p')

# Realay 3 Off Command
time.sleep(0.5)
ser.write(b'q')

# Realay 4 Off Command
time.sleep(0.5)
ser.write(b'r')

# Realay 5 Off Command
time.sleep(0.5)
ser.write(b's')

# Realay 6 Off Command
time.sleep(0.5)
ser.write(b't')

# Realay 7 Off Command
time.sleep(0.5)
ser.write(b'u')

# Realay 8 Off Command
time.sleep(0.5)
ser.write(b'v')



# Get Serial Number Command
time.sleep(0.5)
ser.write(b'8')
time.sleep(0.5)
out = ''
while ser.inWaiting() > 0:
   out += str(ser.read(1))
print(out)

# RGet Software Version Command
time.sleep(0.5)
ser.write(b'Z')
time.sleep(0.5)
out = ''
while ser.inWaiting() > 0:
   out += str(ser.read(1))

print(out)

ser.close()
exit()