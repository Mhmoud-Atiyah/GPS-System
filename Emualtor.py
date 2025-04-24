import time
import serial

def send_file_line_by_line(file_path, port, baudrate=115200):
    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            stopbits=serial.STOPBITS_ONE,
            parity=serial.PARITY_NONE,
            timeout=1
        )

        if not ser.is_open:
            ser.open()

        with open(file_path, 'r', encoding='utf-8') as file:
            print(f"Sending GPS file '{file_path}' to {port} at {baudrate} baud...\n")
            for line_number, line in enumerate(file, start=1):
                line = line.strip()
                if not line:
                    continue

                for ch in line + '\r\n':  # Add CRLF like real GPS output
                    ser.write(ch.encode('utf-8'))
                    time.sleep(0.005)  # 5 ms delay per character

                print(f"Sent line {line_number}: {line}")
                time.sleep(3)  # Wait 3 seconds between lines

        ser.close()
        print("\nDone sending the file.")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    file_path = 'gps_data.txt'     # Replace with your file
    serial_port = '/dev/ttyUSB0'   # Adjust for your system
    send_file_line_by_line(file_path, serial_port)