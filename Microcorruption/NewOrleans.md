
The first thing to check is main and what is done (regarding function calls and manipulations)

The first thing that catches the attention is the call to the function <b> create_password </b>.

This function moves bytes to memory to memory (starting at address 0x2400).
The last byte that is copied to memory is a null terminator, so, and since this is the create_password function, it is most likely a string (the password).

If we break at ret and analyze the address 0x2400 we can see the following byte

```
0x2400: 667c 2f74 6439 2c00 (...)
```

We can submit that if we check the checkbox, but just convert to ASCII and submit.

```
The password: f|/td9,
```