# import time
import os

from datetime import datetime

now = datetime.now()

current_time = now.strftime("%m/%d/%Y, %H:%M:%S")

print("""X-Powered-By: Python2\r
Content-Type: text/html\r
\r
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Document</title>
</head>
<body>
<h1>hello world!!</h1>
""")
print(current_time)
for i in os.environ:
	print("<p>{0}={1}</p>".format(i, os.environ[i]))
print(
"""
</body>
</html>
""")