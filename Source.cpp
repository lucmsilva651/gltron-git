#include "Source.h"

namespace Sound {
  Source::Source() {
    _system = NULL;
    _isPlaying = 0;
    _loop = 0;
    _removable = 0;
    _volume = 0.5;
  }
  void Source::Mix(Uint8 *data, int len) { }
  void Source::Reset() { }
  void Source::Start() { _isPlaying = 1; Reset(); }
  void Source::Stop() { _isPlaying = 0; Reset(); }
  void Source::Pause() { _isPlaying = 0; }
  void Source::UnPause() { _isPlaying = 1; }

  Uint8 Source::IsPlaying() { return _isPlaying; }
  void Source::SetRemovable() { _removable = 1; }
  void Source::SetVolume(float volume) { _volume = volume; }
  float Source::GetVolume() { return _volume; }
  void Source::SetLoop(Uint8 loop) { _loop = loop; }
  Uint8 Source::GetLoop() { return _loop; }
}
