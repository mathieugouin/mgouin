import urllib
import urllib2
import re
import xml.etree.ElementTree as ET
import cgi

################################################################################
# Notes:
# https://appengine.google.com/
# https://code.google.com/apis/console/
# https://developers.google.com/places/documentation/search
################################################################################

GOOGLE_API_KEY = "AIzaSyBzHSfl-SZJpgCwSTnAhHjlDH5W3BDIMDk"
BLANK_LINE = ""
GOOGLE_SENSOR = "false"

################################################################################
def readUrl(url):
    lines = []
    try:
        for l in urllib2.urlopen(url):
            lines.append(l.rstrip())
    except:
        pass
    return lines

################################################################################
def readUrlAll(url):
    html = ""
    try:
        html = urllib2.urlopen(url).read()
    except:
        pass
    return html

################################################################################
def htmlEscape(line):
    return cgi.escape(line)

################################################################################
def encodeDict(in_dict):
    out_dict = {}
    for k, v in in_dict.iteritems():
        if isinstance(v, unicode):
            v = v.encode('utf8')
        elif isinstance(v, str):
            # Must be encoded in UTF-8
            v.decode('utf8')
        out_dict[k] = v
    return out_dict

################################################################################
def surroundDiv(line):
    return "<div>" + line + "</div>\n"

################################################################################
def processLine(line):
    if line == BLANK_LINE:
        return surroundDiv("&nbsp;")
    else:
        return surroundDiv(htmlEscape(line))

################################################################################
def findStation(txt):
    lines = []
    for l in readUrl("http://weather.rap.ucar.edu/surface/stations.txt"):
        if re.search(txt, l, re.IGNORECASE):
            lines.append(l)
    return lines

################################################################################
def getMetar(station):
    metarLines = []
    for l in readUrl("http://weather.noaa.gov/pub/data/observations/metar/stations/" + station + ".TXT"):
        if re.search('was not found on this server', l):
            break
        elif re.search(station, l):
            metarLines.append(l)
    return metarLines

################################################################################
def getMetar2(station):
    metarLines = []
    url = "http://aviationweather.gov/adds/metars/?station_ids=" + station + \
          "&std_trans=standard&chk_metars=on&hoursStr=most+recent+only&submitmet=Submit"
    html = readUrlAll(url)
    match = re.search(r">(" + station + r"\b.+?)</FONT>", html, re.MULTILINE | re.DOTALL)
    if match:
        s = match.group(1)
        metarLines.append(re.sub(r"\n *", " ", s))
    return metarLines

################################################################################
def getTaf(station):
    lines = []
    url = "http://aviationweather.gov/adds/metars/?station_ids=" + station + \
          "&std_trans=standard&hoursStr=most+recent+only&chk_tafs=on&submitmet=Submit"
    html = readUrlAll(url)
    match = re.search(
        re.escape(r"""<PRE><font face="Monospace,Courier" size="+1">""") + \
        "(.+?)" + \
        re.escape("</font></PRE>"),
        html, re.MULTILINE | re.DOTALL)
    if match:
        for l in match.group(1).split("\n"):
            l = l.strip()
            if l != "":
                lines.append(l)
                lines.append(BLANK_LINE)
        if len(lines) >= 2:
            lines.pop() # remove last blank line
    return lines

################################################################################
def metarHandler(station):
    lines = []
    station = station.upper()
    if len(station) > 0: # user provided a station
        metarLines = getMetar2(station)
        if len(metarLines) > 0: # metar data available
            stationName = findStation(station)
            if len(stationName) > 0:
                match = re.match(r"^(...................)", stationName[0])
                if match:
                    lines.append(match.group(1))
            lines += metarLines
            # Find taf data
            tafLines = getTaf(station)
            if len(tafLines) > 0:
                lines.append(BLANK_LINE)
                lines += tafLines
        else: # metar data not found
            for l in findStation(station): # try to find the name of the station
                #                    CO GRAND JUNCTION   KGJT  GJT
                match = re.match(r"^(.............................)", l)
                if match:
                    lines.append(match.group(1))
                    lines.append(BLANK_LINE)
            if len(lines) >= 2:
                lines.pop() # remove last blank line
    else:
        lines = ["METAR Syntax: @mgouin <station>",
                 "Example: @mgouin KJFK",
                 "Airport Finder Syntax:",
                 "@mgouin <keyword>",
                 "Example: @mgouin miami"]

    return lines

################################################################################
def gmlsGetInfo(ref):
    params = {'reference' : ref,
              'sensor' : GOOGLE_SENSOR,
              'key' : GOOGLE_API_KEY}
    url = "https://maps.googleapis.com/maps/api/place/details/xml?"
    url += urllib.urlencode(encodeDict(params))
    lines = []
    try:
        f = urllib.urlopen(url)

        root = ET.parse(f).getroot()
        if root.find('status').text == 'OK':
            for result in root.findall('result'):
                lines.append(result.find('name').text)
                lines.append(result.find('formatted_address').text)
                lines.append(result.find('formatted_phone_number').text)
    except:
        pass

    return lines

################################################################################
def gmlsHandler(query):
    lines = []
    params = {'query' : query,
              'sensor' : GOOGLE_SENSOR,
              'key' : GOOGLE_API_KEY}
    url = "https://maps.googleapis.com/maps/api/place/textsearch/xml?"
    url += urllib.urlencode(encodeDict(params))
    try:
        f = urllib2.urlopen(url)
        root = ET.parse(f).getroot()
        if root.find('status').text == 'OK':
            results = root.findall('result')
            for i in range(min(10, len(results))):
                result = results[i]
                infoLines = gmlsGetInfo(result.find("reference").text)
                s = "#%d. " % (i + 1)
                if len(infoLines) > 0:
                    s += infoLines.pop(0) # put place title with the number
                lines.append(s)
                lines += infoLines
                lines.append(BLANK_LINE)
            if len(lines) >= 2:
                lines.pop() # remove last blank line
    except:
        pass

    return lines

################################################################################
def outputTest(lines):
    for l in lines:
        print processLine(l),

################################################################################
def gmlsTest():
    query = u"caf\xe9 \xe0 montr\xe9al"
    print query
    print urllib.urlencode(encodeDict({'q' : query}))
    outputTest(gmlsHandler(query))

################################################################################
def metarTest():
    station = "CYHU"
    #print getMetar(station) == getMetar2(station)
    #print getMetar(station)
    #print getMetar2(station)
    #outputTest(getTaf(station))
    outputTest(metarHandler(station))

################################################################################
def urlTest():
    import urlparse
    # app log from txt web request
    s = "txtweb-message=caf%C3%A9%20%C3%A0%20montreal"
    print dict(urlparse.parse_qsl(s))

################################################################################
def main():
    metarTest()
    #gmlsTest()
    #urlTest()

if __name__ == '__main__':
    main()

