import re

def main():
    stations = []
    f = open("stations.txt", "r")
    data = f.readlines()
    f.close()

    for l in data:
        #   NANJING/NANKING  ZSNJ
        #           "AK ANCHORAG/NIKISKI PAHG...."
        m =  re.match("................... (\w{4})", l)
        if m:
            #print m.group(1)
            stations.append(m.group(1))
    dataAll = "".join(data)
    duplicate = []
    for station in stations:
        nb = len([m.start() for m in re.finditer(station, dataAll)])
        if nb > 1:
            duplicate.append("%s %d" % (station, nb))
    #print stations.
    for d in duplicate:
        print d

def dupTest():
    f = open("dup.txt", "r")
    dupData = f.readlines()
    f.close()

    f = open("stations.txt", "r")
    data = f.readlines()
    f.close()

    for l in dupData:
        l = l.split()[0]
        #print l

        lines = []
        txt = r"^.{20}\b" + l.strip() + r"\b"
        for ld in data:
            if re.search(txt, ld, re.IGNORECASE):
                lines.append(l)
        nb = len(lines)
        if nb != 1:
            print l, nb

if __name__ == '__main__':
    #main()
    dupTest()
