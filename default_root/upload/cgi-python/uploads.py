#!/usr/bin/python3

import cgi
import html
import os
import sys

upload_directory = os.environ.get('UPLOAD_STORE')

print("Content-Type: text/html;charset=utf-8\r\n")

html_string = """
<!DOCTYPE html>
<html lang='en'>
   <head>
      <meta charset='UTF-8'>
      <link type='image/png' rel='icon' href='./skull.png'>
      <title>CGI-Python Page</title>
      <style>
         body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f5f5f5;
            margin: 0;
            padding: 50px;
         }
         h1 {
            color: #333333;
         }
         p {
            color: #666666;
         }
         #background-video {
            visibility: hidden;
         }
      </style>
      <link rel='stylesheet' href='./webserv.css'>
   </head>
   <body>
"""

if os.environ.get('REQUEST_METHOD', '') == 'GET':
   html_string += "<h1>This script is better with a POST METHOD 🍭 </h1>"

elif os.environ.get('REQUEST_METHOD', '') == 'POST':
   try:
      form = cgi.FieldStorage()
      if 'file' in form:
         file_item = form['file']

         if file_item.file and file_item.filename:
            file_name = os.path.basename(file_item.filename)
            file_name = file_name.replace(' ', '_')
            upload_file = os.path.join(upload_directory, file_name)

            try:
               with open(upload_file, 'wb') as uploaded_file:
                  uploaded_file.write(file_item.file.read())

                  html_string += f"<h1>The file <a href='./{file_name}'>{file_name}</a> has been uploaded successfully using Python-CGI!</h1>\n"
            except Exception as e:
               html_string += f"<h1>Error uploading the file {file_name}: {str(e)}</h1>\n"
         else:
            html_string += "<h1>File upload error: No file selected or file object is empty.</h1>\n"

      else:
         if not all(key in form for key in ('name', 'email', 'message')) or \
            any(form[key].value == '' for key in ('name', 'email', 'message')):
            html_string += "<h1>🤔 Hmmm ... It seems that there are some fields missing, try again!</h1>"
            html_string += "<p>Look what I recovered:</p>"
            data = os.environ.get('CONTENT_LENGTH') and sys.stdin.read(int(os.environ['CONTENT_LENGTH']))
            html_string += f"<p>{data}</p>"
         else:
            name = html.escape(form['name'].value)
            email = html.escape(form['email'].value)
            message = html.escape(form['message'].value)

            html_string += "<h2>Form Submission Result</h2>"
            html_string += f"<p>Name: {name}</p>"
            html_string += f"<p>Email: {email}</p>"
            html_string += f"<p>Message: {message}</p>"
   except Exception as e:
      content_length = int(os.environ.get('CONTENT_LENGTH', 0))
      body_content = sys.stdin.buffer.read(content_length)
      body_content_str = body_content.decode('utf-8')
      html_string += "<h1>🤔 Hmmm ... It seems that whoever sent this did not use a form!</h1>"
      html_string += "<p>Look what I recovered:</p>"
      html_string += f"<p>{body_content_str}</p>"
      

html_string += """
      <p> Coded by mflores- and dnieto-c </p>
      <video id='background-video' autoplay loop muted>
         <source src='./webserv.mp4' type='video/mp4'>
      </video>
   </body>
</html>
"""

# Calculate Content-Length
content_length = len(html_string.encode('utf-8'))

# Output the HTML content
# print("Content-Length: {}".format(content_length))
print("\r\n")
print(html_string)
