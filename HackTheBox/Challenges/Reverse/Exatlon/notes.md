# Exatlon

## Strings

Strings showed some potential flags but they are encrypted (might aswell not be flags)

## Run the program
Just prompts for password and show the banner

## check if packed

use the DIE (detect it easy) to check if it is packed

```
diec.sh exatlon_v1

packer: UPX(3.95)[NRV,brute]
```

Lets extract using UPX

```
upx -d exatlon_v1 -o clean 
```

## Load in ghidra

