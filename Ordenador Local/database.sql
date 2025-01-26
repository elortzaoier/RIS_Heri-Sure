create database rsi;
use rsi;
CREATE TABLE sensores (
    id INT AUTO_INCREMENT PRIMARY KEY ,
    nombre VARCHAR(255),
    ttn_username VARCHAR(255),
    ttn_password VARCHAR(255),
    device_eui VARCHAR (255),
    tb_apikey VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS mediciones (
    id INT AUTO_INCREMENT PRIMARY KEY,
    dispositivo_id INT,
    timestamp TIME,
    bateria FLOAT,
    temperatura FLOAT,
    humedad FLOAT,
    estado_puerta BOOLEAN NULL,
    FOREIGN KEY (dispositivo_id) REFERENCES sensores(id)
);

INSERT INTO sensores (id, nombre, ttn_username, ttn_password, device_eui, tb_apikey) VALUES
(1, 'RAK3172', 'upvdisca-rakwireless-rak3172-app@ttn', 'NNSXS.3TAQMROYTIH4GZL2OEHFNGDQNECADMN64YTJZMA.X3Q6TRT6K6T33ZZB3FGJLGZZ7QMHQZLFO6HYHEAU3FKMYV3KISVQ', 'eui-ac1f09fffe1787f3','9LxuucpiboZIvtLnPehe'),
(2, 'Dragino S31B-LB', 'wimosa-albarracin-trh-drgn-app@ttn', 'NNSXS.V4TWHJ4DUC5IIX6IYMDWRZ2552YDDNUZTM7WMAQ.DOWDEAM4AV7MTPASEBAZ73S7SVVJBWHD7CIZG76YYUEKZ6CONCEQ', 'eui-a84041e8f18646dc','cw2sjit4w5zikpzslnam'),
(3, 'Milesight EM320-TH-868M', 'itaca-mlsght-em320-th-app@ttn', 'NNSXS.6L2V2SVZRRVORN27QWX2NUFT5WHX6F6SSW3YBHY.UCNIYV5ZG357SAVT5AMHEZKDBOEYPUSKUAZT2BNOBTIVYWG5F6CA', 'eui-24e124785d441512', 'mpfuiee4zkd1d456ybwv');

DELIMITER $$

CREATE PROCEDURE InsertSensorData(
    IN p_client_id INT,
	IN p_timestamp TIME,
    IN p_battery_level FLOAT,
    IN p_temperature FLOAT,
    IN p_humidity FLOAT,
    IN p_door_status BOOLEAN
)
BEGIN
	INSERT INTO mediciones (dispositivo_id, timestamp, bateria, temperatura, humedad, estado_puerta)
    VALUES (p_client_id, p_timestamp, p_battery_level, p_temperature, p_humidity, p_door_status);
END $$

DELIMITER ;
DELIMITER $$
CREATE PROCEDURE GetSensorsInfo(
    IN user_id INT
)
BEGIN
	SELECT ttn_username, ttn_password, device_eui, tb_apikey
    FROM sensores
    WHERE id = user_id;
END $$

DELIMITER ;
