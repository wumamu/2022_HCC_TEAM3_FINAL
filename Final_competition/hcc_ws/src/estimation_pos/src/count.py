import numpy as np

file_id = ['0', '1', '3', '4']
my_mapping_dict = {'bottle':1, 'tvmonitor':0, 'keyboard':3, 'teddybear':2}
index = 0
output_list = []


with open('/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws/EEGanswer.txt', 'r') as file:
    data = file.read()
    # print(type(data))
    output_list = data.split()


for id in file_id:
    path = '/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws/'+ id+'.txt'
    # path = '/home/wumamu/5/'+ id+'.txt'
    with open(path, "r") as tf:
        lines = tf.read().split('\n--------------------------\n')
    mean = []
    for line in lines:

        x = []
        y = []
        z = []

        mean_xyz = []

        a = line.split('\n',1)[1]
        b = a.replace('[','').replace(']','').split('\n')
        # print(a)
        for j in b:
            j = j.split(' ')
            j = [float(x) for x in j if x != '']
            # print(j)
            x.append(j[0])
            y.append(j[1])
            z.append(j[2])

        # print(np.mean(x))
        # print(np.std(x))
        # print(np.mean(y))
        # print(np.std(y))
        # print(np.mean(z))
        # print(np.std(z))

        summ = []
        for ele in x:
            if (ele >= np.mean(x)+2*np.std(x)) or (ele <= np.mean(x)-2*np.std(x)):
                pass
            else:
                summ.append(ele)
        mean_xyz.append(np.sum(summ)/len(summ))
        
        summ = []
        for ele in y:
            if (ele >= np.mean(y)+2*np.std(y)) or (ele <= np.mean(y)-2*np.std(y)):
                pass
            else:
                summ.append(ele)
        mean_xyz.append(np.sum(summ)/len(summ))

        summ = []
        for ele in z:
            if (ele >= np.mean(z)+2*np.std(z)) or (ele <= np.mean(z)-2*np.std(z)):
                pass
            else:
                summ.append(ele)
        mean_xyz.append(np.sum(summ)/len(summ))
        mean.append(mean_xyz)
    print(path)
    # mapping to the order i set for each item 
    # 0 tv 
    # 1 bottle 
    # 2 teddy 
    # 3 keyboard 
    # print(my_mapping_dict[output_list[index]])
    print(output_list[index] , mean[my_mapping_dict[output_list[index]]])
    with open("output.txt", 'a') as f:
        output_string = output_list[index] + " " +  str(mean[my_mapping_dict[output_list[index]]]) + '\n'
        f.write(output_string)
    # print(mean[my_mapping_dict[output_list[index]]])
    index+=1
    # for show in mean:

    #     print(show)
        # print(mean)