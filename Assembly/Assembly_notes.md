# Assembly notes (From book: Art of intel x86 Assembly)

## Decimal to binary

Converting 400d to binary…

|Divider|Value|Carry|
| :-: | :-: | :-: |
|2|400|-|
|2|200|0|
|2|100|0|
|2|50|0|
|2|25|0|
|2|12|1|
|2|6|0|
|2|3|0|
|2|1|1|
|2|0|1|

Binary value is obtained from carry column, from bottom to top… <br>

|Numbering|	Value|
| :-: | :-: |
| Decimal | 400|
| Binary  | 110010000|

## Binary to Hexadecimal

5h = \_\_ \* 2^3 + \_\_ \* 2^2 + \_\_ \* 2^1 + \_\_ \* 2^0

5h = 0 + 1 + 0 + 1

|**Hexadecimal**|**Binary**|
| :-: | :-: |
|0|0000|
|1|0001|
|2|0010|
|3|0011|
|4|0100|
|5|0101|
|6|0110|
|7|0111|
|8|1000|
|9|1001|
|A|1010|
|B|1011|
|C|1100|
|D|1101|
|E|1110|
|F|1111|


## Usage by size 


|Nomenclature|Size (possible values)|Usage|
| :-: | :-: | :-: |
|BIT|1 (0 or 1)|True/False (using a byte is easier for this)|
|NIBBLE|4 bits (2^4 = 16)|1 hex value|
|BYTE|8 bits (2^8 = 255), 2 nib|ascii chars|
|WORD|16 bits (2^16 = 65535), 4 nib|ints; offsets; segment values|
|DWORD|32 bits (2^32 = 4 billion+), 8 nib|ints/floats; segment addresses|

## Logical operations (on bits)
|**AND**|**0**|**1**|
| :-: | :-: | :-: |
|0|0|0|
|1|0|1|

<p> </p>


|**OR**|**0**|**1**|
| :-: | :-: | :-: |
|0|0|1|
|1|1|1|

<p> </p>


|**XOR**|**0**|**1**|
| :-: | :-: | :-: |
|0|0|1|
|1|1|0|

<p> </p>

|**NOT**|**0**|**1**|
| :-: | :-: | :-: |
||1|0|

**NEG**
* 2's compliment: 1's compliment (NOT) + 1
* NEG 1001 = 0110 + 1 = 0111

## Signed and Unsigned numbers (two’s complement) 

In x86 the Higher order bit represents if the number is positive or negative:
- 8000h is negative (1000 0000 0000 0000)
- 100h is positive (0001 0000 0000)

Converting (both ways):

1) Flip all the bits 
1) Add 1

Example:

* 0000 0101 (number 5)

* 1111  1010 (flipped)

* 1111 1011 (add 1 = -5) 

<br>

* 1111 1011	 (-5)

* 0000 0100 (flipped)

* 0000 0101 (add 1 = 5)


## Shifts

* Shift left n (SHL n)
  * Shifts the bits to the left n times
  * the last bit (7th) becomes carry
  * Same as multiplying by the number's radix (base) n times
* Shift right n (SHR n)
  * Shifts the bits n times
  * The first bit (0) becomes carry
  * Same as dividing by the number's radix (base) n times 
  * Originates problem when the number is signed
* Arithmetic shift right
  * Used with the explicit intention of dividing (for example)
  * Same as the SHR but preserves the initial bit
  * Rounds to the closest integer
  
## Rotate (Left or Right)

*  Same as Shifts
*  The bit that becomes carry in the shift, is the bit that enters when rotating
*  Example:
   *  1011 -> Rotate right: 1101

## ASCII things

* Upper and lowercase letters only differ in the bit 5
  * Example: 
    * E : 01000101
    * e : 01100101
* Bit 5 and 6 define the group of the character
  
  | Bit 6 | Bit 5 | Group |
  |:-:|:-:|:-:|
  | 0 | 0 | Control characters|
  | 0 | 1 | digits and punctuation|
  | 1 | 0 | Upper case & special|
  | 1 | 1 | Lower case & special|

## Registers

* AX - accumulator
* BX - base address
* CX - count
* DX - data

## Instruction Set
| Instruction | Functionality | Example|
|:-:|:-:|:-:|
| mov | move data between intervinients | mov reg, reg/mem/const_hex (and vice-versa)|
| add | add the 2nd to 1st, stores in 1st | add reg, reg/mem/const_hex |
| sub | sub the 2nd to 1st, stores in 1st | sub reg, reg/mem/const_hex |
| cmp | compare the 1st to 2nd, stores in flag | cmp reg, reg/mem/const_hex |
| and/or | bitwise logical operation, stores in 1st | and/or reg, reg/mem/const_hex |
| not | inverts the bits | not reg/mem |
| ja | jump if above | ja dest |
| jae | jump if above or equal | jae dest |
| jb | jump if below | jb dest |
| jbe | jump if below or equal | jbe dest |
| je | jump if equal | je dest |
| jne | jump if not equal | jne dest |
| jmp | unconditional jump | jmp dest |
| iret | return from an interrupt | iret |
| get | stops and reads hex value from user (stores in AX) | get |
| put | displays the value of AX (in hex) | put |
| halt | exits the program | halt |
| brk | pauses the program | brk | 

## Addressing modes

| Mode | Funtionality |Example |
| :-:|:-: |
| immediate | mov ax, cx ; mov bx, 1 |
| indirect | mov ax, [bx] |
| indexed | mov ax, [bx+1000] |
| direct | mov ax, [1000] |