# 6502

Eventually this will become an NES emulator. But for now, I'm just working on the 6502 part. I'm going to use this readme to log weird little things I find during the process of building the emulator.

## The P Register

This register stores status flags. Its setup is a little strange, though; the 5th bit is always 1. I haven't figured out why yet.

## Shifting

The 6502 shifts are essentially the opposite of the specified C shifts. That is, the 6502 supports signed left shifts, and unsigned right shifts. This is a little strange because it seems simpler to implement signed right shifts and unsigned left shifts. I haven't figured out a smart way to handle this yet.


