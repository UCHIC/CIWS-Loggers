# Instructions:</br></br>
Add this folder (Software) to the home directory of your Raspberry Pi 3.</br>
From the Software directory, run the following commands:
```
chmod +x setup.sh
sudo ./setup.sh
```
You can now use the Logger module in Python. Note that this was built with Python 2.7. The script output may tell you to reboot.</br></br>
Template code for using the Logger module is included as `template.py`.</br></br>
Dependancies:
```
python-dev
wiringpi
```
WARNING: This code is still under development and not ready for use:</br>
Issues:</br>
reportSwap can only be called twice before it causes a segmentation fault (very consistent)</br>
