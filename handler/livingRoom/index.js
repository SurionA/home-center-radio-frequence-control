'use strict';

const exec = require('child_process').exec;

module.exports = {
  onLivingRoomLightOn,
  onLivingRoomLightOff
};

function onLivingRoomLightOn() {
  exec('sudo ' + __dirname + '/../../lib/scripts/Emetteur 3 ' + __dirname + '/../../lib/actions/livingRoom/LightOn.csv', (error, stdout, stderr) => {
    if (error) {
      console.error(`exec error: ${error}`);
      return;
    }
    console.log(`stdout: ${stdout}`);
    console.log(`stderr: ${stderr}`);
  });
}

function onLivingRoomLightOff() {
  exec('sudo  ' + __dirname + '/../../lib/scripts/Emetteur 3  ' + __dirname + '/../../lib/actions/livingRoom/LightOff.csv', (error, stdout, stderr) => {
    if (error) {
      console.error(`exec error: ${error}`);
      return;
    }
    console.log(`stdout: ${stdout}`);
    console.log(`stderr: ${stderr}`);
  });
}
