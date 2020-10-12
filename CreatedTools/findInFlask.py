import sys
import os
defaultKeyword = "subprocess.popen"

supportedFlags = ["-f", "-k"]
    
def help():
    print("Flask subclasses finder.... by: waza")
    print("OPTIONS:")
    print("-h -> print this")
    print("-f -> file with the dump")
    print("-k -> keyword to find (default: %s".format(defaultKeyword))

def getFileContent(filePath):
    print("Loading file [{}]...".format(filePath), end="")
    f = open(filePath, "r")
    fileContent = f.read()
    f.close()
    print("DONE")
    return fileContent


def findMatch(filePath, keyword = defaultKeyword):
    
    content = getFileContent(filePath)
    print("Searching for keyword [{}]...".format(keyword), end="")
    inWord = False
    idxCounter = -1
    currWord = ""
    foundIndexes = []
    for chIdx in range(0, len(content)):
        currChar = content[chIdx]
        if(inWord):
            if(currChar == ">"):
                inWord = False
                if(keyword in currWord.lower()):
                    foundIndexes.append(str(idxCounter))
                currWord = ""
            else:
                currWord = "{}{}".format(currWord, currChar)
        elif(currChar == "<"):
            idxCounter += 1
            inWord = True
        else: 
            continue
    print("DONE")
    return " ; ".join(foundIndexes)
            
            
        

def processArgs(args):
    if "-f" not in args:
        print("no file specified")
        return
    keyword = defaultKeyword
    filePath = ""    
    for i in range(1, len(args), 2):
        currFlag = args[i]
        if( currFlag in supportedFlags):
            if(currFlag == "-f"):
                if(os.path.isfile(args[i+1])):
                    filePath = args[i+1]
                else:
                    print("sourced path invalid!!")
                    return

            elif(currFlag == "-k"):
                keyword = args[i+1]
        else:
            print("Invalid arguments... issue -h")
            return
    print("FOUND INDEXES:  {}".format(findMatch(filePath, keyword)))
    


if __name__ == "__main__":
    if("-h" in sys.argv):
        printHelp()
    else:
        processArgs(sys.argv)
