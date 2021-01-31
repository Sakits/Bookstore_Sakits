#!/usr/bin/python3

time_Limit = 30 # s

import os
import filecmp


def cleanSh():
    os.system('bash ./clean.sh')
    os.system('rm output.txt')

def cleanAll():
    os.system('rm main')
    os.system('rm clean.sh')

def run_Basic_Case(id):
    if id <= 6:
        os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/BasicDataSet/testcase' + str(id) + '.txt >> output.txt')
        #return filecmp.cmp('Data/BasicDataSet/testcase' + str(id) + '_ans.txt', 'output.txt')
        if os.system ('diff output.txt ' + 'Data/BasicDataSet/testcase' + str(id) + '_ans.txt'):
            print (id)
            return 0
    if id == 7:
        for i in range(1, 4):
            os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/BasicDataSet/testcase7/' + str(i) + '.txt >> output.txt')
        #return filecmp.cmp('Data/BasicDataSet/testcase7/ans.txt', 'output.txt')
        if os.system ('diff output.txt ' + 'Data/BasicDataSet/testcase7/ans.txt'):
            print (id)
            return 0
    if id == 8:
        for i in range(1, 101):
            os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/BasicDataSet/testcase8/' + str(i) + '.txt >> output.txt')
        if os.system ('diff output.txt ' + 'Data/BasicDataSet/testcase8/ans.txt'):
            print (id)
            return 0
        #return filecmp.cmp('Data/BasicDataSet/testcase8/ans.txt', 'output.txt')
    return 1

def run_Basic():
    print('Running Basic Test')
    sum = 0
    for i in range(1, 9):
        if run_Basic_Case(i):
            print('Basic Testpoint ' + str(i) + ' Accepted!')
            sum = sum + 1
        else:
            print('Basic Testpoint ' + str(i) + ' Failed!')
        cleanSh()
    print('Your Basic Test Score is', sum / 8 * 3)
    return sum / 8 * 3

def run_Advanced_Case(id):
    if id != 3:
        for i in range(1, 11):
            os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/AdvancedDataSet/testcase' + str(id) + '/' + str(i) + '.in > output.txt')
            #if not filecmp.cmp('Data/AdvancedDataSet/testcase' + str(id) + '/' + str(i) + '.out', 'output.txt'):
                #return 0
            if os.system ('diff output.txt ' + 'Data/AdvancedDataSet/testcase' + str(id) + '/' + str(i) + '.out'):
                print (id, i) ;
                return 0
    else:
        for i in range(1, 6):
            os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/AdvancedDataSet/testcase' + str(id) + '/' + str(i) + '.in > output.txt')
            if not filecmp.cmp('Data/AdvancedDataSet/testcase' + str(id) + '/' + str(i) + '.out', 'output.txt'):
                print (id, i) ;
                return 0
    return 1

def run_Advanced():
    print('Running Advanced Test')
    sum = 0
    for i in range(1, 6):
        if run_Advanced_Case(i):
            print('Advanced Testpoint ' + str(i) + ' Accepted!')
            sum = sum + 1
        else:
            print('Advanced Testpoint ' + str(i) + ' Failed!')
        cleanSh()
    print('Your Advanced Test Score is', sum / 5 * 2)
    return sum / 5 * 2

def run_Complex_Case(id):
    for i in range(1, 11):
        os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/ComplexDataSet/testcase' + str(id) + '/' + str(i) + '.in > output.txt')
        #if not filecmp.cmp('Data/ComplexDataSet/testcase' + str(id) + '/' + str(i) + '.out', 'output.txt'):
            #return 0
        if os.system ('diff output.txt ' + 'Data/ComplexDataSet/testcase' + str(id) + '/' + str(i) + '.out'):
                print (id, i) ;
                return 0
    return 1

def run_Complex():
    print('Running Complex Test')
    sum = 0
    for i in range(1, 6):
        if run_Complex_Case(i):
            print('Complex Testpoint ' + str(i) + ' Accepted!')
            sum = sum + 1
        else:
            print('Complex Testpoint ' + str(i) + ' Failed!')
        cleanSh()
    print('Your Complex Test Score is', sum / 5 * 2)
    return sum / 5 * 2

def run_Insider_Case(id):
    for i in range(1, 11):
        os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/InsiderDataSet/testcase' + str(id) + '/' + str(i) + '.in > output.txt')
        if not filecmp.cmp('Data/InsiderDataSet/testcase' + str(id) + '/' + str(i) + '.out', 'output.txt'):
            return 0
    return 1

def run_Insider():
    print('Running Insider Test')
    sum = 0
    for i in range(1, 4):
        if run_Insider_Case(i):
            print('Insider Testpoint ' + str(i) + ' Accepted!')
            sum = sum + 1
        else:
            print('Insider Testpoint ' + str(i) + ' Failed!')
        cleanSh()
    print('Your Insider Test Score is', sum / 3 * 2)
    return sum / 3 * 2

def run_Robust_Case(id):
    os.system('gtimeout ' + str(time_Limit) + 's ' + './main < Data/RobustDataSet/testcase' + str(id) + '.in > output.txt')
    return filecmp.cmp('Data/RobustDataSet/testcase' + str(id) + '.out', 'output.txt')

def run_Robust():
    print('Running Robust Test')
    sum = 0
    for i in range(1, 6):
        if run_Robust_Case(i):
            print('Robust Testpoint ' + str(i) + ' Accepted!')
            sum = sum + 1
        else:
            print('Robust Testpoint ' + str(i) + ' Failed!')
        cleanSh()
    print('Your Robust Test Score is', sum / 5 * 2)
    return sum / 5 * 2

Score = 0

Score += run_Basic()
Score += run_Advanced()
Score += run_Complex()
# Score += run_Insider()
Score += run_Robust()

print('Your Total Score is', Score)
