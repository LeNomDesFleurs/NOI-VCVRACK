# NOI

![image](ModulesDark.png)
![image](Modules.png)

Now available in dark mode !
`View > Use dark panels if available`


Hello and welcome to the NOI Manual 📚 !<br>
[Link to the VCV library](https://library.vcvrack.com/NOI)

Those [modules are usable in MetaModule](https://github.com/Ericxgao/metamodule-noi) thanks to [Ericxgao](https://github.com/Ericxgao)

# Global UI tips

- ⌾ Inputs are circled 

- ⚫ Outputs are on a solid background 

- ⎸ Plain lines are audio pathes 

- ⦙ Dotted lines are CV pathes 

- ⤫ Little cross at the end of a line indicate normalisation (replaced by the input if connected)

- +/- Trimpot are almost always Attenuverter for CV input 

# Sinensis [filter bank]

Sinensis is a parallel band-pass bank (6 of them).<br>
The two big knobs set the frequency and the Q of all filters (it oscillates at high Q).<br>
The normal ones set the number of band and the relationship between them ("ratio").<br>
If a frequency goes beyond 15kHz or 20Hz, it is folded back in audible range.<br>
If you set the frequency to 2000Hz, ratio to 1.5 and band parameter to 3:<br>
You'll get 3 bandpassed version of your sound at 2000Hz, 3000Hz and 4500Hz.<br>
Here with white noise in input.<br>

![image](https://user-images.githubusercontent.com/117636251/212921153-b8240dd6-806a-485d-9e3d-f4fb1ce3b8fb.png)


# Sunflower [Circular Mixer]
Sunflower crossfade between 12 inputs.<br>
The Knobs with an arrow let you choose the input, the diffusion how nearby input will bleed in.<br>
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

# Pruners [Sampler]
A ring buffer is an audio container that keeps rewriting itself (think ouroboros type stuff, but that stocks samples).
Pruners gives you four reading heads that samples a part of this ring buffer. Each head has a dedicated output, plus a global mix output that you control with the `Head` parameter
- `Position` offsets the part you're sampling
- `Size` resizes the sampled part, from oscillation to granular type sounds
- You can freeze the content of the ring buffer if you crank `Feedback` to its maximum.
- Each head read speed is determined by previous heads, you set the `Speed` of the first one, and then the `Ratio` of one to another.


# Have fun ! Feel free to contact me for any information / bug report !
