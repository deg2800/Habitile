#include "SoundHandler.h"
#include "pitches.h"

bool sound = true;

void playMelody(int melody[], int noteDurations[], int numNotes) {
if (sound) {
    for (int thisNote = 0; thisNote < numNotes; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        const int speakerPin = 5;;
tone(speakerPin, melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);

        noTone(speakerPin);
    }
  }
}


void StartUpSound() {
    int melody[] = {NOTE_C5, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, 0, NOTE_B4, NOTE_C5};
    int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
    int numNotes = sizeof(melody) / sizeof(melody[0]);
    
    playMelody(melody, noteDurations, numNotes);
}

void CompletedSound() {
    int melody[] = {NOTE_B3, NOTE_C4};
    int noteDurations[] = {8, 4};
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void UncompleteSound() {
    int melody[] = {NOTE_C4, NOTE_B3};
    int noteDurations[] = {8, 4};
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void ScrollUpSound() {
    int melody[] = {NOTE_B4};
    int noteDurations[] = {8};
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void ScrollDownSound() {
    int melody[] = {NOTE_B3};
    int noteDurations[] = {8};
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void ConfirmSound() {
    int melody[] = {NOTE_C5, NOTE_C5};
    int noteDurations[] = {8, 8};
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void CustomMelody() {
    int melody[] = {NOTE_A4, NOTE_C5, NOTE_B4, NOTE_D5};
    int noteDurations[] = {8, 4, 8, 4}; // 8 is an eighth note, 4 is a quarter note.
    int numNotes = sizeof(melody) / sizeof(melody[0]);

    playMelody(melody, noteDurations, numNotes);
}

void ToggleSoundBool() {
    sound = !sound;
}

bool GetSoundBool() {
    return sound;
}