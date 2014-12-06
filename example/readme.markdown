The "example" folder contains sample executables that will let you quickly try
out converting sample audio data into syrostream.

### Launch the executable to generate syrostream
*File names in brackets for Mac. Windows otherwise.*

- double click makesyro_win.bat (makesyro_mac.command) in folder example/execute_win (example/execute_mac)
- syro_stream.wav will be generated

You have just performed conversion of "02 Kick 3.wav" contained in the same folder into syrostream "syro_stream.wav" with sample sample slot number 2. Play "syro_stream.wav" into the SYNC IN port of your volca and the sample will be transfered to slot number 2.

\*refer to [6. transferring syrostream to your volca sample](http://korginc.github.io/volcasample/documentation.html#heading_id_doc_6) in documentation for details.

### To specify your own sample audio data and sample slot number

- place your sample in wav format in folder example/execute_win (example/execute_mac)  
- open makesyro_win.bat (makesyro_mac.command) in a text editor.  
- change "02 Kick 3.wav" in line 1 to your sample file name.  
- change the number "2" in line 2 to a sample slot of your choice (0-99).  
- save the changes.  

Now double click "makesyro_win.bat" ("makesyro_mac.command") as before and the new "syro_stream.wav" will be generated from your specified sample file and slot number.  

### GNU/Linux example

See the `execute_gnulinux` directory for a precompiled binary for x86_64 and a
wrapper script.

The wrapper script `makesyro_gnulinux.sh` can be used both interactively and
non-interactively. It is possible to setup script parameters for source file,
sample number, output file, and also play the resulting converted syro stream.

Playing the resulting syro stream requires `aplay`.
