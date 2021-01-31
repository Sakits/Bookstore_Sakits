import os
print('Input the test case you want to run!')
x = int(input())
y = int(input())

os.system('./run.sh')
com = 'rm my.out'
os.system(com)

# for i in range(1, y + 1):
#     print('Runing test data', i)
#     com = 'time ./main < ../Data/ComplexDataSet/testcase' + str(x) + '/' + str(i) + '.in > my.out'
#     os.system(com)
#     print("Diffing Output")
#     com = 'diff my.out ../Data/ComplexDataSet/testcase' + str(x) + '/' + str(i) + '.out'
#     os.system(com)

com = 'time ./main < ../Data/RobustDataSet/testcase' + str(x)  + '.in > my.out'
os.system(com)
print("Diffing Output")
com = 'diff my.out ../Data/RobustDataSet/testcase' + str(x)  + '.out'
os.system(com)

