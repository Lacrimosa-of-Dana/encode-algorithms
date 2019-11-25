from collections import OrderedDict
from time import time

def LZEncode(addr):
    with open(addr, "rb") as f:
        src = f.read()

    dic = OrderedDict()
    # key: list, value: (no., last, add)
    
    readNm = 0
    dicNum = 1
    while readNm < len(src):
        buff = 0
        while readNm < len(src):
            buff <<= 8
            buff += src[readNm]
            if not dic.get(buff):
                if not dic.get(buff >> 8):
                    dic[buff] = (dicNum, 0, src[readNm])
                else:
                    dic[buff] = (dicNum, dic[buff >> 8][0], src[readNm])
                readNm += 1
                dicNum += 1
                break
            readNm += 1
    rest = dic[buff]
    #print(dicNum)
    #print(dic)
    f = open(addr + ".lz", "wb")
    bitLen = len(bin(dicNum)[2:])
    #print(bitLen)
    f.write(bytes([bitLen]))
    writeBuff = 0
    time = 0
    for k, v in dic.items():
        for i in range(bitLen - 1, -1, -1):
            writeBuff <<= 1
            time += 1
            writeBuff ^= (v[1] >> i) & 1
            if time == 8:
                f.write(bytes([writeBuff]))
                writeBuff = 0
                time = 0
        for i in range(7, -1, -1):
            writeBuff <<= 1
            time += 1
            writeBuff ^= (v[2] >> i) & 1
            if time == 8:
                f.write(bytes([writeBuff]))
                writeBuff = 0
                time = 0
    for i in range(bitLen - 1, -1, -1):
        writeBuff <<= 1
        time += 1
        writeBuff ^= (rest[0] >> i) & 1
        if time == 8:
            f.write(bytes([writeBuff]))
            writeBuff = 0
            time = 0
    if 8 - time:
        writeBuff <<= (8 - time)
        f.write(bytes([writeBuff]))
    f.close()

def LZDecode(addr):
    with open(addr, "rb") as f:
        tar = f.read()
    bitLen = tar[0]
    readNm = 1
    readBuff = tar[readNm]
    partNm = 0
    part = 0
    time = 8
    before = 0
    #dic = OrderedDict()
    dic = []
    while readNm < len(tar):
        partNm = 0
        part = 0
        for i in range(bitLen):
            partNm <<= 1
            partNm ^= (readBuff >> (time - 1)) & 1
            time -= 1
            if not time:
                readNm += 1
                if readNm >= len(tar):
                    break
                readBuff = tar[readNm]
                time = 8
        for i in range(8):
            part <<= 1
            part ^= (readBuff >> (time - 1)) & 1
            time -= 1
            if not time:
                readNm += 1
                if readNm >= len(tar):
                    before = 1
                    break
                readBuff = tar[readNm]
                time = 8
        if before and not part:
            dic.append(dic[partNm - 1])
            break
        if not partNm:
            dic.append(bytes([part]))
        else:
            dic.append(b''.join([dic[partNm - 1], bytes([part])]))
    with open(addr[:-3], "wb") as f:
        f.write(b''.join(dic))

if __name__ == '__main__':
    mode = int(input("Input mode: [Encode: 1, Decode: 2]"))
    addr = input("Input address: ")
    start = time()
    if mode == 1:
        LZEncode(addr)
    if mode == 2:
        LZDecode(addr)
    end = time()
    print("Runtime is %.3fs."%(end - start))