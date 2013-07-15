import urllib
import re
import xml.etree.ElementTree as ET
import cgi
#import unicodedata

# Notes:
#   import urllib
#   f = { 'eventName' : 'myEvent', 'eventDescription' : "google search query with spaces"}
#   urllib.urlencode(f)
#   # will give: 'eventName=myEvent&eventDescription=cool+event'
#
# https://code.google.com/apis/console/
#
# https://developers.google.com/places/documentation/search
#
# https://maps.googleapis.com/maps/api/place/textsearch/xml?query=giorgio+near+Montreal&sensor=true&key=AIzaSyBzHSfl-SZJpgCwSTnAhHjlDH5W3BDIMDk
#
# https://maps.googleapis.com/maps/api/place/details/xml?reference=CnRvAAAAtH01YmwBchXwwAEILacJxZrYSh9KMgAll6sSKxDPr8SJWxX2Uz6bpETVzG29cUADa3plL2-G1Cs68C_P8O72f7z1FaC0Dhl9eft368s_XNrHZgQjvpmQYXG-lTS5pUYgIifMwWfxDhdrZhsblEAOBBIQ-pXarAl7OjHDrI4UqCMAyhoU1_cOFN_nGxK7vctY-Ez735fRSJ4&sensor=true&key=AIzaSyBzHSfl-SZJpgCwSTnAhHjlDH5W3BDIMDk
#
#

GOOGLE_API_KEY = "AIzaSyBzHSfl-SZJpgCwSTnAhHjlDH5W3BDIMDk"
BLANK_LINE = ""

################################################################################
def readUrl(url):
  lines = []
  try:
    for l in urllib.urlopen(url):
      lines.append(l.rstrip())
  except:
    pass
  return lines

################################################################################
def htmlEscape(line):
  return cgi.escape(line)

################################################################################
def surroundDiv(line):
  return "<div>" + line + "</div>\n"

################################################################################
def processLine(line):
  if line == "":
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
def metarHandler(station):
  lines = []
  if len(station) > 0:  # user provided a station
    metarLines = getMetar(station)
    if len(metarLines) > 0:  # metar data available
      stationName = findStation(station)
      if len(stationName) > 0:
        match = re.match('^(...................)', stationName[0])
        if match:
          lines.append(match.group(0))
      lines += metarLines
    else: # metar data not found
      for l in findStation(station): # try to find the name of the station
        #                   CO GRAND JUNCTION   KGJT  GJT
        match = re.match('^(.............................)', l)
        if match:
          lines.append(match.group(0))
          lines.append(BLANK_LINE)
  else:
    lines = ["No station provided", "Syntax: @mgouin <STATION>", "Example: @mgouin KJFK"]

  return lines

################################################################################
def gmlsGetInfo(ref):
  params = {'reference' : ref,
            'sensor' : 'false',
            'key' : GOOGLE_API_KEY}
  url = "https://maps.googleapis.com/maps/api/place/details/xml?"
  url += urllib.urlencode(params)
  lines = []
  try:
    f = urllib.urlopen(url)  # real
    #f = open("c:/2.xml", 'r') # debug

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
  #query = ''.join(x for x in unicodedata.normalize('NFKD', query))
  params = {'query' : query,
            'sensor' : 'false',
            'key' : GOOGLE_API_KEY}
  url = "https://maps.googleapis.com/maps/api/place/textsearch/xml?"
  url += urllib.urlencode(params)
  try:
    f = urllib.urlopen(url)  # real
    #f = open("c:/1.xml", 'r') # debug
    root = ET.parse(f).getroot()
    if root.find('status').text == 'OK':
      results = root.findall('result')
      for i in range(min(10, len(results))):
        result = results[i]
        infoLines = gmlsGetInfo(result.find("reference").text)
        s = "#%d. " % (i + 1)
        if len(infoLines) > 0:
          s += infoLines.pop(0)
        lines.append(s)
        lines += infoLines
        lines.append(BLANK_LINE)
  except:
    pass

  return lines

def unitTest():
  query = "Hardware store near St-Hubert, Qc"
  print urllib.urlencode({'q' : query})
  #print gmlsHandler(query)
  for s in ['<', '>', '&']:
    print htmlEscape(s)

def main():
  s = "QC SAINT HUBERT ARP CYHU  YHU   71371  45 31N  073 25W   27   X     T          6 CA\n"
  #    QC SAINT HUBERT ARP
  # "^(...................)"
  s = s.rstrip()
  print s

if __name__ == '__main__':
  main()
  unitTest()

