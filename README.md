# NOI

![image](https://user-images.githubusercontent.com/117636251/217655019-26ec1503-4786-42ff-ae39-2ef6c800d689.png)


Hello and welcome to the NOI Manual !<br>
Link to the VCV library :<br>
https://library.vcvrack.com/NOI

# Global UI tips
- Input are circled

- Output are on a black background

- Plain lines are audio path

- Dotted lines are CV path

- Little cross at the end of a line indicate normalisation (replaced by the input if connected)

- Trimpot are almost always Attenuverter for CV input.

# Sinensis [filter bank]

Sinensis is a parallel band-pass bank (6 of them).<br>
The two big knobs set the frequency and the Q of all filters (it oscillate at high Q).<br>
The normal ones set the number of band and the relationship between them ("ratio").<br>
If a frequency goes beyond 15kHz or 20Hz, it is folded back in audible range.<br>
If you set the frequency to 2000Hz, ratio to 1.5 and band parameter to 3:<br>
You'll get 3 bandpassed version of your sound at 2000Hz, 3000Hz and 4500Hz.<br>
Here with white noise in input.<br>

![image](https://user-images.githubusercontent.com/117636251/212921153-b8240dd6-806a-485d-9e3d-f4fb1ce3b8fb.png)


# Sunflower [Circular Mixer]
Sunflower crossfade between 12 inputs.<br>
The Knobs with an arrow let you choose the input, the diffusion how nerby input will bleed in.<br>
The latch let you lock on connected input only.<br>

# Dicotyledon [Logic Processor]
The upper half is a comparator: <br>
- If input A > input B
  - the comparator output gives +5V
- If input A < input B
  - the comparator output gives 0V
  
If an input is not connected, it default to the voltage define by the "voltage" knob above.
<br>

The lower half is a switch:<br>
- If the control input (in the middle) reveive a voltage > to 0V <br>
  - The Switch output give switch input A <br>
- If the control input receive a voltage < to 0V <br>
  - The Switch output give switch input B


# Wilt [Digital Glitch]
The signal go through Frequency folding and buffer mess.<br>
Redux is a sample and hold being triggered every n sample (n being the value of the redux parameter). <br>
It does a sort of sampling frequency reduction, thus folding frequency. <br>
The CV is attenuverted, the latch quantize the CV (= cut the fractional part) for extra digital steppyness. <br>
Next is a Buffer which is being write at a steady pace but read at a variable rate (set by the speed knob).<br>
It may read in reverse for extreme digital glitch (Try very slow speed for interesting rhythm !).<br>
The other knob set the feedback of the buffer.

# Hellebore [Reverb]
It uses a Moorer reverb implementation with resizable buffers (which repitch !)<br>
The size knob set the size of all delays lines.<br>
The variation is a macro parameter that sets multiple things :
- Bleeding of one channel to an other
- Disparity of buffer sizes (thus repitching)
- Dispartity of feedback of each delays lines

The freeze latch lets you switch to read only mode, the buffers stays the same and you can freely slow down or speed up their contents.<br>

# Have fun ! Feel free to contact me for any information / bug report !
