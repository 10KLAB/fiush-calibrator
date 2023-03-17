import requests
import datetime
import sys
import time
import numpy as np
import socket
import json


print("[!] Be shure you have the right pulse sequence in the file \"define_pulses.csv\"")
print("[!] Follow the instructions")
print("[!] The data will be save in the same folder of this script\n")

def read_pulse_sequence():

    try:
        pulses = np.loadtxt('define_pulses.csv', dtype=np.int32, delimiter = ',', skiprows = 0)
        print("[!] Pulse sequence:", pulses, "\n")
    except FileNotFoundError:
        print("[!] \"define_pulses.csv\" file not found")
        while True:
            time.sleep(10)
    return pulses




def InputData():
    pumps_slots = []
    file_names = []
    pump_counter = 1
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

    print("pumps_slots: " + str(pumps_slots))
    print("file_names: " + str(file_names))
    return file_names, pumps_slots

def BrodcastUDP():
    broadcast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    broadcast_address = ('<broadcast>', 8000)

    data = ''
    while(data != 'hi sup'):

        message = "hi there"
        broadcast_socket.sendto(message.encode(), broadcast_address)

        print(f"Mensaje enviado por broadcast a {broadcast_address}")
        broadcast_socket.settimeout(1.0)

        try:
            data, server = broadcast_socket.recvfrom(4096)
            print(f'Respuesta recibida de {server}: {data}')
            data = data.decode('utf-8')
            print(data)

            if data == 'hi sup':
                break
        except socket.timeout:
            pass
    broadcast_socket.close()

def Caracterization(pulse_sequence, filenames, pumps_slots):
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
            
            pump_progress = round((j*100) / (len(pulse_sequence)-1))
            test_progress = round((progress_counter*100) / (len(pulse_sequence)*len(pumps_slots)))

            # if new pump, prime
            if j == 0:
                # prime_pump = True
                prime_pump = False
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
            print(json_pump_data)

            incoming_data = {'stepFinished': False}
            step_finished = incoming_data['stepFinished']
            if j == 0:
                file_type = "w"
            else:
                file_type = "r+"
            with open(filenames[i], file_type) as file:
                if file_type == "w":
                    file.write("{}\t\t{}\n".format("pulses", "grams"))
                while step_finished == False:
                    # time.sleep(1)
                    print('Esperando por una conexión...')
                    client_socket, client_address = server_socket.accept()
                    print(f'Conexión entrante de {client_address}')
                    client_socket.sendall(json_pump_data)
                    
                    incoming_data = client_socket.recv(1024)
                    # print(f'Respuesta recibida de {client_address}: {incoming_data}')
                    incoming_data = incoming_data.replace(b'\x00', b'')
                    incoming_data = incoming_data.decode('utf-8')
                    incoming_data = json.loads(incoming_data)
                    print(incoming_data)
                    step_finished = incoming_data['stepFinished']
                    if(step_finished == True):
                        grams = round(incoming_data['grams'], 3)
                        print(grams)
                        print("GUARDAR")
                        file.seek(0, 2)
                        file.write("{}\t\t{}\n".format(pulse_sequence[j], grams))

            file.close()
                    

    incoming_data = {'stepFinished': False}
    step_finished = incoming_data['stepFinished']
    pump_data ={"processFinished": True}
    json_pump_data = json.dumps(pump_data).encode('utf-8')

    while step_finished == False:
        # time.sleep(0.5)
        print('Esperando por una conexión...')
        client_socket, client_address = server_socket.accept()
        print(f'Conexión entrante de {client_address}')
        client_socket.sendall(json_pump_data)

        
        incoming_data = client_socket.recv(1024)
        # print(f'Respuesta recibida de {client_address}: {incoming_data}')
        incoming_data = incoming_data.replace(b'\x00', b'')
        incoming_data = incoming_data.decode('utf-8')
        incoming_data = json.loads(incoming_data)
        print("END!!!!!!!!!!!!!!!!!!!")
        print(incoming_data)
        step_finished = incoming_data['stepFinished']
    
    client_socket.close()
    server_socket.close()



def main():
    pulses = [100, 100, 100]
    filenames = ['99uno.txt', '99dos.txt', 'tres.txt']
    pumps_slots = [23]

    BrodcastUDP()
    Caracterization(pulses, filenames, pumps_slots)


    # pulses = read_pulse_sequence()
    # filenames, pumps_slots = InputData()
    # pumps_slots = np.subtract(pumps_slots, 1)


    






if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
