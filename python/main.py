import requests
# import datetime
import sys
import time
import numpy as np
import socket
import json
from pathlib import Path
import os
from datetime import datetime



print("[!] Be shure you have the right pulse sequence in the file \"define_pulses.csv\"")
print("[!] Follow the instructions")
print("[!] The data will be save in the same folder of this script\n")

def read_pulse_sequence():
    pulses_sequence = []

    try:
        pulses = np.loadtxt('define_pulses.csv', dtype=np.int32, delimiter = ',', skiprows = 0)
        print("[!] Pulse sequence:", pulses)
        pulses = pulses.tolist()
        len(pulses)
        # print(len(pulses))

        # pulses_sequence.append(pulses)

    except TypeError:        
        print("[!] Pulse sequence must have more that one")
        while True:
            time.sleep(3)
            sys.exit(0)


    except FileNotFoundError:
        print("[!] \"define_pulses.csv\" file not found")
        while True:
            time.sleep(3)
            sys.exit(0)
    pulses_sequence = pulses
    return pulses_sequence




def InputData():
    pumps_slots = []
    file_names = []
    home_path = Path.cwd()

    while True:
        try:
            pumps_to_calibrate = int(input("[?] Provide the number of pumps to calibrate  "))
            if pumps_to_calibrate >=25:
                print("[!] pumps number out of range (1-24)\n")
            else:
                break
       
        except ValueError:
            print("[!] Invalid input, try again.")
            continue

    while True:
            try:
                path_name = (str(input("\n[?] Provide the Path name for this caracterization  ")))
                path_name = str(home_path) + '/' + path_name
                if os.path.exists(path_name):
                    print('[!] Path already exists')
                else:
                    path_name = Path(path_name)
                    path_name.mkdir()
                    path_name = str(path_name)
                    break
                    

            except ValueError:
                print("[!] Invalid input, try again.")
                continue


    for i in range(pumps_to_calibrate):

        while True:
            try:
                file_name = (str(input("\n[?] Provide the filename for the pump [{}]  ".format(i+1))))
                if np.isin(file_name + '.txt', file_names):
                    print("[!] File name already exist.\n")
                else:
                    file_names.append(file_name + ".txt")
                    break
            except ValueError:
                print("[!] Invalid input, try again.")
                continue

        while True:
            try:
                slot = (int(input("[?] Provide the slot machine for the pump [{}]  ".format(i+1))))
                if np.isin(slot, pumps_slots):
                    print("[!] Slot already in use\n")
                else:
                    pumps_slots.append(slot)
                    break
            except ValueError:
                print("[!] Invalid input, try again.")
                continue
    
    # pumps_slots = np.array([pumps_slots])
    print("pumps_slots: " + str(pumps_slots))
    print("file_names: " + str(file_names))
    return file_names, pumps_slots, path_name

def BrodcastUDP():
    broadcast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    broadcast_address = ('<broadcast>', 8000)
    print("[!]Connecting...")
    print('[!]Join to "Pump Caracterization" in the Calibrator')

    data = ''
    while(data != 'hi sup'):

        message = "hi there"
        broadcast_socket.sendto(message.encode(), broadcast_address)

        # print(f"Mensaje enviado por broadcast a {broadcast_address}")
        broadcast_socket.settimeout(1.0)

        try:
            data, server = broadcast_socket.recvfrom(4096)
            # print(f'Respuesta recibida de {server}: {data}')
            data = data.decode('utf-8')
            # print(data)

            if data == 'hi sup':
                break
        except socket.timeout:
            pass
    broadcast_socket.close()
    print('[!]Conected')
    print('________________________________________________________________')

def Caracterization(pulse_sequence, filenames, pumps_slots, path_name):
    host_name = socket.gethostname()
    ip_address = socket.gethostbyname(host_name)

    server_address = (ip_address, 8000)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(server_address)
    server_socket.listen(1)
    progress_counter = 0

    for i in range(len(pumps_slots)):
        for j in range(len(pulse_sequence)):
            prime_pump = False
            progress_counter+=1

            if len(pulse_sequence) == 1:
                pump_progress = 100
            else:
                pump_progress = round((j*100) / (len(pulse_sequence)-1))

            if len(pumps_slots) == 1:
                test_progress = pump_progress
            else:
                test_progress = round((progress_counter*100) / (len(pulse_sequence)*len(pumps_slots)))



            # if new pump, prime
            if j == 0:
                prime_pump = True
                # prime_pump = False
            else:
                prime_pump = False

            pump_data ={
                "processFinished": False,
                "pump": pumps_slots[i],
                "priority": 0,
                "rotation": 0,
                "pulses": pulse_sequence[j],
                "ka": 1,
                "kb": 0,
                "primePump": prime_pump,
                "pumpProgress": pump_progress,
                "testProgress": test_progress
                }
            json_pump_data = json.dumps(pump_data).encode('utf-8')
            # print(json_pump_data)

            incoming_data = {'stepFinished': False}
            step_finished = incoming_data['stepFinished']
            if j == 0:
                file_type = "w"
            else:
                file_type = "r+"

            file_path = Path(path_name + '/' + filenames[i])
            with open(file_path, file_type) as file:
                if file_type == "w":
                    file.write("{}\t\t{}\n".format("pulses", "grams"))
                while step_finished == False:
                    print('[!]Waiting for connection...')
                    try:
                        client_socket, client_address = server_socket.accept()
                        client_socket.sendall(json_pump_data)
                        incoming_data = client_socket.recv(1024)
                        incoming_data = incoming_data.replace(b'\x00', b'')
                        incoming_data = incoming_data.decode('utf-8')
                        incoming_data = json.loads(incoming_data)
                        # print(incoming_data)
                        step_finished = incoming_data['stepFinished']
                    except ConnectionResetError:
                        print("[!]Lost connection?")
                        continue
                    except socket.error as e:
                        print(f"[!]Socket error: {e}")

                    time.sleep(0.1)

                    # try:
                    #     incoming_data = client_socket.recv(1024)
                    # except socket.error as e:
                    #     print(f"Socket error: {e}")
                    
                    # print(f'Respuesta recibida de {client_address}: {incoming_data}')
                    if(step_finished == True):
                        grams = round(incoming_data['grams'], 3)
                        # print(grams)
                        # print("GUARDAR")
                        file.seek(0, 2)
                        file.write("{}\t\t{}\n".format(pulse_sequence[j], grams))
                        file.close()
                        print('[!]Pump slot: ' + str(pumps_slots[i]+1))
                        print('[!]Pump step: ' + str(j+1) + ' of ' + str(len(pulse_sequence)))
                        print('[!]Pulses: ' + str(pulse_sequence[j]))
                        print('[!]Grams: ' + str(grams))
                        print('[!]Pump progress: ' + str(pump_progress) + '%')
                        print('[!]Overall progress: ' + str(test_progress) + '%')
                        print('________________________________________________________________')
                    else:
                        time.sleep(0.5)
                        # print("!")


            file.close()
                    

    incoming_data = {'stepFinished': False}
    step_finished = incoming_data['stepFinished']
    pump_data ={"processFinished": True}
    json_pump_data = json.dumps(pump_data).encode('utf-8')
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    print("Pump end at =", current_time)


    while step_finished == False:
        # time.sleep(0.5)
        # print('Esperando por una conexión...')
        client_socket, client_address = server_socket.accept()
        # print(f'Conexión entrante de {client_address}')
        client_socket.sendall(json_pump_data)

        
        incoming_data = client_socket.recv(1024)
        # print(f'Respuesta recibida de {client_address}: {incoming_data}')
        incoming_data = incoming_data.replace(b'\x00', b'')
        incoming_data = incoming_data.decode('utf-8')
        incoming_data = json.loads(incoming_data)
        # print("END!!!!!!!!!!!!!!!!!!!")
        # print(incoming_data)
        step_finished = incoming_data['stepFinished']
    
    client_socket.close()
    server_socket.close()
    print('[!]Caracterization finished')



def main():

    pulses = read_pulse_sequence()
    # pulses = pulses.tolist()
    filenames, pumps_slots, path_name = InputData()
    pumps_slots = np.subtract(pumps_slots, 1)
    pumps_slots = pumps_slots.tolist()
    # pumps_slots = [23, 23]
    now = datetime.now()
    
    BrodcastUDP()
    current_time = now.strftime("%H:%M:%S")
    print("Start Time =", current_time)

    Caracterization(pulses, filenames, pumps_slots, path_name)

    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    print("End Time =", current_time)


    # pumps_slots = np.subtract(pumps_slots, 1)



if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
