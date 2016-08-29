import time
import RPi.GPIO as GPIO

scanInterval = 1.0          # Time between scans within the main loop in seconds
recordInterval = 10.0       # Time between recorded values in seconds

# Set up the GPIO pin for the pulse counting meter
# ------------------------------------------------
GPIOPin = 9
GPIO.setmode(GPIO.BCM)
GPIO.setup(GPIOPin, GPIO.IN)

# Initialize timing variables
# --------------------
sampleNum = 0             # Initialize the sample number to 1
currTime = 0.0            # Timing variable
previousTime = 0.0        # Timing variable

# Define sensor related values
# ----------------------------
global pulseCount
pulseCount = 0            # Number of pulses detected by the meter during a scan interval


# Define the function that will increment the pulse counter when a pulse from the multi-jet meter is detected
def gpio_callback(channel):
    global pulseCount
    pulseCount += 1

# Set up pulse detection on the GPIO pin connected to the multi-jet meter
GPIO.add_event_detect(GPIOPin, GPIO.RISING, callback=gpio_callback)

# Main program loop
# -----------------
while True:
    # Get the current time on the logger at the beginning of the measurement loop
    currTime = time.time()

    # Check timing to determine if the scan interval has been reached and measurements should be made
    if (currTime - previousTime) >= scanInterval:
        # Increment the sample counter
        sampleNum += 1

        # Get a correctly formatted date string
        now = time.strftime("\"%Y-%m-%d %H:%M:%S\"", time.localtime(currTime))

        # Create a data record string
        outputList = [now, sampleNum, pulseCount]
        dataRecord = ','.join(map(str, outputList))

        pulseCount = 0  # Set the pulse count back to 0 immediately after reading for the current scan

        # Print the current data record to the console for monitoring
        print dataRecord

        # Remember the time at which the current measurement started
        previousTime = currTime
