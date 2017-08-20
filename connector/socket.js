module.exports = {
    init,
};

const handler = require('../handler');

function init(socket) {
    socket.on('connection', (client) => {
        client.on('livingroom:light:on', handler.livingRoom.onLivingRoomLightOn);
        client.on('livingroom:light:off', handler.livingRoom.onLivingRoomLightOff);
    });
}
