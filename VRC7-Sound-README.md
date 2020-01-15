# VRC7-Sound

Emulator for the expansion sound of the Konami VRC7.

## About

VRC7-Sound is an open-source emulator for the Konami VRC7's expansion sound, featured in the Famicom game *Lagrange Point*.
The VRC7 enables FM-style music through the use of a modified Yamaha YM2413 (OPLL) architecture, which is itself based on the
YM3812, a chip prominently used in the AdLib and Sound Blaster soundcards. The emulator attempts to recreate this sound as accurately
as possible by relying mostly on information gathered from the VRC7's die shot.

## Usage

To use VRC7-Sound, simply embed the source files into your C/C++ project. No additional libraries are required. Detailed 
description of how to use the emulator and it's specific functions are given in the header file (vrc7_sound.h).

## Contact
Questions? Feedback? Bugs? Let me know:

Jonas Rinke (Delphi1024)  
delphi1024 (аt) gmail (dоt) com

## References
* VRC7 die shot by digshadow:
  * https://siliconpr0n.org/archive/doku.php?id=digshadow:konami:vrc_vii_053982

* YM2413 (OPLL) Datasheet:
  * http://www.smspower.org/maxim/Documents/YM2413ApplicationManual

* VRC7 audio on Nesdev Wiki:
  * https://wiki.nesdev.com/w/index.php/VRC7_audio

* OPLx logSin/exp/ksl tables by Olli Niemitalo and Matthew Gambrell:
  * http://yehar.com/blog/?p=665
  * https://docs.google.com/document/d/18IGx18NQY_Q1PJVZ-bHywao9bhsDoAqoIn1rIm42nwo/edit

* Instrument ROM Dump by NukeYKT:
  * https://siliconpr0n.org/archive/doku.php?id=vendor:yamaha:opl2#ym2413_instruments

* Lagrange point filter analysis by kevtris:
  * http://forums.nesdev.com/viewtopic.php?t=4709&p=41523