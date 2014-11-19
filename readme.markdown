## 1. Intro
This section will help you get to grips with the KORG SYRO library. It shows how the functions can be used to generate syrostream (the audio signal that the volca sample understands) from syrodata (the raw data you want to send - sample audio or sequence patterns).  

Good luck and have fun!

***

## 2. what's inside the SDK?
**syro :** syro itself. It generates syrostream from syrodata.  
**example :** examples using syro that creates a syro wav file from an audio sample  
**project :** contains the project files to build the examples.  
there is a visual studio 2010 project file and Makefile for gcc or clang.  
**pattern :** definitions for pattern data structure and functions to initialize pattern data.  
**alldata :** data for restoring factory preset samples or for deleting all sample data.  

***

## 3. how to use the SDK to generate syrostream from syrodata

### 3.1 essential source files
korg\_syro\_volcasample.c  
korg\_syro\_func.c  
korg\_syro\_comp.c  

add the following to call syro functions.

    #include "korg_syro_volcasample.h"

### 3.2 outline of usage

1. prepare the data to be converted  
2. call the conversion start function  
3. process syrodata for each frame  
4. call the conversion end function  

Each step is explained in detail below.  
Please refer to korg\_syro\_volcasample\_example.c for a working example.

### 3.3a preparing the data to be converted

Both sample and sequence data can be converted.  
You can also send a sample delete command.  
It is possible to send multiple data in one operation.  

Once you have the data you want to convert, set the necessary information in the SyroData structure.  

    SyroDataType DataType;
    specifies any of the following data type to be converted.  
    DataType_Sample_Compress        : convert single audio sample  
    DataType_Sample_Erase           : delete single sample slot on volca sample  
    DataType_Sample_AllCompress     : convert all sample data  
    DataType_Pattern                : convert sequence pattern  

    uint8_t *pData;
    pointer which specifies the data to be converted.
    not used when deleting samples.
  
    uint32_t Number;
    the sample (0-99) or sequence pattern (0-9) number.

    uint32_t Size;
    size of data to be converted (in bytes).
    set it to sizeof(VolcaSample_Pattern_Data) when converting sequence data.
    not used when deleting samples.

    uint32_t Quality;
    The conversion bit depth. It can be set to 8-16.
    not used when deleting samples.

    Endian SampleEndian;
    set to LittleEndian or BigEndian to suite your sample data.
    only used when converting single samples.

Only 16bit monaural audio samples are supported, so convert as necessary beforehand.  
Only files with extension .alldata contained in folder 'alldata' can be used with all data conversion.  

Lower bit depths will increase transfer speed at the cost of audio resolution.  
The prepared audio **must** be 16bit regardless of this setting.  
The sample is always stored at 16bit in the volca sample regardless of this value, so memory usage is not affected by this setting.  
  
To send multiple data, create an array of SyroData structures and set the above information for each one. A maximum of 110 SyroData structures can be transferred in one operation.  

### 3.3b calling the conversion start function

    SyroStatus SyroVolcaSample_Start(
        SyroHandle *pHandle, 
        SyroData *pData, 
        int NumOfData,
            uint32_t Flags,
        uint32_t *pNumOfSyroFrame
    );


The arguments are as follows:

    SyroHandle *pHandle         [out]pointer of the handle(resource for proceeding conversions) accquired after calling this function.
    SyroData *pData             [in]pointer of the SyroData structure prepared in 3.2
    int NumOfData               [in]the number of data to be converted (number of SyroData that has been prepared)
    uint32_t Flags              [in]a flag to be used during conversion. It is currently not used so set to 0.
    uint32_t *pNumOfSyroFrame   [out]the pointer of the value which stores the size of the SyroData after conversion
                                    Units in frames (an LR pair is one unit)

After successful conversion, Status\_Success is returned.  
Once this function is called, you **must** call SyroVolcaSample\_End to finish or terminate.

Refer to error codes in 3.4 in case of failure.


### 3.3c process SyroData per frame

Call the SyroVolcaSample_GetSample function

    SyroStatus SyroVolcaSample_GetSample(
        SyroHandle Handle, 
        int16_t *pLeft, 
        int16_t *pRight
    );

The arguments are as follows:

    SyroHandle Handle           the handle obtained at step 3.3b
    int16_t *pLeft              the pointer of an array which is about to store L channel output sample data
    int16_t *pRight             the pointer of an array which is about to store R channel output sample data

Store the obtained sample data as a wav file or output to an audio device.  
Repeat this for pNumOfSyroFrame times, set at step 3.3b.

### 3.3d calling the conversion end function

When you have generated all sample data and/or want to terminate the conversion, call the function:
    
    SyroStatus SyroVolcaSample_End(SyroHandle Handle)

Use the Handle obtained at step 3.3a.

### 3.4 About the returned value (SyroStatus)
    
Status\_Success will be returned after successful conversion.
Otherwise the following error code will be returned

    Status_IllegalDataType      an illegal data type has been specified. Must specify SyroDataType.
    Status_IllegalData          the data specified is abnormal
    Status_IllegalParameter     the number of data specified at SyroVolcaSample_Start is illegal (must be 1 - 110).
    Status_OutOfRange_Number    the specifed sample or sequence number is out of range
                                (sample number must be 0~99, sequence number must be 0~9)
    Status_OutOfRange_Quality   specified bit depth is out of range
                                (must be 8~16)
    Status_NotEnoughMemory      not enough memory

    Status_InvalidHandle        and invalid handle has been used
    Status_NoData               there is no more data to be converted

***

## 4. using the example to generate syrostream

The sample source code in the “example” folder generates syrostream in wav format.  

The VC2010 project and Makefile to build the sample source code is contained in the project folder.  
To create a project using an IDE, use and build the C source code inside the “example” and “syro” folders.  
To create an executable using command line or terminal, use Makefile inside the “project”.  
Type “make” for building. Default compiler is set to gcc. To use clang, type “make CC=clang”.  

To execute within the console type the following:  
   
    >korg_syro_volcasample_example "TargetFile.wav" "SourceFile1" "SourceFile2" ......

####korg\_syro\_volcasample\_example
- name of the sample executable file  
- this is assumed to be the same as the source file name, but is dependant on the build environment.  
    
####TargetFile.wav
- the name of the generated syrostream file.  
- set the extension here to .wav  
- when using spaces in the file name, always put in double quotes.  

####SourceFile
- specifies the data to be converted
- the data type and number are also set here.
- remember to double quote when using spaces.

the format is as follows:
        
        "x17c12:filename"
         TT~TT~T~~~~T~~~
         || || |    +-------    the file name to be converted
         || || |                not necessary when deleting samples
         || || +------------    use : as a seperater
         || ||                  necessary even when a file name is not specified
         || |+--------------    the bit depth of the conversion 8~16 bits. applicable only when converting samples
         || |                   assumes 16 bits if omitted
         || +---------------    lossless compression
         ||                     (it's lossless so always use it)
         |+-----------------    sets sample slot number (0~99) or sequence number (1~10)
         |                      do not specify when converting for all samples
         +------------------    sets type of data to be converted
                                s:sample
                                e:erase sample
                                p:sequence pattern
                                a:all sample
                                only 16bit mono wav samples are supported

examples:

    "s20c:kick.wav"                 converts kick.wav to be transfered to sample slot 20
    "s57c12:snare.wav"              converts snare.wav to be transfered to sample slot 57 at a quality of 12bits
    "e27:"                          generates syro stream to erase sample slot 27
    "p01:pattern1.dat"              converts pattern1.dat to be transfered to sequence 1
    "ac:volcasample_preset.alldata" converts volcasmple_preset.alldata to overwrite all samples

Multiples of these can be written in succession.  
example:

    "s0c:kick1.wav" "s1c:snare1.wav" converts kick1.wav to be transfered to sample slot 0 and snare1.wav to be transfered to sample slot 1.



***
## 5. Pattern data structure
    
SYRO for volca sample can convert sequence data as well.  
Here, we explain the structure of the sequence pattern data.
    
The structure is defined in VolcaSample\_PatternData in volcasample\_pattern.h.  
Always use little endian when storing 16 or 32 bit values.  
Reserved and Padding member names do not have meaning so their explanations are omitted.  

    uint32_t Header;
    uint16_t DevCode;

These are fields to specify that the data is pattern data for the volca sample.  
set to 0x54535450 and 0x33b8.  
(These are also defined as VOLCASAMPLE\_PATTERN\_HEADER、VOLCASAMPLE\_PATTERN\_DEVCODE)

    uint16_t ActiveStep;

Sets active step on/off as a bitmap  
steps 1~16 correspond to bits 0~15, 0=off and 1=on.  
this is usually set to 0xffff (all on)  
0 cannot be set since there would be no steps in the sequence.  
    
    uint32_t Footer;

Field to specify that the data is pattern data for the volca sample.  
set to 0x44455450  
(This is also defined as VOLCASAMPLE\_PATTERN\_FOOTER)  

Each part data is defined in **VolcaSample\_Part\_Data**
the following are contained in its members:

    uint16_t SampleNum;

the sample number (0~99) to be used.
        
    uint16_t StepOn;

a bit map to set step on/off  
steps 1~16 correspond to bits 0~15, 0=off and 1=on.
    
    uint16_t Accent;

this parameter cannot be operated on the volca sample. Must be set to 0.
    
    uint8_t Level;

this parameter cannot be operated on the volca sample. Must be set to 127.

    uint8_t Param[VOLCASAMPLE_NUM_OF_PARAM];

stores each knob value.
the array index and parameter names are as follows, with default values in brackets:

        0 : LEVEL           0~127, (127)
        1 : PAN             1~127, 64=Center (64)
        2 : SPEED           40~88, 64=Center (64) *changes speed in semitones (FUNC+SPEED operation)
                            129~255, 192=Centre   *changes speed continuously
        3 : AMP EG ATTACK   0~127 (0)
        4 : AMP EG DECAY    0~127 (127)
        5 : PITCH EG INT    1~127, 64=Center (64)
        6 : PITCH EG ATTACK 0~127 (0)
        7 : PITCH EG DECAY  0~127 (127)
        8 : START POINT     0~127 (0)
        9 : LENGTH          0~127 (127)
        10: HI CUT          0~127 (127)

these are defined as VOLCASAMPLE\_PARAM\_xxxx
        
    uint8_t FuncMemoryPart;

on/off for each part parameters  

        bit0 : Motion On/Off  
        bit1 : Loop On/Off  
        bit2 : Reverb On/Off  
        bit3 : Reverse On/Off  
        bit4 : Mute On/Off (1=mute off)  

these are defined as VOLCASAMPLE\_PARAM\_xxxx  
VOLCASAMPLE\_FUNC_BIT\_xxxx is number of bit,
        
    uint8_t Motion[VOLCASAMPLE_NUM_OF_MOTION][VOLCASAMPLE_NUM_OF_STEP];

motion sequence data.  
The 1st angle bracket([]) specifies the parameter to be motion sequenced. The 2nd specifies the step number.

        0 : LEVEL (start)
        1 : LEVEL (end)
        2 : PAN (start)
        3 : PAN (end)
        4 : SPEED (start)
        5 : SPEED (end)

the above parameters have values both for the start and end of each motion sequenced step.  
they are interpolated during playback of the step between the start and end values.

        6 : AMP EG ATTACK
        7 : AMP EG DECAY
        8 : PITCH EG INT
        9 : PITCH EG ATTACK
        10: PITCH EG DECAY
        11: SATRT POINT
        12: LENGTH
        13: HI CUT
        
The values for the motion sequence are as follows:

        speed                   knob value (0~127)
        all other parameters    knob value +128
        
        for all parameters, 0=no motion data
        
volcasample\_attern.c contains a function to initialize pattern data:  

    void VolcaSample_Pattern_Init(VolcaSample_Pattern_Data *pattern_data)

This will set the data to be initialized as default parameter values.

***
## <a name="heading_id_doc_6"></a>6. transferring syrostream to your volca sample

<img src="http://korginc.github.io/volcasample/images/connect.png" height="251" width="209">  

Connect the output of your playback device with a stereo cable to the SYNC IN port of your volca sample. Turn the volume up.  
Now playback the generated syrostream and the volca sample will enter receive mode.  

*Be careful when playing syro stream through speakers or headphones. Playback at large volumes may cause damage to equipment and/or ears.*

When volca sample detects syro stream input, [dAtA] and [data type] will be displayed alternately to show it is in receiving mode.  
[End] will be displayed on completion.
    
When an error occurs, [Err] and [error type] will be displayed alternately.  
Push the [FUNC] button to return to normal operation  

**Please make sure that volca sample system version is 1.2 or above.**


Visit our <a href="http://www.korg.com/volca_sample/" target="_blank">product page</a> for information on the latest system releases.  
The system version of your volca sample can be identified by holding [REC] while powering up. The 7-seg display will scroll through the following:  

    x.yy    main system version
    Px.yy   panel system version
    Sx.yy   sample version

where x.yy is the respective version number. SYRO functionality is not affected by panel or sample version, just make sure system version is 1.2 or above.

### 6.1 display codes while volca sample is receiving syro stream

    S.000 ~ S.099  : receiving sample to slot number 0~99
    P.001 ~ P.010  : receiving sequence patttern 1~10
    E.000 ~ E.099  : deleting sample slot 0~99
    ALL            : receiving all sample data

If an error occurs while receiving a sample, the sample in the receiving slot may be deleted.  
If an error occurs during all transfer, all sample memory will be deleted.    

### 6.2 troubleshooting errors

if you get an error try the following:

- is your volca sample firmware up to date? <a href="http://www.korg.com/products/dj/volca_sample/download.php" target="_blank">get latest here</a>.
- is the audio device and volca sample properly connected with a stereo cable?
- is the output level high enough?
- are there any other sounds interrupting? (message alerts??)
- are you sure all audio EQ/effects/optimizers are turned off?
- if you get [Err][tyPE] check your volca sample system is up to date
- if you get [Err][FuLL] free some memory by deleting samples  
    this can be done on your volca sample by using the delete menu (refer to the manual in the <a href="http://www.korg.com/products/dj/volca_sample/download.php" target="_blank">ver.1.22 update package</a>)  
    or by preparing and using sample delete syro streams.
- if you get [Err][btLo] change your batteries or use a power adapter.

If you keep having problems with certain sample data, please let us know <a href="https://github.com/korginc/volcasample/issues" target="_blank">here</a>.







