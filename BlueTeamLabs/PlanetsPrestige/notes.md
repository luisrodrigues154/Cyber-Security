# The Planet's Prestige

CoCanDa, a planet known as 'The Heaven of the Universe' has been having a bad year. A series of riots have taken place across the planet due to the frequent abduction of citizens, known as CoCanDians, by a mysterious force. CoCanDa’s Planetary President arranged a war-room with the best brains and military leaders to work on a solution. After the meeting concluded the President was informed his daughter had disappeared.  <br>

CoCanDa agents spread across multiple planets were working day and night to locate her. Two days later and there’s no update on the situation, no demand for ransom, not even a single clue regarding the whereabouts of the missing people. On the third day a CoCanDa representative, an Army Major on Earth, received an email. 

# Questions

## What is the email service used by the malicious actor?

From the extracted file (**A hope to CoCanda,eml**), we can see the answer in the headers
```
Received: from localhost (emkei.cz. [93.99.104.210])
        by mx.google.com with ESMTPS id s16si170171wmj.176.2021.01.25.22.41.18
        for <themajoronearth@gmail.com>
        (version=TLS1_2 cipher=ECDHE-ECDSA-CHACHA20-POLY1305 bits=256/256);
        Mon, 25 Jan 2021 22:41:18 -0800 (PST)
```

I really did not noticed the **emkei.cz** there, so i ran the **host** program with the IP **93.99.104.210**
```bash
cmd: host 93.99.104.210

210.104.99.93.in-addr.arpa domain name pointer emkei.cz.
```
**Answer:** emkei.cz

## What is the Reply-To email address? 

This information can also be extracted from the headers

```
Importance: Normal
Errors-To: billjobs@microapple.com
Reply-To: negeja3921@pashter.com
MIME-version: 1.0
```
**Answer:** negeja3921@pashter.com

## What is the filetype of the received attachment which helped to continue the investigation?

So, i did not open the **.eml** file in thunderbird this time, i decoded the base64 payload directly to a file

```bash
cmd: cat payload | base64 -d > decoded 
cmd: file decoded
decoded: Zip archive data, at least v2.0 to extract
```

**Answer:** .zip

## What is the name of the malicious actor? 

After extracting the **.zip** file, we have 3 files
```
PuzzleToCoCanDa
├── DaughtersCrown  (JPEG)
├── GoodJobMajor    (PDF)
├── Money.xlsx      (.xlsx)
```

Doing some basic analysis with **exiftool** we can see the answer when running it over **GoodJobMajor**

```
cmd: exiftool GoodJobMajor

(...)
Author                          : Pestero Negeja
(...)
```
**Answer:** Pastero Negeja

## What is the location of the attacker in this Universe?

This was a bit trickier. Running **binwalk** over all the files with the extract flag (**-e**) we end up with multiple files from the **.xlsx** file (which is expected) <br>

``` 
cmd: binwalk -e Money.xlsx

_Money.xlsx.extracted
├── 0.zip
├── [Content_Types].xml
├── _rels
└── xl
    ├── drawings
    │   ├── drawing1.xml
    │   └── drawing2.xml
    ├── _rels
    │   └── workbook.xml.rels
    ├── sharedStrings.xml
    ├── styles.xml
    ├── theme
    │   └── theme1.xml
    ├── workbook.xml
    └── worksheets
        ├── _rels
        │   ├── sheet1.xml.rels
        │   └── sheet2.xml.rels
        ├── sheet1.xml
        └── sheet2.xml
```

if we open the **sharedStrings.xml** we can see an interesting thing
```
</t></si><si><t>VGhlIE1hcnRpYW4gQ29sb255LCBCZXNpZGUgSW50ZXJwbGFuZXRhcnkgU3BhY2Vwb3J0Lg==</t></si></sst>
```

When decoded we have
```
The Martian Colony, Beside Interplanetary Spaceport
```

This string can be seen in an Excel-like application, in the second sheet. The thing is that it is with text color white, so it is not visible at first

**Answer:** The Martian Colony, Beside Interplanetary Spaceport

## What could be the probable C&C domain to control the attacker’s autonomous bots?

Well, i tried all the domains in the **.eml** file and found it was the email's domain

**Answer:** pashter.com