import paho.mqtt.client as mqtt
import json
import threading
import RIS_database_related as db
import RIS_MQTT_Decoder as decoder

sensors = []
threads = []
BROKER = "eu1.cloud.thethings.network"
def get_sensors_information():
    global sensors
    for i in range(3):
        ttn_user, ttn_pass, device_eui, tb_apikey = db.get_sensors_all_info_db(i+1)
        mqtt_user = create_mqtt_user (ttn_user, ttn_pass)
        sensors.append({ "TTN MQTT": mqtt_user, "TTN Username": ttn_user, #"TTN Password": ttn_pass
                        "Device EUI": device_eui, "TB ApiKey": tb_apikey})

def create_mqtt_user(ttn_user, ttn_password):
    client =  mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(username=ttn_user, password=ttn_password)
    return client

def start_TTN_mqtt_clients():
    for i in range(3):
        client = sensors[i]["TTN MQTT"]
        thread = threading.Thread(target=connect_mqtt_client, args=(client,), name="Thread_" + str(i) )
        threads.append(thread)
        thread.start()

def connect_mqtt_client(client):
    client.connect(BROKER, port=1883, keepalive=60)
    client.loop_forever()

def create_topic (client):
    if client == sensors[0]["TTN MQTT"]:
        id = 0
    elif client == sensors[1]["TTN MQTT"]:
        id = 1
    elif client == sensors[2]["TTN MQTT"]:
        id = 2
    
    ttn_user = sensors[id]["TTN Username"]
    device_eui = sensors[id]["Device EUI"]
    topic = f"v3/{ttn_user}/devices/{device_eui}/up" #Add username and device EUI to the TTN topic
    print(topic)
    return topic

def on_connect(client, userdata, flags, rc):
    print("Client succesfully connected!")
    topic = create_topic(client)
    client.subscribe(topic, qos=0)

def on_message(client, userdata, msg):
    payload = json.loads(msg.payload)
    payload = payload.get("uplink_message", None).get("decoded_payload", None)
    if payload != None:
        door_status = bat = None
        if client == sensors[0]["TTN MQTT"]:
            client_id = 0
            temp, hum, c_payload = decoder.decode_payload_RAK(payload)
        elif client == sensors[1]["TTN MQTT"]:
            client_id = 1
            bat, temp, hum, door_status, c_payload  = decoder.decode_payload_ALBARRACIN(payload)
        elif client == sensors[2]["TTN MQTT"]:
            client_id = 2
            bat, temp, hum, c_payload = decoder.decode_payload_ITACA(payload)
        db.store_sensors_measurement_db(client_id, bat, temp, hum, door_status)
        send_message_to_thingsboard(client_id, c_payload)

def send_message_to_thingsboard(client_id, payload):
    tb_client = mqtt.Client()
    tb_client.username_pw_set(sensors[client_id]["TB ApiKey"])
    tb_client.connect("localhost", 1883, 30)
    tb_client.publish("v1/devices/me/telemetry", json.dumps(payload), qos=1)
    tb_client.loop()
    tb_client.disconnect()

if __name__ == "__main__":
    get_sensors_information()
    start_TTN_mqtt_clients()
    for thread in threads:
        thread.join()
