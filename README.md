# DomKey
Intercom key programmer

**Parts used:**
1) Arduino nano 3.0 **Atmega 328p** (RAM = 2kb, ROM = 32 kb)
2) Playe for ibutton key
3) Customized (re-soldered) **RDM 6300** (filter and generator are used. Generator can be turned off for RFID Write)

**Functional:**
This project is able to read and write *TM2004*, *RW1990*, maybe other ibutton keys, where similar write commands are used. RFIDcards based on EM-Marine protocol can be read. Raw signal from RDM6300 filter and in built comparator are used for reading. RFID field generator can be stopped by pulling down special custom pin of RDM6300, so there is full hardware support of EM-Marine write, however currently this functionality is not implemented.

When you run this project, you should connect controller via usb virtual com port, **mycom.py** script may be useful. Then enter “h” for **help message**:
```
Parameters list:
  k - show working Key
  w - Write key
    If key can not be determined send 'f'. It will force write commands. Helps recovering damaged keys
  n - enter New key
  s - Select key from db
    Warning '12' key may make your key unreadable. To recover it use force write
  r - Read key from ibotton-key to working key
  c - Change key type
  t - show key Type
  z - start writing of keyset series
  h - show this help
To abort pending operation send 'q'. It will return you to main menu
```

**Basic scheme:**
![alt text](https://github.com/ErrrOrrr503/DomKey/blob/master/images/basic_scheme.jpg)

**RDM6300 scheme:**
![alt text](https://github.com/ErrrOrrr503/DomKey/blob/master/images/rdm_6300.jpg)

**Finally, overview of our project:**
![alt text](https://github.com/ErrrOrrr503/DomKey/blob/master/images/overview.jpg)

Authors:
1) Egor Titov (https://github.com/ErrrOrrr503)
2) Ravil Zakiryanov (https://github.com/kilka-rav)
3) Sergey Koshelev (https://github.com/SergeyKoshelev)
