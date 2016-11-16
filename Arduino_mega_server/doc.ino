/*
  Modul Documentation
  part of Arduino Mega Server project
*/

/* 
ANALOG
pin A15 - b8 ch
pin A14 - b7 ch
pin A13 - b6 ch
pin A12 - b5 ch
pin A11 - b4 ch
pin A10 - b3 ch
pin  A9 - b2 ch
pin  A8 - b1 ch
pin  A7
pin  A6
pin  A5 - a5 ch
pin  A4 - a4 ch
pin  A3 - a3 ch
pin  A2 - a2 ch
pin  A1 - a1 ch
pin  A0 - U

DIGITAL
pin 53
pin 52 - SCK  (NRF24L01)
pin 51 - MOSI (NRF24L01)
pin 50 - MISO (NRF24L01)
.
.
pin 31 - cont CLO
pin 30 - pir CLO
pin 29
pin 28
pin 27 - cont HOM
pin 26 - pir PRH
pin 25 - pir KUH
pin 24 - pir KID
pin 23 - pir HAL
pin 22 - pir STD

pin 21 SCL - RTC
pin 20 SDA - RTC
pin 19 RX1 - NOO MT1132
pin 18 TX1 - NOO MT1132
pin 17 RX2 - NOO MR1132
pin 16 TX2 - NOO MR1132
pin 15 RX3 - NRF24L01
pin 14 TX3 - NRF24L01
pin 13 Ethernet        (SCK)
pin 12 Ethernet        (MISO)
pin 11 Ethernet  [PWM] (MOSI)
pin 10 Ethernet  [PWM] (SS for Ethernet controller)
pin 9 - NRF24 (CSN)
pin 8 - NRF24 (CE)
pin 7 - ledG
pin 6 - ledR
pin 5 - ledB
pin 4 Ethernet (SS for SD card)
pin 3 [PWM] - key
pin 2 - 1-Wire
pin 1 [TX] x
pin 0 [RX] x


nRF24 packet structure
======================
01234567890123456789012345678901
.........ppppppppddddddddddddddd
        9       8             15

 buff[0] - packet type
 buff[1] - command
 buff[2] - address
 buff[3] - destination address
 buff[4] - required answer
 buff[5] - packet number
 buff[6] - reserve
 buff[7] - reserve
 buff[8] - reserve
 buff[9] - password 1
buff[10] - password 2
buff[11] - password 3
buff[12] - password 4
buff[13] - password 5
buff[14] - password 6
buff[15] - password 7
buff[16] - password 8
buff[17] - data 1
buff[18] - data 2
buff[19] - data 3
buff[20] - data 4
buff[21] - data 5
buff[22] - data 6
buff[23] - data 7
buff[24] - data 8
buff[25] - data 9
buff[26] - data 10
buff[27] - data 11
buff[28] - data 12
buff[29] - data 13
buff[30] - data 14
buff[31] - data 15

packet type
===========
0 - empty
1 - command
2 - request
3 - data
4 - alarm
5 - answer
6 - echo

*/
