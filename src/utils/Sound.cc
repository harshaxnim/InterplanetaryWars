#include "sound.h"

/******** CLASS VARIABLE DECLARATIONS ********/

bool Sound::on = true; //is sound on?
bool Sound::possible = true; //is it possible to play sound?
char * Sound::currentSound; //currently played sound
//FMOD-specific stuff
FMOD_RESULT Sound::result;
FMOD_SYSTEM * Sound::fmodsystem;
FMOD_SOUND * Sound::sound;
FMOD_CHANNEL * Sound::channel;

/******** METHODS' IMPLEMENTATIONS ********/

//initialises sound
void Sound::initialise (void) {
    //create the sound system. If fails, sound is set to impossible
    result = FMOD_System_Create(&fmodsystem);
    if (result != FMOD_OK) possible = false;
    //if initialise the sound system. If fails, sound is set to impossible
    if (possible) result = FMOD_System_Init(fmodsystem,2, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) possible = false;
    //sets initial sound volume (mute)
    if (possible) FMOD_Channel_SetVolume(channel,0.0f);
}

//sets the actual playing sound's volume
void Sound::setVolume (float v) {
    if (possible && on && v >= 0.0f && v <= 1.0f) {
        FMOD_Channel_SetVolume(channel,v);
    }
}

//loads a soundfile
void Sound::load (const char * filename) {
    currentSound = (char *)filename;
    if (possible && on) {
        result = FMOD_Sound_Release(sound);
        result = FMOD_System_CreateStream(fmodsystem,currentSound, FMOD_SOFTWARE, 0, &sound);
        if (result != FMOD_OK) possible = false;
    }
}

//frees the sound object
void Sound::unload (void) {
    if (possible) {
        result = FMOD_Sound_Release(sound);
    }
}

//plays a sound (no argument to leave pause as dafault)
void Sound::play (bool pause) {
    if (possible && on) {
        result = FMOD_System_PlaySound(fmodsystem,FMOD_CHANNEL_FREE, sound, pause, &channel);
        FMOD_Channel_SetMode(channel,FMOD_LOOP_NORMAL);
    }
}

//toggles sound on and off
void Sound::toggleSound (void) {
    on = !on;
    if (on == true) { load(currentSound); play(); }
    if (on == false) { unload(); }
}

//pause or unpause the sound
void Sound::setPause (bool pause) {
    FMOD_Channel_SetPaused (channel, pause);
}

//turn sound on or off
void Sound::setSound (bool s) {
    on = s;
}

//toggle pause on and off
void Sound::togglePause (void) {
    FMOD_BOOL p;
    FMOD_Channel_GetPaused(channel,&p);
    FMOD_Channel_SetPaused (channel,!p);
}

//tells whether the sound is on or off
bool Sound::getSound (void) {
    return on;
}