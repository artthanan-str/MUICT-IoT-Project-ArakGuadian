'use strict';

const express = require('express');
const bodyParser = require('body-parser');
const line = require('@line/bot-sdk');
const app = express();
const credential = require('./credential.json');

const client = new line.Client({
    channelAccessToken: credential.LineChannelAccessToken
});

app.use(bodyParser.json()); 
app.use(bodyParser.urlencoded({ extended: true }));

// mock-up data
var tempValue = 25;
var flameValue = 10;
var humidValue = 70;

app.post('/GetRPIData', function(req, res) {
    if (req.body.type == "rpi_data") {
        tempValue = req.body.temperature
        humidValue = req.body.humidity
        flameValue = req.body.flame
        console.log("Temperature: " + tempValue);
        console.log("Himidity: " + humidValue);
        console.log("Flame level: " + flameValue);
    
        if (req.body.isFired == "Fired") {
            sendAlarmToUser();
            console.log("Server: Fire!!!!!");
        }
    
        res.json({
            status: "Received Message Successfully"
        })
    }
})

app.post('/', function(req, res) {
    console.log(req.body);
    var body = req.body

    if (body.queryResult.intent.displayName === "Check house status") {
        var currentStatus = checkHouseStatus();
        var dataString = "Current status: " ;
        dataString += "temperature is " + currentStatus[0];
        dataString += ", humidity is " + currentStatus[1];
        dataString += ", flame level is " + currentStatus[2];
        console.log(dataString)
    }
    else if (body.queryResult.intent.displayName === "Check temperature") {
        var dataString = "Current temperature: " + tempValue + "C";
        console.log(dataString)
    }
    else if (body.queryResult.intent.displayName === "Check humidity") {
        var dataString = "Current humidity: " + humidValue + "%";
        console.log(dataString)
    }
    else if (body.queryResult.intent.displayName === "Check flame") {
        var dataString = "Current flame level: " + flameValue + "nm";
        console.log(dataString)
    }

    res.json({
        fulfillmentMessages: [
            {
                card: {
                title: dataString
                }
            }
        ]
    })

});

app.listen((process.env.PORT || 8000), function() {
    console.log("Server up and listening");
});

function sendAlarmToUser(){
    var message = {
        type: 'text',
        text: 'Fire!!'
      };

    client.pushMessage(credential.LineUserID, message)
    .then(() => {
        console.log("Send Alarm Success");
    })
    .catch((err) => {
        console.log("Send Alarm Fail");
    });
}

function checkHouseStatus(){
    var status = [];
    if (tempValue > 30) { status[0] = "hot"; }
    else if (tempValue < 15) { status[0] = "cold"; }
    else { status[0] = "so so"; }

    if (humidValue > 75) { status[1] = "humid"; }
    else if (humidValue > 50 && humidValue <= 75) { status[1] = "normal"; }
    else { status[1] = "dry"}

    if (flameValue < 1050) { status[2] = "normal"; }
    else { status[2] = "quite hot, be careful!!"; }

    return status
}