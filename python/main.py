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
import select



print("[!] Be shure you have the right pulse sequence in the file \"define_pulses.csv\"")
print("[!] Follow the instructions")
print("[!] The data will be save in the same folder of this script\n")

def read_pulse_sequence():
    #This function reads a pulse sequence from a CSV file.
    pulses_sequence = []

    try:
        # Load the pulse sequence from a CSV file.
        pulses = np.loadtxt('define_pulses.csv', dtype=np.int32, delimiter = ',', skiprows = 0)
        print("[!] Pulse sequence:", pulses)
        # Convert the pulse sequence to a list.
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
#     """This function inputs data for the calibration process.
#   Args:
#     none.
#   Returns:
#     A tuple of (file_names, pumps_slots, path_name).
#   """
    # Initialize variables.
    pumps_slots = []
    file_names = []
    home_path = Path.cwd()

    # Get the number of pumps to calibrate.
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

     # Get the path name for the characterization.        
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

    # Get the filename for each pump.
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
        # Get the slot machine for each pump.       
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
    # Create a UDP socket.
    broadcast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Set the socket to broadcast mode.
    broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    # Set the broadcast address.
    broadcast_address = ('<broadcast>', 8000)
    print("[!]Connecting...")
    print('[!]Join to "Pump Caracterization" in the Calibrator')

    data = ''
    while(data != 'hi sup'):
        # Create a message to send.
        message = "hi there"
        # Send the message to the broadcast address.
        broadcast_socket.sendto(message.encode(), broadcast_address)

        # print(f"Mensaje enviado por broadcast a {broadcast_address}")
        # Set a timeout of 1 second.
        broadcast_socket.settimeout(1.0)

        # Try to receive a response from the Calibrator.
        try:
            data, server = broadcast_socket.recvfrom(4096)
            # print(f'Respuesta recibida de {server}: {data}')
            # Decode the response.
            data = data.decode('utf-8')
            # print(data)

            # If the response is "hi sup", break out of the loop.
            if data == 'hi sup':
                break
        except socket.timeout:
            pass
    # Close the socket.
    broadcast_socket.close()
    print('[!]Conected')
    print('________________________________________________________________')

def Caracterization(pulse_sequence, filenames, pumps_slots, path_name):
    """This function characterizes a set of pumps.

  Args:
    pulse_sequence: A list of the number of pulses for each pump.
    filenames: A list of the filenames for each pump.
    pumps_slots: A list of the pump slots for each pump.
    path_name: The path to the directory where the output files will be saved.

  Returns:
    None.
  """
    
    host_name = socket.gethostname()
    ip_address = socket.gethostbyname(host_name)
    connections_number = 50

    # Create a socket for the server.
    server_address = (ip_address, 8000)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(server_address)
    server_socket.listen(connections_number)
    server_socket.settimeout(5)
    progress_counter = 0
    reset_socket = 0

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
            # server_socket.settimeout(5)
            
            with open(file_path, file_type) as file:
                if file_type == "w":
                    file.write("{}\t\t{}\n".format("pulses", "grams"))
                server_socket = None
                client_socket = None
                while step_finished == False:
                    print('[!]Waiting for connection...')
                    time.sleep(1)

                    reset_socket+=1
                    # print(reset_socket)
                    if reset_socket >= 5:
                        if server_socket:
                            server_socket.close()
                            server_socket = None
                            print("server closed")
                        if client_socket:
                            client_socket.close()
                            client_socket = None
                            print("client closed")
                        reset_socket = 0
                        print("[!]Connection relunched")

                    #/////////////////////////////////////////////////////////////////////
                    if not server_socket:
                        # print("1")
                        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        # print("2")
                        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                        # print("3")
                        server_socket.bind(server_address)
                        # print("4")
                        server_socket.listen(connections_number)
                        # print("5")
                        server_socket.settimeout(5)
                        # print("6")
                        time.sleep(1)
                    try:
                        # print("7")
                        
                        client_socket, client_address = server_socket.accept()
                        client_socket.settimeout(5)
                    # print("8")
                        client_socket.sendall(json_pump_data)
                    # print(json_pump_data)
                    # print("9")
                        incoming_data = client_socket.recv(1024)
                    # print("10")
                        incoming_data = incoming_data.replace(b'\x00', b'')
                    # print("11")
                        incoming_data = incoming_data.decode('utf-8')
                    # print("12")
                        incoming_data = json.loads(incoming_data)
                    # print(incoming_data)
                        step_finished = incoming_data['stepFinished']
                        # client_socket.close()
                        # server_socket.close()
                    except socket.timeout:
                        # print("[!]Timeout")
                        print('[!]Waiting for connection....')
                    except ConnectionResetError:
                        print("[!]Connection reset")
                        if server_socket:
                            server_socket.close()
                            server_socket = None
                        if client_socket:
                            client_socket.close()
                            client_socket = None
                        continue
                    except socket.error as e:
                        print(f"[!]Socket error: {e}")
                        if server_socket:
                            server_socket.close()
                            server_socket = None
                        if client_socket:
                            client_socket.close()
                            client_socket = None
                        continue
                    except Exception as e:
                        print(f"Se ha producido un error: {str(e)}")
                        if server_socket:
                            server_socket.close()
                            server_socket = None
                        if client_socket:
                            client_socket.close()
                            client_socket = None
                        continue
##//////////////////////////////////////////////////////////////////////////////////////
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
        
                        if server_socket:
                            server_socket.close()
                            server_socket = None
                        if client_socket:
                            client_socket.close()
                            client_socket = None

                            # time.sleep(1)
                            # print("socket cerrado")
                    # else:
                        # time.sleep(0.5)
                        # print("!")


            file.close()
                    
    #finish step of the pump
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
