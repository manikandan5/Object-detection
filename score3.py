#! /usr/bin/env python

import os,sys

if len(sys.argv)<3:
    print("usage: python score.py <prediction-file> <test-file>")
    sys.exit(2)

predictionFile = open(sys.argv[1],'r')
testDataFile = open(sys.argv[2],'r')

predictions = predictionFile.readlines()
testData = testDataFile.readlines()

predictionFile.close()
testDataFile.close()

folders = os.listdir("test")

correct = [[0 for i in range(25)] for j in range(25)]

output = open("output",'w')

for i in range(len(predictions)):
    guess = int(predictions[i].split()[0]) -1
    answer = int(testData[i].split()[0]) -1
    correct[answer][guess] = int(correct[answer][guess]) + 1

for col in range(len(sorted(folders))):
    output.write("\t")
    output.write(sorted(folders)[col][0:2])
    print("\t",end="")
    print(sorted(folders)[col][0:2],end="")

output.write("\n")
print()

rowCount = 0

for row in correct:
    output.write(sorted(folders)[rowCount][0:2])
    output.write("\t")
    print(sorted(folders)[rowCount][0:2],end="")
    print("\t",end="")
    rowCount = rowCount+1
    for col in row:
        output.write(str(col))
        output.write("\t")
        print(str(col),end="")
        print("\t",end="")
    output.write("\n")
    print()

correctGuess = 0

for i in range(25):
    correctGuess = correctGuess + correct[i][i]

print("Accuracy = ",end="")
print((correctGuess*100.0)/len(testData))
output.write("Accuracy = ")
output.write(str((correctGuess*100.0)/len(testData)))

output.close()
