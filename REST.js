const http = require("http");
const fs = require("fs");
const path = require("path");
const readline = require("readline");
const WebSocket = require("ws");
const net = require('net');
const PORT = 3000;
const fifoPath = "EC200U_pipe";

function hexStringToBytes(hexStr) {
    const bytes = [];
    for (let i = 0; i < hexStr.length; i += 2) {
        bytes.push(parseInt(hexStr.substr(i, 2), 16));
    }
    return bytes;
}

function sliceToInt(bytes, start, end) {
    let value = 0;
    for (let i = start; i < end; ++i) {
        value = (value << 8) | bytes[i];
    }
    return value;
}

function computeChecksum(bytes) {
    return bytes.reduce((acc, b) => acc ^ b, 0);
}

function hexToUtf8(byteArray) {
    return Buffer.from(byteArray).toString("utf8");
}

function wrapMessage(msg) {
    return `<<MSG>>${msg}<<END>>`;
}

function unwrapMessage(msg) {
    const start = '<<MSG>>';
    const end = '<<END>>';
    const s = msg.indexOf(start);
    const e = msg.indexOf(end);
    if (s !== -1 && e !== -1 && e > s) {
        return msg.substring(s + start.length, e);
    }
    return '';
}

function writeToFifo(fifoPath, message) {
    if (!fs.existsSync(fifoPath)) {
        fs.mkfifoSync(fifoPath, 0o666);
    }

    try {
        const fd = fs.openSync(fifoPath, "w");
        fs.writeSync(fd, message);
        fs.closeSync(fd);
        console.log(`Sent to ${fifoPath}: ${message.trim()}`);
    } catch (err) {
        console.error(`❌ Error writing to ${fifoPath}:`, err.message);
    }
}

class Packet {
    constructor(command, data) {
        this.header = "FEEF";
        this.packetType = 0x02;
        this.command = command;
        this.deviceId = 0x12345678;
        this.data = Buffer.from(data, "utf8");

        this.dataLength = 4 + 2 + this.data.length;
    }

    computeChecksum(rawBytes) {
        return rawBytes.reduce((acc, b) => acc ^ b, 0);
    }

    createPacket() {
        const rawBytes = [];

        rawBytes.push(this.packetType);
        rawBytes.push(this.command);

        rawBytes.push((this.deviceId >> 24) & 0xff);
        rawBytes.push((this.deviceId >> 16) & 0xff);
        rawBytes.push((this.deviceId >> 8) & 0xff);
        rawBytes.push(this.deviceId & 0xff);

        rawBytes.push((this.dataLength >> 8) & 0xff);
        rawBytes.push(this.dataLength & 0xff);

        for (const b of this.data) {
            rawBytes.push(b);
        }

        const checksum = this.computeChecksum(rawBytes);

        return this.header +
            rawBytes.map((b) => b.toString(16).padStart(2, "0").toUpperCase()).join("") +
            checksum.toString(16).padStart(2, "0").toUpperCase();
    }
}

function parsePacket(hexStr) {
    const packet = {
        type: 0,
        command: 0,
        deviceId: 0,
        data_length: 0,
        data: "",
        checksum: 0,
        valid: false
    };

    const bytes = hexStringToBytes(hexStr);

    if (bytes.length < 11 || hexStr.substring(0, 4).toUpperCase() !== "FEEF") {
        console.error("Invalid or too short packet.");
        return packet;
    }

    try {
        packet.type = bytes[2];
        packet.command = bytes[3];
        packet.deviceId = sliceToInt(bytes, 4, 8);
        packet.data_length = sliceToInt(bytes, 8, 10);

        if (packet.data_length > 0 && bytes.length >= 10 + packet.data_length) {
            const dataBytes = bytes.slice(10, 10 + packet.data_length);
            packet.data = hexToUtf8(dataBytes);
        }

        const receivedChecksum = bytes[bytes.length - 1];
        const calculatedChecksum = computeChecksum(bytes.slice(0, bytes.length - 1));

        packet.checksum = calculatedChecksum;
        packet.valid = (receivedChecksum === calculatedChecksum);
    } catch (err) {
        console.error("Error parsing packet:", err.message);
    }

    return packet;
}

/**********
 * From REST Server --> GPS Server
 * ********/
const client = net.createConnection({port: 8181, host: 'localhost'}, () => {
    console.log('Connected to GPS server');
});
client.on('data', (data) => {
    // main routine
    const unwrapped = unwrapMessage(data.toString());
    console.log('Received From GPS Server:', unwrapped);
    /**********
     * Main Routine
     * *********/
    const pkt = parsePacket(unwrapped);
    // Forward the data to all connected WebSocket clients
    wss.clients.forEach((ws) => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(pkt));
        }
    });
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
    console.log("Client connected via WebSocket");
    //TODO: Client Id Length replicated in std::string Packet::trim()
    //TODO: get Client Id from web interface
    const pkt = new Packet(0x01, "9876543210").createPacket();
    client.write(`${wrapMessage(pkt)}`);
    // Main Routine
    ws.on("message", (message) => {
        const [commandName, commandCode] = message.toString().split(',');
        console.log("Received from Client:", message.toString());
        const pkt = new Packet(commandCode, commandName);
        const pktHEX = pkt.createPacket();
        const MSG = `+QIURC: "recv",0,${pktHEX.length}\n${pktHEX}\n`;
        writeToFifo(fifoPath, MSG);
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