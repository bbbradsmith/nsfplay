#ifndef _MIDI_INTERFACE_H_
#define _MIDI_INTERFACE_H_

/** 標準的なMIDI音源のインタフェース */
class MIDIDeviceI
{
public:
  /**
   * ノートON velocityが0の時は消音
   * MIDIメッセージは $9x, $note, $velocity
   */
  virtual void NoteOn(int ch, int note, int velocity)=0;
  /**
   * ノートOFF velocityは音が消える速さ
   * MIDIメッセージは $8x, $note, $velocity
   */
  virtual void NoteOff(int ch, int note, int velocity)=0;
  
  /**
   * ポリフォニックキープレッシャ
   * MIDIメッセージは $Ax, $pressure
   */
  virtual void PolyKeyPressure(int ch, int note, int pressure)=0;

  /**
   * チャンネルプレッシャ
   * MIDIメッセージは $Dx, $pressure
   */
  virtual void ChannelPressure(int ch, int pressure)=0;

  /**
   * ピッチベンド
   * MIDIメッセージは $Ex, $data
   */
  virtual void PitchBendChange(int ch, int data)=0;
  
  /**
   * コントロールチェンジ
   * MIDIメッセージは $Bx, $ctrl_no, $data
   */
  virtual void ControlChange(int ch, int ctrl_no, int data)=0;

  /**
   * プログラムチェンジ
   * MIDIメッセージは $Cx, $prg_no
   */
  virtual void ProgramChange(int ch, int prg_no)=0;

  /**
   * モードメッセージ
   * MIDIメッセージは $Bn, $mode, $data   $mode は $78～$7f
   */
  virtual void ModeMessage(int ch, int mode, int data)=0;

  /**
   * エクスクルーシブメッセージ
   * MIDI メッセージは $F0, $id, $data, ... $F7
   */
  virtual void ExclusiveMessage(int id, int *data, int size)=0;
};




#endif