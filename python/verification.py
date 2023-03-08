import requests
import datetime
import sys
import time
import numpy as np

teorical_ml = []
real_ml = []
counter = 0
selector = False
pump_ender = False
print("[!] Be shure you are connected to the Fiush Machine")
print("[!] Be shure you have the right pulse sequence in the file \"define_pulses.csv\"")
print("[!] Follow the instructions")
print("[!] The data will be save in the same folder of this script\n")


def main():
    # Keep asking for user input until is valid.
    global teorical_quantity
    global real_quantity
    while not pump_ender:
        try:
            file_name = input("[?] Provide the file name: ")
            file_name = file_name + ".txt"
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break

    while True:
        try:
            pump_number = int(input("[?] Provide the Pump id: "))
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break

    while True:
        try:
            ka = float(input("[?] Provide ka: "))
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break

    while True:
        try:
            kb = float(input("[?] Provide kb: "))
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break
        
    with open(file_name, "w") as file:
        file.write("{}\t{}\n".format("Pump", pump_number))
        file.write("{}\t{}\n".format("ka", "kb"))
        file.write("{}\t{}\n\n".format(ka, kb))
        file.write("{}\t{}\t{}\n".format("provided(ml)", "Real(ml)", "Error(%)"))

        while True:
                # Keep asking for user input until is valid.
                while True:
                    try:
                        teorical_quantity = float(input("\n[?] Provide quantity to dispense (ml): "))
                    except ValueError:
                        print("[!] Invalid input, try again.")
                        continue
                    else:
                        break

                pload = { 
                'name': pump_number, 
                'pump1': pump_number, 
                'ml1': teorical_quantity, 
                'ka1': ka, 
                'kb1': kb, 
                'priority1': 1, 
                'rotation1': 0 
                }
        
                try:
                    url = 'http://192.168.4.1/preparation'
                    requests.post(url, json=pload)
                except:
                    print("[!] Couldn't make the POST request to {}".format(url))
                
                while True:
                    try:
                        real_quantity = float(input("[?] Provide dispensed quantity (ml): "))
                    except ValueError:
                        print("[!] Invalid input, try again.")
                        continue
                    else:
                        break
                error = abs(teorical_quantity - real_quantity)
                error = error / teorical_quantity
                error = round(error * 100, 2)
                print("[!] Error:", error, "%")

                file.write("{}\t{}\t{}\n".format(teorical_quantity, real_quantity, error))
                
                while True:
                    try:
                        end_flag = str(input("[?] Do you wanna continue? [y/n]: "))
                        if end_flag == "n" or end_flag == "N":
                            file.close()
                            sys.exit(0)                     
                    except ValueError:
                        print("[!] Invalid input, try again.")
                        continue
                    else:
                        break
                
        
                # teorical_ml.append(teorical_quantity)
                # real_ml.append(real_quantity)
                
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
