<?php

$htmlString = <<<HTML
<!DOCTYPE html>
<html lang='en'>
   <head>
      <meta charset='UTF-8'>
      <link type='image/png' rel='icon' href='./skull.png'>
      <title>CGI Page</title>
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
            visibility : hidden;
         }
      </style>
      <link rel='stylesheet' href='./webserv.css'>
   </head>
   <body>
HTML;

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        if (!isset($_GET['name']) || !isset($_GET['email']) || !isset($_GET['message'])
            || empty($_GET['name']) || empty($_GET['email']) || empty($_GET['message']))
        {
            echo "400\r\n";
            $htmlString .= "<h1>🤔 Hmmm ... Seems like something is missing!</h1>";
        } else {
            echo "200\r\n";
            $name = htmlspecialchars($_GET["name"]);
            $email = htmlspecialchars($_GET["email"]);
            $message = htmlspecialchars($_GET["message"]);
    
            $htmlString .= "<h2>Form Submission Result</h2>";
            $htmlString .= "<p>Name: $name</p>";
            $htmlString .= "<p>Email: $email</p>";
            $htmlString .= "<p>Message: $message</p>";
        }
    } else {
        echo "400\r\n";
        $htmlString .= "<p>No form submission detected.</p>";
    }
    
    $htmlString .= "      <p> Coded by mflores- and dnieto-c </p>\n";
    $htmlString .= "      <video id='background-video' autoplay loop muted>\n";
    $htmlString .= "         <source src='./webserv.mp4' type='video/mp4'>\n";
    $htmlString .= "      </video>\n";
    $htmlString .= "   </body>\n";
    $htmlString .= "</html>\n";
    echo $htmlString;

?>
