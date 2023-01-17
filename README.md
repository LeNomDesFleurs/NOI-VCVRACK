# NOI

Hello !
Thanks for using this module !

# Global UI tips
- Input are circled

- Output are on a black background

- Plain lines are audio path

- Dotted lines are CV path

- Little cross at the end of a line indicate normalisation (replaced by the input if connected)

- Trimpot are almost always Attenuverter for cv input.

# Sinensis

Sinensis is a parallel band-pass bank (6 of them).<br>
The two big knobs set the frequency and the Q of all filters (it oscillate at high Q).<br>
The normal ones set the number of band and the relationship between them (ratio).<br>
For exemple, if you set the frequency to 2000Hz, ratio to 1.5 and band parameter to 3:<br>
You'll get 3 bandpassed version of your sound at 2000Hz, 3000Hz and 4500Hz.<br>
Here with white noise in input.<br>

![image](https://user-images.githubusercontent.com/117636251/212921153-b8240dd6-806a-485d-9e3d-f4fb1ce3b8fb.png)


# Sunflower

all the trimpot are little attenuator for your inputs. 
Circular mixer, a cross fade between 12 inputs
The Knobs with an arrow let you choose the input, the diffusion how many nerby input will bleed in


# Dicotyledon
Logic module
The upper half is a comparator, 
Input A is compared to input B: 
If A > B, the comparator output gives +5V
Else Comparator output gives 0V.
if an input is not connected, it default to the voltage define by the voltage knob above.

The lower half is a switch, 
If the control input (in the middle) reveive a voltage > to 0V,
The Switch output give switch input A, 
If the control input receive a voltage < to 0V:
The Switch output give switch input B.


# Wilt
Multi-Effect
The signal go through reduction, ring modulation, one- pole low-pass, shroeder reverb.
Redux is a sample and hold being triggered every n sample (n being the value of the redux parameter).
it does a sort of sampling frequency reduction. 
The cv is attenuverted, the latch quantize the cv for extra digital steppyness.
rt60 set the lenght of the reverb, dry/wet set the amount of reverb.
