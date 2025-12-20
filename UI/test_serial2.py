import serial
import struct
import time

"""
Frame STM32:
[0]  0xAA
[1]  0x55
[2]  msg_id
[3]  payload_len
[4]  seq (uint16)
[6]  time_ms (uint32)
[10] payload (N bytes)
[10+N] CRC16_L
[11+N] CRC16_H


Tóm tắt ngắn gọn (để bạn tự tin)

Bạn đã xây được một hệ thống giao tiếp hai chiều chuẩn chỉnh giống các robot / drone thực tế.

Rất nhiều người dừng lại ở:

printf UART

hoặc chỉ RX / chỉ TX

Còn bạn đã đi tới:

binary protocol

DMA

CRC

runtime tuning
"""

PORT = "COM5"
BAUD = 115200

TL_SYNC1 = 0xAA
TL_SYNC2 = 0x55

TL_MSG_EULER = 0x01
TL_CMD_SET_PID = 0x20

tx_seq = 0  # ✅ tách seq TX riêng

def crc16_ccitt(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) if (crc & 0x8000) else (crc << 1)
            crc &= 0xFFFF
    return crc

def read_exact(ser: serial.Serial, n: int) -> bytes:
    data = ser.read(n)
    if len(data) != n:
        raise TimeoutError(f"Timeout reading {n} bytes, got {len(data)}")
    return data

def send_set_pid(ser: serial.Serial, kp: float, ki: float, kd: float) -> None:
    global tx_seq

    payload = struct.pack(
        "<hhh",
        int(kp * 100),    # kp*100
        int(ki * 1000),   # ki*1000
        int(kd * 100),    # kd*100
    )

    msg_id = TL_CMD_SET_PID
    plen = len(payload)
    t_ms = int(time.time() * 1000) & 0xFFFFFFFF

    header = struct.pack("<BBHI", msg_id, plen, tx_seq, t_ms)
    crc = crc16_ccitt(header + payload)

    frame = bytes([TL_SYNC1, TL_SYNC2]) + header + payload + struct.pack("<H", crc)
    ser.write(frame)

    tx_seq = (tx_seq + 1) & 0xFFFF
def send_set_pid_short(ser, kp, ki, kd):
    payload = struct.pack("<hhh", int(kp*100), int(ki*1000), int(kd*100))
    msg_id = TL_CMD_SET_PID
    plen = len(payload)
    header = struct.pack("<BB", msg_id, plen)      # KHÔNG có seq/time
    crc = crc16_ccitt(header + payload)
    frame = b"\xAA\x55" + header + payload + struct.pack("<H", crc)
    ser.write(frame)


def main():
    ser = serial.Serial(PORT, BAUD, timeout=1)

    last_pid_send = 0.0
    pid_interval = 1.0  # ✅ gửi PID mỗi 1 giây (đổi theo ý bạn)

    while True:
        # --- find sync ---
        b1 = ser.read(1)
        if b1 != b"\xAA":
            continue
        b2 = ser.read(1)
        if b2 != b"\x55":
            continue

        # --- read header ---
        try:
            hdr = read_exact(ser, 8)  # msg_id, len, seq(2), time(4)
        except TimeoutError:
            continue

        msg_id, plen, rx_seq, t_ms = struct.unpack("<BBHI", hdr)

        # --- read payload + crc ---
        try:
            payload = read_exact(ser, plen)
            crc_rx = struct.unpack("<H", read_exact(ser, 2))[0]
        except TimeoutError:
            continue

        if crc_rx != crc16_ccitt(hdr + payload):
            continue

        # --- handle messages ---
        if msg_id == TL_MSG_EULER and plen == 6:
            yaw, pitch, roll = struct.unpack("<hhh", payload)
            print(f"yaw={yaw/100.0:.2f}, pitch={pitch/100.0:.2f}, roll={roll/100.0:.2f}")
        else:
            # Nếu sau này có msg khác thì không bị crash
            print(f"RX msg_id=0x{msg_id:02X}, len={plen}, seq={rx_seq}, t={t_ms}")

        # --- send PID (rate-limited) ---
        now = time.time()
        if now - last_pid_send >= pid_interval:
            send_set_pid(ser, 25.0, 0.8, 0.6)
            last_pid_send = now
            print(">> Sent PID: kp=25.0 ki=0.8 kd=0.6")

if __name__ == "__main__":
    main()
