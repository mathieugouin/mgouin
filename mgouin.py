import webapp2
import urllib
import re

import mgouinlib as MGL

# parameter:
#  txtweb-verifyid
#   The txtweb-verifyid string that was sent to your app
#
#  txtweb-message
#   The txtweb-message string that was sent to your app
#
#  txtweb-mobile
#   The txtweb-mobile string that was sent to your app
#
#  txtweb-protocol
#   The txtweb-protocol string that was sent to your app
#
#
#   http://weather.noaa.gov/pub/data/observations/metar/stations/CYHU.TXT
#   http://weather.rap.ucar.edu/surface/stations.txt
#   http://localhost:8080/?txtweb-message=CYHU
#   http://mgouin.appspot.com/?txtweb-message=CYHU

class MainPage(webapp2.RequestHandler):
    def get(self):
        self.response.headers["Content-Type"] = "text/html"
        self.response.write('<html><head><title>mgouin</title><meta name="txtweb-appkey" content="9dd9146a-ad9f-4dea-b38f-065b4165b357" /></head>\n<body>\n')
        #self.response.write("<div>MGouin App</div>")

        #for a in self.request.arguments():
        #    self.response.write("<div>Argument [" + a + "] = [" + self.request.get(a) + "]</div>")

        station = self.request.get("txtweb-message").upper()
        #self.response.write("<div>Asked for station: [" + station + "]</div>")

        lines = MGL.metarHandler(station)

        # Write response
        for l in lines:
            self.response.write("<div>" + l + "</div>\n")

        self.response.write(r"""<script>""")
        self.response.write(r"""  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){""" + "\n")
        self.response.write(r"""  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),""" + "\n")
        self.response.write(r"""  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)""" + "\n")
        self.response.write(r"""  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');""" + "\n")
        self.response.write(r"""  ga('create', 'UA-1787000-3', 'mgouin.appspot.com');""" + "\n")
        self.response.write(r"""  ga('send', 'pageview');""" + "\n")
        self.response.write(r"""</script>""" + "\n")

        self.response.write("\n</body>\n</html>\n")

app = webapp2.WSGIApplication([
    ('/', MainPage),
], debug=True)


