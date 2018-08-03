# IRPatternRecorder-thresh
<h1>Description</h1><br>
An improved version of my firmware to record infrared patterns of IR LEDs straight from the cathode<br>
This one has added support for threshold for pulses, meaning pulses <b>shorter</b> than the threshold will be <b>ignored</b><br>

<h2>How to use this</h2>
<ol>
<li>Open main.c, change the frequency of CPU macro (F_CPU), number of samples(SAMPLES), and threshold (THRESH_COUNT). The threshold is the number of F_CPU / 8 pulses. You can use this simple formula to calculate the threshold time to be set <b>THRESH_COUNT = (threshold_time * F_CPU) / 8</b></li>
<li>Compile using the makefile, you will need <i>avr-binutils, avr-gcc, avr-libc</i></li>
<li>Flash using avrdude or whatever you want to use</li>
<li>Make sure your AVR and the device you're recording share the same ground</li>
<li>Connect the anode of the IR LED to the PD2 (on an ATmega8)</li>
<li>Connect your AVR to the serial port of your computer and set it to 9600</li>
<li>Power on the AVR, and press a button on the remote, which you want to record the IR pattern for</li>
<li>Press any button on your serial port tool, and it will begin to dump the data</li>
</ol>


<h3>NOTE</h3>
<ul>
<li>This was meant to be used with an ATmega8. Please make appropriate changes to the firmware before using it on another AVR.</li>
<li>The output data is the number of CPU cycles a state remained for. This data can be easily imported and interpreted using a spreadsheet software as a CSV file</li>
</ul>

