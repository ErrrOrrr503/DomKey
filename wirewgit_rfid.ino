#include <OneWire.h>

// RFID uses in-built comparator at pin_7 (inv input) pin_6 is connected to inbuilt voltage source 1v1
#define _IBUTTON_PIN 8
#define _CAPACITY 20
#define _BLINK_LED LED_BUILTIN
#define _FREQGEN 11
#define _RFIG_PIN_EXT_CMP 6
#define _BAUD_RATE 115200

// key_types
#define _RW1990 1
#define _NONAME 0

#define _ERR_WRONG_INPUT 32766 //big enough not to be real input number

//// types ////

// a set of keys to write
struct keyset {
  const byte amount;
  const byte *bd;
};

//// global data ////

const byte bd_10[10] = {0, 1, 3, 7, 8, 10, 11, 4, 6, 9};
const struct keyset keysets[1] = {10, bd_10};

const byte BD[_CAPACITY][8] = 
{
/*0*/  0x01, 0xBE, 0x40, 0x11, 0x5A, 0x36, 0x00, 0xE1, // green, black_nekit_with_green_label VIZIT 99% //1 //1
/*1*/  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x9B, // red //green_nekit_3                                                     //2 //3
/*2*/  0x01, 0x78 ,0x00, 0x48, 0xFD, 0xFF, 0xFF, 0xD1, // green_nekit_2
/*3*/  0x01, 0xA9, 0xE4, 0x3C, 0x09, 0x00, 0x00, 0xE6, // black_nekit_1 ( with pink lable) ELTIS 90%            //3 //4
/*4*/  0x01, 0x76, 0xB8, 0x2E, 0x0F, 0x00, 0x00, 0x5C, // FORWARD (nekit + forum)                                        //8
/*5*/  0x01, 0x52, 0xE5, 0xB9, 0x0C, 0x00, 0x00, 0xCC, // red_nekit METAKOM 
/*6*/  0x01, 0x00, 0xBE, 0x11, 0xAA, 0x00, 0x00, 0xFB, // yellow_nekit KEYMAN                                               //9

/*7*/  0x01, 0x53, 0xD4, 0xFE, 0x00, 0x00, 0x00, 0x6F, // - домофоны Vizit - до 99%                                       //4 //5
/*8*/  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xA0, // - Metakom 95%                                                             //5 
/*9*/  0x01, 0x00, 0x00, 0x00, 0x00, 0x90, 0x19, 0xFF, //Отлично?? работает на старых домофонах     //10

/*10*/ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, // - UK-3 Cyfral                                                                //6
/*11*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x14, // - Открываает 98% Metakom и некоторые Cyfral   //7 //2
/*12*/ 0x00, 0x36, 0x5A, 0x11, 0x40, 0xBE, 0x01, 0x71, // block)))
/*13*/ 0x03, 0x4E, 0x94, 0x46, 0x08, 0x00, 0x00, 0x89  // blue_nekit_with_pink_lable CRC: 45
};

//// global strings ////

const char wrong_input_err[] = "Wrong input: '";

const int drec_logo_size = 2626; //bytes or symbols
const PROGMEM char drec_logo[] = "\
                                                   .-:://////:-.`                       ./:-`       \n\
                                              .:+osso+/:::--:://+o+:.              ./oyo:`          \n\
                                           ./sss/-.               `.:+/-       .+ydh+.              \n\
                                         -oss/.                        -/: ./smmy/`                 \n\
                                       -oso-                            -odmmh+`                    \n\
                                     `+ss-                           .odmmms/`                      \n\
                                    `os+`                          /hmmmms.  ..                     \n\
                                   `os/                           smmmmh-     ``                    \n\
                                   +s+                           ommmmo        `                    \n\
                             `.-:/+hhssssyyyyyys:               :mmmm/                              \n\
                  .-:/osyhdmNmmmmmmmmmmmmmmmmmmmmms-            dmmm+                               \n\
         .-/+syhhhyymmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmd+.        +mmmy                                \n\
`..-:://+/:-.`  `/sdhyyso+/:--.`` os.  `-/ohmmmmmmmmmmmy:      dmmm.                                \n\
                                  +s.       `:smmmmmmmmmmd+`  /mmms      `.-::`                     \n\
                                  -s:          `/dmmmmmmmmmms-dmmm/-/oshmmmms.                      \n\
                                   +s`            :hmmmmmmmmmmmmmmmmmmmmmy/`                        \n\
                                   `o/              ommmmmmmmmmmmmmmmy+-                            \n\
                                    `o/        .:ohmmmmmmmmmmmmhs+:`                                \n\
                                      //  `:/+o+/dmmmmmhyo+:-`                                      \n\
                                       .+/.`   `++/:.`                                              \n\
                                         .:.                                                        \n\
                                           `..`                                                     \n\
                                              ````     `+ooooNhosoo: .Msoooood+ yo  `/yy/` -oooNhooo\n\
                                                       yy`   m/   .M..M`    `sh yh+yh/`        d+   \n\
                                                       yh`   m/```-M..Msooooo+` ys.+hy:        d+   \n\
                                                       `+ooooNhooo+- .M`        yo   `+ys-     d/   \n\
";

const int help_message_size = 570;
const PROGMEM char help_message[] = "\
Params list:\n\
  k - show working Key\n\
  w - Write key\n\
    If key can not be determined send 'f'. It will force write commands. Helps recovering damaged keys\n\
  n - enter New key\n\
  s - Select key from db\n\
    Warning '12' key may make your key unreadable. To recover it use force write\n\
  r - Read key from ibotton-key to working key\n\
  c - Change key type\n\
  t - show key Type\n\
  z - start writing of keyset series\n\
  h - show this help\n\
...pss... don`t you think about trying undocumented params\n\
To abort pending operation send 'q'. It will return you to main menu\n\
";


//// global variables ////

// current working key type
byte t_key = _NONAME;

// RFID debugging bit string
byte debug_bits[64] = {0};

// Working new key that is written into ibuttons
byte N_ID[8];

// Working keyset
struct keyset keyset_series = keysets[0];

//ibutton object (onewire.h library) used only for reading as read protocol is standard
OneWire ibutton (_IBUTTON_PIN);

void setup (){
  // turn on 40mhz hse for wawgat atmega clone (lgt8f328p) board
#ifdef WAWGAT_40MHZ
  sysClock(EXT_OSC);
#endif
  pinMode (_BLINK_LED, OUTPUT);
  digitalWrite (_BLINK_LED, LOW);
  // turn on RFID staff like comparator or freqgen if needed
  RFID_AC_setOn ();
  Serial.begin (_BAUD_RATE);
  for (byte i = 0; i < 8; i++) {
    N_ID[i] = BD[0][i];
  }
}

void loop ()
{
  static bool ready_flag = 0;
  while (!Serial) {
    digitalWrite (_BLINK_LED,!digitalRead(_BLINK_LED));   //blink while waiting for serial connection
    delay(100);
    ready_flag = 0;
  }
  if (!ready_flag)
    Serial.println ("Ready");
  ready_flag = 1;
  digitalWrite (_BLINK_LED, HIGH); // poweron led when connected
  
  char tmp = Serial.read();
  switch (tmp) {
    case 'k':
      show_key (N_ID);
      break;
    case 'w':
      write_main (N_ID);
      break;
    case 'n':
      get_new_key (N_ID);
      break;
    case 's':
      select_new_key (N_ID);
      break;
    case 'h':
      print_help ();
      break;
    case 'r':
      read_key (N_ID);
      break;
    case 'e':
      easter_egg ();
      break;
    case 'c':
      change_key_type ();
      break;
    case 't':
      show_key_type ();
      break; 
    case 'z':
      write_series (keyset_series);
      break;
    default:
      std_behavior ();
      RFID_std_behavior ();
      break;
  };
}

void print_help ()
{
  for (int i = 0; i < help_message_size; i++)
    Serial.print ((char) pgm_read_byte (help_message + i));
}

void easter_egg ()
{  
  for (int i = 0; i < drec_logo_size; i++)
    Serial.print ((char) pgm_read_byte (drec_logo + i));
}

void change_key_type ()
{
  Serial.println ("Available key types are:");
  Serial.println ("0) Noname");
  Serial.println ("1) RW1990");
  Serial.println ("default is 0");
  
  Serial_skipread (20);
  while (Serial.available () == 0) ;
  if (Serial.peek () == 'q') {
    Serial.println ("Quitting");
    return;
  }
  int new_t_key = Serial_read_dec_num ();
  if (new_t_key < 0 || new_t_key >1) {
    Serial.print (wrong_input_err);
    Serial.print (new_t_key);
    Serial.println ("'");
    change_key_type ();
    return;
  }
  t_key = new_t_key;
  Serial.print ("Switching to ");
  Serial.print (t_key);
  Serial.println (") key type");
}

void writeByte (byte data)
{
  int data_bit;
  for(data_bit=0; data_bit<8; data_bit++){
    if (data & 1)
      writeBit_0 ();
    else
      writeBit_1 ();
    data = data >> 1;
  }
}

void writeBit_0 ()
{
  pinMode(_IBUTTON_PIN, OUTPUT); digitalWrite(_IBUTTON_PIN, LOW);
  delayMicroseconds(60);
  digitalWrite(_IBUTTON_PIN, HIGH);
  delay(10);
  pinMode(_IBUTTON_PIN, INPUT);
}

void writeBit_1 ()
{
  pinMode(_IBUTTON_PIN, OUTPUT); digitalWrite(_IBUTTON_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(_IBUTTON_PIN, HIGH);
  delay(10);
  pinMode(_IBUTTON_PIN, INPUT);
}

void writeRW1990 (const byte *addr)
{
  Serial.println ("KEY type is RW1990");
  ibutton.skip();
  ibutton.reset();
  ibutton.write(0xD1);
  writeBit_0 (); 
      
  ibutton.skip();
  ibutton.reset();
  ibutton.write(0xD5);
  for (byte i = 0; i < 8; i++){
    writeByte(addr[i]);
    Serial.print('*');
  }
  Serial.print('\n');

  ibutton.skip();
  ibutton.reset();
  ibutton.write(0xD1);
  writeBit_1 ();
  ibutton.reset();
}

void writeNONAME (const byte *addr)
{
  Serial.println ("KEY type is NONAME");
  ibutton.skip();
  ibutton.reset();
  ibutton.write(0xD5);
  for (byte i = 0; i < 8; i++){
    writeByte(addr[i]);
    Serial.print('*');
  }
  Serial.print('\n');
  ibutton.reset();
}

byte write_main (const byte *ID)
{
  Serial.print ("  Writing iButton: ");
  show_key (ID);
  Serial.println ("   Waiting...");
  byte addr[8];
  while (!ibutton.search (addr)) {
    ibutton.reset_search ();
    char tmp = Serial.read ();
    if (tmp == 'q') {
      Serial.println ("Quitting");
      return 1;
    }
    if (tmp == 'f')
      break;
  } 
  Serial.print ("  Before write: ");
  show_key (addr);
  byte new_crc = ibutton.crc8(ID, 7);
  if (new_crc == ID[7]) {
    Serial.print("   CRC correct: ");
    Serial.println(new_crc, HEX);
  }
  else {
    Serial.println("CRC incorrect");
    return -1;
  }

  delay (500);
  Serial.println ("WRITING");
  if (t_key == _RW1990) {
    writeRW1990 (ID);
  }
  if (t_key == _NONAME) {
    writeNONAME (ID);
  }
  delay (100);
  ibutton.reset_search ();
  while (ibutton.search(addr)) {
      ibutton.reset_search ();
      show_key (addr);
      for (int i = 0; i < 8; i++) {
        if (addr[i] != ID[i]) {
          Serial.println ("Retrying");
          delay (500);
          return write_main (ID);
        }
      }
      delay (500);
  }
  return 0;
}

void write_series (struct keyset keyset)
{
  for (byte i = 0; i < keyset.amount; i++) {
    const byte *newID = &BD[keyset.bd[i]][0];
    if (write_main (newID))
      Serial.println ("X");
  }
}

void get_new_key (byte *newID)
{
  Serial_skipread(20);
  Serial.println("Enter key like this: 1 FF 23 fe 3 0 00 5D");
  char tmp = ' ';
  byte ID[8];
  byte i = 0, is_good = 0, incr_byte = 0, started = 0;
  for (i = 0; i < 8; i++)
    ID[i] = 0;
  i = -1;
  while (Serial.available() == 0)
    ;
  if (Serial.peek() == 'q') {
    Serial.println("Quitting");
    return;
  }
  do {
    is_good = 0;
    while (!started && (tmp == ' ' || tmp == '\t')) {
      tmp = Serial_waitread();
      incr_byte = 1;
    }
    started = 1;
    if (tmp == -1 || tmp == '\n' || tmp == '\0') {
      incr_byte = 0;
      is_good = 1;
    }
    if (i == 7 && incr_byte) {
      Serial.println("Too big KEY");
      get_new_key(newID);
    }
    if (tmp <= '9' && tmp >= '0') {
      if (incr_byte) {
        i++;
        incr_byte = 0;
      }
      is_good = 1;
      ID[i] = ID[i] * 16 + (tmp - '0');
    }
    if (tmp >= 'A' && tmp <= 'F') {
      if (incr_byte) {
        i++;
        incr_byte = 0;
      }
      is_good = 1;
      ID[i] = ID[i] * 16 + (tmp - 'A' + 10);
    }
    if (tmp >= 'a' && tmp <= 'f') {
      if (incr_byte) {
        i++;
        incr_byte = 0;
      }
      is_good = 1;
      ID[i] = ID[i] * 16 + (tmp - 'a' + 10);
    }
    if (!is_good) {
      Serial.print(wrong_input_err);
      Serial.print(tmp);
      Serial.println("'");
      get_new_key(newID);
      return;
    }
    tmp = Serial_waitread();
    if (tmp == ' ' || tmp == '\t')
      started = 0;
  } while (tmp != -1 && tmp != '\n' && tmp != '\0');
  if (i < 7) {
    Serial.println("Corrupted key");
    get_new_key(newID);
    return;
  }
  Serial.print("Changing to ");
  show_key(ID);
  for (i = 0; i < 8; i++)
    newID[i] = ID[i];
}

void std_behavior() {
  byte addr[8];
  if (!ibutton.search(addr)) {
    ibutton.reset_search();
    return;
  }
  Serial.print(millis() / 1000);
  Serial.print("> ");
  show_key(addr);
  delay(700);
}

void read_key (byte *ID)
{
  Serial.println ("Waiting for read...");
  while (!ibutton.search (ID)) {
    if (Serial.read () == 'q') {
      Serial.println ("Quitting");
      return;
    }
    ibutton.reset_search();
    delay(100);
  }
  Serial.print("Read: ");
  show_key (ID);
}

void select_new_key (byte *ID)
{
  Serial_skipread (20);
  Serial.println ("Enter key number in BD");
  while (Serial.available () == 0) ;
  if (Serial.peek () == 'q') {
    Serial.println ("Quitting");
    return;
  }
  
  int new_num = Serial_read_dec_num ();
  
  Serial.print ("Changing to N");
  Serial.print (new_num);
  Serial.print (" ");
  if (new_num >= _CAPACITY) {
    Serial.println ("A ty ne ()}{Ye|!?");
    select_new_key (ID);
    return;
  }
  for (int i = 0; i < 8; i++) {
    ID[i] = BD[new_num][i];
  }
  show_key (ID);
}

void show_key (const byte *ID)
{
  Serial.print ("KEY : ");
  for (byte i = 0; i < 8; i++) {  
    Serial.print (ID[i], HEX);
    Serial.print (" ");
  }
  byte crc;
  crc = ibutton.crc8 (ID, 7);
  Serial.print ("CRC: ");
  Serial.println (crc, HEX);
}

int Serial_read_dec_num ()
{
  byte is_good = 0, started = 0;
  int new_num = 0, sign = 1;
  char tmp = '\0';
  do {
    is_good = 0;
    //skip space symbols
    while (!started && (tmp == ' ' || tmp == '\t' || tmp == '\n' || tmp == '\0')) {
      tmp = Serial_waitread ();
    }
    //analyze first non space symbol and determine sign
    if (!started && tmp == '-')
      sign = -1;
    started = 1;
    //read numbers
    if (tmp <= '9' && tmp >= '0') {
      is_good = 1;
      new_num = new_num * 10 + (tmp - '0');
    }
    if (!is_good) {
      Serial.print (wrong_input_err);
      Serial.print (tmp);
      Serial.println ("'");
      return _ERR_WRONG_INPUT;
    }
    tmp = Serial_waitread ();
  } while (tmp != -1 && tmp != '\n' && tmp != '\0' && tmp != ' ' && tmp != '\t');
  return new_num * sign;
}

char Serial_waitread ()
{
  const unsigned timeout = 10; // ms
  unsigned long strt = millis ();
  while ((Serial.available () == 0) && (millis () - strt <= timeout)) ;
  return Serial.read ();
}

int Serial_waitread (const unsigned timeout)
{
  unsigned long strt = millis ();
  while ((Serial.available () == 0) && (millis () - strt <= timeout)) ;
  return Serial.read ();
}

int Serial_skipread (const unsigned timeout)
{
  char tmp = -1;
  int num_read = 0;
  unsigned long strt = millis ();
  while (millis () - strt <= timeout) {
    if ((tmp = Serial.read ()) != -1)
      num_read++;
  }
  return num_read;
}

void show_key_type ()
{
    switch (t_key) {
    case _NONAME:
      Serial.println ("KEY_TYPE : NONAME");
      break;
    case _RW1990:
      Serial.println ("KEY_TYPE : RW1990");
      break;
    default:
      break;
  }
}



void RFID_AC_setOn ()
{
#if 0  
  //125KHz generator on _FREQGEN is not needed, instead inbuilt 125KHz generator of RDM6300 is used.
  pinMode (_FREQGEN, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);  //Toggle on Compare Match at pin 11. Timer 2 counting up to OCR2A
  TCCR2B = _BV(WGM22) | _BV(CS20);                               //Timer 2 divisor is 1 (16 MHz)
  OCR2A = 63;                                                    //63 tacts/period 16000/64/2 = 125 KHz, Duty at pin11 is always 50% 
  OCR2B = 31;                                                    //Duty on COM2B (pin3) 32/64 = 50%. Frequency is 16000/64 = 250 KHz
#endif  
  // Comparator init
  ADCSRB &= ~(1<<ACME);          //AC multiplexor off
  ACSR &= (1<<ACBG);             //AIN0 (pin6) to 1.1V
  ACSR &= (1<<ACD);              //comp on
}

uint8_t RFID_ReadBit (uint32_t timeout_micros) //Manchesters code
{ 
  #define _AC_STATE ((ACSR>>ACO)&1)
  // return 0 or 1 or 2 if timeout
  uint32_t t_end = micros () + timeout_micros;
  uint8_t adc_comp_init_state = _AC_STATE;
  uint8_t adc_comp_current_state = 2;
  while (_AC_STATE == adc_comp_init_state && micros () < t_end) ;
  if (_AC_STATE != adc_comp_init_state) {
    delayMicroseconds (125);    // 1/4 Period on 2 kBps 
    adc_comp_current_state = _AC_STATE;                
    delayMicroseconds (250);   // 1/2 Period on 2 kBps 
    return adc_comp_current_state;  
  }
  return 2;
}

uint8_t RFID_ReadCard (uint8_t* buf, uint32_t timeout)
{
  uint32_t t_start = millis ();
  uint8_t Bit = 0;
  uint8_t j = 0;
  uint8_t tmp_nibble = 0;
  for (uint8_t i = 0; i < 5; i++) buf[i] = 0;
  for (uint8_t i = 0; i < 64; i++) {         //start reading 64bit
    if (millis () - t_start > timeout) {
      return 2; //timeout
    }
    Bit = RFID_ReadBit (10000);
    debug_bits[i] = Bit;
    if ((Bit != 1) && (i <= 8)) {            //checking 9 starting ones
      i = -1;
      j = 0;
      tmp_nibble = 0;
    }
    
    if ((i >= 9) && (i <= 58)) {             //reading NIBBLE : DATA PARITY
      if (((i - 9) % 5) != 4)
        tmp_nibble = tmp_nibble * 2 + Bit;
      else {                                 //checking PARITY Bit
        if (( (   (tmp_nibble & 1) + ((tmp_nibble & 2) >> 1) + ((tmp_nibble & 4) >> 2) + ((tmp_nibble & 8) >> 3)   ) & 1) != Bit) { //PARITY incorrect
          char debug_str[80] = {0};
          sprintf (debug_str, "Parity incorrect nibble: %" PRIx8 " Par. bit: %" PRIu8 " at %" PRIu8 "  nibble\n", tmp_nibble, Bit, (i - 8) / 5);
          Serial.print (debug_str);

          for (uint8_t di = 0; di < 64; di++)
            Serial.print (debug_bits[di]);
          Serial.println ("");
          i = -1;
          j = 0;
        }
        else { //PARITY correct
          if (!(j & 1))
            buf[j / 2] += (tmp_nibble << 4);
          else
            buf[j / 2] += tmp_nibble;
          j++;
        }
        tmp_nibble = 0;
      }
    }

    if ((i >= 59) && (i <= 62)) { //checking PARITY_NIBBLE of all the code
      //FIXME: complex and not really needed as nubble parity is enough
    }

    if ((Bit != 0) && (i == 63)) { //checking terminating 0
      i = -1;
      j = 0;
      tmp_nibble = 0;
    }
  }
  return 0;
}

void RFID_std_behavior ()
{
  uint8_t buf[5];
  char str[15];
  delay (20);
  for (uint8_t i = 0; i < 5; i++)
    buf[i] = 0;
  if (!RFID_ReadCard (buf, 100)) {
    sprintf (str, "%02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8, buf[0], buf[1], buf[2], buf[3], buf[4]);
    Serial.println (str);
    delay (600);
  }
}
