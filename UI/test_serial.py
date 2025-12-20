import serial
import struct
import time
"""
STM32 đang dùng frame:

[0]  0xAA
[1]  0x55
[2]  msg_id
[3]  payload_len
[4]  seq (uint16)
[6]  time_ms (uint32)
[10] payload (N bytes)
[10+N] CRC16_L
[11+N] CRC16_H
"""

ser = serial.Serial('COM5', 115200, timeout=1)
seq = 0
TL_SYNC1 = 0xAA
TL_SYNC2 = 0x55
TL_CMD_SET_PID = 0x10

def crc16_ccitt(data):
    crc = 0xFFFF
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) if (crc & 0x8000) else (crc << 1)
            crc &= 0xFFFF
    return crc
def send_set_pid(kp, ki, kd):
    global seq

    # scale giống STM32
    payload = struct.pack(
        '<hhh',
        int(kp * 100),
        int(ki * 1000),
        int(kd * 100)
    )

    msg_id = TL_CMD_SET_PID
    plen = len(payload)
    t_ms = int(time.time() * 1000) & 0xFFFFFFFF

    header = struct.pack('<BBHI', msg_id, plen, seq, t_ms)

    crc = crc16_ccitt(header + payload)

    frame = (
        bytes([TL_SYNC1, TL_SYNC2]) +
        header +
        payload +
        struct.pack('<H', crc)
    )

    ser.write(frame)
    seq = (seq + 1) & 0xFFFF

while True:
    # tìm sync
    if ser.read(1) != b'\xAA': continue
    if ser.read(1) != b'\x55': continue

    hdr = ser.read(8)  # msg_id, len, seq(2), time(4)
    msg_id, plen, seq, t = struct.unpack('<BBHI', hdr)

    payload = ser.read(plen)
    crc_rx = struct.unpack('<H', ser.read(2))[0]

    if crc_rx != crc16_ccitt(hdr + payload):
        continue

    yaw, pitch, roll = struct.unpack('<hhh', payload)
    print(yaw/100.0, pitch/100.0, roll/100.0)
    send_set_pid(25.0, 0.8, 0.6)

