// store frequency values for 8 octives of musical notes
// const PROGMEM byte notes[] = {
//   31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 93,
//   98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233,
//   247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587,
//   622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397,
//   1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960,
//   3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978
// };
// // how many notes are there?
// const PROGMEM int noteCount = sizeof(notes);

//  Song data
byte pattern[4][32]= {
  { 32,  0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0 },
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
};

// byte patternReset[4][32]={
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
// };

const int DIAL_MAX_VAL = 1010;
const int DIAL_MIN_VAL = 10;


// game vars
String messageTxt = "";


// synth vars
byte bar;
boolean paused = false;
unsigned int counter=0;
byte note = 32;
unsigned char currentVoice = 0;
byte tempo = 30;                   //-Tempo for the playback
boolean playing = true;

const int OUTPUTPIN = 6;
volatile unsigned int PCW[4]={
  0,0,0,0};                       //-Wave phase accumolators
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
  10,20,30,40};                   //-Envelope speed tuning word
volatile unsigned int tim=0;      //-Sample counter eg. for sequencer
volatile unsigned int tim2=0;     //-Sample counter eg. for sequencer

volatile unsigned char divider=4; //-Sample rate decimator for envelope
volatile unsigned char tick=0;
volatile unsigned char envg=0;

// bitmap faces
const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  wink_bmp[] =
  { B00111100,
    B01000010,
    B10000101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  blink_bmp[] =
  { B00111100,
    B01000010,
    B10000001,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },
  sin_bmp[] =
  { B00100000,
    B01010000,
    B10001000,
    B10001000,
    B00001000,
    B00000101,
    B00000101,
    B00000010 },
  tri_bmp[] =
  { B00011000,
    B00011000,
    B00100100,
    B00100100,
    B01000010,
    B01000010,
    B10000001,
    B10000001 },
  square_bmp[] =
  { B11110000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00011111 },
  saw_bmp[] =
  { B10000000,
    B11000000,
    B10100000,
    B10010000,
    B10001000,
    B10000100,
    B10000010,
    B10000001 },
  ramp_bmp[] =
  { B00000001,
    B00000011,
    B00000101,
    B00001001,
    B00010001,
    B00100001,
    B01000001,
    B10000001 },
  noise_bmp[] =
  { B01011001,
    B00100010,
    B10101100,
    B00111001,
    B10001000,
    B10111000,
    B00101101,
    B01010011 },
  env1_bmp[] =
  { B11000000,
    B00100000,
    B00010000,
    B00010000,
    B00001000,
    B00001000,
    B00000100,
    B00000011 },
  env2_bmp[] =
  { B10000000,
    B01000000,
    B01000000,
    B00100000,
    B00100000,
    B00010000,
    B00001100,
    B00000011 },
  env3_bmp[] =
  { B11000000,
    B00100000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00001000,
    B00000111 },
  env4_bmp[] =
  { B00000000,
    B00000000,
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    B00001000,
    B00000111 };


