const fs = require('fs');
const path = require('path');

let myVariable = 120265;

const NODE_PROCESS_INFO_FILENAME = 'node_target_info.json'

function saveProcessInfo() {
    const info = {
        pid: process.pid,
        timestamp: Date.now()
    };
    
    fs.writeFileSync(NODE_PROCESS_INFO_FILENAME, JSON.stringify(info, null, 2));
    console.log(`PID: ${process.pid}`);
}

saveProcessInfo();

const sharedBuffer = Buffer.alloc(8);
sharedBuffer.writeDoubleLE(myVariable, 0);

setInterval(() => {
    const currentBufferValue = sharedBuffer.readDoubleLE(0);

    console.log('setInterval', currentBufferValue)
    
}, 1000);

process.stdin.resume();

process.on('SIGINT', () => {
    process.exit();
});