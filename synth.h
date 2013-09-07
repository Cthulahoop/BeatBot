//**************************************************************************
//*   Simple AVR wavetable synthesizer V1.0                                *
//*                                                                        *
//*   Implements 4 voices using selectable waveform and envelope tables    *
//*   Uses 8-bit PWM @ 62.5 kHz for audio output                           *
//*                                                                        *
//*   (C) DZL 2008                                                         *
//**************************************************************************

/* *************************************************************************
 *
 *   Modified by Ric Ewing for the MakerHaus BeatBot 6000 project.
 *   Thanks to DZL for permission to use and distribute.
 *
 *   v.1.5  20.08.2013
 *    - added tempo change
 *   v.1.4  18.08.2013
 *    - removed MIDI centric comments
 *    - fixed wave modification methods
 *    - added pitch modification
 *   v1.3
 *    - fixed voice modification methods
 *    - commented out new samples (needs modification to work)
 *   v1.2
 *    - moved all wave tables into a single header
 *    - added new sample tables
 *   v.1.1  08.2013
 *    - removed MIDI code
 *
 */

#include <math.h>

#include "avr/interrupt.h"
#include "avr/pgmspace.h"
#include "tables.h"

#define SET(x,y) (x |=(1<<y))		   //-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))  // |
#define CHK(x,y) (x & (1<<y))      // |
#define TOG(x,y) (x^=(1<<y))       //-+


//*********************************************************************
// vars
//*********************************************************************

unsigned char currentVoice = 1;
int tempo = 30;                   //-Tempo for the playback
boolean playing = true;

int OUTPUTPIN = 6;
volatile unsigned int PCW[4]={
  0,0,0,0};			                  //-Wave phase accumolators
volatile unsigned int FTW[4]={
  1000,200,300,400};              //-Wave frequency tuning words
volatile unsigned char AMP[4]={
  255,255,255,255};               //-Wave amplitudes [0-255]
volatile unsigned int PITCH[4]={
  500,500,500,500};               //-Voice pitch
volatile int MOD[4]={
  20,0,64,127};                   //-Voice envelope modulation [0-1023 512=no mod. <512 pitch down >512 pitch up]
volatile unsigned int wavs[4];    //-Wave table selector [address of wave in memory]
volatile unsigned int envs[4];    //-Envelopte selector [address of envelope in memory]
volatile unsigned int EPCW[4]={
  0x8000,0x8000,0x8000,0x8000};   //-Envelope phase accumolator
volatile unsigned int EFTW[4]={
  10,10,10,10};                   //-Envelope speed tuning word
volatile unsigned int tim=0;      //-Sample counter eg. for sequencer
volatile unsigned int tim2=0;     //-Sample counter eg. for sequencer

volatile unsigned char divider=4; //-Sample rate decimator for envelope
volatile unsigned char tick=0;
volatile unsigned char envg=0;

unsigned char synthTick(void)
{
  if(tick)
  {
    tick=0;
    return 1;
  }
  return 0;
}


#define FS 16000.0  //-Sample rate

SIGNAL(TIMER1_COMPA_vect) {
  if(playing) {
    OCR1A+=2000000/FS;  //-Auto sample rate

    if(divider) {
      divider--;
    }
    else {

      //************************************************
      // Volume envelope generator
      //************************************************

      divider=4;

      if(!(EPCW[0]&0x8000)) {
        AMP[0]=pgm_read_byte(envs[0]+ ((EPCW[0]+=EFTW[0])>>7) );
        if(EPCW[0]&0x8000)
          AMP[0]=0;
      }
      else {
        AMP[0]=0;
      }

      if(!(EPCW[1]&0x8000)) {
        AMP[1]=pgm_read_byte(envs[1]+ ((EPCW[1]+=EFTW[1])>>7) );
        if(EPCW[1]&0x8000) {
          AMP[1]=0;
        }
      }
      else {
        AMP[1]=0;
      }

      if(!(EPCW[2]&0x8000)) {
        AMP[2]=pgm_read_byte(envs[2]+ ((EPCW[2]+=EFTW[2])>>7) );
        if(EPCW[2]&0x8000) {
          AMP[2]=0;
        }
      }
      else {
        AMP[2]=0;
      }

      if(!(EPCW[3]&0x8000)) {
        AMP[3]=pgm_read_byte(envs[3]+ ((EPCW[3]+=EFTW[3])>>7) );
        if(EPCW[3]&0x8000) {
          AMP[3]=0;
        }
      }
      else {
        AMP[3]=0;
      }
    }

    //************************************************
    //  Synthesizer/audio mixer
    //************************************************
    OCR0A=127+
      ((
      (((signed char)pgm_read_byte(wavs[0]+((PCW[0]+=FTW[0])>>8))*AMP[0])>>8)+
      (((signed char)pgm_read_byte(wavs[1]+((PCW[1]+=FTW[1])>>8))*AMP[1])>>8)+
      (((signed char)pgm_read_byte(wavs[2]+((PCW[2]+=FTW[2])>>8))*AMP[2])>>8)+
      (((signed char)pgm_read_byte(wavs[3]+((PCW[3]+=FTW[3])>>8))*AMP[3])>>8)
      )>>2);

    tim++;
    tim2++;

    //************************************************
    //  Modulation engine
    //************************************************

    // if(tim>FS/20) // = tim > 1200
    if(tim>FS/tempo)
    {
      switch(envg)
      {
      case 0:
        {
          FTW[0]=PITCH[0]+(PITCH[0]*(EPCW[0]/(32767.5*128.0  ))*((int)MOD[0]-512));
          envg++;
        };
        break;
      case 1:
        {
          FTW[1]=PITCH[1]+(PITCH[1]*(EPCW[1]/(32767.5*128.0  ))*((int)MOD[1]-512));
          envg++;
        };
        break;
      case 2:
        {
          FTW[2]=PITCH[2]+(PITCH[2]*(EPCW[2]/(32767.5*128.0  ))*((int)MOD[2]-512));
          envg++;
        };
        break;
      case 3:
        {
          FTW[3]=PITCH[3]+(PITCH[3]*(EPCW[3]/(32767.5*128.0  ))*((int)MOD[3]-512));
          envg++;
        };
        break;
      case 4:
        {
          tim=0;
          envg=0;
          tick=1;
        };
        break;
      }
    }
  }
}

//*********************************************************************
//  Setup all voice parameters
//*********************************************************************

void setup_voice(unsigned char voice,unsigned int waveform, float pitch, unsigned int envelope, float length, unsigned int mod)
{
  wavs[voice]=waveform;//[address in program memory]
  envs[voice]=envelope;//[address in program memory]
  EFTW[voice]=(1.0/length)/(FS/(32767.5*10.0));//[s];
  PITCH[voice]=pitch/(FS/65535.0); //[Hz]
  MOD[voice]=mod;//0-1023 512=no mod
}

//*********************************************************************
//  Setup wave
//*********************************************************************

void setup_wave(unsigned char voice,unsigned char wave)
{
  switch(wave)
  {
  case 1:
    wavs[voice]=(unsigned int)SinTable;
    break;
  case 2:
    wavs[voice]=(unsigned int)TriangleTable;
    break;
  case 3:
    wavs[voice]=(unsigned int)SquareTable;
    break;
  case 4:
    wavs[voice]=(unsigned int)SawTable;
    break;
  case 5:
    wavs[voice]=(unsigned int)RampTable;
    break;
  case 6:
    wavs[voice]=(unsigned int)NoiseTable;
    break;
  default:
    wavs[voice]=(unsigned int)SinTable;
    break;
  }
}

//*********************************************************************
//  Setup Envelope
//*********************************************************************

void setup_env(unsigned char voice,unsigned char env)
{
  switch(env/16)
  {
  case 1:
    envs[voice]=(unsigned int)Env0;
    break;
  case 2:
    envs[voice]=(unsigned int)Env1;
    break;
  case 3:
    envs[voice]=(unsigned int)Env2;
    break;
  case 4:
    envs[voice]=(unsigned int)Env3;
    break;
  default:
    envs[voice]=(unsigned int)Env0;
    break;
  }
}


unsigned int EFTWS[128];

//*********************************************************************
//  Setup Length
//*********************************************************************

void setup_length(unsigned char voice,unsigned char length)
{

//  EFTW[voice]=EFTWS[length];
//  EFTW[voice]=(1.0/length)/(FS/(32767.5*10.0));//[s];
    if(length > 10) {
      length= length^2;
    }
    else if(length > 15) {
      length= length^3;
    }
    EFTW[voice]=length;
}

//*********************************************************************
//  Setup mod
//*********************************************************************

void setup_mod(unsigned char voice, unsigned int mod)
{
  MOD[voice]=mod;//0-1023 512=no mod
}


//*********************************************************************
//  Setup pitch
//*********************************************************************

void setup_pitch(unsigned char voice, float pitch)
{
  PITCH[voice]=pitch/(FS/65535.0); //[Hz]
}


//*********************************************************************
//  Setup tempo
//*********************************************************************

void setup_tempo(int beat)
{
  tempo = constrain(beat,10,100);
}

unsigned int PITCHS[128];
unsigned int FTWS[128];

//*********************************************************************
//  Midi trigger
//*********************************************************************

void mtrigger(unsigned char voice,unsigned char note)
{
  //  PITCH[voice]=(440. * exp(.057762265 * (note - 69.)))/(FS/65535.0); //[MIDI note]
  PITCH[voice]=PITCHS[note];
  EPCW[voice]=0;
  FTW[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
}

//*********************************************************************
//  Simple trigger
//*********************************************************************

void trigger(unsigned char voice)
{
  EPCW[voice]=0;
}

//*********************************************************************
//  Init synth
//*********************************************************************

void initSynth()
{
  for(unsigned char i=0;i<128;i++)
  {
    EFTWS[i]=(1.0/exp(.057762265 * (i - 69.)))/(FS/(32767.5*10.0));//[s];
    PITCHS[i]=(440. * exp(.057762265 * (i - 69.)))/(FS/65535.0);
    // FTWS[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
  }
  TCCR1B=0x02;                                    //-Start audio interrupt
  SET(TIMSK1,OCIE1A);                             // |
  sei();                                          //-+

  TCCR0A=0x83;                                    //-8 bit audio PWM
  TCCR0B=0x01;                                    // |
  OCR0A=127;                                      //-+
  SET(DDRD,OUTPUTPIN);                            //-PWM pin
}

void pauseSynth() {
  TCCR1B=0x03;                                    //-Start audio interrupt
  CLR(TIMSK1,OCIE1A);                             //-+

  TCCR0A = 0x03;
  TCCR0B = 0x03;
  OCR0A = 0;
  CLR(DDRD,OUTPUTPIN);

  playing = false;
}

void unpauseSynth() {
  TCCR1B=0x02;
  SET(TIMSK1,OCIE1A);

  TCCR0A=0x83;
  TCCR0B=0x01;
  OCR0A=127;
  SET(DDRD,OUTPUTPIN);
  playing = true;
}

