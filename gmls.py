import webapp2
import mgouinlib as MGL

################################################################################
# http://localhost:8080/gmls?txtweb-message=Hardware+store+near+St-Hubert%2C+Qc
# http://mgouin.appspot.com/gmls?txtweb-message=Hardware+store+near+St-Hubert%2C+Qc
#
# parameter:
# txtweb-verifyid
# The txtweb-verifyid string that was sent to your app
#
# txtweb-message
# The txtweb-message string that was sent to your app
#
# txtweb-mobile
# The txtweb-mobile string that was sent to your app
#
# txtweb-protocol
# The txtweb-protocol string that was sent to your app
################################################################################

class MainPage(webapp2.RequestHandler):
    def get(self):
        self.response.headers["Content-Type"] = "text/html; charset=utf-8"

        self.response.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"\n')
        self.response.write('  "http://www.w3.org/TR/html4/loose.dtd">\n')
        self.response.write('<html>\n' +
            '<head>\n' +
            '<title>GMLS</title>\n' +
            '<meta http-equiv="Content-type" content="text/html;charset=UTF-8">\n' +
            '<meta name="txtweb-appkey" content="ae878361-320b-4dcf-90b8-39ac8f7e4e75">\n' +
            '</head>\n' +
            '<body>\n\n')

        lines = []
        query = self.request.get("txtweb-message")
        if query == "":
            lines.extend([
                "Google Map Local Search (GMLS)",
                "Search local business & places.",
                "Example:",
                "@gmls cafe in new york"])
        else:
            lines += MGL.gmlsHandler(query)

        for l in lines:
            self.response.write(MGL.processLine(l))

        self.response.write('\n')
        self.response.write(r"""<script type="text/javascript">""" + "\n")
        self.response.write(r"""    (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){""" + "\n")
        self.response.write(r"""    (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),""" + "\n")
        self.response.write(r"""    m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)""" + "\n")
        self.response.write(r"""    })(window,document,'script','//www.google-analytics.com/analytics.js','ga');""" + "\n")
        self.response.write(r"""    ga('create', 'UA-1787000-3', 'mgouin.appspot.com');""" + "\n")
        self.response.write(r"""    ga('send', 'pageview');""" + "\n")
        self.response.write(r"""</script>""" + "\n")

        self.response.write("\n</body>\n</html>\n")

app = webapp2.WSGIApplication([(r'/.*', MainPage)], debug=True)

