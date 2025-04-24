const http = require("http");
const fs = require("fs");
const path = require("path");
const WebSocket = require("ws");
const net = require('net');
const PORT = 3000;

function createPacket(command, deviceId, data = {}) {
    const header = "FEEF";
    const buffer = [];

    // Helper to convert number to hex byte
    const toHexByte = (num) => num.toString(16).padStart(2, '0').toUpperCase();

    // Push type and command
    buffer.push(0x02);
    buffer.push(command);

    // Push 4-byte device ID (big endian)
    buffer.push((deviceId >> 24) & 0xFF);
    buffer.push((deviceId >> 16) & 0xFF);
    buffer.push((deviceId >> 8) & 0xFF);
    buffer.push(deviceId & 0xFF);

    // Push 2-byte data length (big endian)
    const dataBytes = [...Buffer.from(data, 'utf-8')];
    const dataLength = dataBytes.length;
    buffer.push((dataLength >> 8) & 0xFF);
    buffer.push(dataLength & 0xFF);

    // Append data bytes
    buffer.push(...dataBytes);

    // Compute checksum (XOR of everything except header)
    const checksum = buffer.reduce((acc, byte) => acc ^ byte, 0x00);
    buffer.push(checksum);

    // Convert all to hex
    const hexPacket = header + buffer.map(toHexByte).join('');
    return hexPacket;
}

/**********
 * From REST Server --> GPS Server
 * ********/
const client = net.createConnection({port: 8181, host: 'localhost'}, () => {
    console.log('Connected to GPS server');
});
client.on('data', (data) => {
    console.log('Received:', data.toString());
});
client.on('end', () => {
    console.log('Disconnected from server');
});
client.on('error', (err) => {
    console.error('TCP error:', err.message);
});
/**********
 * From REST Server --> Client Console
 * ********/
const server = http.createServer((req, res) => {
    if (req.url === "/") {
        const filePath = path.join(__dirname, "web", "index.html");
        fs.readFile(filePath, (err, content) => {
            if (err) {
                res.writeHead(500);
                res.end("Error loading page");
            } else {
                res.writeHead(200, {"Content-Type": "text/html"});
                res.end(content);
            }
        });
    } else {
        res.writeHead(404);
        res.end("Not Found");
    }
});
/**********
 * From REST Server --> Client Socket
 * ********/
// WebSocket server for devices
const wss = new WebSocket.Server({server, path: "/ws/device"});
wss.on("connection", (ws) => {
    console.log("Device connected via WebSocket");

    ws.on("message", (message) => {
        console.log("Received from device:", message);
        client.write(`<<MSG>>${createPacket(0x01, 0x12340008, `{'name':'Client'}`)}<<END>>`);
        ws.send("ACK: " + message);
    });

    ws.on("close", () => {
        console.log("Device disconnected");
    });
});
// Start the server
server.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
});