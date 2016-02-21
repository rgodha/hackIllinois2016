# hackIllinois2016
With the Google Safe Browsing API, you can query URLs to detect and alert on malicious or phishing sites. By adding this to the Bro Network Security Monitor, you would gain alerts and protection for all users on a network.

The idea here is to integrate the Bro network security monitor with Google's Safe Browsing API. Bro is a passive network security
monitor -- it receives a copy of your network traffic, and it's able to analyze the higher-level protocols to log details like URLs, file hashes, SSL certificates, etc. At NCSA (University of Illinois, UC) they rely on Bro to monitor and secure everything from their wireless nets to some of the largest supercomputers -- all told, Bro have over 1 Tbps of fiber connectivity that can be monitored with Bro.

The Google Safe Browsing API documentation is available here:

    https://developers.google.com/safe-browsing/developers_guide_v3

In short, Google publishes three blacklists:
  * Sites hosting or redirecting to phishing pages,
  * Sites hosting or redirecting to malware,
  * Sites hosting or redirecting to unwanted software pages.

Bro is unique because it has extensive scripting capabilities. An integration with the Safe Browsing API would allow us to detect very
specific activity, such as: 
  * A user submits a POST request to a URL that's on the phishing blacklist,
  * A user downloads a file with MIME type application/x-dosexec from a malware site,
  * A user that we know is running an outdated and vulnerable version of Adobe Flash visits a malware site.

When these events happen, Bro could alert us, or it could even take things a step further. It could inspect the contents of the POST
request, extract the username and password, and check to see if those credentials are the same as their NCSA credentials (for example, Bro could attempt an LDAP bind as that user). If the user submitted their NCSA credentials to the phishing site, Bro could send them an e-mail and scramble their password -- all automatically in a few seconds.

Similarly, Bro has the capability to spoof and inject RST packets to force a connection teardown. If a user starts downloading a file from a malware site, Bro could interrupt the file transfer.

One of the really neat things it that Bro can do this for other protocols as well. For example, Bro can extract URLs from e-mails, and
check those against the Safe Browsing API. There was an article a couple months ago that some advanced attackers are leveraging the free Let's Encrypt SSL certificates to encrypt their malicious pages. Thanks to Bro's SSL analyzer, Bro could check hostnames from SSL certs against the API as well and RST encrypted connections just as easily.

As more and more exploits are moving to the web, I think this integration would be the single biggest security feature in several
years.


Google Developer Guide: https://developers.google.com/safe-browsing/developers_guide_v3 
It gives step by step process:
  1) Request message format to be sent to get list of blacklisted websites.
  2) Response message format to parse the response from Google and get all lists.
  3) Request the hashs from all the lists and save them.
  4) For each hash request the full hashes.
  5) Convert them in canonical format.
  
Detailed request and response message format is mentioned in the Guide.
