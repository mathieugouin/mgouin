import urllib
import urllib2
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
GOOGLE_SENSOR  = "false"

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
def getMetar2(station):
  metarLines = []
  url = "http://aviationweather.gov/adds/metars/?station_ids=" + station + "&std_trans=standard&chk_metars=on&hoursStr=most+recent+only&chk_tafs=on&submitmet=Submit"
  try:
    html = urllib2.urlopen(url).read()
    match = re.search(r">(" + station + r"\b.+?)</FONT>", html, re.MULTILINE | re.DOTALL)
    if match:
      s = match.group(1)
      metarLines.append(re.sub(r"\n *", " ", s))
  except:
    pass
  return metarLines

################################################################################
def metarHandler(station):
  lines = []
  station = station.upper()
  if len(station) > 0:  # user provided a station
    metarLines = getMetar2(station)
    if len(metarLines) > 0:  # metar data available
      stationName = findStation(station)
      if len(stationName) > 0:
        match = re.match(r"^(...................)", stationName[0])
        if match:
          lines.append(match.group(1))
      lines += metarLines
    else: # metar data not found
      for l in findStation(station): # try to find the name of the station
        #                   CO GRAND JUNCTION   KGJT  GJT
        match = re.match(r"^(.............................)", l)
        if match:
          lines.append(match.group(1))
          lines.append(BLANK_LINE)
  else:
    lines = ["No station provided", "Syntax: @mgouin <STATION>", "Example: @mgouin KJFK"]

  return lines

################################################################################
def gmlsGetInfo(ref):
  params = {'reference' : ref,
            'sensor' : GOOGLE_SENSOR,
            'key' : GOOGLE_API_KEY}
  url = "https://maps.googleapis.com/maps/api/place/details/xml?"
  url += urllib.urlencode(params)
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
  url += urllib.urlencode(params)
  try:
    f = urllib.urlopen(url)
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
  query = "Tim Hortons in St-Laurent"
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

def metarTest():
  station = "kcme"
  #print getMetar(station) == getMetar2(station)
  #print getMetar(station)
  print getMetar2(station)
  #print metarHandler(station)

if __name__ == '__main__':
  #main()
  #unitTest()
  metarTest()
  #print metarHandler("CYUL")

