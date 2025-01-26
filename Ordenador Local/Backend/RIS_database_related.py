import mysql.connector
from datetime import datetime

def open_database_connection():
    conn = mysql.connector.connect(
        host="localhost",
        user="rsi",
        password="rsi",
        database="rsi" 
    )
    return conn

def close_database_connection(conn):
    conn.close()

def get_sensors_all_info_db(id):  
    conn = open_database_connection()
    cursor = conn.cursor()
    cursor.callproc('GetSensorsInfo', (id,))
    for result in cursor.stored_results():
        output = result.fetchall()
    cursor.close()
    close_database_connection(conn)
    return output[0][0], output[0][1], output[0][2], output[0][3]

def store_sensors_measurement_db(client_id, bat, temp, hum, door_status):
    conn = open_database_connection()
    cursor = conn.cursor()
    time = datetime.now().strftime("%H:%M:%S")
    cursor.callproc('InsertSensorData', (client_id+1, time, bat, temp, hum, door_status))
    conn.commit() 
    cursor.close()
    close_database_connection(conn)