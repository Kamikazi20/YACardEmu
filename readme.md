YACardEmu
============

YACardEmu (*YetAnother*CardEmu) is a software emulator to emulate a range of magnetic card readers found in various arcade machines. Currently supported models are Sanwa CRP-1231BR-10, the CRP-1231LR-10NAB, and CR-S31R-10HS3 models which are commonly found in games such as Wangan Midnight Tune 3, Virtual-On Force, THE iDOLM@STER, F-Zero, Mario Kart, and Initial D series arcade cabinets.

**Checkout some gameplay videos [here!](https://www.youtube.com/channel/UCle6xQNwROzwYfYMyrnIcBQ)**

Breaking Changes....
---------
We've recently remodeled how cards are stored. If your card ends with `.track_n` then you must convert this to a single file. If your `.track_1` or `.track_2` files are empty (aka all zeros) then do **not** add the zero'ed track to the file. The resulting file should have either 69, 138, or 207 bytes in it.

Building
---------
#### Prerequisites

Ubuntu / Debian / Raspbian

```sh
sudo apt install build-essential cmake pkg-config libserialport-dev
```

Windows

**[Precompiled Windows binaries can be found here.](https://github.com/GXTX/YACardEmu/tags)**

1. [Visual Studio 2022](https://visualstudio.microsoft.com/vs/)
    * C++ desktop development
    * Windows Universal CRT SDK
    * C++ CMake tools for Windows
    * Git for Windows
		* *Optional if you already have Git installed*

#### Build

Ubuntu / Debian / Raspbian

```
git clone --recursive https://github.com/GXTX/YACardEmu
cd YACardEmu
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Windows

1. If you don't have CMake installed, open `___ Native Tools Command Prompt for VS 20##`.
2. `git clone --recursive https://github.com/GXTX/YACardEmu`
3. `cd` to the `YACardEmu` directory.
4. Run these commands.
    1. `mkdir build & cd build`
    2. `cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release`
        * VS2022 17.0 or later is required.
5. `cmake --build . --config Release`
6. All the required files will be in `build\Release`

Running
---------

You must have a USB to RS232 (or a physical serial port) connected to your machine. Configuration is made via both a `config.ini` file & via a web portal / API.
To access the web portal point your browser to `http://YOURIPHERE:8080/`, this web page is where you'll choose your card and insert your card.

Editing `config.ini` to point to where you want your cards stored is required.

Ubuntu

```
cp ../config.ini.sample config.ini
./YACardEmu
```

Windows

```
Download the Binary here - https://github.com/GXTX/YACardEmu/tags
Extract Files to C:\YaCardEmu
Make a copy of config.ini.sample
Rename it to config.ini
Modify Config.ini - 
(Below is a Windows Example that uses Com3, The card path is C:\docwe_c, the reader is a C1231BR, Set Baud Rate at 9600 & Even

[config]
; basepath MUST end in a trailing slash!
;basepath = /home/wutno/cards/
;Very Important you must Specify a location and there has to be a folder. If the folder is missing you will get an error.
basepath = C:\docwe_c
; Linux
;serialpath = /dev/ttyUSB0
; Windows
serialpath = COM3
;serialpath = \.\pipe\YACardEmu
; Which device should we emulate?
; C1231LR / C1231BR / S31R
targetdevice = C1231BR
;
; Optional
;
serialbaud = 9600
;Acceptable even & none
serialparity = even
apiport = 8080
autoselectedcard = card.bin

Once the Config.ini is updated 
Double click YACardEmu.exe

```

Running On Hardware
---------

If you're wanting to run this on hardware there are some settings you'll need to be aware of.

SEGA Chihiro
```
Wangan Maximum Tune 1 : CRP-1231LR-10NAB | 9600 none
Wangan Maximum Tune 2 : CRP-1231LR-10NAB | 9600 none
```

Namco System N2
```
Wangan Maximum Tune 3    : CR-S31R-10HS3 | 38400 even
Wangan Maximum Tune 3DX  : CR-S31R-10HS3 | 38400 even
Wangan Maximum Tune 3DX+ : CR-S31R-10HS3 | 38400 even
```

SEGA Hikaru
```
Virtual-On Force (電脳戦機バーチャロン フォース) : CRP-1231BR-10 | 9600 even
†Requires SEGA P/N 838-13661 RS232 converter PCB
```

Namco System 246
```
THE iDOLM@STER (アイドルマスタ) : CRP-1231LR-10NAB | 9600 none
```

SEGA NAOMI
```
Derby Owners Club World Edition : CRP-1231BR-10 | 9600 even
†Requires SEGA P/N 838-13661 RS232 converter PCB

Cable - https://www.amazon.com/dp/B075YGKFC1?ref=ppx_yo2ov_dt_b_product_details&th=1
USB RS232 Pin out
Notes you only need 3 wires | CTS/RTS Bridged | You will have to modify a cable|
Pin 2 RXD
Pin 3 TXD
Pin 5 GND
**Pin 7 CTS Wire this together with pin 8
Pin 8 RTS Wire this together with pin 7
Looks like this
![Pining](https://github.com/GXTX/YACardEmu/assets/8756824/8d7a73a0-fb75-4064-8af9-9fc43cc80690)

The flow should be like this : Computer -> USB to RS232 ->RS422 ->Naomi
![bridge_RTX_CTS](https://github.com/GXTX/YACardEmu/assets/8756824/891319d4-67ea-40c2-9e8e-c7437dd4f451)


```

SEGA NAOMI 2
```
Initial D Arcade Stage Ver.1 : CRP-1231BR-10 | 9600 even
Initial D Arcade Stage Ver.2 : CRP-1231BR-10 | 9600 even
Initial D Arcade Stage Ver.3 : CRP-1231BR-10 | 9600 even
```

SEGA Triforce
```
F-Zero AX             : CRP-1231BR-10    | 9600 even
Mario Kart Arcade GP  : CRP-1231LR-10NAB | 9600 none
Mario Kart Arcade GP2 : CRP-1231LR-10NAB | 9600 none
```
