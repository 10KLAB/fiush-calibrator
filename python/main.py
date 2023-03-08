import requests
import datetime
import sys
import time
import numpy as np

pulse_array = []
ml_array = []

pumps = []
pumps_slot = []
file_names = []

print("[!] Be shure you have the right pulse sequence in the file \"define_pulses.csv\"")
print("[!] Follow the instructions")
print("[!] The data will be save in the same folder of this script\n")


def InputFilename():
    # Keep asking for user input until is valid.
    while True:
        try:
            file_name = input("[?] Provide the file name: ")
            file_name = file_name + ".txt"
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break

def InputPumps():
    pump_counter = 1
    while True:
        try:
            pumps_to_calibrate = int(input("[?] Provide the number of pumps to calibrate  "))
        except ValueError:
            print("[!] Invalid input, try again.")
            continue
        else:
            break

    for i in range(pumps_to_calibrate):
        while True:
            try:
                file_names.append(str(input("[?] Provide the filename for the pump [{}]  ".format(i+1))))
            except ValueError:
                print("[!] Invalid input, try again.")
                continue
            else:
                break

        while True:
            try:
                pumps_slot.append(int(input("[?] Provide the slot for the pump [{}]  ".format(i+1))))
            except ValueError:
                print("[!] Invalid input, try again.")
                continue
            else:
                break



    print("pumps_slots: " + str(pumps_slot))
    print("file_names: " + str(file_names))

    


def main():
    InputPumps()

    
#     with open(file_name, "w") as file:
#         while True:
#                     try:
#                         if not selector:
#                             pump = int(input("[?] Provide pump id: "))
#                             selector = True
#                     except ValueError:
#                         print("[!] Invalid input, try again.")
#                         continue
#                     else:
#                         break
#         while True:
#             for pulse in (pulses):
#                 print("\n[!] For Pulses: {}".format(pulse))
#                 # Keep asking for user input until is valid.
#                 # while True:
#                 #     try:
#                 #         if not selector:
#                 #             pump = int(input("[?] Provide pump id: "))
#                 #             selector = True
#                 #     except ValueError:
#                 #         print("[!] Invalid input, try again.")
#                 #         continue
#                 #     else:
#                 #         break
#                 pload = { 
#                     'name': pump, 
#                     'pump1': pump, 
#                     'ml1': int(pulse), 
#                     'ka1': 1, 
#                     'kb1': 0, 
#                     'priority1': 1, 
#                     'rotation1': 0 
#                 }
                
#                 try:
#                     url = 'http://192.168.4.1/preparation'
#                     requests.post(url, json=pload)
#                 except:
#                     print("\n[!] Couldn't make the POST request to {}".format(url))
#                     print("[!] Be shure you are connected to the Fiush Machine")
#                     while True:
#                         time.sleep(10)
#                     # print("[!] Miss me with that shit, beach")
#                     # sys.exit(1)

#                 # Keep asking for user input until is valid.
#                 while True:
#                     try:
#                         quantity = float(input("[?] Provide quantity (ml): "))
#                     except ValueError:
#                         print("[!] Invalid input, try again.")
#                         continue
#                     else:
#                         break
#                 pulse_array.append(pulse)
#                 ml_array.append(quantity)
#                 # print(ml_array)
#                 # counter += 1

#                 file.write("{}\t{}\n".format(pulse, quantity))
                
#                 if len(pulses) == len(ml_array):
#                     print("\n[!] Process finished")
#                     file.close()
#                     while True:
#                         time.sleep(10)
# #                     print("escribir")
# #                     pump_ender = True
# #                     k_range1 = []
# #                     k_range2 = []
# #                     k_range3 = []
# #                     k_range4 = []
# #                     k_range5 = []

# #                     k_range1, k_range2, k_range3, k_range4, k_range5 = linealization(pulse_array, ml_array)
# #                     # print(k_range1)
# #                     # print(k_range2)
# #                     # print(k_range3)
                    
# #                     file.write("{}\t{}\t{}\n".format(k_range1[0], k_range1[1], k_range1[2]))
# #                     file.write("{}\t{}\t{}\n".format(k_range2[0], k_range2[1], k_range2[2]))
# #                     file.write("{}\t{}\t{}\n".format(k_range3[0], k_range3[1], k_range3[2]))
# #                     file.write("{}\t{}\t{}\n".format(k_range4[0], k_range4[1], k_range4[2]))
# #                     file.write("{}\t{}\t{}\n".format(k_range5[0], k_range5[1], k_range5[2]))
#                     # sys.exit()
# # print(pulse_array)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
