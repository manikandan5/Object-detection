import os,sys
if (sys.version).split(".")[0] == "3":
    os.system("python score3.py "+sys.argv[1]+" "+sys.argv[2])
else:
    os.system("python score2.py "+sys.argv[1]+" "+sys.argv[2])
