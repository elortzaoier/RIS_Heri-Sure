def decode_payload_RAK(payload):
    alarm = payload.get("digital_out_3", None)
    if alarm == 0: alarm = False
    if alarm == 1: alarm = True
    temp = payload.get("temperature_1", None)
    hum = payload.get("relative_humidity_2", None)
    return temp, hum, {"temperature": temp, "humidity": hum, "alarmActivated": alarm}

def decode_payload_ALBARRACIN(payload):
    bat = payload.get("BatV", None)
    temp = payload.get("TempC_SHT31", None)
    hum = payload.get("Hum_SHT31", None)
    door_status = payload.get("Door_status", None)
    status = True if door_status == 'OPEN' else False
    return bat, temp, hum, status, {"battery": bat, "temperature": temp, "humidity": hum, "door_status": status}

def decode_payload_ITACA(payload):
    bat = payload.get("battery", None)
    temp = payload.get("temperature", None)
    hum = payload.get("humidity", None)
    return bat, temp, hum, {"battery": bat, "temperature": temp, "humidity": hum}
