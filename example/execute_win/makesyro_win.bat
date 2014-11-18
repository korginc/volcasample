set SYRO_SRC_FILE=02 Kick 3.wav
set SYRO_NO=2
set SYRO_TARGET_FILE=syro_stream.wav

set SYRO_PATH=
%SYRO_PATH%syro_volcasample_example %SYRO_TARGET_FILE% "s%SYRO_NO%c:%SYRO_PATH%%SYRO_SRC_FILE%"
pause
