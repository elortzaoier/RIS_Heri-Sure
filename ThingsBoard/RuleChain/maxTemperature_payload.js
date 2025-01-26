// Custom Base64 Encoding Function
function base64Encode(input) {
    var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    var output = "";
    var i = 0;

    while (i < input.length) {
        var char1 = input.charCodeAt(i++);
        var char2 = i < input.length ? input.charCodeAt(i++) : NaN;
        var char3 = i < input.length ? input.charCodeAt(i++) : NaN;

        var enc1 = char1 >> 2;
        var enc2 = ((char1 & 3) << 4) | (char2 >> 4);
        var enc3 = ((char2 & 15) << 2) | (char3 >> 6);
        var enc4 = char3 & 63;

        if (isNaN(char2)) {
            enc3 = enc4 = 64;
        } else if (isNaN(char3)) {
            enc4 = 64;
        }

        output += chars.charAt(enc1) + chars.charAt(enc2) + chars.charAt(enc3) + chars.charAt(enc4);
    }

    return output;
}

var buffer = [];

// Encode Temperature on Channel 1 (Type 0x67)
buffer.push(0x01); // Channel 1
buffer.push(0x67); // Temperature type

// Assuming msg.maxTemperature is a valid string or number
var tempValue = Math.round(parseFloat(msg.maxTemperature) * 10); // Convert to 0.1°C resolution

buffer.push((tempValue >> 8) & 0xff); // High byte
buffer.push(tempValue & 0xff);        // Low byte

// Convert buffer to Base64 string using custom Base64 encoding
var binaryPayload = new Uint8Array(buffer);
var frmPayload = base64Encode(String.fromCharCode.apply(null, binaryPayload));

// MQTT downlink payload
var mqttPayload = {
    downlinks: [
        {
            f_port: 33,  // Make sure f_port is defined
            frm_payload: frmPayload,
            priority: "NORMAL",
            decodedPayload: msg // msg
            
        }
    ]
};

return {msg: mqttPayload, metadata: metadata, msgType: msgType};