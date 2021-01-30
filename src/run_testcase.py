import os
print('Input the test case you want to run!')
x = int(input())
y = int(input())

os.system('./run.sh')
com = 'rm my.out'
os.system(com)

for i in range(1, y + 1):
    print('Runing test data', i)
    com = 'time ./main < ../Data/AdvancedDataSet/testcase' + str(x) + '/' + str(i) + '.in > my.out'
    os.system(com)
    # print("Diffing Output")
    # com = 'diff my.out ../Data/AdvancedDataSet/testcase' + str(x) + '/' + str(i) + '.out'
    # os.system(com)

