 #!/bin/sh
 for file in /sys/class/hwmon/hwmon0/device/in*
   echo -n "${file}: "; cat $file
 done