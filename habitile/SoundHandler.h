#ifndef SoundHandler_h
#define SoundHandler_h

#include "Arduino.h"

// Function declarations
void playMelody(int melody[], int noteDurations[], int numNotes);
void StartUpSound();
void CompletedSound();
void UncompleteSound();
void CustomMelody();
void ScrollUpSound();
void ScrollDownSound();
void ConfirmSound();
void BTReceiveSound();
void ToggleSoundBool();
bool GetSoundBool();
void SetSoundPrefs();
void SetSoundBoolToPrefValue();

#endif

extern bool sound;
