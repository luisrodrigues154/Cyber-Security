# Cryptohorrific

After uncompressing the challenge files, i went looking around to see what platform was it <br>

Found out, from the file  **_CodeSignature/CodeResources** that it was an iOS application. <br>

Also found out that IDA could decompile/disassemble it (Mach-o x86-64) so i went rigth into the code <br>


Found some interesting functions which i'll name below (i classified them as interesting because i know a very little bit of iOS nomenclature):
- viewDidLoad (when the app starts)
- _main (smililar to libc_start_main, prepares to execute the app)
- SecretManager:key:iv:data (some encryption over here)

The **_main** function is out right of the bat, nothing really important since we have the symbols for **viewDidLoad** <br>


The **viewDidLoad** function do some stuff with some resources and after that it calls the function **SecretManager:key:iv:data** with that data.

```objective-c

v11 = objc_msgSend(v9, "pathForResource:ofType:", CFSTR("challenge"), CFSTR("plist"));
v13 = v12;
v14 = objc_msgSend(v7, "initWithContentsOfFile:", v12);
v15 = v14;
v16 = objc_msgSend(v14, "objectAtIndex:", 0LL);
v17 = (void *)objc_retainAutoreleasedReturnValue(v16);
v18 = v17;
v19 = objc_msgSend(v17, "objectForKey:", CFSTR("flag"));
v20 = objc_retainAutoreleasedReturnValue(v19);
v21 = v20;
v22 = objc_msgSend(v6, "initWithBase64EncodedString:options:", v20, 0LL);
v23 = v22;
v24 = -[ViewController SecretManager:key:iv:data:](
        v4,
        "SecretManager:key:iv:data:",
        1LL,
        "!A%D*G-KaPdSgVkY",
        "QfTjWnZq4t7w!z%C"),
        v22);
v25 = objc_retainAutoreleasedReturnValue(v24);
v26 = v25;
v27 = objc_msgSend(v5, "initWithData:encoding:", v25, 4LL);
objc_msgSend((void *)v2, "setText:", v27, v27)
``` 

In a high level quick analysis what it is doing is quite understandable:
1. It opens the file **challenge.plist**
2. Creates one element at index 0 (key,value pair) 
3. Set its key to **flag**
4. And its content will be a **cipher text base64** encoded

The next step is check the **challenge.plist** file. Its contents are somewhat readable but we cant really extract it with cat because it is a binary file 

```bash
cmd: file challenge.plist 
challenge.plist: Apple binary property list
```

Found out to exist a tool called **plutil** on MacOS to convert **.plist** files to XML. Its equivalent in linux is **plistutil**

```bash
cmd: plistutil -i challenge.plist
# -- snip --
<array>
    <dict>
        <key>flag</key>
        <string>Tq+CWzQS0wYzs2rJ+GNrPLP6qekDbwze6fIeRRwBK2WXHOhba7WR2OGNUFKoAvyW7njTCMlQzlwIRdJvaP2iYQ==</string>
# -- snip --
``` 

Now lets check the encryption ! <br>

The encryption usage can be easily seen in the IDA output, it's not obfuscated. If we go back in the code we can easily map what is being passed. <br>

Also went ahead and search the function prototype in Apple's documentation so i could make sense of what is actually happening <br>

[Prototype](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/CCCrypt.3cc.html) <br>

[Definitions](https://opensource.apple.com/source/CommonCrypto/CommonCrypto-36064/CommonCrypto/CommonCryptor.h)

```objective-c
if ( (unsigned int)
        CCCrypt(v8,         // CCOperation op    -> kCCEncrypt/kCCDecrypt
                0LL,        // CCAlgorithm alg   -> kCCAlgorithmAES128
                3LL,        // CCOptions options 
                &v25,       // const void *key   -> "!A%D*G-KaPdSgVkY"
                16LL,       // size_t keyLength    
                &v24,       // const void *iv    -> "QfTjWnZq4t7w!z%C"
                v10,        
                v16, 
                v14, 
                v16 + 16, 
                &v13) 
    )
```

For a while i was confused because the default method is CBC (which uses an IV, as it is passed) but it was not decrypting for anything <br>

I didn't knew what thos options were so i went search a bit and found something interesting in a [chinese website](https://la0s.github.io/2018/12/07/iOS_Crypto/) (which completely passed above my head), the options used will enter in ECB mode which does not use an IV at all <br>

So, althought the IV is being passed, it is being discarded. Now, we are able to decrypt the flag. First lets decode it

```bash
cmd: echo -n "Tq+CWzQS0wYzs2rJ+GNrPLP6qekDbwze6fIeRRwBK2WXHOhba7WR2OGNUFKoAvyW7njTCMlQzlwIRdJvaP2iYQ==" | base64 -d > flag.enc
```
And then upload to [this](http://aes.online-domain-tools.com/) website which allows the decryption of files and to insert the key as plaintext <br>

## Flag: HTB{%SoC00l_H4ckTh3b0xbyBs3cur31stCh4ll3ng3!!Cr4zY%}
