import numpy as np
import matplotlib.pyplot as plt
# %matplotlib inline


ml= [1.4,1.8,2.0,2.6,5.2,7.6,10.1,18.7,27.6,36.1,44.2,61.6,78.5,85.7,128.8,169.9,254.6,296.6,335.8,419.7,499.4,662.3,739.5,821.2,901.9,983.2,1056.4,1129.9,1202.7]
pulses = [10, 12, 14, 20, 50, 75, 100, 200, 300, 400, 500, 700, 900, 1000, 1500,  2000, 3000, 3500, 4000, 5000, 6000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000]


def linealization(pulses_array, minilliters_array):
    cuantity = minilliters_array
    pulses = pulses_array

    range1 = []
    range2 = []
    range3 = []
    range4 = []
    range5 = []
    ka_array = []
    kb_array = []
    range_array = [1, 2, 3, 4, 5]

    k_range1 = []
    k_range2 = []
    k_range3 = []
    k_range4 = []
    k_range5 = []

    for i in range(6):
        couple = cuantity[i], pulses[i]
        range1.append(couple)

    for i in range(5,12):
        couple = cuantity[i], pulses[i]
        range2.append(couple)

    for i in range(11,19):
        couple = cuantity[i], pulses[i]
        range3.append(couple)

    for i in range(19,24):
        couple = cuantity[i], pulses[i]
        range4.append(couple)

    for i in range(23,29):
        couple = cuantity[i], pulses[i]
        range5.append(couple)

    print(range1)
    print(range2)
    print(range3)
    print(range4)
    print(range5)
    print("/////////////////")

    ranges = np.array([[range1], [range2], [range3], [range4], [range5]], dtype=object)
    # print(ranges[0][0])

    for i in range(5):
        points = np.array(ranges[i][0])
        # get x and y vectors
        x = points[:,0]
        y = points[:,1]
        # calculate polynomial
        z = np.polyfit(x, y, 1)
        a = round(z[0],2)
        b = round(z[1],2)
        ka_array.append(a)
        kb_array.append(b)
        # print("range: ", i, " ka: ", a, " kb: ", b)

    k_range1 = range_array[0], ka_array[0], kb_array[0]
    k_range2 = range_array[1], ka_array[1], kb_array[1]
    k_range3 = range_array[2], ka_array[2], kb_array[2]
    k_range4 = range_array[3], ka_array[3], kb_array[3]
    k_range5 = range_array[4], ka_array[4], kb_array[4]

    return(k_range1, k_range2, k_range3, k_range4, k_range5)

# range_array = []
# ka_array = []
# kb_array = []
# k_range1 = []
# k_range2 = []
# k_range3 = []
# k_range4 = []
k_range1, k_range2, k_range3, k_range4, k_range5 = linealization(pulses, ml)

print(k_range1)
print(k_range2)
print(k_range3)
print(k_range4)
print(k_range5)


