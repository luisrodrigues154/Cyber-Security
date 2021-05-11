var root = require("google-cloudstorage-commands");
const express = require('express');
const { exec } = require("child_process");     
const bodyParser = require('body-parser');     
const _ = require('lodash');                                                                  
const app = express();
var fs = require('fs');
                                                                                              
const users = [                                                                               
  {name: 'felamos', password: 'Winter2021'},
  {name: 'admin', password: Math.random().toString(32), canDelete: true, canUpload: true},      
];

let messages = [];                             
let lastId = 1;                                
                                                                                              
function findUser(auth) {                                                                     
  return users.find((u) =>                                                                    
    u.name === auth.name &&                                                                   
    u.password === auth.password);                                                            
}                                    
                                               
app.use(bodyParser.json());                                                                   
                                               
app.get('/', (req, res) => {                   
  res.send(messages);                                                                         
});                                                                                           
                                                                                              
app.put('/', (req, res) => {   
  const user = findUser(req.body.auth || {});                                                 
                                               
  if (!user) {                                 
    res.status(403).send({ok: false, error: 'Access denied'});                                
    return;
  }

  const message = {
    icon: '__',
  };

  _.merge(message, req.body.message, {
    id: lastId++,
    timestamp: Date.now(),
    userName: user.name,
  });

  messages.push(message);
  res.send({ok: true});
});

app.delete('/', (req, res) => {
  const user = findUser(req.body.auth || {});

  if (!user || !user.canDelete) {
    res.status(403).send({ok: false, error: 'Access denied'});
    return;
  }

  messages = messages.filter((m) => m.id !== req.body.messageId);
  res.send({ok: true});
});
app.post('/upload', (req, res) => {
  const user = findUser(req.body.auth || {});
  if (!user || !user.canUpload) {
    res.status(403).send({ok: false, error: 'Access denied'});
    return;
  }


  filename = req.body.filename;
  root.upload("./",filename, true);
  res.send({ok: true, Uploaded_File: filename});
});

app.post('/todo', (req, res) => {
    const user = findUser(req.body.auth || {});
    if (!user) {
        res.status(403).send({ok: false, error: 'Access denied'});
        return;
    }

    filename = req.body.filename;
    testFolder = "/usr/src/app";
    fs.readdirSync(testFolder).forEach(file => {
        if (file.indexOf(filename) > -1) {
                var buffer = fs.readFileSync(filename).toString();
                res.send({ok: true, content: buffer});
        }
    });
});

app.listen(3000);
console.log('Listening on port 3000...');