duality

This is a non-realtime audio effects plugin based on a "spectral transform -> basic audio effect -> inverse spectral transform" signal flow. The resulting sound can be unpredictable, so this plugin encourages wild sound design and experimentation. If you like noise and texture, you've come to the right place!

How to use:
To open a file, you can use the open button or just drag and drop onto the plugin.

The transform section lets you choose which spectral transformation you would like to use. The little graphic shows you how the spectrogram of the sound is transformed. There is also a "transform only" toggle that bypasses the effects and the inverse transform in case you want to add your own effects in your daw. Sometimes the "transform only" output is also just a cool glitchy/fluttery percussion sound.

The effects section lets you choose which effect is used between the spectral transform and the corresponding inverse transform
Here's a brief explanation of the available effects:

    Distortion
        This is simply a hard clipper with an input gain control (in dB) and a dry/wet control. Since normally distortion spreads a sound across the frequency domain, this will result in spreading the sound across the time domain when used in this plugin. It sounds a bit like reverb/delay and adds space

    Noise Convolution
        This generates an impulse response of white noise with the given volume envelope and convolves the sample. This is essentially a reverb. Since reverb normally spreads a sound across the time domain, in this plugin, it will spread a sound across the frequency domain. At high decays, this results in a sort of inharmonic/noisy distortion. At low decays, it results in a fast volume lfo (almost granular sound) since the interference "beating" of neighboring frequencies is pronounced.

        Parameters:
            decay [0,1] - determines the length of the impulse response. 0.0 means zero seconds and 1.0 means the length of the input sample.
            order [0, 100] - this controls the shape of the IR. 0 means no volume envelope (constant volume), 1 is a linear ramp, and 100 means the noise decays very quickly
            width [0,1] - controls the stereo width of the sound
            dry/wet [0,1] - i think you know this one

    Pad
        This lets you set a new length for the sample. Values higher than 1 will just pad the extra space with 0, and values lower than 1 will truncate the end of the sample. This results in time stretching and frequency shifting. Depending on the transform mode, this either pushes the frequencies up or down. 

    Delay
        This is simply a delay, and much like the reverb results in inharmonic distortion as well (but sounds distinctly different), since it essentially creates copies of the spectrum at either higher/lower frequencies depending on the transform mode. This can sometimes sound a bit like downsampling without an aliasing filter afterwards.

        Parameters:
            delays [0,100] - the amount of delay taps. The higher, the more dense the spectrum copies will be
            feedback [0,2] - feedback of the delay line
            dry/wet

    Sample Skew
        This skews the sample towards the front/back of the sample. The way this sounds is a little unpredictable, so some experimentation would be wise. I've been able to create some cool lasery/phase dispersion sounds. You can also use this to just absolutely ruin a sample and make some intersting percussion.

        Parameters:
            skew [0,1] - values below 0.5 will push samples to the front and values above 0.5 will push them to the end. Exactly at 0.5, the sound is unchanged
            dry/wet

The output section is fairly self explanatory. You can playback the output, save the file, or drag and drop it right into your daw.

That's it for now! I'll update it with more effects in the future, but it is more than ready now to do some crazy sound design and experimentation. Have fun!



